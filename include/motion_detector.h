#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

struct MotionDetector
{
    bool debug_output;
    cv::Mat previous_frame;

    std::vector<cv::Rect> apply(const cv::Mat &input);
};
