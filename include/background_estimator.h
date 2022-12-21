#pragma once

#include <deque>

#include <opencv2/opencv.hpp>

enum EstimationType {
    BG_ESTIMATOR_BRIGHTNESS,
    BG_ESTIMATOR_HUE
};

struct BackgroundEstimator {
    EstimationType type;
    cv::Mat model;

    BackgroundEstimator(EstimationType type);

    cv::Mat apply(const cv::Mat input, double alpha);
};
