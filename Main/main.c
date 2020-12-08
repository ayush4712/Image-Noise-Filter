#include<stdio.h>
#include<math.h>
#include "image.h"


int main(){
    char fileName[100] = "../img/12003.bmp";
    //printf("Enter the file location: "); scanf("%s", fileName);
    Image im = ReadImage(fileName);
    printf("cool\n");
    Image newimage = applyGaussFilter(im, 5, 5);
    
    WriteImage("./12003med.bmp", newimage);
    printf("cool\n");
    return 0;
}

