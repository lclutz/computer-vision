#include "position_tracker.h"

PositionTracker::PositionTracker()
    : last_position_initialized(false)
    , last_position(cv::Vec2i())
    , v(cv::Vec2i())
{}

cv::Vec2i
PositionTracker::predict(cv::Vec2i current_position, int delta_time)
{
    if (!last_position_initialized) {
        last_position = current_position;
        last_position_initialized = true;
        return current_position;
    }

    v = (current_position - last_position) / delta_time;

    cv::Vec2i next_position = current_position + v * delta_time;

    return next_position;
}
