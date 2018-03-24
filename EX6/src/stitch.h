#ifndef _STITCH_H_
#define _STITCH_H_

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "CImg.h"
#include "Sift.h"

using namespace std;
using namespace cimg_library;

struct bestSift
{
  int x;
  int y;
  double value;
};

struct imageDetails
{
  double value;
  string name;
  CImg<double> image;
};
const unsigned char red[] = { 255,0,0 };

const double siftThresh = 250;
static double RATIO_THRESH = 0.8;
static bool LAST_TWO = false;
class Stitch
{
private:
    vector<CImg<float> > img_array;
    CImg<float> result;
public:
    Stitch();
    //读入图片
    void LoadImg(string);
    void Perform();
private:
    CImg<float> preprocess(CImg<float>);
    CImg<float> toGrayScale(CImg<float>);
    CImg<float> cylinderProjection(CImg<float>& input);
    CImg<float> stitching(CImg<float>,CImg<float>);
    double descriptCompare(SiftDescriptor d1, SiftDescriptor d2);

};


#endif
