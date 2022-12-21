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
            output.convertTo(output, CV_8UC1, 0xff);

            cv::threshold(
                output,           // srv
                output,           // dst
                0x10/255.0,       // thresh
                0xff/255.0,       // maxval
                cv::THRESH_BINARY // type
            );
        } break;

        case BG_ESTIMATOR_HUE: {
            title = "Background Estimator (Color)";

            cv::Mat hsv;
            cv::cvtColor(input, hsv, cv::COLOR_BGR2HSV);
            hsv.convertTo(hsv, CV_32FC3, 1.0/255.0);

            if (model.empty()) { model = hsv.clone(); }

            cv::absdiff(hsv, model, output);

            cv::threshold(
                output,           // srv
                output,           // dst
                0x10/255.0,       // thresh
                0xff/255.0,       // maxval
                cv::THRESH_BINARY // type
            );
        } break;

        default: {
            assert(false && "UNREACHABLE");
        } break;
    }

    cv::accumulateWeighted(input, model, alpha);

    cv::imshow(title, output);

    return output;
}
