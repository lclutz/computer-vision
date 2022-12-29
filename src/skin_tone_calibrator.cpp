#include "logging.h"
#include "skin_tone_calibrator.h"

static cv::Scalar
hsv_to_bgr(double h, double s, double v)
{
    cv::Mat rgb;
    cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(h, s, v));
    cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
    return cv::Scalar(rgb.data[0], rgb.data[1], rgb.data[2]);
}

cv::Mat
SkinToneCalibrator::apply(const cv::Mat &input)
{
    cv::Mat result = input.clone();

    int x = (input.cols / 6);
    int y = (input.rows / 6);
    int height = input.rows / 3;
    int width = height;

    cv::putText(
        result,
        "Press [Enter] to calibrate",
        cv::Point(x, y + width + 20), // top-left position
        cv::FONT_HERSHEY_DUPLEX,      // font face
        0.5,                          // font scale
        cv::Scalar(0, 0xff, 0)        // font color
    );
    cv::Rect rect(x, y, width, height);

    skin_tone_crop = input(rect);
    cv::rectangle(result, rect, cv::Scalar(0, 0xff, 0));

    return result;
}

bool
SkinToneCalibrator::calibrate()
{
    if (skin_tone_calibrated) {
        return false;
    }

    if (skin_tone_crop.empty()) {
        return false;
    }

    // Switch to HSV color space so we can ignore brightness/shadows
    cv::cvtColor(
        skin_tone_crop,
        skin_tone_crop,
        cv::COLOR_BGR2HSV
    );

    cv::Mat mask;
    cv::inRange(
        skin_tone_crop,
        cv::Scalar( 0.0,   0.0,   0.0),
        cv::Scalar(60.0, 255.0, 255.0),
        mask
    );

    cv::Scalar color = cv::mean(skin_tone_crop, mask);

    cv::Mat m(
        100,     // width
        100,     // height
        CV_8UC3, // type
        hsv_to_bgr(color[0], color[1], color[2])
    );

    logi("Skin tone: {h=%.3f, s=%.3f, v=%.3f}",
         color[0], color[1], color[2]);

    cv::namedWindow("Skin tone", cv::WINDOW_NORMAL);
    cv::imshow("Skin tone", m);

    skin_tone = color;
    skin_tone_calibrated = true;
    return true;
}
