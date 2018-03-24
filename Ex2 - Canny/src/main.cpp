#include "canny.h"


int main()
{
    const char * filepath1 = "../img/bigben.bmp";
    const char * filepath2 = "../img/lena.bmp";
    const char * filepath3 = "../img/stpietro.bmp";
    const char * filepath4 = "../img/twows.bmp";
    Canny cny1 (filepath1);
    Canny cny2 (filepath2);
    Canny cny3 (filepath3);
    Canny cny4 (filepath4);
    
    return 0;
}