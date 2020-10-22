#include<stdio.h>
#include "image.h"


void pixToArray(unsigned char *pixels, int height, int width, unsigned char image[height][width][3], RGBQUAD* rgb){
        unsigned char *curr = pixels;
        int i = 0, j = 0;
        if(rgb == NULL){
                for(i=0;i<height;i++){
                        for(j=0;j<width;j++){
                                unsigned char curB = *(curr), curG = *(curr+1), curR = *(curr+2); 
                                image[i][j][0]=curR;
                                image[i][j][1]=curG;
                                image[i][j][2]=curB;
                                curr+=3;
                        }
                }
        }
        else{
                for(i=0;i<height;i++){
                        for(j=0;j<width;j++){
                                int cur = *curr; 
                                image[i][j][0]=rgb[cur].rgbBlue;
                                image[i][j][1]=rgb[cur].rgbGreen;
                                image[i][j][2]=rgb[cur].rgbRed;
                                curr++;
                        }
                }
        }
}

void arrayToPix(unsigned char* pixels, int height, int width, unsigned char imageArr[height][width][3]){
        unsigned char *curr = pixels;
        int i = 0, j = 0;
        for(i=0;i<height;i++){
                for(j=0;j<width;j++){
                        *(curr) = imageArr[i][j][2];
                        *(curr+1) = imageArr[i][j][1];
                        *(curr+2) = imageArr[i][j][0];
                        curr+=3;
                }
        }
        
}