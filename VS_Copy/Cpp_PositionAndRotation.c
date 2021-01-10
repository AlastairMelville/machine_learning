#include <Python.h>

/*
 * Implements an example function.
 */
PyDoc_STRVAR(Cpp_PositionAndRotation_example_doc, "example(obj, number)\
\
Example function");

PyObject *Cpp_PositionAndRotation_example(PyObject *self, PyObject *args, PyObject *kwargs) {
    /* Shared references that do not need Py_DECREF before returning. */
    PyObject *obj = NULL;
    int number = 0;

    /* Parse positional and keyword arguments */
    static char* keywords[] = { "obj", "number", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &obj, &number)) {
        return NULL;
    }

    /* Function implementation starts here */

    if (number < 0) {
        PyErr_SetObject(PyExc_ValueError, obj);
        return NULL;    /* return NULL indicates error */
    }

    Py_RETURN_NONE;
}

/*
 * List of functions to add to Cpp_PositionAndRotation in exec_Cpp_PositionAndRotation().
 */
static PyMethodDef Cpp_PositionAndRotation_functions[] = {
    { "example", (PyCFunction)Cpp_PositionAndRotation_example, METH_VARARGS | METH_KEYWORDS, Cpp_PositionAndRotation_example_doc },
    { NULL, NULL, 0, NULL } /* marks end of array */
};

/*
 * Initialize Cpp_PositionAndRotation. May be called multiple times, so avoid
 * using static state.
 */
int exec_Cpp_PositionAndRotation(PyObject *module) {
    PyModule_AddFunctions(module, Cpp_PositionAndRotation_functions);

    PyModule_AddStringConstant(module, "__author__", "alime");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");
    PyModule_AddIntConstant(module, "year", 2019);

    return 0; /* success */
}

/*
 * Documentation for Cpp_PositionAndRotation.
 */
PyDoc_STRVAR(Cpp_PositionAndRotation_doc, "The Cpp_PositionAndRotation module");


static PyModuleDef_Slot Cpp_PositionAndRotation_slots[] = {
    { Py_mod_exec, exec_Cpp_PositionAndRotation },
    { 0, NULL }
};

static PyModuleDef Cpp_PositionAndRotation_def = {
    PyModuleDef_HEAD_INIT,
    "Cpp_PositionAndRotation",
    Cpp_PositionAndRotation_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
    Cpp_PositionAndRotation_slots,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_Cpp_PositionAndRotation() {
    return PyModuleDef_Init(&Cpp_PositionAndRotation_def);
}
