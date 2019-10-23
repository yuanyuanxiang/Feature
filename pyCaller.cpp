#include "pyCaller.h"

wchar_t pyCaller::pyHome[] = { 0 };

tfOutput pyCaller::ParseResult(PyObject *pRetVal, tfOutput *tf)
{
	tfOutput out;
	out = tf ? *tf : out;
	PyArrayObject *pMatrix = (PyArrayObject *) pRetVal;

	int x1 = pMatrix->dimensions[0];
	if (x1 == 512)
		memcpy(out.feature, pMatrix->data, x1 * sizeof(float));
	My_DECREF(pMatrix);

	return out;
}
