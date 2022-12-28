#pragma once

enum EventType {
    EVENT_UNINITIALIZED,
    EVENT_TOGGLE_SETTINGS_WINDOW_VISIBILITY,
    EVENT_TOGGLE_STATS_WINDOW_VISIBILITY,
    EVENT_CALIBRATE_SKIN_TONE,

    EVENT_CAMERA_OPENED,
    EVENT_CAMERA_CLOSED,

    EVENT_START,

    EVENT_COUNT // Must be last
};

struct Event {
    EventType type;

    Event()
        : type(EVENT_UNINITIALIZED)
    {}

    Event(EventType type)
        : type(type)
    {}
};
