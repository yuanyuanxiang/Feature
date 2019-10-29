# Feature

## 本项目依赖

### python + keras + tensorflow 64位

cd 至项目所在目录，该目录下有一个py文件"index.py",
调用命令行 python index.py，确认不会报任何错误。如果报错了，
就将错误一一解决，例如拷贝dll,pip安装依赖包等。

### OpenCV 3.2 64位

需要把python头文件目录添加到项目附加包含目录，需要将python库目录
添加到项目附加库目录。一种推荐的做法是，修改"Microsoft.Cpp.x64.user"，
这样所有项目会继承这个属性管理器的设置。

### Visual Studio 2015

## 运行方法

### Feature arg1, arg2, arg3

其中参数1为图片目录，参数2为分割阈值，参数3为python所在目录.
程序会将arg1目录下的图片进行特征提取，保存在"result"目录。
然后对这些图片利用CW算法进行分类，分类结果保存在classify目录.

arg2的推荐阈值为0.6.

## 问题反馈

如有任何意见、建议、八阿哥，请联系本人，谢谢!
2019-10-29
