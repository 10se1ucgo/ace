#include "util/event.h"
#include "game_client.h"
#include <algorithm>

namespace ace { namespace util {
    void TaskScheduler::update(double dt) {
        while(!this->tasks.empty() && this->client.time >= this->tasks.begin()->first) {
            this->tasks.begin()->second();
            tasks.erase(this->tasks.begin());
        }
        for(auto &loop : loops) {
            if(this->client.time >= loop.next_call) {
                loop.func();
                loop.next_call += loop.interval;
            }
        }
    }

    double TaskScheduler::get_time(double seconds) {
        return seconds + this->client.time;
    }
}}
