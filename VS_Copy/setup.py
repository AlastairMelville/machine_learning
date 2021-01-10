import os, sys

from distutils.core import setup, Extension, DEBUG
from distutils import sysconfig

cpp_args = ['-Wall', '-O0', '-g', '-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7']

sfc_module = Extension(
    'DetectSelfPositionAndRotation', # needs to be the same as the name given in DetectSelfPositionAndRotation.cpp - PYBIND11_MODULE(DetectSelfPositionAndRotation, m)
    sources = ['DetectSelfPositionAndRotation.cpp', 'Module.cpp', 'source/Engine/private/Engine/GameEngine.cpp', 'source/Engine/private/Engine/World.cpp', 'source/Engine/private/Engine/TickTaskManager.cpp'],
    include_dirs=['pybind11/include', 'C:/Users/alime/OneDrive/My Documents/Python Projects/VisualStudio/PythonApp_PositionAndRotation/Cpp_PositionAndRotation'],
    language='c++',
    extra_compile_args = cpp_args,
    )

setup(
    name = 'DetectSelfPositionAndRotation',
    version = '1.0',
    description = 'Python package with Cpp_PositionAndRotation C++ extension (PyBind11)',
    ext_modules = [sfc_module],
)