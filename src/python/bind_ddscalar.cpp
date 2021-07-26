#include "common.h"

#include <hyperjet.h>

template <hyperjet::index TOrder, typename TScalar, hyperjet::index TSize = hyperjet::Dynamic>
void bind_ddscalar(pybind11::module& m, const std::string& name)
{
    using namespace pybind11::literals;

    namespace py = pybind11;
    namespace hj = hyperjet;

    using Type = hyperjet::DDScalar<TOrder, TScalar, TSize>;

    auto py_class = py::class_<Type>(m, name.c_str())
        // constructor
        .def(py::init(py::overload_cast<TScalar, hj::index>(&Type::constant)), "f"_a=0, "size"_a)
        .def(py::init(py::overload_cast<const typename Type::Data&>(&Type::create)), "data"_a)
        // properties
        .def_property("f", py::overload_cast<>(&Type::f, py::const_), &Type::set_f)
        // static read-only properties
        .def_property_readonly_static("is_dynamic", [](py::object) { return Type::is_dynamic(); })
        .def_property_readonly_static("order", [](py::object) { return Type::order(); })
        // read-only properties
        .def_property_readonly("data", py::overload_cast<>(&Type::adata))
        .def_property_readonly("g", py::overload_cast<>(&Type::ag))
        .def_property_readonly("size", &Type::size)
        // static methods
        .def_static("constant", py::overload_cast<TScalar, hj::index>(&Type::constant), "f"_a, "size"_a)
        .def_static("constant", py::overload_cast<TScalar>(&Type::constant), "f"_a)
        .def_static("empty", py::overload_cast<>(&Type::empty))
        .def_static("empty", py::overload_cast<hj::index>(&Type::empty), "size"_a)
        .def_static("variable", py::overload_cast<hj::index, double, hj::index>(&Type::variable), "i"_a, "f"_a, "size"_a)
        .def_static("zero", py::overload_cast<>(&Type::zero))
        .def_static("zero", py::overload_cast<hj::index>(&Type::zero), "size"_a)
        // methods
        .def("__abs__", &Type::abs)
        .def("__len__", &Type::size)
        .def("__pow__", &Type::pow)
        .def("__repr__", &Type::to_string)
        .def("abs", &Type::abs)
        .def("eval", &Type::eval, "d"_a)
        // methods: arithmetic operations
        .def("reciprocal", &Type::reciprocal)
        .def("sqrt", &Type::sqrt)
        .def("cbrt", &Type::cbrt)
        .def("cos", &Type::cos)
        .def("sin", &Type::sin)
        .def("tan", &Type::tan)
        .def("acos", &Type::acos)
        .def("asin", &Type::asin)
        .def("atan", &Type::atan)
        .def("atan2", &Type::atan2)
        .def("arccos", &Type::acos)
        .def("arcsin", &Type::asin)
        .def("arctan", &Type::atan)
        .def("arctan2", &Type::atan2)
        .def("hypot", py::overload_cast<const Type&, const Type&>(&Type::hypot))
        .def("hypot", py::overload_cast<const Type&, const Type&, const Type&>(&Type::hypot))
        // methods: hyperbolic functions
        .def("cosh", &Type::cosh)
        .def("sinh", &Type::sinh)
        .def("tanh", &Type::tanh)
        .def("acosh", &Type::acosh)
        .def("arccosh", &Type::acosh)
        .def("asinh", &Type::asinh)
        .def("arcsinh", &Type::asinh)
        .def("atanh", &Type::atanh)
        .def("arctanh", &Type::atanh)
        // methods: exponents and logarithms
        .def("exp", &Type::exp)
        .def("log", py::overload_cast<>(&Type::log, py::const_))
        .def("log", py::overload_cast<TScalar>(&Type::log, py::const_), "base"_a)
        .def("log2", &Type::log2)
        .def("log10", &Type::log10)
        // operators
        .def(-py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def(py::self == double())
        .def(py::self != double())
        .def(py::self < double())
        .def(py::self > double())
        .def(py::self <= double())
        .def(py::self >= double())
        .def(double() == py::self)
        .def(double() != py::self)
        .def(double() < py::self)
        .def(double() > py::self)
        .def(double() <= py::self)
        .def(double() >= py::self)
        .def(py::self + py::self)
        .def(py::self + double())
        .def(py::self - py::self)
        .def(py::self - double())
        .def(py::self * py::self)
        .def(py::self * double())
        .def(py::self / py::self)
        .def(py::self / double())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= py::self)
        .def(py::self *= double())
        .def(py::self /= py::self)
        .def(py::self /= double())
        .def(double() + py::self)
        .def(double() - py::self)
        .def(double() * py::self)
        .def(double() / py::self)
        // serialization
        .def(py::pickle(
            [](const Type& self) {
                return py::make_tuple(self.m_data);
            },
            [](py::tuple tuple) {
                if (hyperjet::length(tuple) != 1) {
                    throw std::runtime_error("Invalid state!");
                }

                const auto data = tuple[0].cast<typename Type::Data>();

                if constexpr(Type::is_dynamic()) {
                    return Type(data, Type::size_from_data_length(hj::length(data)));
                } else {
                    return Type(data);
                }
            }))
        .def("__copy__", [](const Type& self) { return self; })
        .def("__deepcopy__", [](const Type& self, py::dict& memo) { return self; }, "memodict"_a);

    if constexpr(Type::order() == 1) {
        py_class
            // constructors
            .def(py::init(&Type::from_gradient), "f"_a, "g"_a);
    } else {
        py_class
            // constructors
            .def(py::init(&Type::from_arrays), "f"_a, "g"_a, "hm"_a)
            // methods
            .def("h", py::overload_cast<hj::index, hj::index>(&Type::h), "row"_a, "col"_a)
            .def("set_h", py::overload_cast<hj::index, hj::index, TScalar>(&Type::set_h), "row"_a, "col"_a, "value"_a)
            .def("hm", py::overload_cast<std::string>(&Type::hm, py::const_), "mode"_a="full")
            .def("set_hm", &Type::set_hm, "value"_a);
    }

    if constexpr(Type::is_dynamic()) {
        py_class
            // constructor
            .def(py::init(py::overload_cast<TScalar>(&Type::constant)), "f"_a=0)
            // methods
            .def("resize", &Type::resize, "size"_a)
            .def("pad_right", &Type::pad_right, "new_size"_a)
            .def("pad_left", &Type::pad_left, "new_size"_a)
            // static methods
            .def_static("variables", [](const std::vector<TScalar>& values) { return Type::variables(values); }, "values"_a);
    } else {
        py_class
            // constructor
            .def(py::init(py::overload_cast<TScalar>(&Type::constant)), "f"_a=0)
            // static methods
            .def_static("variable", py::overload_cast<hj::index, double>(&Type::variable), "i"_a, "f"_a)
            .def_static("variables", [](const std::array<TScalar, TSize>& values) { return Type::template variables<TSize>(values); }, "values"_a);
    }

    m.def("hypot", py::overload_cast<const Type&, const Type&>(&Type::hypot));
    m.def("hypot", py::overload_cast<const Type&, const Type&, const Type&>(&Type::hypot));
}

template void bind_ddscalar<1, double>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 1>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 2>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 3>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 4>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 5>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 6>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 7>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 8>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 9>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 10>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 11>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 12>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 13>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 14>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 15>(pybind11::module&, const std::string&);
template void bind_ddscalar<1, double, 16>(pybind11::module&, const std::string&);

template void bind_ddscalar<2, double>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 1>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 2>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 3>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 4>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 5>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 6>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 7>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 8>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 9>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 10>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 11>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 12>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 13>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 14>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 15>(pybind11::module&, const std::string&);
template void bind_ddscalar<2, double, 16>(pybind11::module&, const std::string&);