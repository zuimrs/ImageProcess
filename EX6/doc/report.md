# Ex6：Panorama Image Stitching
###输入图像:
dataset1:4张图像
dataset2:18张图像（环形全景）
###输出图像：
拼接好的图像
###算法：
1. sift特征提取
2. 两张图片sift特征匹配
3. 求解原坐标和映射后的新坐标之间的映射。
4. 递归的拼接两张图片
###结果：
全部请见result文件夹下图片。<br>
dataset1：
![fg1](/home/zuimrs/Code/ANN/EX6/result/test1.jpg  "fg1")
dataset2：
![fg2](/home/zuimrs/Code/ANN/EX6/result/test2.jpg  "fg2")