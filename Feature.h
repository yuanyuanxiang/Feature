#pragma once

#include "pyCaller.h"

/************************************************************************
* @class Feature
* @brief 对图像进行特征提取的类
************************************************************************/
class Feature
{
private:
	int w, h; // 图片尺寸
	std::string module; // python脚本名
	std::string func; // python函数名
	pyCaller m_py; // python调用者

public:
	bool Init(const char *python, const char *mod, const char *fun);
	void Uninit();

	Feature();
	~Feature();

	tfOutput features(const cv::Mat &m, int x, int y, int z);
};

extern Feature F;
