#pragma once
#include "common.h"

#include <functional>
#include <utility>
#include <vector>
#include <chrono>
#include <future>


namespace ace { namespace util {
    struct TaskScheduler;

    struct Task {
        TaskScheduler *manager;

        explicit Task(TaskScheduler *manager, std::function<void(Task &)> func) :
            manager(manager),
            _function(std::move(func)) {
        }

        void keep_going() {
            this->repeat_in(this->_interval);
        }

        void repeat_in(double seconds);

        template<typename TRep, typename TPeriod>
        void repeat_in(std::chrono::duration<TRep, TPeriod> duration) {
            this->repeat_in(std::chrono::duration_cast<std::chrono::duration<double>>(duration).count());
        }

        uint64_t call_count() const { return this->_num_calls; }

        // Time since last call, in seconds.
        double dt() const;

        // Time since first call, in seconds.
        double time() const;

        Task &after(std::function<void(Task &)> func);
    private:
        std::function<void(Task &)> _function;
        double _interval = 0.0, _first_call = 0.0, _next_call = 0.0, _last_call = 0.0;
        uint64_t _num_calls = 0;
        bool _done = false;
        Task *_after = nullptr;

        bool call();

        friend TaskScheduler;
    };

    struct TaskScheduler {
        void update(double time, double dt) {
            this->time = time;

            this->tasks.reserve(this->tasks.size() + this->queued_tasks.size());
            while (!this->queued_tasks.empty()) {
                this->tasks.emplace_back(std::move(this->queued_tasks.back()));
                this->queued_tasks.pop_back();
            }
            
            for (auto i = this->tasks.begin(); i != this->tasks.end();) {
                if (this->time >= (*i)->_next_call && (*i)->call()) {
                    i = this->tasks.erase(i);
                } else {
                    ++i;
                }
            }
        }

        template<typename TTime, typename TFunc>
        Task &schedule(TTime time, TFunc&& func) {
            this->queued_tasks.emplace_back(std::make_unique<Task>(this, std::forward<TFunc>(func)));
            auto &task = *this->queued_tasks.back();
            task.repeat_in(time);
            return task;
        }

                // todo i need to figure this out
        // template<typename TReturn, typename TFunc>
        // std::future<TReturn> create_task(TFunc &&func) {
        //     using X = decltype([](int b) {
        //         return std::pow(b, b);
        //     });
        //     std::packaged_task<TReturn()> f(std::forward<TFunc>(func));
        //
        //
        //     std::thread thread(std::move(f));
        //
        //     return f.get_future();
        // }

        std::vector<std::unique_ptr<Task>> tasks, queued_tasks;
        double time;
    };
}}