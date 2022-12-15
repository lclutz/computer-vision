#include <chrono>

#include <opencv2/videoio.hpp>

#include "logging.h"
#include "webcam.h"

std::future<cv::Mat>
Webcam::_read()
{
    return std::async(std::launch::async,
        [](cv::VideoCapture &cap) -> cv::Mat {
            cv::Mat result;
            cap.read(result);
            return result;
        },
        std::ref(this->cap)
    );
}

bool
Webcam::open(int id, int backend)
{
    cap.open(id, backend);

    bool camera_opened = cap.isOpened();
    if (camera_opened) {
        cap.set(cv::CAP_PROP_FPS, 30);
        next_frame = _read();

        std::thread(
            [](cv::VideoCapture &cam) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                double exposure = cam.get(cv::CAP_PROP_EXPOSURE);
                if (exposure != 0) {
                    if (cam.set(cv::CAP_PROP_EXPOSURE, exposure)) {
                        logi("Exposure fixed to %.3f", exposure);
                    } else {
                        logw("Failed to set the camera exposure manually");
                    }
                } else {
                    logw("Failed to read exposure property of camera");
                }
            },
            std::ref(cap)
        ).detach();
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
    next_frame = _read();
    return result;
}
