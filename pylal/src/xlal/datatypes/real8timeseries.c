/*
 * Copyright (C) 2009  Kipp Cannon
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


/*
 * ============================================================================
 *
 *                                  Preamble
 *
 * ============================================================================
 */


#include <Python.h>
#include <numpy/arrayobject.h>
#include <lal/LALDatatypes.h>
#include <lal/TimeSeries.h>
#include <lal/Sequence.h>
#include <lal/Units.h>
#include <lalunit.h>
#include <ligotimegps.h>
#include <real8timeseries.h>


#define MODULE_NAME PYLAL_REAL8TIMESERIES_MODULE_NAME


/*
 * ============================================================================
 *
 *                                    Type
 *
 * ============================================================================
 */


/*
 * Methods
 */


static PyObject *__new__(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	pylal_REAL8TimeSeries *obj;
	LIGOTimeGPS zero = {0, 0};

	obj = (pylal_REAL8TimeSeries *) PyType_GenericNew(type, args, kwds);
	if(!obj)
		return NULL;
	obj->series = XLALCreateREAL8TimeSeries(NULL, &zero, 0.0, 0.0, &lalDimensionlessUnit, 0);
	obj->owner = NULL;

	return (PyObject *) obj;
}


static int __init__(PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *key, *value;
	Py_ssize_t pos = 0;

	if(kwds)
		while(PyDict_Next(kwds, &pos, &key, &value))
			if(PyObject_SetAttr(self, key, value) < 0)
				return -1;
	return 0;
}


static void __del__(PyObject *self)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	if(obj->owner)
		Py_DECREF(obj->owner);
	else
		/* we are the owner */
		XLALDestroyREAL8TimeSeries(obj->series);
	self->ob_type->tp_free(self);
}


static PyObject *get_name(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	return PyString_FromString(obj->series->name);
}


static PyObject *get_epoch(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	return pylal_LIGOTimeGPS_new(obj->series->epoch);
}

static PyObject *get_f0(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	return PyFloat_FromDouble(obj->series->f0);
}


static PyObject *get_deltaT(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	return PyFloat_FromDouble(obj->series->deltaT);
}


static PyObject *get_sampleUnits(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	return pylal_LALUnit_new(0, obj->series->sampleUnits);
}


static PyObject *get_data(PyObject *self, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	npy_intp dims[] = {obj->series->data->length};
	PyObject *array = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, obj->series->data->data);
	PyObject *copy;
	if(!array)
		return NULL;
	/* incref self to prevent data from disappearing while
	 * array is still in use, and tell numpy to decref self
	 * when the array is deallocated */
	Py_INCREF(self);
	PyArray_BASE(array) = self;
	copy = PyArray_NewCopy((PyArrayObject *) array, NPY_ANYORDER);
	Py_DECREF(array);
	return copy;
}


static int set_name(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	const char *s = PyString_AsString(value);
	if(PyErr_Occurred())
		return -1;
	if(strlen(s) >= sizeof(obj->series->name)) {
		PyErr_Format(PyExc_ValueError, "name too long \"%s\"", s);
		return -1;
	}
	strcpy(obj->series->name, s);
	return 0;
}


static int set_epoch(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	if(!PyObject_TypeCheck(value, &pylal_LIGOTimeGPS_Type)) {
		PyErr_SetObject(PyExc_TypeError, value);
		return -1;
	}
	obj->series->epoch = ((pylal_LIGOTimeGPS *) value)->gps;
	return 0;
}


static int set_f0(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	double f0 = PyFloat_AsDouble(value);
	if(PyErr_Occurred())
		return -1;
	obj->series->f0 = f0;
	return 0;
}


static int set_deltaT(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	double deltaT = PyFloat_AsDouble(value);
	if(PyErr_Occurred())
		return -1;
	obj->series->deltaT = deltaT;
	return 0;
}


static int set_sampleUnits(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	if(!PyObject_TypeCheck(value, &pylal_LALUnit_Type)) {
		PyErr_SetObject(PyExc_TypeError, value);
		return -1;
	}
	obj->series->sampleUnits = ((pylal_LALUnit *) value)->unit;
	return 0;
}


static int set_data(PyObject *self, PyObject *value, void *unused)
{
	pylal_REAL8TimeSeries *obj = (pylal_REAL8TimeSeries *) self;
	int n;
	/* require array of double precision floats */
	if(!PyArray_Check(value) || (PyArray_TYPE(value) != NPY_DOUBLE)) {
		PyErr_SetObject(PyExc_TypeError, value);
		return -1;
	}
	/* require exactly 1 dimension */
	if(((PyArrayObject *) value)->nd != 1) {
		PyErr_SetObject(PyExc_ValueError, value);
		return -1;
	}
	n = PyArray_DIM(value, 0);
	if(n != obj->series->data->length)
		obj->series->data = XLALResizeREAL8Sequence(obj->series->data, 0, n);
	memcpy(obj->series->data->data, PyArray_GETPTR1(value, 0), n * sizeof(*obj->series->data->data));
	return 0;
}


/*
 * Type
 */


#define GETSETDEF(attr, doc, closure) {#attr, get_ ## attr, set_ ## attr, doc, closure}
static struct PyGetSetDef getsetdefs[] = {
	GETSETDEF(name, NULL, NULL),
	GETSETDEF(epoch, NULL, NULL),
	GETSETDEF(f0, NULL, NULL),
	GETSETDEF(deltaT, NULL, NULL),
	GETSETDEF(sampleUnits, NULL, NULL),
	GETSETDEF(data, NULL, NULL),
	{NULL, NULL, NULL, NULL, NULL}
};
#undef GETSETDEF


PyTypeObject pylal_real8timeseries_type = {
	PyObject_HEAD_INIT(NULL)
	.tp_basicsize = sizeof(pylal_REAL8TimeSeries),
	.tp_dealloc = __del__,
	.tp_doc = "REAL8TimeSeries structure",
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_getset = &getsetdefs,
	.tp_name = MODULE_NAME ".REAL8TimeSeries",
	.tp_new = __new__,
	.tp_init = __init__
};


/*
 * ============================================================================
 *
 *                            Module Registration
 *
 * ============================================================================
 */


void initreal8timeseries(void)
{
	PyObject *module = Py_InitModule3(MODULE_NAME, NULL, "Wrapper for LAL's REAL8TimeSeries type.");

	import_array();
	pylal_lalunit_import();
	pylal_ligotimegps_import();

	/*
	 * REAL8TimeSeries
	 */

	_pylal_REAL8TimeSeries_Type = &pylal_real8timeseries_type;
	if(PyType_Ready(&pylal_REAL8TimeSeries_Type) < 0)
		return;
	Py_INCREF(&pylal_REAL8TimeSeries_Type);
	PyModule_AddObject(module, "REAL8TimeSeries", (PyObject *) &pylal_REAL8TimeSeries_Type);
}
