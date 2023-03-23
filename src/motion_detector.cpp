#include <algorithm>
#include <vector>

#include "motion_detector.h"

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

    cv::imshow("MotionDetector Mask", output);
    cv::pollKey();

    return output;
}
