#include "skin_tone_detector.h"

SkinToneDetector::SkinToneDetector()
    : skin_tone(cv::Scalar())
    , h_high_delta(0.0)
    , h_low_delta(0.0)
    , s_high_delta(0.0)
    , s_low_delta(0.0)
{}

SkinToneDetector::SkinToneDetector(cv::Scalar skin_tone)
    : skin_tone(skin_tone)
    , h_high_delta(2.0)
    , h_low_delta(10.0)
    , s_high_delta(70.0)
    , s_low_delta(20.0)
{}

cv::Mat
SkinToneDetector::apply(const cv::Mat input)
{
    cv::Mat result = cv::Mat(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    cv::cvtColor(input, result, cv::COLOR_BGR2HSV);

    cv::Scalar lower_bound(skin_tone[0] - h_low_delta,  skin_tone[1] - s_low_delta,    0.0);
    cv::Scalar upper_bound(skin_tone[0] + h_high_delta, skin_tone[1] + s_high_delta, 255.0);

    cv::inRange(result, lower_bound, upper_bound, result);

    cv::imshow("SkinToneDetector", result);

    return result;
}
