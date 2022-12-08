#pragma once

#include <string>

#include <opencv2/opencv.hpp>

#include "webcam.h"
#include "state.h"

struct Application {
    State state;

    int camera_id;
    int camera_api;
    Webcam webcam;

    std::string asset_folder_path;

    int fps;

    Application();
};
