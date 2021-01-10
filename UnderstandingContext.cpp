
#include "C:/Program Files (x86)/Microsoft Visual Studio/Shared/Python36_64/include/pybind11/pybind11.h"
#include "vector.h"

void AnalysePixels(std::vector< std::vector< std::vector<int> > > Pixels)
{
    
}

namespace py = pybind11;

PYBIND11_MODULE(KeyProcessor, m) {
	m.def("fast_tanh2", &tanh_impl, R"pbdoc(
        Compute a hyperbolic tangent of a single argument expressed in radians.
    )pbdoc");

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}