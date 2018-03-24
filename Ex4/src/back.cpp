
#include "CImg.h"
#include "edge_detect.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define PI 3.14159265
#define gFilterx 5
#define gFiltery 5
#define sigma 1
#define threshold_low 100
#define threshold_high 120
#define theta_size 360
#define theta_width 180/theta_size
#define max_thres 0.18

using namespace std;
using namespace cimg_library;


EdgeDetect::EdgeDetect(string filename)
{   
    // 读取图片
    this->img.load(filename.c_str());
    this->img = this->img.resize(600,800);
    this->img_detect = this->img;
    if(this->img.empty())
    {
        std::cout << "Could not open or find the image" <<std:: endl;
        return;
    }
    this->img.display("origin");
    // 转灰度图
    CImg<unsigned char> gray = this->toGrayScale();
    CImg<float> grayscaled = gray;
    // 高斯滤波
    cout << "Gauss Filter"<<endl;
    vector<vector<float> >filter = createFilter(gFilterx,gFiltery,sigma);
    CImg<float> gFiltered = this->useFilter(grayscaled,filter);
    // Canny边缘提取
    cout << "Canny" << endl;
    CImg<float> angles;
    CImg<float> sFiltered = this->sobel(gFiltered,angles);
    // sFiltered.display("sobel");
    CImg<float> non = this->nonMaxSupp(sFiltered,angles);
    // non.display("non max supp");
    CImg<float> thres = this->threshold(non,threshold_low,threshold_high);
    //thres.display("threshold");

    // 计算sin，cos值并记录
    for(int i = 0 ; i < theta_size; ++i)
    {
        tabSin.push_back(sin(PI*i*theta_width/180.0));
        tabCos.push_back(cos(PI*i*theta_width/180.0));
    }
    // 霍夫变换
    int max_hough = 0;
    vector<vector<int> > houghSpace = houghLinesTransform(thres,max_hough);
    CImg<float> gFiltered = this->useFilter(this->img_hough,filter);
    CImg<float> sFiltered2 = this->sobel(gFiltered2,this->img_hough);
    this->img_hough = this->nonMaxSupp(sFiltered2,this->img_hough);
    this->img_hough.display("non max");
    // 阈值处理
    vector<int> hough;
    vector<pair<int,int> >hough_line_array = houghFilter(houghSpace,max_hough,hough);
    // 绘制直线 
    this->edge =  drawEdge(hough_line_array,this->img_detect,hough);
    this->drawPoint(this->edge,this->img_detect);
}



vector<vector<int> > EdgeDetect::houghLinesTransform(CImg<unsigned char>  imgin,int & max_hough)
{
    int width = imgin._width;
    int height = imgin._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
    
    this->img_hough = CImg<float>(2*max_length,theta_size);

    vector <vector<int> > houghspace;
    vector<int> row;
    for(int j = 0 ; j < 2*max_length; ++j)
    {
        row.push_back(0);
    }
    for(int i = 0; i < theta_size;++i)
    {
        houghspace.push_back(row);
    }

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
            r += max_length;
            if(r < 0 || r >= 2 * max_length)
            {
                continue;
            }
            houghspace[i][r] = houghspace[i][r] + 1;
            this->img_hough ._atXY(r,i) += 1; 
            if(houghspace[i][r] > max_hough)
                max_hough = houghspace[i][r];
        }
    }
    cout << "max_hough = " << max_hough << endl;

    this->img_hough.display("hough");
    return houghspace;
}

vector<pair<int,int> > EdgeDetect::houghFilter(vector<vector<int> > & houghspace,int & max_hough,vector<int> & hough)
{
    int thres = max_thres * max_hough;
    int max_length = houghspace[0].size()/2;
    vector<pair<int,int> > line_array;
    for(int row = 0 ; row < houghspace.size();++row)
    {
        for(int col = 0 ; col < houghspace[0].size(); ++col)
        {
            if(houghspace[row][col] < thres)
                continue;
            bool is_new_line = true;
            for(int k = 0 ; k < line_array.size();++k)
            {
                int theta = line_array[k].first;
                int r = line_array[k].second;
                if((abs(theta - row)< theta_size/8
                    && abs(r - col)< 100)
                    || (2*theta_size -theta -  row  < theta_size/8 
                    && abs(abs(r-max_length)-abs(col - max_length))<100 )) 
                {

                    if(houghspace[row][col] > houghspace[theta][r])
                    {
                        line_array[k].first = row;
                        line_array[k].second = col;
                        hough[k] = houghspace[row][col];
                    }
                    is_new_line = false;
                }
            }
            if(is_new_line == true)
            {
                line_array.push_back(make_pair(row,col));
                hough.push_back(houghspace[row][col]);
            }
        }
    }
    return line_array;
}
CImg<unsigned char> EdgeDetect::drawEdge(vector<pair<int,int> > &line_array,CImg<unsigned char>imgin,vector<int> &hough)
{
    int width = imgin._width;
    int height = imgin._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
    
    CImg<unsigned char> edge(width,height,1,1);
    edge.fill(0);


    // int count = line_array.size()>4?line_array.size()-4:0;
    // vector<int>ordered  = hough;
    // sort(ordered.begin(),ordered.end());
    // for(int i = 0 ; i < count; ++i)
    // {
    //     int n = ordered[i];
    //     vector<int>::iterator iter =  find(hough.begin(),hough.end(),n);
    //     int index = iter - hough.begin();
    //     line_array.erase(line_array.begin()+index,line_array.begin()+index + 1);
    //     hough.erase(iter,iter+1);
    // }

    for(int i = 0 ; i < line_array.size();++i)
    {
        cout<<"k:" << line_array[i].first<<" " << line_array[i].second<<"->"<<hough[i]<<endl;
    }
    for(int k = 0 ; k < line_array.size();++k)
    {
        int theta = line_array[k].first;
        int r = line_array[k].second - max_length; 
        
        cimg_forXY(edge,x,y)
        {
            int x0 = x - width/2;
            int y0 = height/2 - y;
            if(r == int(x0*tabCos[theta] + y0*tabSin[theta]))
            {
                edge._atXY(x,y) = 255;
            }
        }
    }
    edge.display("line");
    return edge;
}
void EdgeDetect::drawPoint(CImg<unsigned char> &edge,CImg<unsigned char>&imgin )
{ 
    vector<float> row(5,1);
    vector<vector<float> > filter;
    for(int i = 0 ; i < 5;++i)
    {
        filter.push_back(row);
    }
    filter[0][1] = filter[0][3] = filter[1][0] = filter[1][2]
    = filter[1][4] = filter[3][0] = filter[3][2] = filter[3][4]
    = filter[4][1] = filter[4][3] = 0;
    CImg<float> edge_f = edge;
    CImg<float> point = this->useFilter(edge_f,filter);
    point.display();
    unsigned char red[3] =  {255,0,0};;
    cimg_forXY(imgin,x,y)
    {
        if(point._atXY(x,y)>=1700)
        {
            imgin.draw_circle(x,y,10,red);            
        }
    }
    imgin.display();
}
