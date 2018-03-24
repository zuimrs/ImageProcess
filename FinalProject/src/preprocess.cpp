#include "preprocess.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define PI 3.14159265
#define gFilterx 5
#define gFiltery 5
#define sigma 1
#define threshold_low 100
#define threshold_high 120
#define theta_size 500

using namespace std;
using namespace cimg_library;

Preprocess::Preprocess(string input,string output)
{
    // 读取图片
    this->point_num = 4;
    this->img.load(input.c_str());
	CImg<float> temp = this->img;
    // 下采样，增加运算速度
    this-> scale = temp._width / 300;
    temp.resize(img._width/scale,img._height/scale);

    // 转灰度图
    CImg<float> gray =this->toGrayScale(temp);
    // this->gray = this->toGrayScale(this->img);

    // 高斯滤波
    this->filter = createFilter(gFilterx,gFiltery,sigma);
    CImg<float> gFiltered = this->useFilter(gray,this->filter);
    // Canny
    CImg<float> angles;
    CImg<float> sFiltered = this->sobel(gFiltered,angles);
    CImg<float> non = this->nonMaxSupp(sFiltered,angles);
	this->thres = this->threshold(non,threshold_low,threshold_high);
    // 霍夫变换
    for(int i = 0 ; i < theta_size; ++i)
    {
        tabSin.push_back(sin(PI*i/(theta_size)));
        tabCos.push_back(cos(PI*i/(theta_size)));
    }
    this->houghLinesTransform(thres);
    this->houghLinesDetect();
    // 检测边界
    this->findEdge();
    this->findPoint();
    this->edge_line.display();

	// morphing
	// 读取并标定角点顺序
	this->sort_corner = this->SortCorner(this->corner);
	// 判断方向
	if(this->direction == VERTICAL)
		this->result = CImg<unsigned char>(210*2,297*2,1,3,0);
	else
		this->result = CImg<unsigned char>(297*2,210*2,1,3,0);
	// 初始化投影坐标
	vector<pair<int,int> > uv;
	uv.push_back(make_pair(0,0));
	uv.push_back(make_pair(0,this->result._height));
	uv.push_back(make_pair(this->result._width,this->result._height));
	uv.push_back(make_pair(this->result._width,0));
	// 计算投影矩阵
	this->trans_matrix = ComputeMatrix(this->sort_corner,uv);
	// 投影
	cimg_forXY(this->result,x,y)
	{
			pair<int, int> point = this->Transform(this->trans_matrix, make_pair(x,y));
			int u = point.first;
			int v = point.second;
			this->result._atXY(x,y,0,0) = this->img._atXY(u,v,0,0);
			this->result._atXY(x,y,0,1) = this->img._atXY(u,v,0,1);
			this->result._atXY(x,y,0,2) = this->img._atXY(u,v,0,2);
	}
	this->result.display();
    gray = this->toGrayScale(this->result);
	//  Canny
    // gray = this->useFilter(gray,this->filter);
    // gray.display();
	sFiltered = this->sobel(gray,angles);
	non = this->nonMaxSupp(sFiltered,angles);
	CImg<float> binary = this->threshold(non,127,180);
	binary.display();
    CImg<bool> se0(3,3),se1(3,3);
    se0.fill(1);   //  Structuring element 1
    se1.fill(0,1,0,1,1,1,0,1,0);
    binary = binary.get_dilate(se0);
	binary.display();
    binary = binary.get_erode(se1);
    binary.display();
    binary = toBinaryScale(binary);
    // // binary = this->useFilter(binary,this->filter);
    binary.save(output.c_str());
}
CImg<float> Preprocess::toBinaryScale(CImg<float> input){
	CImg<float> output =  CImg<float>(input._width,input._height,1,1);
	cimg_forXY(input,x,y)
	{
		if(input._atXY(x,y) == 255){
			output._atXY(x,y) = 0;
		}else{
            output._atXY(x,y) = 255;
        }
	}
	return output;
}
float Preprocess::getMeanPixel(CImg<float> input,int x,int y,int window_size)
{
	float sum = 0;
	int width = input._width;
	int height = input._height;
	for(int i = x - window_size/2; i < x + window_size/2;++i)
	{
		if(i < 0 || i > width)
			continue;
		for(int j = y - window_size/2;j < y+window_size/2;++j)
		{

			if(j < 0 || j > height)
				continue;
			sum += input._atXY(i,j);
		}
	}
	return sum/(window_size*window_size);
}
CImg<float> Preprocess::toGrayScale(CImg<float> input)
{
    CImg<float>output = CImg<float>(input._width,input._height,1,1);
    cimg_forXY(input,x,y)
    {
        float r = input._atXY(x,y,0,0);
        float g = input._atXY(x,y,0,1);
        float b = input._atXY(x,y,0,2);

        int newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
        output._atXY(x,y) = newValue;
    }
	return output;
}

vector<vector<float> > Preprocess::createFilter(int row,int column,float sigmaIn)
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

CImg<float> Preprocess::useFilter(CImg<float> & img_in,vector<vector<float> >& filterIn)
{
    int size = (int)filterIn.size()/2;
	CImg<float> filteredImg(img_in._width , img_in._height , 1,1);
    filteredImg.fill(0);
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

            filteredImg._atXY(i, j) = sum;
		}
	}
	return filteredImg;
}
CImg<float> Preprocess::sobel(CImg<float> & gFiltered,CImg<float>& angles )
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

	CImg<float> filteredImg(gFiltered._width , gFiltered._height ,1,1);
    filteredImg.fill(0);
    angles = CImg<float>(gFiltered._width , gFiltered._height ,1,1); //AngleMap
    angles.fill(0);
	for (int i = size + gFilterx/2; i < gFiltered._width - size - gFilterx/2 ; i++)
	{
		for (int j = size + gFilterx/2; j < gFiltered._height - size - gFilterx/2; j++)
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
            filteredImg._atXY(i, j) = sq2;

            if(sumx==0) //Arctan Fix
                angles._atXY(i, j) = 90;
            else
                angles._atXY(i, j) = atan(sumy/sumx) * 180 / PI;
		}
	}

    return filteredImg;
}

CImg<float> Preprocess::nonMaxSupp(CImg<float> & sFiltered,CImg<float> & angles)
{
    CImg<float> nonMaxSupped (sFiltered._width, sFiltered._height, 1,1);
    nonMaxSupped.fill(0);
    for (int i=1; i< sFiltered._width - 1; i++) {
        for (int j=1; j<sFiltered._height - 1; j++) {
            float Tangent = angles._atXY(i,j);

            nonMaxSupped._atXY(i, j) = sFiltered._atXY(i,j);
            //Horizontal Edge
            if ((-22.5 < Tangent) && (Tangent <= 22.5))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i,j-1)))
                    nonMaxSupped._atXY(i, j) = 0;
            }
            //Vertical Edge
            if ((Tangent <= -67.5) || (67.5 < Tangent))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j)))
                    nonMaxSupped._atXY(i, j) = 0;
            }

            //-45 Degree Edge
            if ((-67.5 < Tangent) && (Tangent <= -22.5))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j-1)))
                    nonMaxSupped._atXY(i, j) = 0;
            }

            //45 Degree Edge
            if ((22.5 < Tangent) && (Tangent <= 67.5))
            {
                if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j-1)))
                    nonMaxSupped._atXY(i, j) = 0;
            }
        }
    }
    return nonMaxSupped;
}

CImg<float> Preprocess::threshold(CImg<float> & imgin,int low, int high)
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
// 映射到霍夫空间
void Preprocess::houghLinesTransform(CImg<float> &imgin)
{
    int width = imgin._width;
    int height = imgin._height;
    int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
    int rows = theta_size,cols = max_length*2;
    this->houghspace = CImg<float>(cols,rows);
    this->houghspace.fill(0);

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
            int r = int(x0 * tabCos[i] + y0 * tabSin[i] ) + max_length;

            if(r < 0 || r >=  max_length*2)
            {
                continue;
            }
            this->houghspace ._atXY(r,i) += 1;
        }
    }


}

bool compare(pair<int,int> a,pair<int,int> b)
{
    return a.second > b.second;
}
void Preprocess::houghLinesDetect()
{
    int width = this->houghspace._width;
    int height = this->houghspace._height;

    // 霍夫空间取极大值点
    int window_size = 40;
    for(int i = 0 ; i < height ;i += window_size/2)
    {
        for(int j = 0 ; j < width ;j += window_size/2)
        {
            int max = getMaxValue(this->houghspace,window_size,i,j);
            int y_max = i + window_size < height?i + window_size : height;
            int x_max = j + window_size < width?j + window_size:width;
            bool is_max = true;
            for(int y = i; y < i + window_size;++y)
            {
                for(int x = j;x < j + window_size;++x)
                {
                    if(this->houghspace._atXY(x,y) < max)
                        this->houghspace._atXY(x,y) = 0;
                    else if(!is_max)
                    {
                        this->houghspace._atXY(x,y) = 0;
                    }
                    else
                    {
                        is_max = false;
                    }
                }
            }
        }
    }

    // 所有的极大值点保存到line数组中
    cimg_forXY(this->houghspace,x,y)
    {
        if(this->houghspace._atXY(x,y)>0)
            this->lines.push_back(make_pair(y*width+x,this->houghspace._atXY(x,y)));
    }
    // 根据权重从大到小排序
    sort(this->lines.begin(),this->lines.end(),compare);
}

// 获取一个窗口内最大权重
int Preprocess::getMaxValue(CImg<float> &img,int &size,int &y,int &x)
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
void Preprocess::findEdge()
{

    int max_length = this->houghspace._width / 2;

    this->edge_line = CImg<float>(this->thres._width,this->thres._height,1,1,0);
    // 取前point_num条边
    for(int i = 0 ; i < this->point_num; ++i)
    {
        int n = this->lines[i].first;
        int theta = n / this->houghspace._width;
        int r = n % this->houghspace._width - max_length;
        this->edge.push_back(make_pair(theta,r));
        cout <<"theta:"<< (theta*1.0/500)*180 << " r:" << r <<" weight:" << this->lines[i].second<< endl;
        for(int x = 0 ; x < this->thres._width  ;++x)
        {
            for(int y = 0 ; y < this->thres._height  ; ++y )
            {
                int x0 = x - this->thres._width/2 ;
                int y0 = this->thres._height/2 - y ;
                if(r == int(x0 * tabCos[theta] + y0 * tabSin[theta] ))
                {
                    this->edge_line._atXY(x,y) += 255.0/2;
                }
            }
        }
    }

}

void Preprocess::findPoint()
{
    int width = this->thres._width;
    int height = this->thres._height;
    int max_length = this->houghspace._width / 2;
    int n1,n2,r1,r2,theta1,theta2;
    double x,y;
    unsigned char red[3] = {255,0,0};
    for(int i = 0; i < this->point_num;++i){
        for(int j=i+1;j< this->point_num;++j){
            r1 = this->edge[i].second;
            r2 = this->edge[j].second;
            theta1 =this->edge[i].first;
            theta2 =this->edge[j].first;
            if(abs(theta1-theta2)<40)
                continue;
            y = (r2)*1.0/tabCos[int(theta2)] - (r1)*1.0/tabCos[int(theta1)];
            y = y*1.0/(tabSin[int(theta2)]/tabCos[int(theta2)] - tabSin[int(theta1)]/tabCos[int(theta1)]);
            x = r1/tabCos[int(theta1)] - y*tabSin[int(theta1)]/tabCos[int(theta1)];
            cout <<"x: " << (x+width/2) <<"  y:" << (height/2-y) << endl;
            this->corner.push_back(make_pair(int(scale*(x+width/2)),int(scale*(height/2-y))));
            // this->img.draw_circle(int(scale*(x+width/2)) ,int(scale*(height/2-y)) ,3,red);
        }
    }


}
vector<pair<int,int> > Preprocess::SortCorner(vector<pair<int,int> > & corner)
{
    vector<pair<int,int> >result(4);
    int center_x = 0,center_y = 0;
    for(int i = 0 ; i < corner.size(); ++i)
    {
        center_x += corner[i].first;
        center_y += corner[i].second;
    }
    center_x /= corner.size();
    center_y /= corner.size();
    int count = 0;
    for(int i = 0 ; i < corner.size(); ++i)
    {
        if(corner[i].first <= center_x &&
            corner[i].second <= center_y)
        {
            count ++;
        }
    }
    if(count ==1 )
    {
        for(int i = 0 ; i < corner.size(); ++i)
        {
            if(corner[i].first <= center_x &&
                corner[i].second <= center_y)
                result[0] = corner[i];
            else if(corner[i].first <= center_x &&
                corner[i].second >= center_y)
                result[1] = corner[i];
            else if(corner[i].first >= center_x &&
                corner[i].second >= center_y)
                result[2] = corner[i];
            else if(corner[i].first >= center_x &&
                corner[i].second <= center_y)
                result[3] = corner[i];
        }
        int delta_x = abs(result[0].first - center_x);
        int delta_y = abs(result[0].second - center_y);
        this->direction = delta_x < delta_y?VERTICAL:HORIZONTAL;
    }else if(count == 2)
    {
        vector<pair<int,int> >left;
        vector<pair<int,int> >right;
        for(int i = 0 ; i < corner.size(); ++i)
        {
            if(corner[i].first <= center_x &&
                corner[i].second <= center_y)
            {
                left.push_back(corner[i]);
            }
            else if(corner[i].first >= center_x &&
                corner[i].second >= center_y)
            {
                right.push_back(corner[i]);
            }
        }
        result[0] = left[0].first > left[1].first ? left[0]:left[1];
        result[1] = left[0].first < left[1].first ? left[0]:left[1];
        result[2] = right[0].first < right[1].first ? right[0]:right[1];
        result[3] = right[0].first > right[1].first ? right[0]:right[1];
        int delta_x = abs(result[0].first - center_x);
        int delta_y = abs(result[0].second - center_y);
        this->direction = delta_x > delta_y?VERTICAL:HORIZONTAL;
    }
    return result;
}

vector<float> Preprocess::ComputeMatrix(vector<pair<int,int> > uv,vector<pair<int,int> >xy)
{
    	//get the 8 point
	float u1 = uv[0].first;
	float u2 = uv[1].first;
	float u3 = uv[2].first;
	float u4 = uv[3].first;
	float x1 = xy[0].first;
	float x2 = xy[1].first;
	float x3 = xy[2].first;
	float x4 = xy[3].first;
	float v1 = uv[0].second;
	float v2 = uv[1].second;
	float v3 = uv[2].second;
	float v4 = uv[3].second;
	float y1 = xy[0].second;
	float y2 = xy[1].second;
	float y3 = xy[2].second;
	float y4 = xy[3].second;

	float A[8][9] = {
					{x1, y1, 1, 0, 0, 0, -u1*x1, -u1*y1, u1},
					{0, 0, 0, x1, y1, 1, -v1*x1, -v1*y1, v1},
					{x2, y2, 1, 0, 0, 0, -u2*x2, -u2*y2, u2},
					{0, 0, 0, x2, y2, 1, -v2*x2, -v2*y2, v2},
					{x3, y3, 1, 0, 0, 0, -u3*x3, -u3*y3, u3},
					{0, 0, 0, x3, y3, 1, -v3*x3, -v3*y3, v3},
					{x4, y4, 1, 0, 0, 0, -u4*x4, -u4*y4, u4},
					{0, 0, 0, x4, y4, 1, -v4*x4, -v4*y4, v4},
				  };

	if(A[0][0] == 0)
	{
		for(int i = 1;i < 8;i++)
		{
			if(A[i][0] != 0)
			{
				//swap the row and break
				float temp;
				for(int j = 0;j < 9;j++)
				{
					temp = A[0][j];
					A[0][j] = A[i][j];
					A[i][j] = temp;
				}
				break;
			}
		}
	}

	for(int i = 1;i < 8;i++)
	{
		float max = 0;
		int index;
		for(int j = i-1;j < 8;j++)
		{
			if(abs(A[j][i-1]) > max)
			{
				max = abs(A[j][i-1]);
				index = j;
			}
		}
		for(int j = 0;j < 9;j++)
		{
			float temp = A[i-1][j];
			A[i-1][j] = A[index][j];
			A[index][j] = temp;
		}
		for(int j = i;j < 8;j++)
		{
			float x = A[j][i-1] / A[i-1][i-1];
			for(int k = i-1;k < 9;k++)
			{
				A[j][k] = A[j][k] - x*A[i-1][k];
			}
		}
		if(A[i][i] == 0)
		{
			for(int j = i+1;j < 8;j++)
			{
				if(A[j][i] != 0)
				{
					float temp;
					for(int k = 0;k < 9;k++)
					{
						temp = A[i][k];
						A[i][k] = A[j][k];
						A[j][k] = temp;
					}
					break;
				}
			}
		}
	}
	vector<float> result(8);
	for(int i = 7;i >= 0;i--)
	{
		float b = A[i][8];
		for(int j = 7;j >= i+1;j--)
			b = b - A[i][j] * result[j];
		result[i] = b/A[i][i];
	}
	return result;
}

pair<int, int> Preprocess::Transform(vector<float> matrix, pair<int,int> point)
{
    int u = point.first;
    int v = point.second;
	float q = matrix[6]*u + matrix[7]*v + 1;
	float x = (matrix[0]*u + matrix[1]*v + matrix[2])/q;
	float y = (matrix[3]*u + matrix[4]*v + matrix[5])/q;
    // 四舍五入，最近临
	return pair<int, int>((int)x+0.5f, (int)y+0.5f);
}
