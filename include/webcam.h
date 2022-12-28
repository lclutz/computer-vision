#pragma once

#include <future>
#include <mutex>
#include <optional>

#include <opencv2/opencv.hpp>

#include "event_receiver.h"

struct Webcam : public EventReceiver
{
    int camera_id;
    int camera_api;
    std::mutex cap_lock;
    cv::VideoCapture cap;
    std::future<std::optional<cv::Mat>> next_frame;

    std::future<std::optional<cv::Mat>> _read();

    bool open();
    bool is_open();
    void close();
    std::optional<cv::Mat> read();

    void handle_event(const Event &e) override;

    Webcam(const Webcam &) = delete;
    Webcam &operator=(const Webcam &) = delete;
    Webcam(const Webcam &&) = delete;
    Webcam &operator=(const Webcam &&) = delete;

    static auto &instance() {
        static Webcam webcam;
        return webcam;
    }

private:
    Webcam();
};
