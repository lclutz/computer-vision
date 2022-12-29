#include "hand_detector.h"
#include "logging.h"

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

                // Sort boxes by area from largest to smallest
                std::sort(boxes.begin(), boxes.end(), [](const cv::Rect &a, const cv::Rect &b) -> bool {
                    return a.width * a.height > b.width * b.height;
                });

                std::vector<cv::Rect> merged_boxes;
                for (const cv::Rect &box : boxes) {
                    bool merged = false;

                    for (cv::Rect &merged_box : merged_boxes) {
                        // Check if box is inside merged_box
                        if (box.x < merged_box.x + merged_box.width &&
                            box.x + box.width > merged_box.x &&
                            box.y < merged_box.y + merged_box.height &&
                            box.y + box.height > merged_box.y) {

                            int x_min = std::min(box.x, merged_box.x);
                            int y_min = std::min(box.y, merged_box.y);
                            int x_max = std::max(box.x + box.width, merged_box.x + merged_box.width);
                            int y_max = std::max(box.y + box.height, merged_box.y + merged_box.height);

                            merged_box.x = x_min;
                            merged_box.y = y_min;
                            merged_box.width  = x_max - x_min;
                            merged_box.height = y_max - y_min;

                            merged = true;
                        }
                    }

                    if (!merged) {
                        merged_boxes.push_back(box);
                    }
                }

                output = input;
                for (const cv::Rect &merged_box : merged_boxes) {
                    cv::rectangle(output, merged_box, cv::Scalar(0x00, 0x00, 0xff));
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
