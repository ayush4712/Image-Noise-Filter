#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "image.h"


double max(double r, double g, double b){
    if(r>=g && r>=b) return r;
    else if(g>=r && g>=b) return g;
    else return b;
}

double min(double r, double g, double b){
    if(r<=g && r<=b) return r;
    else if(g<=r && g<=b) return g;
    else return b;
}

void rgb2hsv(double r, double g, double b, double* h, double* s, double* v){
    r = r/255.0;
    g = g/255.0;
    b = b/255.0;
    double minC = min(r, g, b);
    double maxC = max(r, g, b);
    double diff = maxC - minC;
    double hue, sat, val;
    if (maxC == minC) 
        hue = 0; 
    else if (maxC == r)
        hue = (int)(60*((g - b)/diff) + 360)%360;  
    else if (maxC == g)
        hue = (int)(60 * ((b - r) / diff) + 120) % 360; 
    else if (maxC == b) 
        hue = (int)(60 * ((r - g) / diff) + 240) % 360; 
    if (maxC == 0) 
        sat = 0; 
    else
        sat = (diff / maxC) * 100; 

    val = maxC * 100;
    *h = hue;
    *s = sat;
    *v = val;
}

void hsv2rgb(double hue, double sat, double val, unsigned char* red, unsigned char* green, unsigned char* blue){
    sat = sat/100.0;
    val = val/100.0;
    double c = sat*val;
    double x = c*(1-fabs(fmod(hue/60.0, 2)-1));
    double m = val-c;
    double r,g,b;
    if(hue >= 0 && hue < 60){
        r = c, g = x, b = 0;
    }
    else if(hue >= 60 && hue < 120){
        r = x, g = c, b = 0;
    }
    else if(hue >= 120 && hue < 180){
        r = 0, g = c, b = x;
    }
    else if(hue >= 180 && hue < 240){
        r = 0, g = x, b = c;
    }
    else if(hue >= 240 && hue < 300){
        r = x, g = 0, b = c;
    }
    else{
        r = c, g = 0, b = x;
    }
    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;
    *red = R;
    *green = G;
    *blue = B;
}

void convHSV(int height, int width, unsigned char image[height][width][3], double **H, double**S, double**V){
    double h, s, v;
    for(int i = 0;i<height;i++){
        for(int j =0;j<width;j++){
            rgb2hsv(image[i][j][0], image[i][j][1],image[i][j][2], &h, &s, &v);
            H[i][j] = h;
            S[i][j] = s;
            V[i][j] = v;
        }
    }
}

void backRGB(int height, int width, double **H, double**S, double**V, unsigned char imageRGB[height][width][3]){
    unsigned char r, g, b;
    for(int i = 0;i<height;i++){
        for(int j =0;j<width;j++){
            hsv2rgb(H[i][j], S[i][j], V[i][j], &r, &g, &b);
            imageRGB[i][j][0] = (unsigned char)r;
            imageRGB[i][j][1] = (unsigned char)g;
            imageRGB[i][j][2] = (unsigned char)b;
        }
    }
}

int main(){
     
    char fileName[100] = "./images/lena_2.bmp";
    //printf("Enter the file location: "); scanf("%s", fileName);
    Image image = ReadImage(fileName);
    
    int h = image.height;
    int w =image.width;
    unsigned char imageA[h][w][3];
    pixToArray(image.pixels, h, w, imageA, NULL);
    double **imageH = (double **)malloc(h * sizeof(double *)); 
    for (int i=0; i<h; i++) 
         imageH[i] = (double *)malloc(w * sizeof(double));
    double **imageS = (double **)malloc(h * sizeof(double *)); 
    for (int i=0; i<h; i++) 
         imageS[i] = (double *)malloc(w * sizeof(double));
    double **imageV = (double **)malloc(h * sizeof(double *)); 
    for (int i=0; i<h; i++) 
         imageV[i] = (double *)malloc(w * sizeof(double)); 
  
    
    convHSV(h, w, imageA, imageH, imageS, imageV);
    
    backRGB(h, w, imageH, imageS, imageV, imageA);
    arrayToPix(image.pixels, h, w, imageA);
    
    WriteImage("./images/lenahsv_rgb.bmp", image);
    
    free(imageH);
    free(imageS);
    free(imageV);
    return 0;
}