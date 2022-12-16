#pragma once

#include <deque>

#include <opencv2/opencv.hpp>

struct HandDetector {
    cv::Scalar skin_tone;
    double h_high_delta;
    double h_low_delta;
    double s_high_delta;
    double s_low_delta;
    std::deque<cv::Mat> masks;

    HandDetector();
    HandDetector(cv::Scalar skin_tone);

    cv::Mat apply(const cv::Mat &input, cv::Mat *hint = nullptr);
};
