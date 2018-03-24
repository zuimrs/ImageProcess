#include"CImg.h"
#include <iostream>
using namespace cimg_library;
int main()
{
    //input
    CImg<unsigned char> img("../img/1.bmp");

    //get height and width
    int height = img._height;
    int width = img._width;
    
    img.display();

    //transform
    cimg_forXY(img,x,y)
    {
        if(img._atXY(x,y,0,0) == 0 &&
        img._atXY(x,y,0,1) == 0 &&
        img._atXY(x,y,0,2) == 0)
        {
            img._atXY(x,y,0,1) = 255;
        }
        if(img._atXY(x,y,0,0) == 255 &&
        img._atXY(x,y,0,1) == 255 &&
        img._atXY(x,y,0,2) == 255)
        {
            img._atXY(x,y,1) = 0;
            img._atXY(x,y,2) = 0;            
        }
    }   
    img.display();
    //transform
    cimg_forXY(img,x,y)
    {
        int a = x - 50;
        int b = y - 50;
        if( a*a + b*b <= 50*50 )
        {
            img._atXY(x,y,0) = 0;
            img._atXY(x,y,1) = 0;
            img._atXY(x,y,2) = 255;
        }
        
    }
    img.display();
    //transform
    cimg_forXY(img,x,y)
    {
        int a = x - 50;
        int b = y - 50;
        if( a*a + b*b <= 3*3 )
        {
            img._atXY(x,y,0) = 255;
            img._atXY(x,y,1) = 255;
            img._atXY(x,y,2) = 0;
        }
        
    }
    img.display();

    return 0;    
}