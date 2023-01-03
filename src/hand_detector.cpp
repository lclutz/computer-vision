#include <utility>
#include <vector>

#include "hand_detector.h"
#include "logging.h"
#include "rect_helper.h"

cv::Mat
HandDetector::apply(const cv::Mat input)
{
    cv::Mat output;

    if (started) {
        if (skin_tone_calibrator->skin_tone_calibrated) {
            cv::Mat background_mask_motion = motion_detector->apply(input);
            cv::Mat background_mask_hue = bg_estimator_hue->apply(input, 0.001);
            cv::Mat background_mask_brightness = bg_estimator_brightness->apply(input, 0.01);

            cv::Mat combined_mask = background_mask_motion & background_mask_hue & background_mask_brightness;

            cv::Mat hand_mask = skin_tone_detector->apply(input, combined_mask);

            { // Draw boxes around the hands
                // Find bounding boxes
                std::vector<std::vector<cv::Point>> contours;
                cv::findContours(hand_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                std::vector<cv::Rect> boxes;
                for (size_t i = 0; i < contours.size(); ++i)
                {
                    cv::Rect box = boundingRect(contours[i]);
                    boxes.push_back(box);
                }

                std::vector<cv::Rect> combined_boxes = merge_rects(boxes,
                    [](const cv::Rect &r1, const cv::Rect &r2) -> bool {
                        return std::abs(get_min_distance_rectangles(r1, r2)) < 100.0;
                    }
                );

                // Sort boxes by area from largest to smallest
                std::sort(combined_boxes.begin(), combined_boxes.end(),
                    [](const cv::Rect &a, const cv::Rect &b) -> bool {
                        return a.width * a.height > b.width * b.height;
                    }
                );

                // Merge them together if they intersect
                combined_boxes = merge_rects(combined_boxes, rects_intersect);

                output = input;
                for (const cv::Rect &hand_rect : combined_boxes) {
                    cv::rectangle(output, hand_rect, cv::Scalar(0x00, 0x00, 0xff), 5);
                    cv::putText(
                        output, "Hand",
                        cv::Point(hand_rect.x, hand_rect.y),
                        cv::FONT_HERSHEY_DUPLEX, 1.0,
                        cv::Scalar(0x00, 0x00, 0xff), 2
                    );
                }
            }

        } else {
            output = skin_tone_calibrator->apply(input);
        }
    } else {
        output = input;
    }

    return output;
}

void
HandDetector::handle_event(const Event &e)
{
    switch (e.type)
    {
    case EVENT_START:
        started = true;
        break;

    case EVENT_CALIBRATE_SKIN_TONE:
        if (skin_tone_calibrator->calibrate()) {
            cv::Scalar skin_tone = skin_tone_calibrator->skin_tone;
            skin_tone_detector = std::make_unique<SkinToneDetector>(SkinToneDetector(skin_tone));
            logi("Skin tone calibrated");
        } else {
            loge("Faiiled to calibrate skin tone");
        }
        break;
    default:
        break;
    }
}
