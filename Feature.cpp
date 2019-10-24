#include "Feature.h"

Feature F;

bool Feature::Init(const char *python, const char *mod, const char *fun)
{
	module = mod;
	func = fun;
	return m_py.SetPythonHome(python) && m_py.Init(mod) && m_py.ActivateFunc(fun);
}

void Feature::Uninit()
{
}

Feature::Feature()
{
	w = 224;
	h = 224;
}


Feature::~Feature()
{
}

tfOutput Feature::features(const cv::Mat &m, int x, int y, int z)
{
	cv::Mat b;
	cv::resize(m, b, cv::Size(w,h));
	switch (b.step[1])
	{
	case 3: break;
	case 1: cvtColor(b, b, CV_GRAY2RGB); break;
	case 4: cvtColor(b, b, CV_RGBA2RGB); break;
	case 2: default: return NULL;
	}
	npy_intp dims[] = { b.rows, b.cols, 3 };
	PyObject *PyArray = PyArray_SimpleNewFromData(3, dims, NPY_UBYTE, b.data);
	PyObject *ArgArray = PyTuple_New(1);
	PyTuple_SetItem(ArgArray, 0, PyArray);

	tfOutput output(x, y, z);
	m_py.CallFunction(func.c_str(), ArgArray, output);
	return output;
}
