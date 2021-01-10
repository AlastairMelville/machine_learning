#include <Windows.h>
#include <cmath>
#include <iostream>

#include <pybind11/pybind11.h> // Use "pip install ." from the command line with the folder containing this file as thecurrent working directory.
#include <pybind11/stl.h>

#include "DetectSelfPositionAndRotation.h"
#include "source/Engine/classes/Engine/GameEngine.h"


struct FEngineLoop
{
	FEngineLoop()
		: GameEngine( std::make_unique<UGameEngine>() )
	{}

	void Main()
	{
		printf("start");

		static bool bHasInitialised = false;
		if (!bHasInitialised)
		{
			GameEngine->Init();
			bHasInitialised = true;
		}

		GameEngine->Tick(0.0f);

		printf("end");
	}

private:
	std::unique_ptr<UGameEngine> GameEngine;
};


namespace py = pybind11;

void Init_GameEngine(py::module&);
void Init_World(py::module&);
void Init_TickTaskManagerInterface(py::module&);

PYBIND11_MODULE(DetectSelfPositionAndRotation, m)
{
	Init_GameEngine(m);
	Init_World(m);
	Init_TickTaskManagerInterface(m);

	py::class_<FEngineLoop>(m, "FEngineLoop")
		.def(py::init<>())
		.def("Main", &FEngineLoop::Main);

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}
