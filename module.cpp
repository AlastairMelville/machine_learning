//#include <pybind11/pybind11.h>
#include "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python36_64/include/pybind11/pybind11.h"
//#include "C:/Users/alime/Anaconda3/envs/tfdeeplearning2/include/pybind11/pybind11.h"

#include <Windows.h>
#include <cmath>

// Source: https://docs.microsoft.com/en-us/visualstudio/python/working-with-c-cpp-python-in-visual-studio?view=vs-2019

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

int GetInt(int InValue)
{
	InValue *= 2;
	InValue *= 3;
	InValue *= 4;
	return InValue;
}

namespace py = pybind11;

PYBIND11_MODULE(module, m) {
	m.def("fast_tanh2", &tanh_impl, R"pbdoc(
        Compute a hyperbolic tangent of a single argument expressed in radians.
    )pbdoc");

	m.def("GetInt", &GetInt, R"pbdoc(
        Compute a hyperbolic tangent of a single argument expressed in radians.
    )pbdoc");

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}

//#include <pybind11/pybind11.h>
//#include "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python36_64/include/pybind11/pybind11.h"

//int add(int i, int j) {
//	return i + j;
//}
//
//PYBIND11_MODULE(example, m) {
//	m.doc() = "pybind11 example plugin"; // optional module docstring
//
//	m.def("add", &add, "A function which adds two numbers");
//}