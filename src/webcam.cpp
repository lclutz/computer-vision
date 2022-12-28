#include <thread>

#include "logging.h"
#include "webcam.h"

// Letting OpenCV decide the video capture api is really slow on Windows so we
// decide to use DirectShow by default instead of the automatic mode
#ifdef _WIN32
#define PLATFORM_DEFAULT_VIDEO_API cv::CAP_DSHOW
#else
#define PLATFORM_DEFAULT_VIDEO_API cv::CAP_ANY
#endif

Webcam::Webcam()
    : camera_id(0)
    , camera_api(PLATFORM_DEFAULT_VIDEO_API)
    , cap_lock(std::mutex())
    , cap(cv::VideoCapture())
{}

void
Webcam::handle_event(const Event &e)
{
    (void)e;
}

std::future<std::optional<cv::Mat>>
Webcam::_read()
{
    return std::async(std::launch::async,
        [](cv::VideoCapture &cap, std::mutex &cap_lock) -> std::optional<cv::Mat> {
            std::lock_guard<std::mutex> lock(cap_lock);
            if (cap.isOpened()) {
                cv::Mat result;
                cap.read(result);
                return std::optional<cv::Mat>(result);
            } else {
                return std::nullopt;
            }
        },
        std::ref(this->cap),
        std::ref(cap_lock)
    );
}

bool
Webcam::open()
{
    cap.open(camera_id, camera_api);

    bool camera_opened = cap.isOpened();
    if (camera_opened) {
        // cap.set(cv::CAP_PROP_FPS, 30);
        next_frame = _read();

        std::thread(
            [](cv::VideoCapture &cam, std::mutex &cap_lock) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                std::lock_guard<std::mutex> lock(cap_lock);
                if (cam.isOpened()) {
                    double exposure = cam.get(cv::CAP_PROP_EXPOSURE);
                    logi("Read exposure value: %.3f", exposure);

#if 0
                // Note: For Lenovo Carbon x1 3rd gen on Windows with the
                // DirectShow backend the values for the integrated webcam are
                // in the range of [-12.0, -3.0]
                exposure = -3.0;
#endif

                    if (exposure != 0) {
                        if (cam.set(cv::CAP_PROP_EXPOSURE, exposure)) {
                            logi("Exposure fixed to %.3f", exposure);
                        } else {
                            logw("Failed to set the camera exposure manually");
                        }
                    } else {
                        logw("Failed to read exposure property of camera");
                    }
                }
            },
            std::ref(cap),
            std::ref(cap_lock)
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
    std::lock_guard<std::mutex> lock(cap_lock);
    cap.release();
}

std::optional<cv::Mat>
Webcam::read()
{
    if (cap.isOpened()) {
        if (next_frame.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            auto result = next_frame.get();
            next_frame = _read();
            return result;
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}
