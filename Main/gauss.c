#include <math.h>
#include<stdlib.h>
#include<stdio.h>
#include "image.h"

void generateGaussFilter(int kHeight, int kWidth, double gk[kHeight][kWidth]){
    double stdv = 2.0;
    double r, s = 2.0 * stdv * stdv;  // Assigning standard deviation 
    double sum = 0.0;   // Initialization of sum for normalization
    int hVal = kHeight/2, wVal = kWidth/2;
    for (int x = -hVal; x <= hVal; x++)
    {
        for(int y = -wVal; y <= wVal; y++)
        {
            r = sqrt(x*x + y*y);
            gk[x + hVal][y + wVal] = (exp(-(r*r)/s))/(M_PI * s);
            sum += gk[x + hVal][y + wVal];
        }
    }

    for(int i = 0; i < kHeight; ++i){ // Loop to normalize the kernel
        for(int j = 0; j < kWidth; ++j){
            gk[i][j] /= sum;
        }
    }
}

Image applyGaussFilter(Image image, int kHeight, int kWidth){
    double gk[kHeight][kWidth];
    generateGaussFilter( kHeight, kWidth, gk);
    
    int ih = image.height;
    int iw = image.width;
    unsigned char imageArr[ih][iw][3];
    unsigned char* pixels = image.pixels;
    pixToArray(pixels, ih, iw, imageArr, image.rgb);
    
    int newImageHeight = ih - kHeight + 1, newImageWidth = iw - kWidth + 1;
    
    unsigned char filtered_image[newImageHeight][newImageWidth][3];
    int i = 0, j = 0, h = 0, w = 0;
    for (i=0 ; i<newImageHeight ; i++) {
        for (j=0 ; j<newImageWidth ; j++) {
            for (h=i ; h<i+kHeight ; h++) {
                for (w=j ; w<j+kWidth ; w++) {
                    filtered_image[i][j][0] += gk[h-i][w-j]*imageArr[h][w][0];
                    filtered_image[i][j][1] += gk[h-i][w-j]*imageArr[h][w][1];
                    filtered_image[i][j][2] += gk[h-i][w-j]*imageArr[h][w][2];
                }
            }
        }
    }


    unsigned char* new_pixels = (unsigned char*)malloc(newImageHeight*newImageWidth*3);
 
    Image newImage;
    newImage.height = newImageHeight;
    newImage.width = newImageWidth;
    newImage.bytesPerPixel = 3;
    newImage.rgb = image.rgb;
    newImage.xRes = image.xRes;
    newImage.yRes = image.yRes;
    free(image.pixels);

    arrayToPix(new_pixels, newImageHeight, newImageWidth, filtered_image);
    newImage.pixels = new_pixels;

    return newImage;
}

int main(){
    char fileName[100] = "./images/lship.bmp";
    Image im = ReadImage(fileName);
    Image newimage = applyGaussFilter(im, 5, 5);
    
    WriteImage("./lpl.bmp", newimage);
    printf("cool\n");
    return 0;
}

//gcc gauss.c readwrite.c pixarr.c -o gauss