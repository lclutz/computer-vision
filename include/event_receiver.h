#pragma once

#include "event.h"

struct EventReceiver {

    virtual void handle_event(const Event &e) = 0;

};
