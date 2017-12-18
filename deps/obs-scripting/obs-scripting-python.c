/******************************************************************************
    Copyright (C) 2015 by Andrew Skinner <obs@theandyroid.com>
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

#include "obs-scripting-python.h"
#include "obs-scripting-config.h"
#include <util/base.h>
#include <util/platform.h>
#include <util/darray.h>
#include <util/dstr.h>

#include <obs.h>

/* ========================================================================= */

// #define DEBUG_PYTHON_STARTUP

static const char *startup_script = "\n\
import sys\n\
import os\n\
import obspython\n\
class stdout_logger(object):\n\
	def write(self, message):\n\
		obspython.blog(obspython.LOG_INFO | obspython.LOG_TEXTBLOCK,\n\
				message)\n\
	def flush(self):\n\
		pass\n\
class stderr_logger(object):\n\
	def write(self, message):\n\
		obspython.blog(obspython.LOG_ERROR | obspython.LOG_TEXTBLOCK,\n\
				message)\n\
	def flush(self):\n\
		pass\n\
os.environ['PYTHONUNBUFFERED'] = '1'\n\
sys.stdout = stdout_logger()\n\
sys.stderr = stderr_logger()\n";

#if RUNTIME_LINK
static wchar_t home_path[1024] = {0};
#endif

DARRAY(char*) python_paths;
static bool python_loaded = false;

#define libobs_to_py(type, obs_obj, ownership, py_obj) \
	libobs_to_py_(#type " *", obs_obj, ownership, py_obj, \
			NULL, __func__, __LINE__)

/* ========================================================================= */

static void add_functions_to_py_module(PyObject *module,
		PyMethodDef *method_list)
{
	PyObject *dict = PyModule_GetDict(module);
	PyObject *name = PyModule_GetNameObject(module);
	if (!dict || !name) {
		return;
	}
	for (PyMethodDef *ml = method_list; ml->ml_name != NULL; ml++) {
		PyObject *func = PyCFunction_NewEx(ml, module, name);
		if (!func) {
			continue;
		}
		PyDict_SetItemString(dict, ml->ml_name, func);
		Py_DECREF(func);
	}
	Py_DECREF(name);
}

/* -------------------------------------------- */

PyObject *py_obspython = NULL;

static PyObject *py_get_current_script_path(PyObject *self, PyObject *args)
{
	UNUSED_PARAMETER(args);
	return PyDict_GetItemString(PyModule_GetDict(self),
			"__script_dir__");
}

static void load_python_script(struct obs_python_script *data)
{
	PyObject *py_file     = NULL;
	PyObject *py_module   = NULL;
	PyObject *py_success  = NULL;
	int       ret;

	py_file   = PyUnicode_FromString(data->file.array);
	py_module = PyImport_Import(py_file);
	if (py_error() || !py_module)
		goto fail;

	Py_XINCREF(py_obspython);
	ret = PyModule_AddObject(py_module, "obspython", py_obspython);
	if (py_error() || ret != 0)
		goto fail;

	ret = PyModule_AddStringConstant(py_module, "__script_dir__",
			data->dir.array);
	if (py_error() || ret != 0)
		goto fail;

	PyMethodDef global_funcs[] = {
		{"get_script_path",
		 py_get_current_script_path,
		 METH_NOARGS,
		 "Gets the script path"},
		{0}
	};

	add_functions_to_py_module(py_module, global_funcs);

fail:
	Py_XDECREF(py_success);
	Py_XDECREF(py_module);
	Py_XDECREF(py_file);
}

static void unload_python_script(PyObject *py_module)
{
	PyObject *py_funcname = NULL;
	PyObject *py_func     = NULL;
	PyObject *py_ret      = NULL;

	py_funcname = PyUnicode_FromString("script_unload");
	py_func     = PyObject_GetAttr(py_module, py_funcname);
	if (PyErr_Occurred() || !py_func)
		goto fail;

	py_ret = PyObject_CallObject(py_func, NULL);
	if (py_error())
		goto fail;

fail:
	Py_XDECREF(py_ret);
	Py_XDECREF(py_func);
	Py_XDECREF(py_funcname);
	Py_XDECREF(py_module);
}

static void add_to_python_path(const char *path)
{
	PyObject *py_path_str = NULL;
	PyObject *py_path     = NULL;
	int       ret;

	if (!path || !*path)
		return;

	for (size_t i = 0; i < python_paths.num; i++) {
		const char *python_path = python_paths.array[i];
		if (strcmp(path, python_path) == 0)
			return;
	}

	ret = PyRun_SimpleString("import sys");
	if (py_error() || ret != 0)
		goto fail;

	/* borrowed reference here */
	py_path = PySys_GetObject("path");
	if (py_error() || !py_path)
		goto fail;

	py_path_str = PyUnicode_FromString(path);
	ret = PyList_Append(py_path, py_path_str);
	if (py_error() || ret != 0)
		goto fail;

	char *new_path = bstrdup(path);
	da_push_back(python_paths, &new_path);

fail:
	Py_XDECREF(py_path_str);
}

/* -------------------------------------------- */

bool obs_python_script_load(obs_script_t *s)
{
	struct obs_python_script *data = (struct obs_python_script *)s;
	if (!data->base.loaded) {
		data->base.loaded = load_python_script(data);
	}

	return data->base.loaded;
}

obs_script_t *obs_python_script_create(const char *path)
{
	struct obs_python_script *data = bzalloc(sizeof(*data));

	data->base.type = OBS_SCRIPT_LANG_PYTHON;

	dstr_copy(&data->base.path, path);
	dstr_replace(&data->base.path, "\\", "/");
	path = data->base.path.array;

	const char *slash = path && *path ? strrchr(path, '/') : NULL;
	if (slash) {
		slash++;
		dstr_copy(&data->file, slash);
		dstr_left(&data->dir, &data->base.path, slash - path);
	} else {
		dstr_copy(&data->file, path);
	}

	path = data->file.array;

	const char *ext = strstr(path, ".py");
	if (ext)
		dstr_resize(&data->file, ext - path);

	lock_python();
	add_to_python_path(data->dir.array);
	load_python_script(data);
	unlock_python();
	return (obs_script_t *)data;
}

void obs_python_script_unload(obs_script_t *s)
{
	struct obs_python_script *data = (struct obs_python_script *)s;

	if (!s->loaded)
		return;

	s->loaded = false;
}

void obs_python_script_destroy(obs_script_t *s)
{
	struct obs_python_script *data = (struct obs_python_script *)s;

	if (data) {
		dstr_free(&data->base.path);
		dstr_free(&data->dir);
		dstr_free(&data->file);
		bfree(data);
	}
}

/* -------------------------------------------- */

void obs_python_unload(void);

bool obs_scripting_python_runtime_linked(void)
{
	return (bool)RUNTIME_LINK;
}

bool obs_scripting_python_loaded(void)
{
	return python_loaded;
}

void obs_python_load(void)
{
	da_init(python_paths);
}

bool obs_scripting_load_python(const char *python_path)
{
	if (python_loaded)
		return true;

	/* Use external python on windows and mac */
#if RUNTIME_LINK
# if 0
	struct dstr old_path  = {0};
	struct dstr new_path  = {0};
# endif

	if (!import_python(python_path))
		return false;

	if (python_path && *python_path) {
		os_utf8_to_wcs(python_path, 0, home_path, 1024);
		Py_SetPythonHome(home_path);
# if 0
		dstr_copy(&old_path, getenv("PATH"));
		_putenv("PYTHONPATH=");
		_putenv("PATH=");
# endif
	}
#else
	UNUSED_VARIABLE(python_path);
#endif

	Py_Initialize();

#if 0
# ifdef _DEBUG
	if (pythondir && *pythondir) {
		dstr_printf(&new_path, "PATH=%s", old_path.array);
		_putenv(new_path.array);
	}
# endif

	bfree(pythondir);
	dstr_free(&new_path);
	dstr_free(&old_path);
#endif

	PyEval_InitThreads();

	/* ---------------------------------------------- */
	/* Must set arguments for guis to work            */

	wchar_t *argv[] = {L"", NULL};
	int      argc   = sizeof(argv) / sizeof(wchar_t*) - 1;

	PySys_SetArgv(argc, argv);

#ifdef DEBUG_PYTHON_STARTUP
	/* ---------------------------------------------- */
	/* Debug logging to file if startup is failing    */

	PyRun_SimpleString("import os");
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("os.environ['PYTHONUNBUFFERED'] = '1'");
	PyRun_SimpleString("sys.stdout = open('./stdOut.txt','w',1)");
	PyRun_SimpleString("sys.stderr = open('./stdErr.txt','w',1)");
	PyRun_SimpleString("print(sys.version)");
#endif

	/* ---------------------------------------------- */
	/* Load main interface module                     */

	py_obspython = PyImport_ImportModule("obspython");
	bool success = !py_error();
	if (!success) {
		warn("Error importing obspython.py', unloading obs-python");
		goto out;
	}

	python_loaded = PyRun_SimpleString(startup_script) == 0;
	py_error();

out:
	/* ---------------------------------------------- */
	/* Free data                                      */

	PyEval_ReleaseThread(PyGILState_GetThisThreadState());

	if (!success) {
		warn("Failed to load python plugin");
		obs_python_unload();
	}

	return python_loaded;
}

void obs_python_unload(void)
{
	for (size_t i = 0; i < python_paths.num; i++)
		bfree(python_paths.array[i]);
	da_free(python_paths);

	if (!Py_IsInitialized())
		return;

	PyGILState_Ensure();

	Py_XDECREF(py_obspython);
	Py_Finalize();
}
