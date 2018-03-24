
#include "canny.h"
#include <iostream>
#include "CImg.h"
#include <vector>

#define gFilterx 3
#define gFiltery 3
#define sigma 1
#define threshold_low 30
#define threshold_high 50


using namespace cimg_library;
Canny::Canny(const char * filename)
{   
    this->img.load(filename);
    if(this->img.empty())
    {
        std::cout << "Could not open or find the image" <<std:: endl;
        return;
    }

    vector<vector<float> >filter = createFilter(gFilterx,gFiltery,sigma);

    //Print filter
    for (int i = 0; i < filter.size(); ++i)
    {
        for (int j = 0; j < filter[i].size(); ++j)
        {
            cout << filter[i][j] << " ";
        }
    }
    cout << endl;

    this->grayscaled = this->toGrayScale();
    this->gFiltered = this->useFilter(this->grayscaled,filter);
    this->sFiltered = this->sobel();

    this->non = this->nonMaxSupp();
    this->thres = this->threshold(this->non,threshold_low,threshold_high);

    this->img.display("Original");
    this->grayscaled.display("GrayScaled");
    this->gFiltered.display("Gaussian Blur");
    this->sFiltered.display("Sobel Filtered");
    this->non.display("Non-Maxima Supp");
    this->thres.display("Final");
}

CImg<float> Canny::toGrayScale()
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
    return gray;
}

vector<vector<float> > Canny::createFilter(int row,int column,float sigmaIn)
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

CImg<float> Canny::useFilter(CImg<float> img_in,vector<vector<float> >filterIn)
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

CImg<float> Canny::sobel()
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
    float y1[] = {1.0, 2.0, 1.0};
    float y2[] = {0, 0, 0};
    float y3[] = {-1.0, -2.0, -1.0};
    
    vector<vector<float> > yFilter(3);
    yFilter[0].assign(y1, y1+3);
    yFilter[1].assign(y2, y2+3);
    yFilter[2].assign(y3, y3+3);
    
    //Limit Size
    int size = (int)xFilter.size()/2;
    
	CImg<float> filteredImg(gFiltered._width - 2*size, gFiltered._height - 2*size, 1,1);
    
    CImg<float> tmp(gFiltered._width - 2*size, gFiltered._height - 2*size, 1,1); //AngleMap
    this->angles = tmp;
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
                angles._atXY(i-size, j-size) = atan(sumy/sumx)*360/3.1415926;
		}
	}
    
    return filteredImg;
}

CImg<float> Canny::nonMaxSupp()
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

CImg<float> Canny::threshold(CImg<float> imgin,int low, int high)
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