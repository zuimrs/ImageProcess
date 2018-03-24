#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include "CImg.h"
#include <vector>
#include <string>
#include <utility>
using namespace std;
using namespace cimg_library;

enum DIRECTION
{
    VERTICAL,
    HORIZONTAL
};
class Preprocess
{
private:
    int point_num;
    float scale;
    vector<float> tabSin;
    vector<float> tabCos;
    vector<vector<float> >filter;
    vector<pair<int,int> >lines;
    vector<pair<int,int> >edge;
    vector<pair<int,int> >corner;
    CImg<float> img;
    CImg<float> thres;
    CImg<float> houghspace;
    CImg<float> edge_line;
	// morphing
	CImg<unsigned char> result;
	vector<pair<int,int> > sort_corner;
	int direction;
	vector<float> trans_matrix;
public:
    vector<pair<int,int> >getCornerPoint(){return this->corner;}
    Preprocess(string,string);
private:
    CImg<float> toGrayScale(CImg<float>);
	CImg<float> toBinaryScale(CImg<float>);
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
	//Morphing
    vector<pair<int,int> > SortCorner(vector<pair<int,int> > &);
    vector<float> ComputeMatrix(vector<pair<int,int> >,vector<pair<int,int> >);
    pair<int, int> Transform(vector<float>, pair<int,int>);
	float getMeanPixel(CImg<float> input,int x,int y,int window_size);

};


#endif
