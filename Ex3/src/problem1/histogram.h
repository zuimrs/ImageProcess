#ifndef _HISTOGRAM_H
#define _HISTOGRAM_H

#include "CImg.h"
#include <vector>
using namespace std;
using namespace cimg_library;

class Histogram
{
private:
    CImg<float> img;
    CImg<float> img_gray;
    CImg<float> img_histGray;
    CImg<float> img_histRGB;
    CImg<float> img_histHSI;

public:
    Histogram(const char *);
private:
    void toGrayScale();
    void GrayEqualization();
    void RGBEqualization();
    void HSIEqualization();
    float cumsum(vector<float> & ,int );


};

#endif