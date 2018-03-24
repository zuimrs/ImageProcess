#Final Project
##项目要求
1. 校正图像为标准的 A4 图(作业 3 的代码)；
2. 用 Adaboost 或者 SVM 训练一个手写体数字的分类器(作业 6 的代码)；
3. 切割字符;
4. 识别并输出连串数字（如手机号码、邮编等）。
##编程语言
   Python，C++
##算法描述
###1.边缘检测
####1.1下采样
双线性插值将图片宽度缩放至300像素，加快边缘检测运算速度。
####1.2Canny边缘检测
通过Canny算法检测边缘.
####1.3Hough变换
通过Hough变换提取目标纸张的4条边缘.
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/01.png  "01")
###2.投影变换
####2.1计算边缘交点
####2.2通过4交点坐标计算投影矩阵
####2.3投影
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/02.png  "01")
###3.分割预处理
####3.1 转灰度图
####3.2 Canny算法二值化图像
通过Canny提取数字边缘，避免普通二值化易受光照等影响。
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/03.png  "01")
####3.3腐蚀和膨胀
通过腐蚀和膨胀，去除噪声，使数字更明显。
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/04.png  "01")
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/05.png  "01")
###4.训练分类器
在Python下通过调用sklearn库实例化一个svm分类器，输入为28*28的二值图，输出为分类结果。
尝试过基于ANN的分类器，但效果差。
####4.1训练
利用mnist数据集中的50000张图片作为训练样本。

	def train(clf, samples = 50000):
	  tr, _, _ = wrap_data()
	  X = []
	  y  =[]
	  counter = 0
	  for img in tr:
	    if (counter > samples):
	      break
	    counter += 1
	    data, label = img
	    X.append(data.ravel().tolist())
	    y.append(label)
	  print "train set: %d " %(len(X))
	  clf.fit(X,y)
	  return clf
####4.2测试
利用mnist数据集中的10000张图片作为测试样本。

	def validate(clf,samples = 10000):
	  _,test,_ = wrap_data()
	  X = []
	  y  =[]
	  counter = 0
	  for img in test:
	    if (counter > samples):
	      break
	    counter += 1
	    data, label = img
	    X.append(data.ravel().tolist())
	    y.append(label)
	  print "validate set: %d " %(len(X))
	  return clf.score(X,y)
	  
####**Mnist取50000为训练集10000为测试集，svm正确率94.11%**
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/06.png  "01")
###5.OCR
####5.1 数字切分
首先识别图像的轮廓，通过轮廓迭代，放弃所有完全包含在其他矩形中的矩形，只添加不包含在其他矩形 中和不超过图像宽度的好的矩形。
分类器需要feed28*28的图片，但是例如“1”这种狭长的数字不能简单的插值到28*28，所以首先确定矩形的宽和高分别为w和h(w<h),生成h*h的正方形图片，将矩形填充至正方形中心。
#####分割出的字符：
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/b03.png  "01")
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/b04.png  "01")
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/b05.png  "01")
#####切分结果：
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/07.png  "01")
####5.2 分类器预测
将分割出的字符插值到28*28，使用svm分类器预测，并将结果输出。
![01](/home/zuimrs/Code/CVPR/FinalProject/doc/08.png  "01")
###结果分析
数字“7”与Mnist训练集差别较大，经常被误分类成5，而且连在一起的数字很难切分，故存在错误。