#if !BUILD_DLL

#include "FeatureSDK.h"
#include "Classify.h"

// 注意调用SetPythonHome修改当前使用的python.
// main: 将src目录下的图片进行特征提取，保存为"result.bin"
// 对这些图片利用CW算法进行分类，分类结果保存在classify目录.
int main(int argc, const char *argv[])
{
	const char *path = argc > 1 ? argv[1] : "./src";
	if (!Init("D:\\ProgramData\\Anaconda3\\envs\\tfcpu", 
		"index", "Feature"))
		return -1;

	Results m = LoadFile("./result.bin");
	if (m.size() == 0)
	{
		m = getFeature(path);
		SaveFile(m, "./result.bin");
	}
	
	// 阈值越小，分类数越多，阈值越大，分类数越少.
	Classify(m, 0.6);
	system("PAUSE");
	return 0;
}

#endif
