#include "FeatureSDK.h"
#include "Feature.h"

extern Feature F;

bool Init(const char *python, const char *mod, const char *fun)
{
	return F.Init(python, mod, fun);
}

void Uninit()
{
	return F.Uninit();
}

tfOutput features(const cv::Mat & m, int x, int y, int z)
{
	return F.features(m, x, y, z);
}
