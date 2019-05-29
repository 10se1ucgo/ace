#pragma once
#include "pybind11/pybind11.h"

// enum module bindings largely stolen from https://github.com/pybind/pybind11/pull/781
template<typename T>
struct pyenum_info {
    pybind11::handle type;
    std::unordered_map<T, pybind11::handle> enum_cache;

    pyenum_info() = default;

    pyenum_info(pybind11::handle type, const pybind11::dict &values) : type(type) {
        for (auto item : values) {
            auto t = item.second.cast<T>();
            this->enum_cache[t] = this->type(t);
        }
    }

    pybind11::handle get(T v) {
        auto it = this->enum_cache.find(v);
        if (it == this->enum_cache.end()) {
            try {
                // not cached
                return this->enum_cache.emplace(v, this->type(v)).first->second;
            } catch (pybind11::error_already_set &) {
                return {};
            }
        }
        return it->second;
    }
};

namespace pybind11 {
    namespace detail {
        template<typename T>
        class type_caster<T, std::enable_if_t<std::is_enum<T>::value>> {
            using underlying_t = std::underlying_type_t<T>;
            using base_caster = type_caster_base<T>;

            base_caster caster;

            static std::unique_ptr<pyenum_info<underlying_t>> e_info;

            T value;
        public:
            template<typename U> using cast_op_type = cast_op_type<U>;
            
            operator T*() { return e_info ? &this->value : static_cast<T*>(this->caster); }
            operator T&() { return e_info ? this->value : static_cast<T&>(this->caster); }

            static handle cast(const T &src, return_value_policy rvp, handle parent) {
                if (e_info) {
                    return e_info->get(static_cast<underlying_t>(src)).inc_ref();
                }
                return base_caster::cast(src, rvp, parent);
            }

            bool load(handle src, bool convert) {
                if (!src)
                    return false;
                if (e_info) {
                    if (!isinstance(src, e_info->type))
                        return false;
                    this->value = static_cast<T>(src.cast<underlying_t>());
                    return true;
                }
                return this->caster.load(src, convert);
            }

            static PYBIND11_DESCR name() {
                return base_caster::name();
            }

            static void bind(handle type, const dict &values) {
                e_info = std::make_unique<pyenum_info<underlying_t>>(type, values);
            }
        };

        template<typename T>
        std::unique_ptr<pyenum_info<std::underlying_type_t<T>>> type_caster<T, std::enable_if_t<std::is_enum<T>::value>>::e_info{ nullptr };
    }
}

template<typename T>
struct pyenum34 {
    using underlying_t = std::underlying_type_t<T>;

    pyenum34(pybind11::handle scope, const char *name, bool flag = false) :
        scope(scope),
        name(name),
        enum_type(pybind11::module::import("enum").attr(flag ? "IntFlag" : "IntEnum")),
        unique(pybind11::module::import("enum").attr("unique")) {

    }

    pyenum34 &value(const char *name, T value) {
        this->entries[pybind11::str(name)] = pybind11::cast(static_cast<underlying_t>(value), pybind11::return_value_policy::copy);
        return *this;
    }

    void finish() {
        pybind11::object t = this->unique(this->enum_type(this->name, this->entries));
        pybind11::setattr(this->scope, this->name, t);
        pybind11::detail::type_caster<T>::bind(t, entries);
    }
private:
    pybind11::handle scope;
    const char *name;
    pybind11::dict entries;
    pybind11::object enum_type, unique;
};


void bind_sdl(pybind11::module &);
void bind_gl(pybind11::module &);
void bind_glm(pybind11::module &);
void bind_font(pybind11::module &);
void bind_draw(pybind11::module &);
void bind_map(pybind11::module &);
void bind_map_editor(pybind11::module &);
void bind_client(pybind11::module &);