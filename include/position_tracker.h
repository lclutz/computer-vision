#pragma once

#include <opencv2/opencv.hpp>

struct PositionTracker {
    bool last_position_initialized;
    cv::Vec2i last_position;
    cv::Vec2i v;

    PositionTracker();

    cv::Vec2i predict(cv::Vec2i current_position, int delta_time);
};
