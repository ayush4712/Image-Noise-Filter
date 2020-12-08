#include<fftw3.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"image.h"

void generateGaussFilter(int kHeight, int kWidth, double **gk){
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

int main(){
    

    fftwf_complex    *data_inR;
    fftwf_complex    *data_inG;
    fftwf_complex    *data_inB;
    fftwf_complex    *data_inH;
    fftwf_complex    *fftR;        
    fftwf_complex    *ifftR;
    fftwf_complex    *fftG;        
    fftwf_complex    *ifftG;
    fftwf_complex    *fftB;        
    fftwf_complex    *ifftB;
    fftwf_complex    *fftH;
    fftwf_complex    *ifftH;
    fftwf_complex    *fftRsR;
    fftwf_complex    *fftRsG;
    fftwf_complex    *fftRsB;
    fftwf_plan       plan_fR;
    fftwf_plan       plan_fG;
    fftwf_plan       plan_fB;
    fftwf_plan       plan_fH;
    fftwf_plan       plan_bR;
    fftwf_plan       plan_bG;
    fftwf_plan       plan_bB;
    fftwf_plan       plan_bH;
    
    
    Image image = ReadImage("./images/118035.bmp");
    int height = image.height;
    int width = image.width;


    unsigned char imageArr[height][width][3];
    pixToArray(image.pixels, height, width, imageArr, NULL);



    data_inR = fftwf_alloc_complex(width * height);
    data_inG = fftwf_alloc_complex(width * height);
    data_inB = fftwf_alloc_complex(width * height);
    data_inH = fftwf_alloc_complex(width * height);
    fftR     = fftwf_alloc_complex(width * height);
    ifftR    = fftwf_alloc_complex(width * height);
    fftG     = fftwf_alloc_complex(width * height);
    ifftG    = fftwf_alloc_complex(width * height);
    fftB     = fftwf_alloc_complex(width * height);
    ifftB    = fftwf_alloc_complex(width * height);
    fftH     = fftwf_alloc_complex(width * height);
    ifftH    = fftwf_alloc_complex(width * height);
    fftRsR   = fftwf_alloc_complex(width * height);
    fftRsG   = fftwf_alloc_complex(width * height);
    fftRsB   = fftwf_alloc_complex(width * height);



    plan_fR = fftwf_plan_dft_2d( width, height, data_inR, fftR,  FFTW_FORWARD,  FFTW_ESTIMATE );
    plan_fG = fftwf_plan_dft_2d( width, height, data_inG, fftG,  FFTW_FORWARD,  FFTW_ESTIMATE );
    plan_fB = fftwf_plan_dft_2d( width, height, data_inB, fftB,  FFTW_FORWARD,  FFTW_ESTIMATE );
    plan_fH = fftwf_plan_dft_2d( width, height, data_inH, fftH,  FFTW_FORWARD,  FFTW_ESTIMATE );
    
    plan_bR = fftwf_plan_dft_2d( width, height, fftRsR, ifftR, FFTW_BACKWARD, FFTW_ESTIMATE );
    plan_bG = fftwf_plan_dft_2d( width, height, fftRsG, ifftG, FFTW_BACKWARD, FFTW_ESTIMATE );
    plan_bB = fftwf_plan_dft_2d( width, height, fftRsB, ifftB, FFTW_BACKWARD, FFTW_ESTIMATE );



    for( int i = 0, k = 0 ; i < height ; i++ ) {
        for(int j = 0 ; j < width ; j++ ) {
            data_inR[k][0] = ( double )imageArr[i][j][0];
            data_inR[k][1] = 0.0;
            data_inG[k][0] = ( double )imageArr[i][j][1];
            data_inG[k][1] = 0.0;
            data_inB[k][0] = ( double )imageArr[i][j][2];
            data_inB[k][1] = 0.0;
            k++;
        }
    }



    fftwf_execute(plan_fR);
    fftwf_execute(plan_fG);
    fftwf_execute(plan_fB);


    int kHeight = 5;
    int kWidth = 5;
    double **gk = (double **)malloc(height * sizeof(double *));
    for (int i=0; i<height; i++) 
        gk[i] = (double *)malloc(width * sizeof(double));
    
    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){
            gk[i][j]=0;
        }
    }


    generateGaussFilter(kHeight, kWidth, gk);

    
    for( int i = 0, k = 0 ; i < height ; i++ ) {
        for(int j = 0 ; j < width ; j++ ) {
            data_inH[k][0] = gk[i][j];
            data_inH[k][1] = 0.0;
            k++;
        }
    }

    fftwf_execute(plan_fH);


    double SNR = 0.3;



    fftwf_complex *wienerFilter;
    wienerFilter = fftwf_alloc_complex(width * height);

    for(int i = 0 ; i < ( width * height ) ; i++ ){
        wienerFilter[i][0] = fftH[i][0];
        wienerFilter[i][1] = -fftH[i][1];
        wienerFilter[i][0] /= (fftH[i][0]*fftH[i][0] + fftH[i][1]*fftH[i][1] + (double)SNR);
        wienerFilter[i][1] /= (fftH[i][0]*fftH[i][0] + fftH[i][1]*fftH[i][1] + (double)SNR); 
    }




    for(int i = 0 ; i < ( width * height ) ; i++ ){
        fftRsR[i][0] = wienerFilter[i][0] * fftR[i][0] - wienerFilter[i][1] * fftR[i][1];
        fftRsG[i][0] = wienerFilter[i][0] * fftG[i][0] - wienerFilter[i][1] * fftG[i][1];
        fftRsB[i][0] = wienerFilter[i][0] * fftB[i][0] - wienerFilter[i][1] * fftB[i][1];

        fftRsR[i][1] = wienerFilter[i][0] * fftR[i][1] + wienerFilter[i][1] * fftR[i][0];
        fftRsG[i][1] = wienerFilter[i][0] * fftG[i][1] + wienerFilter[i][1] * fftG[i][0];
        fftRsB[i][1] = wienerFilter[i][0] * fftB[i][1] + wienerFilter[i][1] * fftB[i][0];
    }



    fftwf_execute(plan_bR);
    fftwf_execute(plan_bG);
    fftwf_execute(plan_bB);

    


    for(int i = 0 ; i < ( width * height ) ; i++ ) {
        ifftR[i][0] /= ( double )( width * height );
        ifftG[i][0] /= ( double )( width * height );
        ifftB[i][0] /= ( double )( width * height );
    }
    

    for(int i = 0 ; i < ( width * height ) ; i++ ) {
        ifftR[i][0] = ( double ) sqrt(ifftR[i][0]*ifftR[i][0] + ifftR[i][1]*ifftR[i][1]);
        ifftG[i][0] = ( double ) sqrt(ifftG[i][0]*ifftG[i][0] + ifftG[i][1]*ifftG[i][1]);
        ifftB[i][0] = ( double ) sqrt(ifftB[i][0]*ifftB[i][0] + ifftB[i][1]*ifftB[i][1]);
    }


    unsigned char newImageArr[height][width][3];


    for(int i = 0, kR = 0, kG = 0, kB = 0 ; i < height ; i++ ) {
		for(int j = 0 ; j < width ; j++ ) {
			newImageArr[i][j][0] = ( unsigned char )ifftR[kR++][0];
            newImageArr[i][j][1] = ( unsigned char )ifftG[kG++][0];
            newImageArr[i][j][2] = ( unsigned char )ifftB[kB++][0];
		}
	}



    unsigned char* new_pixels = (unsigned char*)malloc(height*width*3);



    Image newImage;
    newImage.height = height;
    newImage.width = width;
    newImage.bytesPerPixel = 3;
    newImage.rgb = image.rgb;
    newImage.xRes = image.xRes;
    newImage.yRes = image.yRes;
    free(image.pixels);

    arrayToPix(new_pixels, height, width, newImageArr);
    newImage.pixels = new_pixels;

    WriteImage("./images/fftu.bmp", newImage);
    
    return 0;
  }





//gcc wiener.c readwrite.c pixarr.c -lfftwf3-3 -o wr