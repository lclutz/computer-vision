#pragma once

#include <opencv2/opencv.hpp>

struct SkinToneCalibrator {
    cv::Mat skin_tone_crop;

    cv::Mat apply(const cv::Mat &input);
    cv::Scalar calibrate();
};
