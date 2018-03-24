#include "stitch.h"
#define PI 3.1415926535
const float scale = 0.1;
// 构造函数
Stitch::Stitch(){}
// 读入图片
void Stitch::LoadImg(string filepath)
{
    CImg<float> img;
    cout << filepath << endl;
    img.load(filepath.c_str());
    this->img_array.push_back(img);
}
// 处理
void Stitch::Perform()
{
    vector<CImg<float> >cylinder_array;
    for(int i = 0 ; i < img_array.size();++i)
    {
        // 下采样
        CImg<float> img = preprocess(img_array[i]);
        // 柱面投影
        // CImg<float> cylinder = cylinderProjection(img);
        CImg<float> cylinder = img;
        cylinder_array.push_back(cylinder);
        // cylinder.display();
    }

	vector<CImg<float> > stitch_array = cylinder_array;

	int times = 0;

	while(stitch_array.size()!=1)
	{
		RATIO_THRESH = RATIO_THRESH - 0.06*times;
		times ++;
		vector<CImg<float> > temp;
		int i;
		for(i = 0; i < stitch_array.size()/2;++i)
		{
			CImg<float> image = stitching(stitch_array[2*i],stitch_array[2*i+1]);
			if(stitch_array.size() % 2 == 1 && 2*i+2 == stitch_array.size() - 1)
				image = stitching(image,stitch_array[2*i+2]);
			image.display();
			temp.push_back(image);
		}
		stitch_array = temp;
		if(stitch_array.size()==2)
			LAST_TWO = true;
	}
	result = stitch_array[0];

	result.display();
	result.save("../result/test2.jpg");
}
// 拼接两张图
CImg<float> Stitch::stitching(CImg<float>input1,CImg<float>input2)
{
	int height = 0;
	if(input1.height() > input2.height())
	{
		height = input1.height();
	}
	else
	{
		height = input2.height();
	}

	//Filling the float sized output with both input images
	CImg<float> output(input1.width()+input2.width(), height,1,3,0);
	for (int y = 0; y < input1.height(); y++)
	{
		for (int x = 0; x < input1.width(); x++)
		{
			output(x,y,0,0) = input1(x,y,0,0);
			output(x,y,0,1) = input1(x,y,0,1);
			output(x,y,0,2) = input1(x,y,0,2);
		}
	}
	for (int y = 0; y < input2.height(); y++)
	{
		for (int x = input1.width(); x < output.width(); x++)
		{
			output(x,y,0,0) = input2(x-input1.width(),y,0,0);
			output(x,y,0,1) = input2(x-input1.width(),y,0,1);
			output(x,y,0,2) = input2(x-input1.width(),y,0,2);
		}
	}
	
	bestSift best;
	bestSift best2;
	vector<bestSift> matches;
	double meanx = 0;
	double meany = 0;
	vector<double> xEst;
	vector<double> yEst;

	// convert image1 to grayscale
	CImg<float> gray = toGrayScale(input1);
	vector<SiftDescriptor> descriptors1 = Sift::compute_sift(gray);

	// convert image2 to grayscale
	CImg<float> gray2 = toGrayScale(input2);
	vector<SiftDescriptor> descriptors2 = Sift::compute_sift(gray2);

	float lowestDist, lowestDist2;
	float results = 0;

	for(int x = 0; x < descriptors1.size(); x++)
	{
		lowestDist = 10000;
		lowestDist2 = 10000;

		for(int y = 0; y < descriptors2.size(); y++)
		{
			results = descriptCompare(descriptors1[x], descriptors2[y]);
			//Finding the lowest x and y distances for sift differences...

			if(results < lowestDist)
			{
				lowestDist = results;
				best.x = x;
				best.y = y;
				best.value = lowestDist;
			}
			else if(results < lowestDist2)
			{
				lowestDist2 = results;	
				best2.x = x;
				best2.y = y; 
				best2.value = lowestDist2;
			}
		}

		double ratio = best.value/best2.value;

		if ((ratio < RATIO_THRESH)&&(best.value < siftThresh))
		{ 
			if(LAST_TWO == true)
			{
				if(descriptors1[best.x].col < descriptors2[best.y].col)
					continue;
			}
			matches.push_back(best);
			
			//Drawing visualization only 
			
			for(int j=0; j<5; j++)
			{
				for(int k=0; k<5; k++)
				{
					if(j==2 || k==2)
					{
						for(int p=0; p<3; p++)
						{
							output(descriptors1[best.x].col+k, descriptors1[best.x].row+j, 0, p)=0;
							output(descriptors2[best.y].col+k+input1.width(), descriptors2[best.y].row+j, 0, p)=0;
						}
					}
				}
			}
			
			output.draw_line(descriptors1[best.x].col, descriptors1[best.x].row,
					descriptors2[best.y].col+input1.width(), descriptors2[best.y].row, red);
			

			meanx += descriptors1[best.x].col - descriptors2[best.y].col;
			meany += descriptors1[best.x].row - descriptors2[best.y].row;
			
			xEst.push_back(descriptors1[best.x].col - descriptors2[best.y].col);
			yEst.push_back(descriptors1[best.x].row - descriptors2[best.y].row);
			
		}
	}
	output.display();

	cout << "Number of matching sift values: " << matches.size() << endl;
	meanx = meanx / matches.size();
	meany = meany / matches.size();

	cout << "Average X mean displacement: " << meanx << endl;
	cout << "Average Y mean displacement: " << meany << endl;

	double maxX, maxY,curX, curY, curCount;

	double maxCount = 0;
	//Calculating best x distance...
	for (int i = 0; i < matches.size(); i++)
	{
		curCount = 0;
		curX = xEst[i];

		for(int j = 0; j < xEst.size(); j++)
		{
			if(abs(curX - xEst[j]) <= 3)
			{
				curCount++;
			} 
		}

		if(curCount > maxCount)
		{
			maxCount = curCount;
			maxX = curX;
		}
	}

	maxCount = 0;
	//Calculating best y distance...
	for (int i = 0; i < matches.size(); i++)
	{
		curCount = 0;
		//int randomValue = rand() % yEst.size();
		//curY = yEst[randomValue];
		curY = yEst[i];

		for(int j = 0; j < xEst.size(); j++)
		{
			if(abs(curY - yEst[j]) <= 3)
			{
				curCount++;
			} 
		}

		if(curCount > maxCount)
		{
			maxCount = curCount;
			maxY = curY;
		}
	}

	cout << "Average X displacement w/o outliers: " << maxX << endl;
	cout << "Average Y displacement w/o outliers: " << maxY << endl;

	int newWidth = abs(maxX) + input1.width();
	int newHeight = abs(maxY) + input1.height();

	CImg<double> stitchedImage(newWidth, newHeight,1,3,0);

	//draw image 1
	int startX1 = 0;
	int startY1 = 0;

	if(maxX < 0)
	{
		startX1 = abs(maxX);
	}
	if(maxY < 0)
	{
		startY1 = abs(maxY);
	}

	int x1 = 0;
	int y1 = 0;
	for (int y = startY1; y < input1.height()+startY1; y++)
	{
		x1 = 0;
		for (int x = startX1; x < input1.width()+startX1; x++)
		{
			stitchedImage(x,y,0,0) = input1(x1,y1,0,0);
			stitchedImage(x,y,0,1) = input1(x1,y1,0,1);
			stitchedImage(x,y,0,2) = input1(x1,y1,0,2);
			x1++;
		}
		y1++;
	}

	//draw image 2

	int startX2 = 0;
	int startY2 = 0;

	if(maxX > 0)
	{
		startX2 = maxX;
	}
	if(maxY > 0)
	{
		startY2 = maxY;
	}

	x1 = 0;
	y1 = 0;
	for (int y = startY2; y < input2.height()+startY2; y++)
	{
		x1 = 0;
		for (int x = startX2; x < input2.width()+startX2; x++)
		{
			stitchedImage(x,y,0,0) = input2(x1,y1,0,0);
			stitchedImage(x,y,0,1) = input2(x1,y1,0,1);
			stitchedImage(x,y,0,2) = input2(x1,y1,0,2);
			//}
			x1++;
		}
		y1++;
	}
  
	return stitchedImage;
}
double Stitch::descriptCompare(SiftDescriptor d1, SiftDescriptor d2)
{
  
	double total = 0;
	for(int i=0; i<128; i++)
	{
    	total+= pow((d1.descriptor[i] - d2.descriptor[i]),2.0);
	}
  	total = sqrt(total);
 
	return total; 
}
CImg<float> Stitch::preprocess(CImg<float> img)
{
	CImg<float>temp = img;
	return temp.resize(temp._width*scale,temp._height*scale);
}

CImg<float> Stitch::toGrayScale(CImg<float> img)
{
	CImg<float> gray = CImg<float>(img._width,img._height,1,1);
	cimg_forXY(img,x,y)
	{
		float r = img._atXY(x,y,0,0);
		float g = img._atXY(x,y,0,1);
		float b = img._atXY(x,y,0,2);

		int newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
		gray._atXY(x,y) = newValue;
	}
	return gray;
}
	// 
CImg<float> Stitch::cylinderProjection(CImg<float>& input)
{
	int width = input._width;
	int height = input._height;
	int centerX = width / 2;
	int centerY = height / 2;

	CImg<float> output = input;
	float f = width / (2 * tan(PI / 6 / 2)); //视角设为30度

	float theta, pointX, pointY;
	for (int i = 0; i < height; i ++) {
		for (int j = 0; j < width; j ++) {
			theta = asin((j - centerX) / f);
			pointY = f * tan((j - centerX) / f) + centerX;
			pointX = (i - centerY) / cos(theta) + centerY;
			if (pointX >= 0 && pointX <= height && pointY >= 0 && pointY <= width)
			{
				output._atXY(j,i,0,0) = input._atXY(pointY, pointX,0,0);
				output._atXY(j,i,0,1) = input._atXY(pointY, pointX,0,1);
				output._atXY(j,i,0,2) = input._atXY(pointY, pointX,0,2);
			}
			else
			{ 
				output._atXY(j,i,0,0) = 0;
				output._atXY(j,i,0,1) = 0;
				output._atXY(j,i,0,2) = 0;
			}
		}
	}
	return output;

}