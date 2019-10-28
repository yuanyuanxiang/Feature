#pragma once
#include <vector>
#include <string>
#include "config.h"

// @see https://github.com/yuanyuanxiang/public
// 请将此项目克隆
#include "../public/fileop.h"

// 图片及其特征
struct Result
{
	std::string name;
	tfOutput feature;
	Result(const std::string &n, const tfOutput &f):name(n), feature(f) { }
};

// 数组-图片及其特征
typedef std::vector<Result> Results;

// 进行特征提取
Results getFeature(const std::string &folder_path);

// 分别进行保存-即保存文件列表为文件，保存特征为另一个文件
void SaveFile(const Results &m, const std::string &name, const std::string &feats);

// 保存结果到文件
void SaveFile(const Results & m, const std::string &file);

// 分类
void Classify(const Results &m, double threshold);

// 从文件加载结果
const Results LoadFile(const std::string &file);
