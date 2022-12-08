#include "hand_detector.h"

HandDetector::HandDetector()
    : skin_tone(cv::Scalar())
    , h_high_delta(0.0)
    , h_low_delta(0.0)
    , s_high_delta(0.0)
    , s_low_delta(0.0)
    , masks(std::deque<cv::Mat>())
{}

HandDetector::HandDetector(cv::Scalar skin_tone)
    : skin_tone(skin_tone)
    , h_high_delta(2.0)
    , h_low_delta(10.0)
    , s_high_delta(70.0)
    , s_low_delta(20.0)
    , masks(std::deque<cv::Mat>())
{}

cv::Mat
HandDetector::apply(const cv::Mat &input)
{
    cv::Mat result;
    cv::cvtColor(input, result, cv::COLOR_BGR2HSV);

    cv::Scalar lower_bound(skin_tone[0] - h_low_delta,  skin_tone[1] - s_low_delta,    0.0);
    cv::Scalar upper_bound(skin_tone[0] + h_high_delta, skin_tone[1] + s_high_delta, 255.0);

    cv::inRange(result, lower_bound, upper_bound, result);

    cv::Mat input_mask = result.clone();
    for (auto &mask : masks) { result &= mask; }

    masks.push_back(input_mask);
    while (masks.size() > 3) { masks.pop_front(); }

#if 0
    cv::erode(
        result,
        result,
        cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3))
    );
#endif

#if 0
    cv::dilate(
        result,
        result,
        cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3))
    );
#endif

    cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    return result;
}
