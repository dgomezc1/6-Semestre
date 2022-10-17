#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyObject * spam_six(PyObject*, PyObject*);

static PyMethodDef SpamMethods[] = {
    {"six",  spam_six, METH_VARARGS, "return 6"},
    {NULL},
};

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT, "spam", NULL,  -1, SpamMethods
};

PyMODINIT_FUNC PyInit_spam(void)
{
    return PyModule_Create(&spammodule);
}

PyObject* spam_six(PyObject *self, PyObject *args)
{
    return PyLong_FromLong(6);
}