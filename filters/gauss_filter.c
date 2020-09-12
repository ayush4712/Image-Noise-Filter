#include <stdio.h>
#include <math.h>


void generateFilter(int kHeight, int kWidth, double gk[kHeight][kWidth]); // Declaration of function to create Gaussian filter

void applyFilter(int height, int width, unsigned char image[height][width][3], int kHeight, int kWidth, double gk[kHeight][kWidth], int fHeight, int fWidth, unsigned char filtered_image[fHeight][fWidth][3]){
    int newImageHeight = height - kHeight + 1, newImageWidth = width - kWidth + 1;
    int i = 0, j = 0, h = 0, w = 0;
    for (i=0 ; i<newImageHeight ; i++) {
        for (j=0 ; j<newImageWidth ; j++) {
            for (h=i ; h<i+kHeight ; h++) {
                for (w=j ; w<j+kWidth ; w++) {
                    filtered_image[0][i][j] += gk[h-i][w-j]*image[0][h][w];
                    filtered_image[1][i][j] += gk[h-i][w-j]*image[1][h][w];
                    filtered_image[2][i][j] += gk[h-i][w-j]*image[2][h][w];
                }
            }
        }
    }
}

int main()
{   

    int iHeight, iWidth;
    int kHeight = 7, kWidth = 7;
    int fHeight = iHeight - kHeight + 1;
    int fWidth = iWidth - kWidth + 1;
    double gk[kHeight][kWidth];
    //unsigned char image[iHeight][iWidth][3];
    generateFilter(kHeight, kWidth, gk); // Function call to create a filter
    unsigned char filtered_image[fHeight][fWidth][3];
    //applyFilter(iHeight, iWidth, image, kHeight, kWidth, gk, fHeight, fWidth, filtered_image);
}
void generateFilter(int kHeight, int kWidth, double gk[kHeight][kWidth])
{
    double stdv = 1.0;
    double r, s = 2.0 * stdv * stdv;  // Assigning standard deviation to 1.0
    double sum = 0.0;   // Initialization of sum for normalization
    int hVal = kHeight/2, wVal = kWidth/2;
    for (int x = -hVal; x <= hVal; x++) // Loop to generate 5x5 kernel
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