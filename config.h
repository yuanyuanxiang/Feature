// 【注意】请将OpenCV的包含目录、python包含目录添加到项目附加包含目录
// 将python的库目录添加到项目的附加库目录
// 然后根据实际情况修改本文件。

#pragma once

// 检查内存泄漏，如不需要，需注释这行
//#include "vld.h"

//////////////////////////////////////////////////////////////////////////
// OpenCV 配置
#define CV_VER "320"

#define USING_STL 1

#if _MSC_VER < 1900 // Visual Studio 2015以下版本可使用下述宏定义快速链接库

#ifdef _DEBUG
#define CV_LIB_PATH "D:/opencv/opencv32/x64/lib/Debug/"
#define CV_LIB_X(LIB, VER) CV_LIB_PATH##"opencv_"##LIB##VER##"d.lib"
#else
#define CV_LIB_PATH "D:/opencv/opencv32/x64/lib/Release/"
#define CV_LIB_X(LIB, VER) CV_LIB_PATH##"opencv_"##LIB##VER##".lib"
#endif

#define USING_CV_LIB(LIB) CV_LIB_X(LIB, CV_VER) // 使用CV的库


//////////////////////////////////////////////////////////////////////////
#include "opencv2/opencv.hpp" 
#include "opencv/cv.h"
using namespace cv;

#pragma comment(lib, USING_CV_LIB("core"))
#pragma comment(lib, USING_CV_LIB("highgui"))
#pragma comment(lib, USING_CV_LIB("imgproc"))
#pragma comment(lib, USING_CV_LIB("video"))
#pragma comment(lib, USING_CV_LIB("videoio"))
#pragma comment(lib, USING_CV_LIB("imgcodecs"))
#pragma comment(lib, USING_CV_LIB("photo"))

#else

#include "opencv2/opencv.hpp" 
#include "opencv/cv.h"
using namespace cv;

#ifdef _DEBUG
#pragma comment(lib, "opencv_core320d.lib")
#pragma comment(lib, "opencv_highgui320d.lib")
#pragma comment(lib, "opencv_imgproc320d.lib")
#pragma comment(lib, "opencv_video320d.lib")
#pragma comment(lib, "opencv_videoio320d.lib")
#pragma comment(lib, "opencv_imgcodecs320d.lib")
#pragma comment(lib, "opencv_photo320d.lib")
#else
#pragma comment(lib, "opencv_core320.lib")
#pragma comment(lib, "opencv_highgui320.lib")
#pragma comment(lib, "opencv_imgproc320.lib")
#pragma comment(lib, "opencv_video320.lib")
#pragma comment(lib, "opencv_videoio320.lib")
#pragma comment(lib, "opencv_imgcodecs320.lib")
#pragma comment(lib, "opencv_photo320.lib")
#endif

#endif

#ifdef _AFX
#define OUTPUT TRACE
#else
#define OUTPUT printf
#endif


// 是否支持IPC
#define SUPPORT_IPC 1

// 是否使检测函数为static(能避免频繁查找map)
#define STATIC_DETECTING 1

// 保存已识别人脸图像需满足的最小尺寸
#define MIN_FACESIZE 64

#define SAFE_DELETE_ARRAY(p) if(p) { delete [] (p); (p) = NULL; }

#define TRUE 1

#define FALSE 0

/************************************************************************
* @class tfOutput
* @brief tensorflow模型输出的参数结构, 存放numpy array
* @note 由于该结构需要被频繁使用, 故该类通过引用计数ref管理内存, \n
如有内存泄漏, 请联系本人
************************************************************************/
class tfOutput
{
public:
	int nx, ny, nz;			// numpy array的维度信息
protected:
	int *ref;				// 引用计数
	int addref() const { return ++(*ref); }
	int removeref() const { return --(*ref); }
	void destroy()			// 销毁
	{
		if (0 == removeref())
		{
			SAFE_DELETE_ARRAY(ref);
			SAFE_DELETE_ARRAY(feature);
		}
	}
	void assign(const tfOutput &o) {
		nx = o.nx; ny = o.ny; nz = o.nz;
		ref = o.ref;
		feature = o.feature;
		addref();
	}
public:
	float *feature;			// 512维的特征向量

	tfOutput(int x, int y = 1, int z = 1)
	{
		memset(this, 0, sizeof(tfOutput));
		ref = new int(1);
		nx = x; ny = y; nz = z;
		feature = new float[nx * ny * nz];
	}
	~tfOutput()
	{
		destroy();
	}
	tfOutput(const tfOutput &o)
	{
		assign(o);
	}
	tfOutput operator = (const tfOutput &o)
	{
		if (this != &o)// 防止自己赋值给自己
		{
			destroy();// 先清理本对象
			assign(o);// this被o代替
		}
		return *this;
	}
	friend double distance(const tfOutput &v1, const tfOutput &v2) {
		float sum = 0;
		for (int i=0; i<v1.nx; ++i)
		{
			float t = v1.feature[i] - v2.feature[i];
			sum += t*t;
		}
		return sqrt(sum);
	}
};
