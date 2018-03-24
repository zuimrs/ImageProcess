// #include "edge_detect.h"
// #include "morphing.h"
#include "preprocess.h"
//#include "gaussin.h"
#include <iostream>
using namespace std;

vector<string> getFiles(string cate_dir);

int main()
{
    string basepath = "./dataset/";
    string outputpath = "./result/";
    vector<string> filenames = getFiles(basepath);
    for(int i = 0; i < filenames.size();++i)
    {
        Preprocess * pp = new Preprocess(basepath+filenames[i],outputpath+filenames[i]);
        // EdgeDetect * edgedetect = new EdgeDetect(basepath+filenames[i],outputpath+filenames[i],4);
        // Morphing * morphing = new Morphing(basepath+filenames[i],outputpath+filenames[i],edgedetect->getCornerPoint());
    }

}

vector<string> getFiles(string cate_dir)
{
    vector<string> files;//存放文件名

#ifdef WIN32
    _finddata_t file;
    long lf;
    //输入文件夹路径
    if ((lf=_findfirst(cate_dir.c_str(), &file)) == -1) {
        cout<<cate_dir<<" not found!!!"<<endl;
    } else {
        while(_findnext(lf, &file) == 0) {
            //输出文件名
            //cout<<file.name<<endl;
            if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
                continue;
            files.push_back(file.name);
        }
    }
    _findclose(lf);
#endif

#ifdef linux
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(cate_dir.c_str())) == NULL)
        {
        perror("Open dir error...");
                exit(1);
        }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
        else if(ptr->d_type == 8)    ///file
            files.push_back(ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            continue;
        else if(ptr->d_type == 4)    ///dir
        {
            files.push_back(ptr->d_name);
        }
    }
    closedir(dir);
#endif

    //排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}
