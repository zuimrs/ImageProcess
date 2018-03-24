#include "histogram.h"
#include <iostream>
#include <vector>

using namespace cimg_library;
using namespace std;

Histogram::Histogram(const char * filepath)
{
    this->img.load_bmp(filepath);
    this->img.display("origin");
    this->toGrayScale();
    this->img_gray.display("gray");
    this->GrayEqualization();
    this->img_histGray.display();
    this->RGBEqualization();
    this->img_histRGB.display("rgb equalization");
    this->HSIEqualization();
    this->img_histHSI.display("hsv equalization");

}
void Histogram::toGrayScale()
{
    CImg<float> gray(this->img._width,this->img._height,1,1);
    cimg_forXY(this->img,x,y)
    {
        float r = this->img._atXY(x,y,0,0);
        float g = this->img._atXY(x,y,0,1);
        float b = this->img._atXY(x,y,0,2);

        float newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
        gray._atXY(x,y) = newValue;
    }
    this->img_gray = gray;
}
void Histogram::GrayEqualization()
{
    this->img_histGray = this->img_gray;
    // 直方图和累计直方图
    vector<float> hist(256,0),cumsum_hist(256,0);
    // 灰度最大值和最小值
    float max = 0.0f,min = 255.0f;
    // 直方图
    cimg_forXY(this->img_histGray,x,y)
    {
        int n = this->img_histGray._atXY(x,y);
        hist[n] = hist[n] + 1;
        if(n > max)
        {
            max = n;
        }
        if(n < min)
        {
            min = n;
        }
    }
    int width = this->img._width;
    int height = this->img._height;

    // 归一化
    for (int i = 0; i < 256 ; ++i)
    {
        hist[i] = hist[i] / (width*height*1.0f);
    }

    // 累计归一化直方图
    for (int i = 0 ; i < 256 ; ++i)
    {
        cumsum_hist[i] = cumsum(hist,i);
    }

    // 均衡化
    cimg_forXY(this->img,x,y)
    {
        float n = cumsum_hist[this->img._atXY(x,y)]
                        *(max - min) + min;
        this->img_histGray._atXY(x,y) = n;
    }

}
void Histogram::RGBEqualization()
{
    this->img_histRGB = this->img;
    // 直方图和累计直方图
    vector<vector<float> > hist;
    vector<float> r_hist(256,0);
    vector<float> g_hist(256,0);
    vector<float> b_hist(256,0);
    hist.push_back(r_hist);
    hist.push_back(g_hist);
    hist.push_back(b_hist);
    vector<vector<float> > cumsum_hist = hist;
    // 灰度最大值和最小值
    vector<float> max(3,0.0f);
    vector<float> min(3,255.0f);

    // 直方图
    cimg_forXYC(this->img,x,y,c)
    {
        int n = this->img._atXY(x,y,0,c);
        hist[c][n] = hist[c][n] + 1.0f;
        if(n > max[c])
        {
            max[c] = n;
        }
        if(n < min[c])
        {
            min[c] = n;
        }
    }
    int width = this->img._width;
    int height = this->img._height;

    // 归一化
    for (int i = 0 ; i < 256 ; ++i)
    {
        hist[0][i] = hist[0][i] / (width*height*1.0f);
        hist[1][i] = hist[1][i] / (width*height*1.0f);
        hist[2][i] = hist[2][i] / (width*height*1.0f);
    }
    // 累计归一化直方图
    for (int i = 0 ; i < 256 ; ++i)
    {
        cumsum_hist[0][i] = cumsum(hist[0],i);
        cumsum_hist[1][i] = cumsum(hist[1],i);
        cumsum_hist[2][i] = cumsum(hist[2],i);
    }
    // 均衡化
    cimg_forXYC(this->img,x,y,c)
    {
        float n = cumsum_hist[c][this->img._atXY(x,y,0,c)]  
                         *(max[c] - min[c]) + min[c];
        this->img_histRGB._atXY(x,y,0,c) = n ;
    }
}

void Histogram::HSIEqualization()
{
    this->img_histHSI = this->img.get_RGBtoHSI();
    // 直方图和累计直方图
    vector<float> hist(256,0),cumsum_hist(256,0);
    // 灰度最大值和最小值
    float max = 0.0f,min = 255.0f;
    // 直方图
    cimg_forXY(this->img_histHSI,x,y)
    {
        // cout << this->img_histHSI._atXY(x,y,0,2)<<endl;
        int n = this->img_histHSI._atXY(x,y,0,2) * 255.0f;
        hist[n] = hist[n] + 1;
        if(n > max)
        {
            max = n;
        }
        if(n < min)
        {
            min = n;
        }
    }
    int width = this->img._width;
    int height = this->img._height;

    // 归一化
    for (int i = 0; i < 256 ; ++i)
    {
        hist[i] = hist[i] / (width*height*1.0f);
    }

    // 累计归一化直方图
    for (int i = 0 ; i < 256 ; ++i)
    {
        cumsum_hist[i] = cumsum(hist,i);
    }

    // 均衡化
    cimg_forXY(this->img,x,y)
    {
        float n = cumsum_hist[this->img._atXY(x,y,0,2)]
                        *(max - min) + min;
        this->img_histHSI._atXY(x,y,0,2) = n/255.0f;
    }

    // 转换为RGB
    this->img_histHSI = this->img_histHSI.get_HSItoRGB();
}


float Histogram::cumsum(vector<float> & hist,int L)
{
    float sum = 0.0f;
    for (int i = 0 ; i <= L; ++i)
        sum = sum + hist[i];
    return sum;
}
