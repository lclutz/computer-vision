#pragma once

#include "application.h"
#include "hand_detector.h"

struct GUI {
    bool show_stats_window;
    bool show_settings_window;
    bool show_hand_detector_settings;
    bool failed_to_open_camera;

    GUI();

    void render_stats(const Application &app);
    void render_settings(Application &app);
    void render_hand_detector_settings(HandDetector &hand_detector);
    void render(Application &app, HandDetector &hand_detector);
};
