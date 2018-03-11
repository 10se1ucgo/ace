#include "util/event.h"

#include <algorithm>

#include "game_client.h"

namespace ace { namespace util {
    void TaskScheduler::update(double dt) {
        while(!this->tasks.empty() && this->client.time >= this->tasks.begin()->first) {
            this->tasks.begin()->second();
            tasks.erase(this->tasks.begin());
        }

        for (auto i = loops.begin(); i != loops.end();) {
            auto loop(i->lock());
            if (loop) {
                if (this->client.time >= loop->next_call) {
                    loop->func();
                    loop->next_call += loop->interval;
                }
                ++i;
            } else {
                i = loops.erase(i);
            }
        }
    }

    double TaskScheduler::get_time(double seconds) {
        return seconds + this->client.time;
    }
}}
