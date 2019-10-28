#if !BUILD_DLL

#include "FeatureSDK.h"
#include "Classify.h"
#include <direct.h>

// 获取目录名称.
std::string getDirName(const char *dir) {
	if (*dir)
	{
		char buf[_MAX_PATH], *p = buf;
		strcpy_s(buf, dir);
		while (*p) ++p;
		if (*(p - 1) == '\\' || *(p - 1) == '/') { --p; *p = 0; }
		while (p != buf && *p != '\\' && *p != '/') --p;
		std::string name = p+1;
		return name;
	}
	return "src";
}

// 注意调用SetPythonHome修改当前使用的python.
// main: 将src目录下的图片进行特征提取，保存为"result.bin"
// 对这些图片利用CW算法进行分类，分类结果保存在classify目录.
// 参数1， 参数2， 参数3：参数1为图片目录，参数2为分割阈值.
// 参数3为python所在目录.
// 此程序将生成result目录，把已经计算的结果放在里面.
int main(int argc, const char *argv[])
{
	const char *path = argc > 1 ? argv[1] : "./src";
	const float thresh = argc > 2 ? atof(argv[2]) : 0.6f;
	const char *python = argc > 3 ? argv[3] : 
		"D:\\ProgramData\\Anaconda3\\envs\\tfcpu";
	printf("Processing \"%s\".\n", path);

	_mkdir("./result");
	std::string bin = "./result/" + getDirName(path) + ".bin";
	Results m = LoadFile(bin);
	if (m.size() == 0)
	{
		if (!Init(python, "index", "Feature")) {
			system("PAUSE");
			return -1;
		}
		printf("Getting features - powered by VGG-16.\n");
		m = getFeature(path);
		SaveFile(m, bin);
	}
	
	// 阈值越小，分类数越多，阈值越大，分类数越少.
	if (m.size())
	{
		std::string classify = "./classify/";
		std::string targetDir = getFileName(path);
		removeDir(classify + targetDir);
		Classify(m, thresh);
	}
#ifdef _DEBUG
	system("PAUSE");
#endif
	return 0;
}

#endif
