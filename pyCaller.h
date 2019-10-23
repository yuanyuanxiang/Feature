#include "python.h"
#include <map>
#include <string>
#include "strTransfer.h"
#include "config.h"

#pragma once

// ���ɼ��Ŀ�����
#define MAX_BOXES_NUM 100

// Py_DECREF ���³����������ʱ����
#define My_DECREF(p) 

// ���� numpy �е�ͷ�ļ�arrayobject.h
#include "..\Lib\site-packages\numpy\core\include\numpy\arrayobject.h"

/************************************************************************
* @class tfOutput
* @brief tensorflowģ������Ĳ����ṹ
* @note ���ڸýṹ��Ҫ��Ƶ��ʹ��, �ʸ���ͨ�����ü���ref�����ڴ�, \n 
    �����ڴ�й©, ����ϵ����
************************************************************************/
class tfOutput
{
protected:
	int *ref;				// ���ü���
	int addref() const { return ++ (*ref); }
	int removeref() const { return -- (*ref); }
	void destroy()			// ����
	{
		if (0 == removeref())
		{
			SAFE_DELETE_ARRAY(ref);
			SAFE_DELETE_ARRAY(feature);
		}
	}
public:
	float *feature;			// 512ά����������

	tfOutput()
	{
		memset(this, 0, sizeof(tfOutput));
		ref = new int(1);
		feature = new float[512];
	}
	~tfOutput()
	{
		destroy();
	}
	tfOutput(const tfOutput &o)
	{
		ref = o.ref;
		feature = o.feature;
		addref();
	}
	tfOutput operator = (const tfOutput &o)
	{
		if (this != &o)// ��ֹ�Լ���ֵ���Լ�
		{
			// ������������
			destroy();
			// this��o����
			ref = o.ref;
			feature = o.feature;
			addref();
		}
		return *this;
	}
};

/************************************************************************
* @struct Item
* @brief �����Ϣ�ṹ�壨�������, ���ID��
************************************************************************/
struct Item 
{
	int id;				// ���ID(��1��ʼ)
	char name[64];		// �������
	Item() { id = 0; memset(name, 0, sizeof(name)); }
	Item(const char *_name, int _id) { strcpy_s(name, _name); id = max(_id, 1); }
};

/************************************************************************
* @class LabelMap
* @brief ����ǩ
************************************************************************/
class labelMap
{
public:
	int num;			// ������������һ����
	Item *items;		// �����Ϣ
	labelMap() { num = 0; items = 0; }
	~labelMap() { if(items) delete [] items; }

	// ��������ǩ
	void Create(int n) { num = max(n, 1); if(0 == items) items = new Item[num]; }
	// ��������ǩ
	void Destroy() { num = 0; if(items) delete [] items; items = 0; }
	// �����µ����
	void InsertItem(const Item & it) { if(it.id > 0 && it.id <= num) items[it.id - 1] = it; }
	// ����ID��ȡ����
	const char* getItemName(int id) const { return (id > 0 && id <= num) ? items[id - 1].name : ""; }
};

/************************************************************************
* @class pyCaller
* @brief python������: �����ڶ�ͼƬ����Ƶ����ʶ��
* @author Ԭ����, 2018-4-11
************************************************************************/
class pyCaller
{
private:
	static wchar_t pyHome[_MAX_PATH];			// python·��
	PyObject* pModule;							// pythonģ��
	std::map<std::string, PyObject*> pFunMap;	// �����б�
	bool bMultiThread;							// ���߳�

	// ��ʼ�� numpy ִ�л�������Ҫ�ǵ����
	// python2.7��void�������ͣ�python3.0������int��������
	inline int init_numpy()
	{
		import_array();
		return 1;
	}

	// ����python���
	tfOutput ParseResult(PyObject *pRetVal, tfOutput *tf);

public:
	// ����python��װĿ¼
	static bool SetPythonHome(const char *py)
	{
		char pyExe[_MAX_PATH] = { 0 };
		strcat_s(pyExe, py);
		strcat_s(pyExe, "\\python.exe");
		if (-1 != _access(pyExe, 0))
		{
			size_t s;
			mbstowcs_s(&s, pyHome, py, strlen(py));
			OUTPUT("======> SetPythonHome: %s\n", py);
			return true;
		}
		else
		{
			OUTPUT("======> SetPythonHome: \"%s\" don't include python.exe.\n", py);
			return false;
		}
	}

	/**
	* @brief ����һ��pyCaller����
	*/
	pyCaller(bool multi_thread = true)
	{
		pModule = NULL;
		bMultiThread = multi_thread;
	}

	/**
	* @brief ��ʼ��pyCaller���󣬽���py�ű�������Ϊ�������
	*/
	bool Init(const char * module_name)
	{
		if (pyHome[0] && NULL == pModule)
		{
			clock_t t = clock();
			Py_SetPythonHome(pyHome);
			Py_Initialize();
			if (0 == Py_IsInitialized())
			{
				OUTPUT("Py_IsInitialized = 0.\n");
				return false;
			}
			if (bMultiThread)
				PyEval_InitThreads(); // ���߳�֧��
			if (NUMPY_IMPORT_ARRAY_RETVAL == init_numpy())
			{
				OUTPUT("init_numpy failed.\n");
				return false;
			}
			PyObject *py = PyImport_ImportModule(module_name);
			if (bMultiThread && PyEval_ThreadsInitialized())
				PyEval_SaveThread();
			if (NULL == py)
				OUTPUT("PyImport_ImportModule failed.\n");
			t = clock() - t;
			char szOut[128];
			sprintf_s(szOut, "PyImport_ImportModule using %d ms.\n", t);
			OutputDebugStringA(szOut);
#ifndef _AFX
			printf(szOut);
#endif
			pModule = py;
		}
		else
		{
			OUTPUT("Py_SetPythonHome is not called.\n");
		}
		return pModule;
	}

	/**
	* @brief ����ʼ��python�������ͷ�python������ڴ�
	*/
	~pyCaller()
	{
		if (pModule)
			Py_DECREF(pModule);
		for (std::map<std::string, PyObject*>::iterator p = pFunMap.begin(); 
			p != pFunMap.end(); ++p)
			if (p->second) Py_DECREF(p->second);
		if (pyHome[0])
		{
			if (bMultiThread && !PyGILState_Check())
				PyGILState_Ensure();
			Py_Finalize();
		}
	}

	// �Ƿ������ָ��ģ��
	bool IsModuleLoaded() const { return pModule; }

	/**
	* @brief ʹ��ǰ����ָ�����Ƶĺ���
	*/
	bool ActivateFunc(const char * func_name)
	{
		bool bFind = false;
		std::string fun(func_name);
		for (std::map<std::string, PyObject*>::iterator p = pFunMap.begin(); 
			p != pFunMap.end(); ++p)
		{
			if (p->first == fun)
			{
				bFind = true;
				break;
			}
		}
		if (bFind)
			return true;
		PyObject *pFunc = NULL;

		PyGILState_STATE gstate;
		int nHold = bMultiThread ? PyGILState_Check() : TRUE;
		if (!nHold) gstate = PyGILState_Ensure();
		Py_BEGIN_ALLOW_THREADS;
		Py_BLOCK_THREADS;

		pFunc =  pModule ? PyObject_GetAttrString(pModule, func_name) : 0;

		Py_UNBLOCK_THREADS;
		Py_END_ALLOW_THREADS;
		if (!nHold) PyGILState_Release(gstate);

		pFunMap.insert(std::make_pair(func_name, pFunc));

		return pFunc;
	}

	/**
	* @brief ����python�ű��е�ָ������
	*/
	tfOutput CallFunction(const char * func_name, const char *arg, tfOutput *tf = NULL)
	{
		tfOutput out;
		out = tf ? *tf : out;
		PyObject *pFunc = pFunMap[func_name];
		if (pFunc)
		{
			const char *utf8 = MByteToUtf8(arg);
			PyObject* pArg = Py_BuildValue("(s)", utf8);
			delete [] utf8;
			if (NULL == pArg)
				return out;
			PyObject* pRetVal = PyEval_CallObject(pFunc, pArg);
			if (NULL == pRetVal)
				return out;
			out = ParseResult(pRetVal, tf);
		}
		return out;
	}
	/**
	* @brief ����python�ű��е�ָ������
	*/
	tfOutput CallFunction(const char * func_name, PyObject *arg, tfOutput *tf = NULL)
	{
		tfOutput out;
		out = tf ? *tf : out;
#if STATIC_DETECTING
		static PyObject *pFunc = pFunMap[func_name];
#else 
		PyObject *pFunc = pFunMap[func_name];
#endif
		if (pFunc)
		{
			PyGILState_STATE gstate;
			int nHold = bMultiThread ? PyGILState_Check() : TRUE;
			if (!nHold)gstate = PyGILState_Ensure();
			Py_BEGIN_ALLOW_THREADS;
			Py_BLOCK_THREADS;

 			PyObject* pRetVal = PyEval_CallObject(pFunc, arg);
			if (pRetVal)
				out = ParseResult(pRetVal, tf);

			Py_UNBLOCK_THREADS;
			Py_END_ALLOW_THREADS;
			if (!nHold)PyGILState_Release(gstate);
		}
		return out;
	}
};