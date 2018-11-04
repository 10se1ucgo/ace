#include "bindings.h"
#include <pybind11/operators.h>
#include <pybind11/embed.h>
#include "glm/glm.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/type_trait.hpp> 
#include <glm/gtx/hash.hpp> 

namespace py = pybind11;
using namespace pybind11::literals;

template<typename T, glm::precision P>
void bind_mat_members(py::class_<glm::tmat4x4<T, P>> &cls) {
    using mat = glm::tmat4x4<T, P>;
    using col_type = typename mat::col_type;

    using T_CR = const T &;
    cls.def(py::init<T_CR>(), "x"_a = 1) // scalar constructor
        .def(py::init<
             T_CR, T_CR, T_CR, T_CR,
             T_CR, T_CR, T_CR, T_CR,
             T_CR, T_CR, T_CR, T_CR,
             T_CR, T_CR, T_CR, T_CR
        >())
        .def(py::init<const col_type &, const col_type &, const col_type &, const col_type &>())
        .def(py::init<const mat &>()); // copy constructor
     // TODO conversion to/from other mat types
}

template<typename T, glm::precision P>
void bind_mat_members(py::class_<glm::tmat3x3<T, P>> &cls) {
    using mat = glm::tmat3x3<T, P>;
    using col_type = typename mat::col_type;

    using T_CR = const T &;
    cls.def(py::init<T_CR>(), "x"_a = 1) // scalar constructor
        .def(py::init<
             T_CR, T_CR, T_CR,
             T_CR, T_CR, T_CR,
             T_CR, T_CR, T_CR
        >())
        .def(py::init<const col_type &, const col_type &, const col_type &>())
        .def(py::init<const mat &>()); // copy constructor
     // TODO conversion to/from other mat types
}

#define DEF_MAT_OPERATOR(type, op) \
    def( py::self op py::self ) \
    .def( py::self op type{} ) \
    .def( type{} op py::self ) \
    .def( py::self op##= py::self ) \
    .def( py::self op##= type{} )

#define DEF_MAT_VEC_OPERATOR(type, op) 

template<template<typename, glm::precision> class TMat, typename T, glm::precision P, typename = std::enable_if_t<glm::type<TMat, T, P>::is_mat>>
void bind_mat_operators_base(py::class_<TMat<T, P>> &cls) {
    using mat = TMat<T, P>;
    using col_type = typename mat::col_type;
    using row_type = typename mat::row_type;

    cls.DEF_MAT_OPERATOR(T, +)
        .DEF_MAT_OPERATOR(T, -)
        .DEF_MAT_OPERATOR(T, / )
        .DEF_MAT_OPERATOR(T, *);

    cls.def(py::self * row_type())
        .def(col_type() * py::self)
        .def(py::self / row_type())
        .def(col_type() / py::self);
}



#define DEF_VEC_MEMBER(type, member) \
    def_readwrite( #member, &type::member )

#define DEF_VEC_OPERATOR(type, op) \
    def( py::self op py::self ) \
    .def( py::self op type{} ) \
    .def( type{} op py::self ) \
    .def( py::self op##= py::self ) \
    .def( py::self op##= type{} )


template<typename T, glm::precision P>
void bind_vec_members(py::class_<glm::tvec2<T, P>> &cls) {
    using vec = glm::tvec2<T, P>;
    cls.def(py::init<T, T>(), "x"_a, "y"_a) // constructor
        .def(py::init<T>(), "x"_a = 0) // scalar constructor
        .def(py::init<const vec &>()) // copy constructor

        .DEF_VEC_MEMBER(vec, x)
        .DEF_VEC_MEMBER(vec, y)

        .DEF_VEC_MEMBER(vec, r)
        .DEF_VEC_MEMBER(vec, g)

        .DEF_VEC_MEMBER(vec, s)
        .DEF_VEC_MEMBER(vec, t);
}

template<typename T, glm::precision P>
void bind_vec_members(py::class_<glm::tvec3<T, P>> &cls) {
    using vec = glm::tvec3<T, P>;
    cls.def(py::init<T, T, T>(), "x"_a, "y"_a, "z"_a) // constructor
        .def(py::init<T>(), "x"_a = 0) // scalar constructor
        .def(py::init<const vec &>()) // copy constructor

        .DEF_VEC_MEMBER(vec, x)
        .DEF_VEC_MEMBER(vec, y)
        .DEF_VEC_MEMBER(vec, z)

        .DEF_VEC_MEMBER(vec, r)
        .DEF_VEC_MEMBER(vec, g)
        .DEF_VEC_MEMBER(vec, b)

        .DEF_VEC_MEMBER(vec, s)
        .DEF_VEC_MEMBER(vec, t)
        .DEF_VEC_MEMBER(vec, p);
}

template<typename T, glm::precision P>
void bind_vec_members(py::class_<glm::tvec4<T, P>> &cls) {
    using vec = glm::tvec4<T, P>;
    cls.def(py::init<T, T, T, T>(), "x"_a, "y"_a, "z"_a, "w"_a) // constructor
        .def(py::init<T>()) // scalar constructor
        .def(py::init<const vec &>()) // copy constructor

        .DEF_VEC_MEMBER(vec, x)
        .DEF_VEC_MEMBER(vec, y)
        .DEF_VEC_MEMBER(vec, z)
        .DEF_VEC_MEMBER(vec, w)

        .DEF_VEC_MEMBER(vec, r)
        .DEF_VEC_MEMBER(vec, g)
        .DEF_VEC_MEMBER(vec, b)
        .DEF_VEC_MEMBER(vec, a)

        .DEF_VEC_MEMBER(vec, s)
        .DEF_VEC_MEMBER(vec, t)
        .DEF_VEC_MEMBER(vec, p)
        .DEF_VEC_MEMBER(vec, q);
}

template<template<typename, glm::precision> class TVec, typename T, glm::precision P, typename = std::enable_if_t<glm::type<TVec, T, P>::is_vec>>
void bind_vec_operators_base(py::class_<TVec<T, P>> &cls) {
    using vec = TVec<T, P>;
    cls.DEF_VEC_OPERATOR(T, +)
        .DEF_VEC_OPERATOR(T, -)
        .DEF_VEC_OPERATOR(T, / )
        .DEF_VEC_OPERATOR(T, *);
}

template<template<typename, glm::precision> class TVec, typename T, glm::precision P>
std::enable_if_t<!std::is_integral<T>::value> bind_vec_operators(py::class_<TVec<T, P>> &cls, py::module &m) {
    bind_vec_operators_base(cls);

    // Geometric functions
    // m.def("cross", &glm::cross<T, P, TVec>, "x"_a, "y"_a, "Returns the cross product of x and y.");
    m.def("distance", &glm::distance<T, P, TVec>, "x"_a, "y"_a, "Returns the distance between x and y, i.e., length(x - y).", py::call_guard<py::gil_scoped_release>());
    m.def("dot", &glm::dot<T, P, TVec>, "x"_a, "y"_a, "Returns the dot product of x and y, i.e., result = x * y.", py::call_guard<py::gil_scoped_release>());
    m.def("length", &glm::length<T, P, TVec>, "x"_a, "Returns the length of x, i.e., sqrt(x * x).", py::call_guard<py::gil_scoped_release>());
    m.def("normalize", &glm::normalize<T, P, TVec>, "x"_a, "Returns a vector in the same direction as x but with length of 1.", py::call_guard<py::gil_scoped_release>());
}

template<template<typename, glm::precision> class TVec, typename T, glm::precision P>
std::enable_if_t<std::is_integral<T>::value> bind_vec_operators(py::class_<TVec<T, P>> &cls, py::module &m) {
    bind_vec_operators_base(cls);
    cls.DEF_VEC_OPERATOR(T, &)
        .DEF_VEC_OPERATOR(T, | )
        .DEF_VEC_OPERATOR(T, ^)
        .DEF_VEC_OPERATOR(T, << )
        .DEF_VEC_OPERATOR(T, >> );
}

template<typename T>
struct vec_binder { };


template<template<typename, glm::precision> class TVec, typename T, glm::precision P>
struct vec_binder<TVec<T, P>> {
    using vec_t = TVec<T, P>;
    using component_type = T;
    using index_t = typename vec_t::length_type;
    constexpr static auto precision = P;
    constexpr static auto components = glm::type<TVec, T, P>::components;

    static py::class_<TVec<T, P>> bind(py::module &m, const char *name) {
        py::class_<TVec<T, P>> vec(m, name, py::buffer_protocol());
        bind_vec_members(vec);
        bind_vec_operators(vec, m);

        vec.def("__getitem__", [](vec_t &self, index_t i) -> component_type& {
            if (i < 0 || i >= components) {
                throw py::index_error("vector index out of range");
            }
            return self[i];
        }, py::return_value_policy::reference_internal);
        vec.def("__setitem__", [](vec_t &self, index_t i, const component_type &v) {
            if (i < 0 || i >= components) {
                throw py::index_error("vector index out of range");
            }
            self[i] = v;
        });
        vec.def("__len__", [](const vec_t &) { return components; });
        vec.def("__repr__", &glm::to_string<TVec, T, P>);
        vec.def(hash(py::self));
        vec.def(py::self == py::self);
        vec.def(py::self != py::self);
        vec.def_buffer([](vec_t &self) {
            return py::buffer_info(
                &self[0],
                sizeof(component_type),
                py::format_descriptor<component_type>::format(), components);
        });

        // Common Functions
        m.def("min", py::overload_cast<const vec_t &, const vec_t &>(&glm::min<T, P, TVec>), "x"_a, "y"_a,
              "Returns y if x < y; otherwise, it returns x.", py::call_guard<py::gil_scoped_release>());
        m.def("min", py::overload_cast<const vec_t &, component_type>(&glm::min<T, P, TVec>), "x"_a, "y"_a,
              "Returns y if x < y; otherwise, it returns x.", py::call_guard<py::gil_scoped_release>());

        m.def("max", py::overload_cast<const vec_t &, const vec_t &>(&glm::max<T, P, TVec>), "x"_a, "y"_a,
              "Returns y if x < y; otherwise, it returns x.", py::call_guard<py::gil_scoped_release>());
        m.def("max", py::overload_cast<const vec_t &, component_type>(&glm::max<T, P, TVec>), "x"_a, "y"_a,
              "Returns y if x < y; otherwise, it returns x.", py::call_guard<py::gil_scoped_release>());



        return std::move(vec);
    }
};

// todo learn sfinae magic so mat_binder and vec_binder can just be binder and deduce type from the template arguments
template<typename T>
struct mat_binder { };



template<template<typename, glm::precision> class TMat, typename T, glm::precision P>
struct mat_binder<TMat<T, P>> {
    using mat_t = TMat<T, P>;
    using component_t = T;
    using col_t = typename mat_t::col_type;
    using row_t = typename mat_t::row_type;
    using index_t = typename mat_t::length_type;
    constexpr static auto precision = P;
    constexpr static auto cols = glm::type<TMat, T, P>::cols;
    constexpr static auto rows = glm::type<TMat, T, P>::rows;


    static py::class_<TMat<T, P>> bind(py::module &m, const char *name) {
        py::class_<TMat<T, P>> mat(m, name, py::buffer_protocol());
        bind_mat_members(mat);
        bind_mat_operators_base(mat);

        mat.def("__getitem__", [](mat_t &self, index_t i) -> col_t& {
            return self[i];
        }, py::return_value_policy::reference_internal);
        mat.def("__setitem__", [](mat_t &self, index_t i, const col_t &v) {
            self[i] = v;
        });
        mat.def("__getitem__", [](mat_t &self, std::pair<index_t, index_t> i) -> component_t& {
            return self[i.first][i.second];
        }, py::return_value_policy::reference_internal);
        mat.def("__setitem__", [](mat_t &self, std::pair<index_t, index_t> i, component_t v) {
            self[i.first][i.second] = v;
        });
        mat.def("__len__", [](const mat_t &) { return cols; });
        mat.def("__repr__", &glm::to_string<TMat, T, P>);
        mat.def(hash(py::self));
        mat.def_buffer([](mat_t &self) {
            return py::buffer_info(
                &self[0][0],
                sizeof(component_t),
                py::format_descriptor<component_t>::format(),
                2,
                { cols, rows },
                { sizeof(component_t), sizeof(component_t) * rows }
            );
        });

        return std::move(mat);
    }
};


#define BIND_VEC_TYPE(name) auto py_##name(vec_binder<glm::name>::bind(m, #name))
#define BIND_MAT_TYPE(name) auto py_##name(mat_binder<glm::name>::bind(m, #name))

// FROM t1 TO t2
#define IMPLICIT_CONVERSION(t1, t2) \
    py_##t2.def(py::init<const glm::t1 &>()); \
    py::implicitly_convertible<glm::t1, glm::t2>();

void bind_glm(py::module &a) {
    auto m(a.def_submodule("glm", "Python bindings to the OpenGL Mathematics library"));

    BIND_VEC_TYPE(vec2);
    BIND_VEC_TYPE(vec3);
    BIND_VEC_TYPE(vec4);
    BIND_VEC_TYPE(ivec2);
    BIND_VEC_TYPE(ivec3);
    BIND_VEC_TYPE(ivec4);
    BIND_VEC_TYPE(uvec2);
    BIND_VEC_TYPE(uvec3);
    BIND_VEC_TYPE(uvec4);

    // i am disgusted with myself.
    // IMPLICIT_CONVERSION(vec2, vec3);
    // IMPLICIT_CONVERSION(vec2, vec4);
    //
    // IMPLICIT_CONVERSION(vec3, vec4);
    
    // IMPLICIT_CONVERSION(ivec2, ivec3);
    // IMPLICIT_CONVERSION(ivec2, ivec4);
    IMPLICIT_CONVERSION(ivec2, vec2);
    // IMPLICIT_CONVERSION(ivec2, vec3);
    // IMPLICIT_CONVERSION(ivec2, vec4);
    
    // IMPLICIT_CONVERSION(ivec3, ivec4);
    IMPLICIT_CONVERSION(ivec3, vec3);
    // IMPLICIT_CONVERSION(ivec3, vec4);
    
    IMPLICIT_CONVERSION(ivec4, vec4);
    
    // IMPLICIT_CONVERSION(uvec2, uvec3);
    // IMPLICIT_CONVERSION(uvec2, uvec4);
    IMPLICIT_CONVERSION(uvec2, vec2);
    // IMPLICIT_CONVERSION(uvec2, vec3);
    // IMPLICIT_CONVERSION(uvec2, vec4);
    
    // IMPLICIT_CONVERSION(uvec3, uvec4);
    IMPLICIT_CONVERSION(uvec3, vec3);
    // IMPLICIT_CONVERSION(uvec3, vec4);
    
    IMPLICIT_CONVERSION(uvec4, vec4);

    BIND_VEC_TYPE(u8vec3);
    BIND_VEC_TYPE(u8vec4);

    // BIND_MAT_TYPE(mat2);
    BIND_MAT_TYPE(mat3);
    BIND_MAT_TYPE(mat4);
}