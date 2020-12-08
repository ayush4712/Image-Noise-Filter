#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct tagRGBQUAD {
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char rgbReserved;
} RGBQUAD;

typedef struct Image
{
    int height;
    int width;
    int bytesPerPixel;
    int xRes, yRes;
    unsigned char* pixels;
    RGBQUAD* rgb;
}Image;


extern Image ReadImage(const char* filename);

extern void WriteImage(const char* filename, Image image);

extern void pixToArray(unsigned char *pixels, int height, int width, unsigned char imageArr[height][width][3], RGBQUAD* rgb);

extern void arrayToPix(unsigned char* pixels, int height, int width, unsigned char imageArr[height][width][3]);

extern void convHSV(int height, int width, unsigned char image[height][width][3], double **H, double**S, double**V);

extern void backRGB(int height, int width, double **H, double**S, double**V, unsigned char imageRGB[height][width][3]);

#endif