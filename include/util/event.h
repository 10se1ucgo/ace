#pragma once
#include "common.h"

#include <functional>
#include <utility>
#include <vector>
#include <chrono>


namespace ace { 
class GameClient;

namespace util {
    struct TaskScheduler;

    struct Task {
        TaskScheduler *manager;

        explicit Task(TaskScheduler *manager, std::function<void(Task &)> func) :
            manager(manager),
            function(std::move(func)) {
        }

        void keep_going() {
            this->call_in(this->_interval);
        }

        template<typename TTime>
        void keep_going(TTime time) {
            this->call_in(time);
        }

        void call_in(double seconds);

        template<typename TRep, typename TPeriod>
        void call_in(std::chrono::duration<TRep, TPeriod> duration) {
            this->call_in(std::chrono::duration_cast<std::chrono::duration<double>>(duration).count());
        }

        uint64_t call_count() const { return this->_num_calls; }

        double dt() const;
    private:
        std::function<void(Task&)> function;
        double _interval = 0.0, _next_call = 0.0, _last_call = 0.0;
        uint64_t _num_calls = 0;
        bool _done = false;

        bool call();

        friend TaskScheduler;
    };

    struct TaskScheduler {
        GameClient &client;

        std::vector<Task> tasks;
        std::vector<Task> persistent_tasks;


        explicit TaskScheduler(GameClient& client) : client(client) { }

        void update(double dt) {
            for (auto i = this->persistent_tasks.begin(); i != this->persistent_tasks.end();) {
                if (this->get_time() >= i->_next_call && i->call()) {
                    i = this->persistent_tasks.erase(i);
                } else {
                    ++i;
                }
            }

            for (auto i = this->tasks.begin(); i != this->tasks.end();) {
                if (this->get_time() >= i->_next_call && i->call()) {
                    i = this->tasks.erase(i);
                } else {
                    ++i;
                }
            }
        }

        template<typename TTime, typename TFunc>
        Task &schedule(TTime time, TFunc&& func) {
            this->tasks.emplace_back(this, std::forward<TFunc>(func));
            auto &task = this->tasks.back();
            task.call_in(time);
            return task;
        }

        template<typename TTime, typename TFunc>
        Task &schedule_persistent(TTime time, TFunc&& func) {
            this->persistent_tasks.emplace_back(this, std::forward<TFunc>(func));
            auto &task = this->persistent_tasks.back();
            task.call_in(time);
            return task;
        }

        double get_time();
    };
}}
