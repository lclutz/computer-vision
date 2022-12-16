#include <algorithm>
#include <vector>

#include "motion_detector.h"

MotionDetector::MotionDetector()
    : debug_output(false)
    , previous_frame(cv::Mat())
{}

cv::Mat
MotionDetector::apply(const cv::Mat &input)
{
    cv::Mat gray;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

    if (previous_frame.empty()) { previous_frame = gray; }

    cv::Mat output;
    cv::absdiff(previous_frame, gray, output);

    cv::threshold(
        output,           // srv
        output,           // dst
        0x10,             // thresh
        0xff,             // maxval
        cv::THRESH_BINARY // type
    );

    previous_frame = gray;

    // Find bounding boxes
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(output, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> boxes;
    for (int i = 0; i < contours.size(); ++i)
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

    if (debug_output) {
        cv::Mat debug_image = output.clone();
        cv::cvtColor(debug_image, debug_image, cv::COLOR_GRAY2BGR);
        for (const cv::Rect &merged_box : merged_boxes) {
            cv::rectangle(debug_image, merged_box, cv::Scalar(0, 0, 255));
        }
        cv::imshow("Motion detection", debug_image);
    }

    for (const cv::Rect &merged_box : merged_boxes) {
        cv::rectangle(output, merged_box, cv::Scalar(0xff), cv::FILLED);
    }

    cv::imshow("Bewegung", output);

    return output;
}
