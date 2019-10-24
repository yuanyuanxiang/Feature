#pragma once

#include "config.h"

#ifdef _MSC_VER
	#ifdef BUILD_LIB
		#define FBC_API
	#else
		#if BUILD_DLL
			#define  SDK  __declspec(dllimport)
		#else
			#define  SDK  __declspec(dllexport)
		#endif
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/************************************************************************
	* @func Init
	* @brief 初始化资源
	* @param[in] python python所在位置
	* @param[in] mod python脚本名
	* @param[in] func python函数名
	* @return 是否成功
	************************************************************************/
	SDK bool  Init(const char *python, const char *mod, const char *fun);

	/************************************************************************
	* @func Uninit
	* @brief 释放资源
	************************************************************************/
	SDK void  Uninit();

	/************************************************************************
	* @func features
	* @brief 提取图像特征
	* @param[in] m 原始图像
	* @param[in] x numpy维度x
	* @param[in] y numpy维度y
	* @param[in] z numpy维度z
	* @return numpy array
	************************************************************************/
	SDK tfOutput features(const cv::Mat &m, int x, int y, int z);


#ifdef __cplusplus
}
#endif
