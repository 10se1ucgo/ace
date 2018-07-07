#pragma once
#include <map>
#include <functional>
#include <utility>
#include <vector>
#include <memory>
#include <chrono>

namespace ace { 
class GameClient;

namespace util {
    namespace detail {
        struct LoopingCall {
            LoopingCall(double interval, std::function<void()> func, double next_call):
                interval(interval),
                func(std::move(func)),
                next_call(next_call) {
            }

            double interval;
            std::function<void()> func;
            double next_call;
        };
    }

    // wow im bad at designing anything
    class TaskScheduler {
        std::multimap<double, std::function<void()>> tasks;
        std::vector<std::weak_ptr<detail::LoopingCall>> loops;
        GameClient &client;

    public:
        using task_type = decltype(tasks)::iterator;
        using loop_type = std::shared_ptr<decltype(loops)::value_type::element_type>;

        explicit TaskScheduler(GameClient& client): client(client) {}

        void update(double dt);

        template<typename TFunc, typename... TArgs>
        task_type call_later(double seconds, TFunc&& func, TArgs&&... args) {
            return tasks.emplace(this->get_time(seconds), std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
        }

        template<typename Rep, typename Period, typename TFunc, typename... TArgs>
        task_type call_later(std::chrono::duration<Rep, Period> time, TFunc&& func, TArgs&&... args) {
            return tasks.emplace(this->get_time(std::chrono::duration_cast<std::chrono::duration<double>>(time).count()), std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
        }

        template<typename TFunc, typename... TArgs>
        loop_type call_every(double interval, bool now, TFunc&& func, TArgs&&... args) {
            loop_type ptr(std::make_shared<detail::LoopingCall>(interval, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...), now ? 0.0 : this->get_time(interval)));
            this->loops.emplace_back(ptr);
            return ptr;
        }

        double get_time(double seconds);
    };
}}
