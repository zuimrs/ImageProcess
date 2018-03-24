#include "colorization.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace cimg_library;

Colorization::Colorization(const char * path1,const char * path2)
{
    this->img.load_bmp(path1);
    this->img.display("cover");
    this->reference.load_bmp(path2);
    this->reference.display("reference");
    this->output = this->img;
    this->transfer();
    this->output.display("output");
}

void Colorization::transfer()
{
    CImg<float> img_lab = this->img.get_RGBtoLab();
    CImg<float> reference_lab = this->reference.get_RGBtoLab();
    float mean_a[3],mean_b[3],std_a[3],std_b[3];
    this->compute(img_lab,mean_a,std_a);
    this->compute(reference_lab,mean_b,std_b);
    cimg_forXYC(img_lab,x,y,c)
    {   
        float val = img_lab._atXY(x,y,0,c);
        this->output._atXY(x,y,0,c) = (val - mean_a[c])*std_b[c]/std_a[c] + mean_b[c];
    }
    this->output = this->output.get_LabtoRGB();
}
void Colorization::compute(CImg<float> & img,float* mean,float* std)
{
    mean[0] = mean[1] = mean[2] = 0;
    std[0] = std[1] = std[2] = 0;
    cimg_forXYC(img,x,y,c)
    {
        mean[c] = mean[c] + img._atXY(x,y,0,c);
    }
    mean[0] = mean[0] / (img._height * img._width);
    mean[1] = mean[1] / (img._height * img._width);
    mean[2] = mean[2] / (img._height * img._width);
    cimg_forXYC(img,x,y,c)
    {
        float val = img._atXY(x,y,0,c)-mean[c];
        std[c] = std[c] + val*val;
    }
    std[0] = std[0] / (img._height * img._width);
    std[1] = std[1] / (img._height * img._width);
    std[2] = std[2] / (img._height * img._width);
    
}
