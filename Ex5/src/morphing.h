#ifndef MORPHING_H
#define MORPHING_H

#include "CImg.h"
#include <string>
#include <vector>
#include <utility>
using namespace std;
using namespace cimg_library;
enum DIRECTION
{
    VERTICAL,
    HORIZONTAL
};
class Morphing
{
private:
    CImg<unsigned char> img;
    CImg<unsigned char> result;
    vector<pair<int,int> > corner;
    int direction;
    vector<float> trans_matrix;
public:
    Morphing(string,string,vector<pair<int,int> >);
private:
    vector<pair<int,int> > SortCorner(vector<pair<int,int> > &);
    vector<float> ComputeMatrix(vector<pair<int,int> >,vector<pair<int,int> >);
    pair<int, int> Transform(vector<float>, pair<int,int>);
};


#endif