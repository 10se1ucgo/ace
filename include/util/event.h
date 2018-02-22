#pragma once
#include <map>
#include <functional>
#include <utility>
#include <list>

namespace ace { 
class GameClient;

namespace util {
    namespace detail {
        struct LoopingCall {
            LoopingCall(double interval, std::function<void()> func, double next_call):
                interval(interval),
                func(std::move(func)),
                next_call(next_call) {
            };

            double interval;
            std::function<void()> func;
            double next_call;

            bool operator==(const LoopingCall &other) const {
                return interval == other.interval && next_call == other.next_call && func.target_type() == other.func.target_type();
            }
        };


    }

    using tasks_t = std::map<double, std::function<void()>>;
    using loops_t = std::list<detail::LoopingCall>;

    // wow im bad at designing anything
    struct TaskScheduler {
        explicit TaskScheduler(GameClient& client): client(client) {}

        void update(double dt);

        tasks_t::iterator call_later(double seconds, std::function<void()> &&func) {
            return tasks.emplace(this->get_time(seconds), func).first;
            
        }

        template<typename TFunc, typename... TArgs>
        tasks_t::iterator call_later(double seconds, TFunc&& func, TArgs&&... args) {
            // YIKES visual studio just hangs when i try to compile this
            // return this->call_later(seconds, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
            return tasks.emplace(this->get_time(seconds), std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...)).first;
        }

        loops_t::reference call_every(double interval, bool now, std::function<void()>&& func) {
            loops.emplace_back(interval, func, now ? 0.0 : this->get_time(interval));
            return loops.back();
        }

        template<typename TFunc, typename... TArgs>
        loops_t::reference call_every(double interval, bool now, TFunc&& func, TArgs&&... args) {
            // this too
            // i guess it doesnt like the variadic template
            //return this->call_every(interval, now, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...));
            loops.emplace_back(interval, std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...), now ? 0.0 : this->get_time(interval));
            return loops.back();
        }

        void remove_loop(loops_t::reference loop) {
            loops.remove(loop);
        }

        double get_time(double seconds);

    private:
        tasks_t tasks;
        loops_t loops;
        GameClient &client;
    };
}}
