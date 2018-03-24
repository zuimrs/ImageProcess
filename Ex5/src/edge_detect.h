#ifndef EDGE_DETECT_H_
#define EdGE_DETECT_H_

#include "CImg.h"
#include <vector>
#include <string>
#include <utility>
using namespace std;
using namespace cimg_library;


class EdgeDetect
{
private:
    int point_num;
    vector<float> tabSin;
    vector<float> tabCos;
    vector<vector<float> >filter;
    vector<pair<int,int> >lines;
    vector<pair<int,int> >edge;
    vector<pair<int,int> >corner;
    CImg<float> img;
    CImg<float> gray;
    CImg<float> thres;
    CImg<float> houghspace;
    CImg<float> edge_line;
    CImg<float> img_temp;
public:
    vector<pair<int,int> >getCornerPoint(){return this->corner;}
    EdgeDetect(string,string,int);
private:
    void toGrayScale();
    vector<vector<float> > createFilter(int ,int ,float);
    CImg<float> useFilter(CImg<float> & ,vector<vector<float> >& ) ;   
    CImg<float> sobel(CImg<float> & ,CImg<float>&);
    CImg<float> nonMaxSupp(CImg<float> & ,CImg<float> & );
    CImg<float> threshold(CImg<float> & imgin,int low, int high);
    void houghLinesTransform(CImg<float> &);
    void houghLinesDetect( );
    int getMaxValue(CImg<float> &,int &,int &,int &);
    void findEdge();
    void findPoint();

};


#endif