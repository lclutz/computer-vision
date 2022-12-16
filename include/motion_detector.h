#pragma once

#include <opencv2/opencv.hpp>

struct MotionDetector
{
    bool debug_output;
    cv::Mat previous_frame;

    MotionDetector();

    cv::Mat apply(const cv::Mat &input);
};
