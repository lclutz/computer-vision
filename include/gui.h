#pragma once

#include "application.h"
#include "skin_tone_detector.h"

struct GUI {
    bool show_stats_window;
    bool show_settings_window;
    bool show_skin_tone_detector_settings;
    bool failed_to_open_camera;

    GUI();

    void render_stats(const Application &app);
    void render_settings(Application &app);
    void render_skin_tone_detector_settings(SkinToneDetector &skin_tone_detector);
    void render(Application &app, SkinToneDetector &skin_tone_detector);
};
