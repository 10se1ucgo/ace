#include "util/event.h"

#include <algorithm>
#include <utility>

#include "game_client.h"

namespace ace { namespace util {
    double TaskScheduler::get_time() {
        return this->client.time;
    }

    void Task::call_in(double seconds) {
        this->_interval = seconds;
        this->_next_call = seconds + this->manager->get_time();
        this->_done = false;
    }

    double Task::dt() const {
        if (this->call_count() == 0) return 0;
        return this->manager->get_time() - this->_last_call;
    }

    double Task::time() const {
        return this->manager->get_time() - this->_first_call;
    }

    bool Task::call() {
        if (this->call_count() == 0) {
            this->_first_call = this->manager->get_time();
        }

        this->_done = true;
        this->function(std::ref(*this));
        this->_num_calls++;
        this->_last_call = this->manager->get_time();
        return this->_done;
    }
}}
