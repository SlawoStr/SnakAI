#include <pybind11/pybind11.h>
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <pybind11/stl.h>
#include "Simulator.h"

namespace py = pybind11;

const double e = 2.7182818284590452353602874713527;

double sinh_impl(double x) {
    return (1 - pow(e, (-2 * x))) / (2 * pow(e, -x));
}

double cosh_impl(double x) {
    return (1 + pow(e, (-2 * x))) / (2 * pow(e, -x));
}

double tanh_impl(double x) {
    return sinh_impl(x) / cosh_impl(x);
}

struct ClassTest {
    ClassTest(const std::string& name) : name(name) { for (int i = 0; i < 1000; i++) tab[i] = 0; }
    void setName(const std::string& name_) { name = name_; }
    const std::string& getName() const { return name; }
    std::string name;
    int tab[1000];
};

struct ClassTest2 {
    ClassTest2(const std::string& name) : name(name) { for (int i = 0; i < 1000; i++) tab[i] = 0; }
    void setName(const std::string& name_) { name = name_; }
    const std::string& getName() const { return name; }
    void printList(std::vector<int>vecValues) { for (int i = 0; i < vecValues.size(); i++) std::cout << vecValues[i] << std::endl; }
    std::string name;
    int tab[1000];
};


PYBIND11_MODULE(Simulation, m) {
    py::class_<ClassTest>(m, "ClassTest")
        .def(py::init<const std::string&>())
        .def("setName", &ClassTest::setName)
        .def("getName", &ClassTest::getName);

    py::class_<ClassTest2>(m, "ClassTest2")
        .def(py::init<const std::string&>())
        .def("setName", &ClassTest2::setName)
        .def("getName", &ClassTest2::getName)
        .def("printList", &ClassTest2::printList)
        .def_readwrite("name", &ClassTest2::name);

    // define Simulation2 class (make this class and its functions visible after importing) 
    py::class_<Simulation2>(m, "Simulation2")
        .def(py::init<>())
        .def("isAlive", &Simulation2::isAlive)
        .def("makeMove", &Simulation2::makeMove)
        .def("getFiness", &Simulation2::getFitness)
        .def("getVision", &Simulation2::getVision)
        .def("resetPopulation", &Simulation2::resetPopulation);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}