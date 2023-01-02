#pragma once

#include "event_receiver.h"

struct GUI : public EventReceiver
{
    bool show_stats_window;
    bool show_settings_window;
    bool show_skin_tone_detector_settings;
    bool failed_to_open_camera;

    void render_stats();
    void render_settings();
    void render();

    void handle_event(const Event &e) override;

    GUI(const GUI &) = delete;
    GUI &operator=(const GUI &) = delete;
    GUI(const GUI &&) = delete;
    GUI &operator=(const GUI &&) = delete;

    static auto &instance() {
        static GUI gui;
        return gui;
    }

private:
    GUI();
};
