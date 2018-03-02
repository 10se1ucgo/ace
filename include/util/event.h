#pragma once
#include <map>
#include <functional>
#include <utility>
#include <vector>
#include <memory>

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

            bool operator==(const LoopingCall &other) const {
                return interval == other.interval && next_call == other.next_call && func.target_type() == other.func.target_type();
            }
        };


    }

    // wow im bad at designing anything
    class TaskScheduler {
        std::map<double, std::function<void()>> tasks;
        std::vector<std::weak_ptr<detail::LoopingCall>> loops;
        GameClient &client;

    public:
        using task_type = decltype(tasks)::iterator;
        using loop_type = std::shared_ptr<decltype(loops)::value_type::element_type>;

        explicit TaskScheduler(GameClient& client): client(client) {}

        void update(double dt);

        task_type call_later(double seconds, std::function<void()> &&func) {
            return tasks.emplace(this->get_time(seconds), func).first;
            
        }

        template<typename TFunc, typename... TArgs>
        task_type call_later(double seconds, TFunc&& func, TArgs&&... args) {
            // YIKES visual studio just hangs when i try to compile this
            // return this->call_later(seconds, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
            return tasks.emplace(this->get_time(seconds), std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...)).first;
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
