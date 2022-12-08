#include "application.h"

// Letting OpenCV decide the video capture api is really slow on Windows so we
// decide to use DirectShow by default instead of the automatic mode
#ifdef _WIN32
#define PLATFORM_DEFAULT_VIDEO_API cv::CAP_DSHOW
#else
#define PLATFORM_DEFAULT_VIDEO_API cv::CAP_ANY
#endif

Application::Application()
    : state(INIT)
    , camera_id(0)
    , camera_api(PLATFORM_DEFAULT_VIDEO_API)
    , webcam(Webcam())
    , asset_folder_path(std::string())
    , fps(0)
{}
