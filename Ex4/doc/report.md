##Ex 4 : 霍夫变换边缘检测
###输入图像:
普通 A 4 打印纸,上面可能有手写笔记或者打印内容,但是拍照时可能角度
不正。(参考 Dataset1)
###输出:
1.  图像的边缘
2.  计算 A4 纸边缘的各直线方程
3.  提取 A4 纸的四个角点
**结果如下：**
![figure 01](/home/zuimrs/Code/ANN/Ex4/result/01.jpg  "figure 01")
![figure 02](/home/zuimrs/Code/ANN/Ex4/result/02.jpg  "figure 02")
![figure 03](/home/zuimrs/Code/ANN/Ex4/result/03.jpg  "figure 03")
![figure 04](/home/zuimrs/Code/ANN/Ex4/result/04.jpg  "figure 04")
![figure 05](/home/zuimrs/Code/ANN/Ex4/result/05.jpg  "figure 05")
![figure 06](/home/zuimrs/Code/ANN/Ex4/result/06.jpg  "figure 06")
**结果分析：**
由于光照不均匀，Figure 04图右上角A4纸一角信号较强，掩盖所求A4纸较暗部分的边缘信息，所以最后结果同样将其标出，如果使用同态滤波，可能会得到更好的效果。
###思考：
####如何在保证精度的情况下提高运行速度？
1. 下采样，在下采样的图片中进行边缘提取，在标定到原图中。
2. 随机霍夫变换，canny算法求得的边缘点中随机一部分进行霍夫变换，另一部分舍弃。
3. 极坐标方程使用cos和sin计算，可以用一个数组保存中间值，使用时查表。