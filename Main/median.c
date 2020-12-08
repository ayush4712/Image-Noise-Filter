#include<stdio.h>
#include<stdlib.h>
#include "image.h"

int findMedian(unsigned char* window, int length){
    int i, key, j; 
    for (i = 1; i < length; i++) 
    { 
        key = window[i]; 
        j = i - 1; 
        while (j >= 0 && window[j] > key) 
        { 
            window[j + 1] = window[j]; 
            j = j - 1; 
        } 
        window[j + 1] = key; 
    }
    return window[length/2]; 
}

Image applyMedian(Image image, int windowSize){
    int ih = image.height;
    int iw = image.width;
    unsigned char imageArr[ih][iw][3];
    unsigned char* pixels = image.pixels;
    
    pixToArray(pixels, ih, iw, imageArr, image.rgb);
    double **imageH = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        imageH[i] = (double *)malloc(iw * sizeof(double));
    double **imageS = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        imageS[i] = (double *)malloc(iw * sizeof(double));
    double **imageV = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        imageV[i] = (double *)malloc(iw * sizeof(double));

    convHSV(ih, iw, imageArr, imageH, imageS, imageV);
    double **newImageH = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        newImageH[i] = (double *)malloc(iw * sizeof(double));
    double **newImageS = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        newImageS[i] = (double *)malloc(iw * sizeof(double));
    double **newImageV = (double **)malloc(ih * sizeof(double *)); 
    for (int i=0; i<ih; i++) 
        newImageV[i] = (double *)malloc(iw * sizeof(double));
    
    int nh = ih - windowSize + 1;
    int nw = iw - windowSize + 1;
    unsigned char newImageArr[ih][iw][3];
    
    unsigned char windowH[windowSize*windowSize], windowS[windowSize*windowSize], windowV[windowSize*windowSize];

    for(int i = 0;i<nh;i++){
        for(int j = 0;j<nw;j++){
            int x = 0;
            for(int k = i;k<i+windowSize;k++){
                int y = 0;
                for(int l = j;l<j+windowSize;l++){
                    windowH[x+y]=imageH[k][l];
                    windowS[x+y]=imageS[k][l];
                    windowV[x+y]=imageV[k][l];
                    y++;
                }
                x += windowSize;
            }
            int medH = findMedian(windowH, windowSize*windowSize);
            int medS = findMedian(windowS, windowSize*windowSize);
            int medV = findMedian(windowV, windowSize*windowSize);
            int avg = 1;
            newImageH[i][j] = (unsigned char) medH;
            newImageS[i][j] = (unsigned char) medS;
            newImageV[i][j] = (unsigned char) medV;
        }
    }
    backRGB(ih, iw, newImageH, newImageS, newImageV, newImageArr);
    unsigned char* new_pixels = (unsigned char*)malloc(ih*iw*3);

    Image newImage;
    newImage.height = ih;
    newImage.width = iw;
    newImage.bytesPerPixel = 3;
    newImage.rgb = image.rgb;
    newImage.xRes = image.xRes;
    newImage.yRes = image.yRes;
    free(image.pixels);

    arrayToPix(new_pixels, ih, iw, newImageArr);
    newImage.pixels = new_pixels;

    return newImage;
    
}

int main(){
    Image image = ReadImage("./images/lena_bw_speckle.bmp");
    Image newImage = applyMedian(image, 5);
    WriteImage("./images/speck.bmp", newImage);
    return 0;
}

//gcc median.c readwrite.c pixarr.c  -o med