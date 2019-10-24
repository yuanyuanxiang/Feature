#include "pyCaller.h"

wchar_t pyCaller::pyHome[] = { 0 };

void pyCaller::ParseResult(PyObject *pRetVal, tfOutput &tf)
{
	PyArrayObject *pMatrix = (PyArrayObject *) pRetVal;

	int x1 = tf.nx > 1 ? pMatrix->dimensions[0] : 1,
		x2 = tf.ny > 1 ? pMatrix->dimensions[1] : 1,
		x3 = tf.nz > 1 ? pMatrix->dimensions[2] : 1;
	if (x1 == tf.nx && x2 == tf.ny && x3 == tf.nz)
		memcpy(tf.feature, pMatrix->data, x1*x2*x3 * sizeof(float));
	My_DECREF(pMatrix);
}
