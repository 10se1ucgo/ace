#include "util/event.h"

#include <algorithm>
#include <utility>

#include "game_client.h"

namespace ace { namespace util {
    void Task::repeat_in(double seconds) {
        this->_interval = seconds;
        this->_next_call = seconds + this->manager->time;
        this->_done = false;
    }

    double Task::dt() const {
        if (this->call_count() == 0) return 0;
        return this->manager->time - this->_last_call;
    }

    double Task::time() const {
        return this->manager->time - this->_first_call;
    }

    Task &Task::after(std::function<void(Task &)> func) {
        this->_after = &this->manager->schedule(std::numeric_limits<double>::infinity(), func);
        return *this->_after;
    }

    bool Task::call() {
        if (this->call_count() == 0) {
            this->_first_call = this->manager->time;
        }

        this->_done = true;
        this->_function(std::ref(*this));
        this->_num_calls++;
        this->_last_call = this->manager->time;

        if(this->_done && this->_after != nullptr) {
            this->_after->repeat_in(0);
        }

        return this->_done;
    }
}}
