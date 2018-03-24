#ifndef _COLORIZATION_H
#define _COLORIZATION_H

#include "CImg.h"
#include <vector>
using namespace cimg_library;
using namespace std;

class Colorization
{
private:
    CImg<float> img;
    CImg<float> reference;
    CImg<float> output;
public:
    Colorization(const char * ,const char *);
private:
    void transfer();
    void compute(CImg<float> & img,float* mean,float* std);

};


#endif