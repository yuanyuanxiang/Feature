#include "pyCaller.h"

// 注意调用SetPythonHome修改当前使用的python.
int main(int argc, const char *argv[])
{
	const char *path = argc == 1 ? "image.jpg" : argv[1];

	pyCaller py;
	// 设置当前用的python
	py.SetPythonHome("D:\\ProgramData\\Anaconda3\\envs\\tfcpu");
	py.Init("index");
	py.ActivateFunc("Feature");
	tfOutput output;
	cv::Mat m = imread("./image.JPG");
	cv::resize(m, m, cv::Size(224, 224));
	switch (m.step[1])
	{
	case 3: break;
	case 1: cvtColor(m, m, CV_GRAY2RGB); break;
	case 4: cvtColor(m, m, CV_RGBA2RGB); break;
	case 2: default: return -1;
	}
	npy_intp dims[] = { m.rows, m.cols, 3 };
	PyObject *PyArray = PyArray_SimpleNewFromData(3, dims, NPY_UBYTE, m.data);
	PyObject *ArgArray = PyTuple_New(1);
	PyTuple_SetItem(ArgArray, 0, PyArray);

	output = py.CallFunction("Feature", ArgArray);
	system("PAUSE");
	return 0;
}
