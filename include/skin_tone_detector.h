#pragma once

#include <opencv2/opencv.hpp>

struct SkinToneDetector {
    cv::Scalar skin_tone;
    double h_high_delta;
    double h_low_delta;
    double s_high_delta;
    double s_low_delta;

    SkinToneDetector();
    SkinToneDetector(cv::Scalar skin_tone);

    cv::Mat apply(const cv::Mat input);
};
