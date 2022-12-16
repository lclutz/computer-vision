#include "background_estimator.h"


BackgroundEstimator::BackgroundEstimator()
{}

cv::Mat
BackgroundEstimator::apply(cv::Mat input, float alpha)
{
#if 0
    cv::Mat gray;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    gray.convertTo(gray, CV_32FC1, 1.0/255.0);

    // cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);


    cv::Mat new_model;
    new_model = model * (1.0f - alpha) + alpha * input;

    cv::Mat tmp;
    new_model.convertTo(tmp, CV_8UC1, 255.0);
#endif

    cv::cvtColor(input, input, cv::COLOR_BGR2GRAY);
    input.convertTo(input, CV_32FC1, 1.0/255.0);
    if (model.empty()) { model = input.clone(); }

    cv::Mat output;

    cv::absdiff(input, model, output);

    cv::threshold(
        output,           // srv
        output,           // dst
        0x10/255.0,             // thresh
        0xff/255.0,             // maxval
        cv::THRESH_BINARY // type
    );

    cv::accumulateWeighted(input, model, alpha);

    cv::imshow("Background Estimator", output);
    cv::imshow("Model", model);

    return model.clone();
}
