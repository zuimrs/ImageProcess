# Ex5：Morphing
###输入图像:
普通 A 4 打印纸,上面可能有手写笔记或者打印内容,但是拍照时可能角度
不正。
###输出图像：
已经矫正好的标准普通 A 4 纸(长宽比),并裁掉无用的其他内容,只保留完整
A 4 纸张。
###算法：
1. canny边缘提取
2. 霍夫变换确定边界和角点
3. 求解角点原坐标和映射后的新坐标之间的映射矩阵。
4. 对新图像的每个像素位置与该映射矩阵相乘，获得在原图中的位置。
5. 最近邻原则填充新图像的像素值。
###结果（部分）：
全部请见result文件夹下图片(图片可以以更大尺寸存储，只需修改程序中的一个参数，这里为减小体积，图片尺寸较小)。<br>
![1](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143133.jpg  "1")
![2](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143145.jpg  "2")
![3](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143211.jpg  "3")
![4](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143220.jpg  "4")
![5](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143334.jpg  "5")
![6](/home/zuimrs/Code/ANN/Ex5/result/IMG_20150320_143345.jpg  "6")