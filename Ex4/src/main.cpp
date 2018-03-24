#include "edge_detect.h"
#include <iostream>
using namespace std;


int main()
{
    //test
    string input,output;
    input = "./dataset/01.jpg";
    output = "../result/01.jpg";
    EdgeDetect * edge1 = new EdgeDetect(input,output,4);
    input = "./dataset/02.jpg";
    output = "../result/02.jpg";    
    EdgeDetect * edge2 = new EdgeDetect(input,output,4);
    input = "./dataset/03.jpg";
    output = "../result/03.jpg";   
    EdgeDetect * edge3 = new EdgeDetect(input,output,4);
    input = "./dataset/04.jpg";
    output = "../result/04.jpg";   
    EdgeDetect * edge4 = new EdgeDetect(input,output,6);
    input = "./dataset/05.jpg";
    output = "../result/05.jpg";
    EdgeDetect * edge5 = new EdgeDetect(input,output,4);
    input = "./dataset/06.jpg";
    output = "../result/06.jpg";
    EdgeDetect * edge6 = new EdgeDetect(input,output,4);
}