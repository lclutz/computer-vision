#pragma once

#include <deque>

#include "event.h"

struct MessageQueue {
    std::deque<Event> q;

    bool poll(Event *output)
    {
        if (q.empty()) { return false; }

        *output = q.front();
        q.pop_front();
        return true;
    }

    void push(const Event &e)
    {
        q.push_back(e);
    }

    MessageQueue(const MessageQueue &) = delete;
    MessageQueue &operator=(const MessageQueue &) = delete;
    MessageQueue(const MessageQueue &&) = delete;
    MessageQueue &operator=(const MessageQueue &&) = delete;

    static auto &instance() {
        static MessageQueue message_queue;
        return message_queue;
    }

private:
    MessageQueue() = default;
};
