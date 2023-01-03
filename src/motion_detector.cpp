#include <algorithm>
#include <vector>

#include "motion_detector.h"
#include "rect_helper.h"

MotionDetector::MotionDetector()
    : previous_frame(cv::Mat())
{}

cv::Mat
MotionDetector::apply(const cv::Mat input)
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
    for (size_t i = 0; i < contours.size(); ++i)
    {
        cv::Rect box = boundingRect(contours[i]);
        boxes.push_back(box);
    }

    // Sort boxes by area from largest to smallest
    std::sort(boxes.begin(), boxes.end(),
        [](const cv::Rect &a, const cv::Rect &b) -> bool {
            return a.width * a.height > b.width * b.height;
        }
    );

    // Merge them together if they intersect
    std::vector<cv::Rect> merged_boxes = merge_rects(boxes, rects_intersect);

    for (const cv::Rect &merged_box : merged_boxes) {
        cv::rectangle(output, merged_box, cv::Scalar(0xff), cv::FILLED);
    }

    cv::imshow("MotionDetector Mask", output);

    return output;
}
