#pragma once

#include <opencv2/opencv.hpp>

struct MotionDetector
{
    cv::Mat previous_frame;

    MotionDetector();

    cv::Mat apply(const cv::Mat input);
};
