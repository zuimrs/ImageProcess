#include "edge_detect.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define PI 3.14159265
#define gFilterx 5
#define gFiltery 5
#define sigma 1
#define threshold_low 120
#define threshold_high 140
#define theta_size 360
#define theta_step 360.0/theta_size
#define window_sz 60
using namespace std;
using namespace cimg_library;

EdgeDetect::EdgeDetect(string input,string output,int num)
{
    // 读取图片
    this->point_num = num;
    this->img.load(input.c_str());
    int w = img._width,h = img._height;
    this->img.resize(600,800);
    if(this->img.empty())
    {
        std::cout << "Could not open or find the image" <<std:: endl;
        return;
    }
    this->img.display("origin");
    this->output = this->img;

    // 转灰度图
    this->toGrayScale();
    //this->gray.display("gray");

    // 高斯滤波
    this->filter = createFilter(gFilterx,gFiltery,sigma);
    CImg<float> gFiltered = this->useFilter(this->gray,this->filter);

    // Canny
    CImg<float> angles;
    CImg<float> sFiltered = this->sobel(gFiltered,angles);
    CImg<float> non = this->nonMaxSupp(sFiltered,angles);
    this->thres = this->threshold(non,threshold_low,threshold_high);
    this->thres.display();

    // 霍夫变换
    for(int i = 0 ; i < theta_size; ++i)
    {
        tabSin.push_back(sin(2*PI*i/(theta_size)));
        tabCos.push_back(cos(2*PI*i/(theta_size)));
    }
    this->houghLinesTransform(this->thres);
    // this->hough_img.display("hough");
    this->houghLinesDetect();

    // 标定直线和点
    this->drawEdge();
    this->drawPoint();

    // 输出
    this->output.resize(w,h);
    this->output.save(output.c_str());
}

void EdgeDetect::toGrayScale()
{
    this->gray = CImg<float>(this->img._width,this->img._height,1,1);
    cimg_forXY(this->img,x,y)
    {
        float r = this->img._atXY(x,y,0,0);
        float g = this->img._atXY(x,y,0,1);
        float b = this->img._atXY(x,y,0,2);

        int newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
        this->gray._atXY(x,y) = newValue;
    }   
}

vector<vector<float> > EdgeDetect::createFilter(int row,int column,float sigmaIn)
{
    vector<vector<float> > filter;

	for (int i = 0; i < row; i++)
	{
        vector<float> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
		filter.push_back(col);
	}

	float coordSum = 0;
	float constant = 2.0 * sigmaIn * sigmaIn;

	// Sum is for normalization
	float sum = 0.0;

	for (int x = - row/2; x <= row/2; x++)
	{
		for (int y = -column/2; y <= column/2; y++)
		{
			coordSum = (x*x + y*y);
			filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
			sum += filter[x + row/2][y + column/2];
		}
	}

	// Normalize the Filter
	for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            filter[i][j] /= sum;

	return filter;
}

CImg<float> EdgeDetect::useFilter(CImg<float> & img_in,vector<vector<float> >& filterIn)
{
    int size = (int)filterIn.size()/2;
	CImg<float> filteredImg(img_in._width - 2*size, img_in._height - 2*size, 1,1);
	for (int i = size; i < img_in._width - size; i++)
	{
		for (int j = size; j < img_in._height - size; j++)
		{
			float sum = 0;
            
			for (int x = 0; x < filterIn.size(); x++)
				for (int y = 0; y < filterIn.size(); y++)
				{
                    sum += filterIn[x][y] * (float)(img_in._atXY(i + x - size, j + y - size));
				}
            
            filteredImg._atXY(i-size, j-size) = sum;
		}

	}
	return filteredImg;
}
CImg<float> EdgeDetect::sobel(CImg<float> & gFiltered,CImg<float>& angles )
{

    //Sobel X Filter
    float x1[] = {-1.0, 0, 1.0};
    float x2[] = {-2.0, 0, 2.0};
    float x3[] = {-1.0, 0, 1.0};

    vector<vector<float> > xFilter(3);
    xFilter[0].assign(x1, x1+3);
    xFilter[1].assign(x2, x2+3);
    xFilter[2].assign(x3, x3+3);

    //Sobel Y Filter
    float y1[] = {-1.0, -2.0, -1.0};
    float y2[] = {0, 0, 0};
    float y3[] = {1.0, 2.0, 1.0};
    
    vector<vector<float> > yFilter(3);
    yFilter[0].assign(y1, y1+3);
    yFilter[1].assign(y2, y2+3);
    yFilter[2].assign(y3, y3+3);
    
    //Limit Size
    int size = (int)xFilter.size()/2;
    
	CImg<float> filteredImg(gFiltered._width - 2*size, gFiltered._height - 2*size, 1,1);   
    angles = CImg<float>(gFiltered._width - 2*size, gFiltered._height - 2*size, 1,1); //AngleMap
	for (int i = size; i < gFiltered._width - size; i++)
	{
		for (int j = size; j < gFiltered._height - size; j++)
		{
			float sumx = 0;
            float sumy = 0;
            
			for (int x = 0; x < xFilter.size(); x++)
				for (int y = 0; y < xFilter.size(); y++)
				{
                    sumx += xFilter[y][x] * (float)(gFiltered._atXY(i + x - size, j + y - size)); //Sobel_X Filter Value
                    sumy += yFilter[y][x] * (float)(gFiltered._atXY(i + x - size, j + y - size)); //Sobel_Y Filter Value
				}
            float sumxsq = sumx*sumx;
            float sumysq = sumy*sumy;
            
            float sq2 = sqrt(sumxsq + sumysq);
            
            if(sq2 > 255) //Unsigned Char Fix
                sq2 =255;
            filteredImg._atXY(i-size, j-size) = sq2;
 
            if(sumx==0) //Arctan Fix
                angles._atXY(i-size, j-size) = 90;
            else
                angles._atXY(i-size, j-size) = atan(sumy/sumx);
		}
	}
    
    return filteredImg;
}

CImg<float> EdgeDetect::nonMaxSupp(CImg<float> & sFiltered,CImg<float> & angles)
{
    CImg<float> nonMaxSupped (sFiltered._width-2, sFiltered._height-2, 1,1);
    for (int i=1; i< sFiltered._width - 1; i++) {
        for (int j=1; j<sFiltered._height - 1; j++) {
            float Tangent = angles._atXY(i,j);

            nonMaxSupped._atXY(i-1, j-1) = sFiltered._atXY(i,j);
            //Horizontal Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i,j-1)))
                    nonMaxSupped._atXY(i-1, j-1) = 0;
            }
            //Vertical Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j)))
                    nonMaxSupped._atXY(i-1, j-1) = 0;
            }
            
            //-45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j-1)))
                    nonMaxSupped._atXY(i-1, j-1) = 0;
            }
            
            //45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j-1)))
                    nonMaxSupped._atXY(i-1, j-1) = 0;
            }
        }
    }
    return nonMaxSupped;
}

CImg<float> EdgeDetect::threshold(CImg<float> & imgin,int low, int high)
{
    if(low > 255)
        low = 255;
    if(high > 255)
        high = 255;
    
    CImg<float> EdgeMat(imgin._width, imgin._height, 1,1);
    
    for (int i=0; i<imgin._width; i++) 
    {
        for (int j = 0; j<imgin._height; j++) 
        {
            EdgeMat._atXY(i,j) = imgin._atXY(i,j);
            if(EdgeMat._atXY(i,j) > high)
                EdgeMat._atXY(i,j) = 255;
            else if(EdgeMat._atXY(i,j) < low)
                EdgeMat._atXY(i,j) = 0;
            else
            {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x=i-1; x < i+2; x++) 
                {
                    for (int y = j-1; y<j+2; y++) 
                    {
                        if(x <= 0 || y <= 0 || EdgeMat._height || y > EdgeMat._width) //Out of bounds
                            continue;
                        else
                        {
                            if(EdgeMat._atXY(x,y) > high)
                            {
                                EdgeMat._atXY(i,j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if(EdgeMat._atXY(x,y) <= high && EdgeMat._atXY(x,y) >= low)
                                anyBetween = true;
                        }
                    }
                    if(anyHigh)
                        break;
                }
                if(!anyHigh && anyBetween)
                    for (int x=i-2; x < i+3; x++) 
                    {
                        for (int y = j-1; y<j+3; y++) 
                        {
                            if(x < 0 || y < 0 || x > EdgeMat._height || y > EdgeMat._width) //Out of bounds
                                continue;
                            else
                            {
                                if(EdgeMat._atXY(x,y) > high)
                                {
                                    EdgeMat._atXY(i,j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if(anyHigh)
                            break;
                    }
                if(!anyHigh)
                    EdgeMat._atXY(i,j) = 0;
            }
        }
    }
    return EdgeMat;
}

void EdgeDetect::houghLinesTransform(CImg<float> &imgin)
{
    int width = imgin._width;
    int height = imgin._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
    int rows = theta_size,cols = max_length;
    this->hough_img = CImg<float>(cols,rows);
    this->hough_img.fill(0);
    this->hough_weight = vector<int>(rows*cols,0);


    int r = 0;
    cimg_forXY(imgin,x,y)
    {
        int p = imgin._atXY(x,y);

        if(p == 0 )
        {
            continue;
        }
        int x0 = x - width/2;
        int y0 = height/2 - y;
        for(int i = 0 ; i < theta_size;++i)
        {
            r = int(x0 * tabCos[i] + y0 * tabSin[i]);
            if(r < 0 || r >=  max_length)
            {
                continue;
            }
            this->hough_weight[i*cols+r] += 1;
            this->hough_img ._atXY(r,i) += 1; 
        }
    }


}

void EdgeDetect::houghLinesDetect()
{
    int width = this->hough_img._width;
    int height = this->hough_img._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
   
    int window_size = window_sz;
    int max;
    for(int i = 0 ; i < height ;i += window_size/2)
    {
        for(int j = 0 ; j < width ;j += window_size/2)
        {
            max = getMaxHough(this->hough_img,window_size,i,j);
            int y_max = i + window_size < height?i + window_size : height;
            int x_max = j + window_size < width?j + window_size:width;
            for(int y = i; y < i + window_size;++y)
            {
                for(int x = j;x < j + window_size;++x)
                {
                    if(this->hough_img._atXY(x,y) < max)
                        this->hough_img._atXY(x,y) = 0;
                }
            }
        }
    }

    cimg_forXY(this->hough_img,x,y)
    {
        if(this->hough_img._atXY(x,y)==0)
            continue;
        bool is_new_line = true;
        if(is_new_line)
        {
            lines.push_back(make_pair(y,x));
            line_weight.push_back(hough_img._atXY(x,y));
        }
    }
}
int EdgeDetect::getMaxHough(CImg<float> &img,int &size,int &y,int &x)
{
    int max = 0;
    int width = x+size > img._width?img._width:x+size;
    int height = y + size > img._height?img._height:y + size;
    for(int j = x; j < width;++j )
    {
        for(int i = y ; i < height ;++i)
        {
            if(img._atXY(j,i) > max)
                max = img._atXY(j,i);
        }
    }
    return max;
}
void EdgeDetect::drawEdge()
{
    int width = this->img._width;
    int height = this->img._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));

    this->edge = CImg<float>(width,height,1,1,0);
    this->sort_line_weight = this->line_weight;
    sort(sort_line_weight.begin(),sort_line_weight.end(),greater<int>());
    
    vector<pair<int,int> >top;
    for(int i = 0 ; i < this->point_num; ++i )
    {
        int n = sort_line_weight[i];
        vector<int>::iterator iter = find(line_weight.begin(),line_weight.end(),n);
        int index = iter - line_weight.begin();
        top.push_back(lines[index]);
    }
    // print
    for(int i = 0 ; i < top.size();++i)
    {
        cout<<"k:" << top[i].first<<" " << top[i].second<<"->"<<line_weight[i]<<endl;
    }
    for(int k = 0 ; k < top.size();++k)
    {
        int theta = top[k].first ;
        int r = top[k].second ; 
        
        cimg_forXY(this->edge,x,y)
        {
            int x0 = x - width/2;
            int y0 = height/2 - y;
            if(r == int(x0*tabCos[theta] + y0*tabSin[theta]))
            {
                this->edge._atXY(x,y) += 255.0/2;
                this->output._atXY(x,y,0,2) = 255;
            }
        }
    }
}

void EdgeDetect::drawPoint()
{ 
    unsigned char red[3] = {255,0,0};
    for(int y = 0 ; y < output._height-1;++y)
    {
        for(int x = 0; x < output._width-1;++x)
        {
            int sum[4];
            sum[0] = this->edge._atXY(x,y);
            sum[1] = this->edge._atXY(x+1,y);
            sum[2] = this->edge._atXY(x,y+1);
            sum[3] = this->edge._atXY(x+1,y+1);
            
            if(sum[0]+sum[1]+sum[2]+sum[3]>= 255.0/2*3)
            {
                output.draw_circle(x,y,3,red);            
            }

        }
    }
    output.display();
}