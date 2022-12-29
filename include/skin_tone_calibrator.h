#pragma once

#include <opencv2/opencv.hpp>

struct SkinToneCalibrator {
    bool skin_tone_calibrated;

    cv::Mat skin_tone_crop;
    cv::Scalar skin_tone;

    cv::Mat apply(const cv::Mat &input);
    bool calibrate();

    SkinToneCalibrator()
        : skin_tone_calibrated(false)
        , skin_tone_crop(cv::Mat())
        , skin_tone(cv::Scalar())
    {}
};
