#pragma once

#include <string>

#include "event.h"
#include "webcam.h"
#include "state.h"

struct Application {
    bool should_close;
    int fps;

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;
    Application(const Application &&) = delete;
    Application &operator=(const Application &&) = delete;

    static auto &instance() {
        static Application app;
        return app;
    }

private:
    Application();
};
