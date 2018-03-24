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
    vector<int> hough_weight;
    vector<int> line_weight;
    vector<int> sort_line_weight;
public:
    CImg<float> img;
    CImg<float> gray;
    CImg<float> thres;
    CImg<float> hough_img;
    CImg<float> edge;
    CImg<float> output;
public:
    EdgeDetect(string,string,int);
    int getMaxHough(CImg<float> &,int &,int &,int &);
    void drawEdge();
    void drawPoint();
private:
    void toGrayScale();
    vector<vector<float> > createFilter(int ,int ,float);
    CImg<float> useFilter(CImg<float> & ,vector<vector<float> >& ) ;   
    CImg<float> sobel(CImg<float> & ,CImg<float>&);
    CImg<float> nonMaxSupp(CImg<float> & ,CImg<float> & );
    CImg<float> threshold(CImg<float> & imgin,int low, int high);
    void houghLinesTransform(CImg<float> &);
    void houghLinesDetect( );

};


#endif