#pragma once

#include <string>

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
