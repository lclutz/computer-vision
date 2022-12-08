#include <chrono>

#include <opencv2/videoio.hpp>

#include "webcam.h"

static cv::Mat
_read(cv::VideoCapture *cap)
{
    cv::Mat result;
    cap->read(result);
    return result;
}

bool
Webcam::open(int id, int backend)
{
    cap.open(id, backend);

    bool camera_opened = cap.isOpened();
    if (camera_opened) {
        cap.set(cv::CAP_PROP_FPS, 30);
        next_frame = std::async(std::launch::async, _read, &cap);
    }

    return camera_opened;
}

bool
Webcam::is_open()
{
    return cap.isOpened();
}

void
Webcam::close()
{
    cap.release();
}

bool
Webcam::new_frame_available()
{
    return next_frame.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

cv::Mat
Webcam::read()
{
    cv::Mat result = next_frame.get();
    next_frame = std::async(std::launch::async, _read, &cap);
    return result;
}
