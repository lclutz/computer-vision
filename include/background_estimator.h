#pragma once

#include <deque>

#include <opencv2/opencv.hpp>

struct BackgroundEstimator {
    cv::Mat model;

    BackgroundEstimator();

    cv::Mat apply(cv::Mat input, float alpha);
};
