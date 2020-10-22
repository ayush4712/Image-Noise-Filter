#include<stdio.h>
#include<math.h>
#include "image.h"


int main(){
    char fileName[100] = "./images/lena_2.bmp";
    //printf("Enter the file location: "); scanf("%s", fileName);
    Image image = ReadImage(fileName);
    Image newimage = applyGaussFilter(image,5,5);
    WriteImage("./images/lena_gauss.bmp", newimage);
    return 0;
}

