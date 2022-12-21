#pragma once

#include <future>

#include <opencv2/opencv.hpp>

struct Webcam {
    cv::VideoCapture cap;
    std::future<cv::Mat> next_frame;

    std::future<cv::Mat> _read();

    bool open(int id, int backend);
    bool is_open();
    void close();
    bool new_frame_available();
    cv::Mat read();
};
