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

#include <util/dstr.h>
#include <util/platform.h>

#define NO_REDEFS
#include "obs-scripting-python-import.h"
#include "obs-scripting-config.h"

#ifdef _MSC_VER
#pragma warning(disable : 4152)
#endif

int (*Import_PyType_Ready)(PyTypeObject *);
PyObject *(*Import_PyObject_GenericGetAttr)(PyObject *, PyObject *);
int (*Import_PyObject_IsTrue)(PyObject *);
void (*Import_Py_DecRef)(PyObject *);
void *(*Import_PyObject_Malloc)(size_t size);
void (*Import_PyObject_Free)(void *ptr);
PyObject *(*Import_PyObject_Init)(PyObject *, PyTypeObject *);
PyObject *(*Import_PyUnicode_FromFormat)(const char *format, ...);
PyObject *(*Import_PyUnicode_Concat)(PyObject *left, PyObject *right);
PyObject *(*Import_PyLong_FromVoidPtr)(void *);
PyObject *(*Import_PyBool_FromLong)(long);
PyGILState_STATE (*Import_PyGILState_Ensure)(void);
PyThreadState *(*Import_PyGILState_GetThisThreadState)(void);
void (*Import_PyErr_SetString)(PyObject *exception, const char *string);
PyObject *(*Import_PyErr_Occurred)(void);
void (*Import_PyErr_Fetch)(PyObject **, PyObject **, PyObject **);
void (*Import_PyErr_Restore)(PyObject *, PyObject *, PyObject *);
void (*Import_PyErr_WriteUnraisable)(PyObject *);
int (*Import_PyArg_UnpackTuple)(PyObject *, const char *, Py_ssize_t, Py_ssize_t, ...);
PyObject *(*Import_Py_BuildValue)(const char *, ...);
int (*Import_PyRun_SimpleStringFlags)(const char *, PyCompilerFlags *);
void (*Import_PyErr_Print)(void);
void (*Import_Py_SetPythonHome)(wchar_t *);
void (*Import_Py_Initialize)(void);
void (*Import_Py_Finalize)(void);
int (*Import_Py_IsInitialized)(void);
void (*Import_PyEval_InitThreads)(void);
void (*Import_PyEval_ReleaseThread)(PyThreadState *tstate);
void (*Import_PySys_SetArgv)(int, wchar_t **);
PyObject *(*Import_PyImport_ImportModule)(const char *name);
PyObject *(*Import_PyObject_CallFunctionObjArgs)(PyObject *callable, ...);
PyObject (*Import__Py_NotImplementedStruct);
PyObject *(*Import_PyExc_TypeError);
PyObject *(*Import_PyExc_RuntimeError);
PyObject *(*Import_PyObject_GetAttr)(PyObject *, PyObject *);
PyObject *(*Import_PyUnicode_FromString)(const char *u);
PyObject *(*Import_PyDict_GetItemString)(PyObject *dp, const char *key);
int (*Import_PyDict_SetItemString)(PyObject *dp, const char *key, PyObject *item);
PyObject *(*Import_PyCFunction_NewEx)(PyMethodDef *, PyObject *, PyObject *);
PyObject *(*Import_PyModule_GetDict)(PyObject *);
PyObject *(*Import_PyModule_GetNameObject)(PyObject *);
int (*Import_PyModule_AddObject)(PyObject *, const char *, PyObject *);
int (*Import_PyModule_AddStringConstant)(PyObject *, const char *, const char *);
PyObject *(*Import_PyImport_Import)(PyObject *name);
PyObject *(*Import_PyObject_CallObject)(PyObject *callable_object, PyObject *args);
struct _longobject (*Import__Py_FalseStruct);
struct _longobject (*Import__Py_TrueStruct);
void (*Import_PyGILState_Release)(PyGILState_STATE);
int (*Import_PyList_Append)(PyObject *, PyObject *);
PyObject *(*Import_PySys_GetObject)(const char *);
PyObject *(*Import_PyImport_ReloadModule)(PyObject *m);
PyObject *(*Import_PyObject_GetAttrString)(PyObject *, const char *);
PyObject *(*Import_PyCapsule_New)(void *pointer, const char *name, PyCapsule_Destructor destructor);
void *(*Import_PyCapsule_GetPointer)(PyObject *capsule, const char *name);
int (*Import_PyArg_ParseTuple)(PyObject *, const char *, ...);
PyTypeObject (*Import_PyFunction_Type);
int (*Import_PyObject_SetAttr)(PyObject *, PyObject *, PyObject *);
PyObject *(*Import__PyObject_New)(PyTypeObject *);
void *(*Import_PyCapsule_Import)(const char *name, int no_block); 
void (*Import_PyErr_Clear)(void);
PyObject *(*Import_PyObject_Call)(PyObject *callable_object, PyObject *args, PyObject *kwargs);
PyObject *(*Import_PyList_New)(Py_ssize_t size);
Py_ssize_t (*Import_PyList_Size)(PyObject *);
PyObject *(*Import_PyList_GetItem)(PyObject *, Py_ssize_t);

#ifdef _WIN32
#define SO_EXT ".dll"
#else
#define SO_EXT ".so"
#endif

bool import_python(const char *python_path)
{
	struct dstr lib_path;
	bool success = false;
	void *lib;

	if (!python_path)
		python_path = "";

	dstr_init_copy(&lib_path, python_path);
	dstr_replace(&lib_path, "\\", "/");
	if (!dstr_is_empty(&lib_path)) {
		dstr_cat(&lib_path, "/");
	}
	dstr_cat(&lib_path, PYTHON_LIB SO_EXT);

	lib = os_dlopen(lib_path.array);
	if (!lib) {
		blog(LOG_WARNING, "[Python] Could not load library: %s",
				lib_path.array);
		goto fail;
	}

#define IMPORT_FUNC(x) \
	do { \
		Import_##x = os_dlsym(lib, #x); \
		if (!Import_##x) { \
			blog(LOG_WARNING, "[Python] Failed to import: %s", \
					#x); \
			goto fail; \
		} \
	} while (false)

	IMPORT_FUNC(PyType_Ready);
	IMPORT_FUNC(PyObject_GenericGetAttr);
	IMPORT_FUNC(PyObject_IsTrue);
	IMPORT_FUNC(Py_DecRef);
	IMPORT_FUNC(PyObject_Malloc);
	IMPORT_FUNC(PyObject_Free);
	IMPORT_FUNC(PyObject_Init);
	IMPORT_FUNC(PyUnicode_FromFormat);
	IMPORT_FUNC(PyUnicode_Concat);
	IMPORT_FUNC(PyLong_FromVoidPtr);
	IMPORT_FUNC(PyBool_FromLong);
	IMPORT_FUNC(PyGILState_Ensure);
	IMPORT_FUNC(PyGILState_GetThisThreadState);
	IMPORT_FUNC(PyErr_SetString);
	IMPORT_FUNC(PyErr_Occurred);
	IMPORT_FUNC(PyErr_Fetch);
	IMPORT_FUNC(PyErr_Restore);
	IMPORT_FUNC(PyErr_WriteUnraisable);
	IMPORT_FUNC(PyArg_UnpackTuple);
	IMPORT_FUNC(Py_BuildValue);
	IMPORT_FUNC(PyRun_SimpleStringFlags);
	IMPORT_FUNC(PyErr_Print);
	IMPORT_FUNC(Py_SetPythonHome);
	IMPORT_FUNC(Py_Initialize);
	IMPORT_FUNC(Py_Finalize);
	IMPORT_FUNC(Py_IsInitialized);
	IMPORT_FUNC(PyEval_InitThreads);
	IMPORT_FUNC(PyEval_ReleaseThread);
	IMPORT_FUNC(PySys_SetArgv);
	IMPORT_FUNC(PyImport_ImportModule);
	IMPORT_FUNC(PyObject_CallFunctionObjArgs);
	IMPORT_FUNC(_Py_NotImplementedStruct);
	IMPORT_FUNC(PyExc_TypeError);
	IMPORT_FUNC(PyExc_RuntimeError);
	IMPORT_FUNC(PyObject_GetAttr);
	IMPORT_FUNC(PyUnicode_FromString);
	IMPORT_FUNC(PyDict_GetItemString);
	IMPORT_FUNC(PyDict_SetItemString);
	IMPORT_FUNC(PyCFunction_NewEx);
	IMPORT_FUNC(PyModule_GetDict);
	IMPORT_FUNC(PyModule_GetNameObject);
	IMPORT_FUNC(PyModule_AddObject);
	IMPORT_FUNC(PyModule_AddStringConstant);
	IMPORT_FUNC(PyImport_Import);
	IMPORT_FUNC(PyObject_CallObject);
	IMPORT_FUNC(_Py_FalseStruct);
	IMPORT_FUNC(_Py_TrueStruct);
	IMPORT_FUNC(PyGILState_Release);
	IMPORT_FUNC(PyList_Append);
	IMPORT_FUNC(PySys_GetObject);
	IMPORT_FUNC(PyImport_ReloadModule);
	IMPORT_FUNC(PyObject_GetAttrString);
	IMPORT_FUNC(PyCapsule_New);
	IMPORT_FUNC(PyCapsule_GetPointer);
	IMPORT_FUNC(PyArg_ParseTuple);
	IMPORT_FUNC(PyFunction_Type);
	IMPORT_FUNC(PyObject_SetAttr);
	IMPORT_FUNC(_PyObject_New);
	IMPORT_FUNC(PyCapsule_Import);
	IMPORT_FUNC(PyErr_Clear);
	IMPORT_FUNC(PyObject_Call);
	IMPORT_FUNC(PyList_New);
	IMPORT_FUNC(PyList_Size);
	IMPORT_FUNC(PyList_GetItem);

#undef IMPORT_FUNC

	success = true;

fail:
	if (!success && lib)
		os_dlclose(lib);

	dstr_free(&lib_path);

	return success;
}
