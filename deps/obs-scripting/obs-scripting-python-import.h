/******************************************************************************
    Copyright (C) 2017 by Hugh Bailey <jim@obsproject.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <util/c99defs.h>

#if defined(_WIN32) || defined(__APPLE__)
#define RUNTIME_LINK 1
#define Py_NO_ENABLE_SHARED
#else
#define RUNTIME_LINK 0
#endif

#if defined(_WIN32) && defined(_DEBUG)
# undef _DEBUG
# include <Python.h>
# define _DEBUG
#else
# include <Python.h>
#endif

#if RUNTIME_LINK

extern int (*Import_PyType_Ready)(PyTypeObject *);
extern PyObject *(*Import_PyObject_GenericGetAttr)(PyObject *, PyObject *);
extern int (*Import_PyObject_IsTrue)(PyObject *);
extern void (*Import_Py_DecRef)(PyObject *);
extern void *(*Import_PyObject_Malloc)(size_t size);
extern void (*Import_PyObject_Free)(void *ptr);
extern PyObject *(*Import_PyObject_Init)(PyObject *, PyTypeObject *);
extern PyObject *(*Import_PyUnicode_FromFormat)(const char *format, ...);
extern PyObject *(*Import_PyUnicode_Concat)(PyObject *left, PyObject *right);
extern PyObject *(*Import_PyLong_FromVoidPtr)(void *);
extern PyObject *(*Import_PyBool_FromLong)(long);
extern PyGILState_STATE (*Import_PyGILState_Ensure)(void);
extern PyThreadState *(*Import_PyGILState_GetThisThreadState)(void);
extern void (*Import_PyErr_SetString)(PyObject *exception, const char *string);
extern PyObject *(*Import_PyErr_Occurred)(void);
extern void (*Import_PyErr_Fetch)(PyObject **, PyObject **, PyObject **);
extern void (*Import_PyErr_Restore)(PyObject *, PyObject *, PyObject *);
extern void (*Import_PyErr_WriteUnraisable)(PyObject *);
extern int (*Import_PyArg_UnpackTuple)(PyObject *, const char *, Py_ssize_t, Py_ssize_t, ...);
extern PyObject *(*Import_Py_BuildValue)(const char *, ...);
extern int (*Import_PyRun_SimpleStringFlags)(const char *, PyCompilerFlags *);
extern void (*Import_PyErr_Print)(void);
extern void (*Import_Py_SetPythonHome)(wchar_t *);
extern void (*Import_Py_Initialize)(void);
extern void (*Import_Py_Finalize)(void);
extern int (*Import_Py_IsInitialized)(void);
extern void (*Import_PyEval_InitThreads)(void);
extern void (*Import_PyEval_ReleaseThread)(PyThreadState *tstate);
extern void (*Import_PySys_SetArgv)(int, wchar_t **);
extern PyObject *(*Import_PyImport_ImportModule)(const char *name);
extern PyObject *(*Import_PyObject_CallFunctionObjArgs)(PyObject *callable, ...);
extern PyObject (*Import__Py_NotImplementedStruct);
extern PyObject *(*Import_PyExc_TypeError);
extern PyObject *(*Import_PyExc_RuntimeError);
extern PyObject *(*Import_PyObject_GetAttr)(PyObject *, PyObject *);
extern PyObject *(*Import_PyUnicode_FromString)(const char *u);
extern PyObject *(*Import_PyDict_GetItemString)(PyObject *dp, const char *key);
extern int (*Import_PyDict_SetItemString)(PyObject *dp, const char *key, PyObject *item);
extern PyObject *(*Import_PyCFunction_NewEx)(PyMethodDef *, PyObject *, PyObject *);
extern PyObject *(*Import_PyModule_GetDict)(PyObject *);
extern PyObject *(*Import_PyModule_GetNameObject)(PyObject *);
extern int (*Import_PyModule_AddObject)(PyObject *, const char *, PyObject *);
extern int (*Import_PyModule_AddStringConstant)(PyObject *, const char *, const char *);
extern PyObject *(*Import_PyImport_Import)(PyObject *name);
extern PyObject *(*Import_PyObject_CallObject)(PyObject *callable_object, PyObject *args);
extern struct _longobject (*Import__Py_FalseStruct);
extern struct _longobject (*Import__Py_TrueStruct);
extern void (*Import_PyGILState_Release)(PyGILState_STATE);
extern int (*Import_PyList_Append)(PyObject *, PyObject *);
extern PyObject *(*Import_PySys_GetObject)(const char *);
extern PyObject *(*Import_PyImport_ReloadModule)(PyObject *m);
extern PyObject *(*Import_PyObject_GetAttrString)(PyObject *, const char *);
extern PyObject *(*Import_PyCapsule_New)(void *pointer, const char *name, PyCapsule_Destructor destructor);
extern void *(*Import_PyCapsule_GetPointer)(PyObject *capsule, const char *name);
extern int (*Import_PyArg_ParseTuple)(PyObject *, const char *, ...);
extern PyTypeObject (*Import_PyFunction_Type);
extern int (*Import_PyObject_SetAttr)(PyObject *, PyObject *, PyObject *);
extern PyObject *(*Import__PyObject_New)(PyTypeObject *);
extern void *(*Import_PyCapsule_Import)(const char *name, int no_block); 
extern void (*Import_PyErr_Clear)(void);
extern PyObject *(*Import_PyObject_Call)(PyObject *callable_object, PyObject *args, PyObject *kwargs);

extern bool import_python(const char *python_path);

# ifndef NO_REDEFS
# define PyType_Ready Import_PyType_Ready
# define PyObject_GenericGetAttr Import_PyObject_GenericGetAttr
# define PyObject_IsTrue Import_PyObject_IsTrue
# define Py_DecRef Import_Py_DecRef
# define PyObject_Malloc Import_PyObject_Malloc
# define PyObject_Free Import_PyObject_Free
# define PyObject_Init Import_PyObject_Init
# define PyUnicode_FromFormat Import_PyUnicode_FromFormat
# define PyUnicode_Concat Import_PyUnicode_Concat
# define PyLong_FromVoidPtr Import_PyLong_FromVoidPtr
# define PyBool_FromLong Import_PyBool_FromLong
# define PyGILState_Ensure Import_PyGILState_Ensure
# define PyGILState_GetThisThreadState Import_PyGILState_GetThisThreadState
# define PyErr_SetString Import_PyErr_SetString
# define PyErr_Occurred Import_PyErr_Occurred
# define PyErr_Fetch Import_PyErr_Fetch
# define PyErr_Restore Import_PyErr_Restore
# define PyErr_WriteUnraisable Import_PyErr_WriteUnraisable
# define PyArg_UnpackTuple Import_PyArg_UnpackTuple
# define Py_BuildValue Import_Py_BuildValue
# define PyRun_SimpleStringFlags Import_PyRun_SimpleStringFlags
# define PyErr_Print Import_PyErr_Print
# define Py_SetPythonHome Import_Py_SetPythonHome
# define Py_Initialize Import_Py_Initialize
# define Py_Finalize Import_Py_Finalize
# define Py_IsInitialized Import_Py_IsInitialized
# define PyEval_InitThreads Import_PyEval_InitThreads
# define PyEval_ReleaseThread Import_PyEval_ReleaseThread
# define PySys_SetArgv Import_PySys_SetArgv
# define PyImport_ImportModule Import_PyImport_ImportModule
# define PyObject_CallFunctionObjArgs Import_PyObject_CallFunctionObjArgs
# define _Py_NotImplementedStruct (*Import__Py_NotImplementedStruct)
# define PyExc_TypeError (*Import_PyExc_TypeError)
# define PyExc_RuntimeError (*Import_PyExc_RuntimeError)
# define PyObject_GetAttr Import_PyObject_GetAttr
# define PyUnicode_FromString Import_PyUnicode_FromString
# define PyDict_GetItemString Import_PyDict_GetItemString
# define PyDict_SetItemString Import_PyDict_SetItemString
# define PyCFunction_NewEx Import_PyCFunction_NewEx
# define PyModule_GetDict Import_PyModule_GetDict
# define PyModule_GetNameObject Import_PyModule_GetNameObject
# define PyModule_AddObject Import_PyModule_AddObject
# define PyModule_AddStringConstant Import_PyModule_AddStringConstant
# define PyImport_Import Import_PyImport_Import
# define PyObject_CallObject Import_PyObject_CallObject
# define _Py_FalseStruct (*Import__Py_FalseStruct)
# define _Py_TrueStruct (*Import__Py_TrueStruct)
# define PyGILState_Release Import_PyGILState_Release
# define PyList_Append Import_PyList_Append
# define PySys_GetObject Import_PySys_GetObject
# define PyImport_ReloadModule Import_PyImport_ReloadModule
# define PyObject_GetAttrString Import_PyObject_GetAttrString
# define PyCapsule_New Import_PyCapsule_New
# define PyCapsule_GetPointer Import_PyCapsule_GetPointer
# define PyArg_ParseTuple Import_PyArg_ParseTuple
# define PyFunction_Type (*Import_PyFunction_Type)
# define PyObject_SetAttr Import_PyObject_SetAttr
# define _PyObject_New Import__PyObject_New
# define PyCapsule_Import Import_PyCapsule_Import
# define PyErr_Clear Import_PyErr_Clear
# define PyObject_Call Import_PyObject_Call
# endif

#endif
