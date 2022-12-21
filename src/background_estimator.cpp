#include <assert.h>

#include <string>

#include "background_estimator.h"

BackgroundEstimator::BackgroundEstimator(EstimationType type)
    : type(type)
{}

cv::Mat
BackgroundEstimator::apply(const cv::Mat input, double alpha)
{
    cv::Mat model_input, output;
    std::string title;

    switch (type)
    {
        case BG_ESTIMATOR_BRIGHTNESS: {
            title = "Background Estimator (Brightness)";

            cv::Mat gray;
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
            gray.convertTo(gray, CV_32FC1, 1.0/0xff);

            if (model.empty()) { model = gray.clone(); }

            cv::absdiff(gray, model, output);

            cv::accumulateWeighted(gray, model, alpha);

            output.convertTo(output, CV_8UC1, 0xff/1.0);
            cv::threshold(
                output,           // srv
                output,           // dst
                0x10,             // thresh
                0xff,             // maxval
                cv::THRESH_BINARY // type
            );
        } break;

        case BG_ESTIMATOR_HUE: {
            title = "Background Estimator (Color)";

            cv::Mat hsv;
            cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);

            std::vector<cv::Mat> channels;
            cv::split(hsv, channels);

            // Note: For HSV, hue range is [0,179], saturation range is [0,255],
            // and value range is [0,255]
            cv::Mat hue = channels[0];
            hue.convertTo(hue, CV_32FC1, 1.0/179.0);

            cv::GaussianBlur(hue, hue, cv::Size(21, 21), 0);

            if (model.empty()) { model = hue.clone(); }

            cv::absdiff(hue, model, output);

            cv::accumulateWeighted(hue, model, alpha);

            output.convertTo(output, CV_8UC1, 0xff/1.0);
            cv::threshold(
                output,     // srv
                output,     // dst
                0x10,       // thresh
                0xff,       // maxval
                cv::THRESH_BINARY // type
            );
        } break;

        default: {
            assert(false && "UNREACHABLE");
        } break;
    }

    cv::imshow(title, output);

    return output;
}
