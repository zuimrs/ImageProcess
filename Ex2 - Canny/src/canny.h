#ifndef _CANNY_H
#define _CANNY_H

#include "CImg.h"
#include <vector>
using namespace cimg_library;
using namespace std;
class Canny
{
private:
    CImg<float> img;
    CImg<float> grayscaled;
    CImg<float> gFiltered;
    CImg<float> sFiltered;
    CImg<float> angles;
    CImg<float> non;
    CImg<float> thres;
public:
    Canny(const char *);
    CImg<float> toGrayScale();
    vector<vector<float> > createFilter(int,int,float);
    CImg<float> useFilter(CImg<float>,vector<vector<float> >);
    CImg<float> sobel();
    CImg<float> nonMaxSupp();
    CImg<float> threshold(CImg<float>,int,int);


};




#endif