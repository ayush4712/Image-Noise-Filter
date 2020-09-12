#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define X_RES_OFFSET 0x0026
#define Y_RES_OFFSET 0x002A
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0
#define RGB_OFFSET 0x0036


typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

typedef struct tagRGBQUAD {
  byte rgbBlue;
  byte rgbGreen;
  byte rgbRed;
  byte rgbReserved;
} RGBQUAD;


//***Inputs*****
//fileName: The name of the file to open 
//***Outputs****
//pixels: A pointer to a byte array. This will contain the pixel data
//width: An int pointer to store the width of the image in pixels
//height: An int pointer to store the height of the image in pixels
//bytesPerPixel: An int pointer to store the number of bytes per pixel that are used in the image
void ReadImage(const char *fileName,byte **pixels, int32 *width, int32 *height, int32 *bytesPerPixel,  int32 *xRes, int32 *yRes, RGBQUAD **rgb)
{
        //Open the file for reading in binary mode
        FILE *imageFile = fopen(fileName, "rb");
        //Read data offset
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);
        //Read width
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(width, 4, 1, imageFile);
        //Read height
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(height, 4, 1, imageFile);
        //Read bits per pixel
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);
        //Allocate a pixel array
        *bytesPerPixel = ((int32)bitsPerPixel) / 8;

        fseek(imageFile, X_RES_OFFSET, SEEK_SET);
        fread(xRes, 4, 1, imageFile);

        fseek(imageFile, Y_RES_OFFSET, SEEK_SET);
        fread(yRes, 4, 1, imageFile);
        if(dataOffset > 54){
                int rgb_offset = 1024; 
                int j = 0;
                *rgb = (RGBQUAD*)malloc(rgb_offset);
                RGBQUAD* curr = *rgb;
                fseek(imageFile, RGB_OFFSET, SEEK_SET);
                for(j=0;j<256;j++){
                        fread(curr, 4, 1, imageFile);
                        curr++;
                }
        }
        //Rows are stored bottom-up
        //Each row is padded to be a multiple of 4 bytes. 
        //We calculate the padded row size in bytes
        int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f))*(*bytesPerPixel);
        //We are not interested in the padded bytes, so we allocate memory just for
        //the pixel data
        int unpaddedRowSize = (*width)*(*bytesPerPixel);
        //Total size of the pixel data in bytes
        int totalSize = unpaddedRowSize*(*height);
        *pixels = (byte*)malloc(totalSize);
        //Read the pixel data Row by Row.
        //Data is padded and stored bottom-up
        int i = 0;
        //point to the last row of our pixel array (unpadded)
        byte *currentRowPointer = *pixels+((*height-1)*unpaddedRowSize);
        for (i = 0; i < *height; i++)
        {
                //put file cursor in the next row from top to bottom
	        fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
	        //read only unpaddedRowSize bytes (we can ignore the padding bytes)
	        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
	        //point to the next row (from bottom to top)
	        currentRowPointer -= unpaddedRowSize;
        }

        fclose(imageFile);
}

//***Inputs*****
//fileName: The name of the file to save 
//pixels: Pointer to the pixel data array
//width: The width of the image in pixels
//height: The height of the image in pixels
//bytesPerPixel: The number of bytes per pixel that are used in the image
void WriteImage(const char *fileName, byte *pixels, int32 width, int32 height,int32 bytesPerPixel, int32 xRes, int32 yRes, RGBQUAD *rgb)
{
        //Open file in binary mode
        FILE *outputFile = fopen(fileName, "wb");
        //*****HEADER************//
        //write signature
        const char *BM = "BM";
        fwrite(&BM[0], 1, 1, outputFile);
        fwrite(&BM[1], 1, 1, outputFile);
        //Write file size considering padded bytes
        int paddedRowSize = (int)(4 * ceil((float)width/4.0f))*bytesPerPixel;
        int32 fileSize = paddedRowSize*height + HEADER_SIZE + INFO_HEADER_SIZE;
        printf("%d\n", fileSize);
        fwrite(&fileSize, 4, 1, outputFile);
        //Write reserved
        int32 reserved = 0x0000;
        fwrite(&reserved, 4, 1, outputFile);
        //Write data offset
        int32 dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;
        fwrite(&dataOffset, 4, 1, outputFile);

        //*******INFO*HEADER******//
        //Write size
        int32 infoHeaderSize = INFO_HEADER_SIZE;
        fwrite(&infoHeaderSize, 4, 1, outputFile);
        //Write width and height
        fwrite(&width, 4, 1, outputFile);
        fwrite(&height, 4, 1, outputFile);
        //Write planes
        int16 planes = 1; //always 1
        fwrite(&planes, 2, 1, outputFile);
        //write bits per pixel
        int16 bitsPerPixel = bytesPerPixel * 8;
        fwrite(&bitsPerPixel, 2, 1, outputFile);
        //write compression
        int32 compression = NO_COMPRESION;
        fwrite(&compression, 4, 1, outputFile);
        //write image size (in bytes)
        int32 imageSize = width*height*bytesPerPixel;
        fwrite(&imageSize, 4, 1, outputFile);
        //write resolution (in pixels per meter)
        int32 resolutionX=xRes;
        int32 resolutionY=yRes;
        if(xRes == 0 | yRes ==0){
                resolutionX = 96;
                resolutionY = 96; 
        }
        printf("%d %d %d %d %d %d",height,width,planes,bitsPerPixel,xRes, yRes);
        fwrite(&resolutionX, 4, 1, outputFile);
        fwrite(&resolutionY, 4, 1, outputFile);
        //write colors used 
        int32 colorsUsed = MAX_NUMBER_OF_COLORS;
        fwrite(&colorsUsed, 4, 1, outputFile);
        //Write important colors
        int32 importantColors = ALL_COLORS_REQUIRED;
        fwrite(&importantColors, 4, 1, outputFile);
        
        if(rgb != NULL){
                int j = 0;
                for(j = 0;j<256;j++){
                        fwrite(&rgb[j], 4, 1, outputFile);
                }
        }

        //write data
        int i = 0;
        int unpaddedRowSize = width*bytesPerPixel;
        for ( i = 0; i < height; i++)
        {
                //start writing from the beginning of last row in the pixel array
                int pixelOffset = ((height - i) - 1)*unpaddedRowSize;
                fwrite(&pixels[pixelOffset], 1, paddedRowSize, outputFile);	
        }
        fclose(outputFile);
}

void pixToArray(byte *pixels, int32 width, int32 height, RGBQUAD *rgb, byte image[height][width][3]){
        byte *curr = pixels;
        int i = 0, j = 0;
        if(rgb==NULL){
                for(i=0;i<height;i++){
                        for(j=0;j<width;j++){
                                byte curB = *(curr), curG = *(curr+1), curR = *(curr+2); 
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

void arrayToPix(byte *pixels, int32 width, int32 height, RGBQUAD *rgb, byte image[height][width][3]){
        byte *curr = pixels;
        int i = 0, j = 0;
        printf("ok\n");
        if(rgb==NULL){
                for(i=0;i<height;i++){
                        printf("ok\n");
                        for(j=0;j<width;j++){
                               // byte curB = *(curr), curG = *(curr+1), curR = *(curr+2); 
                                *(curr) = image[i][j][2];
                                *(curr+1) = image[i][j][1];
                                *(curr+2) = image[i][j][0];
                                curr+=3;
                        }
                        printf("%d\n",i);
                }
        }
        printf("ok1\n");
        // else{
        //         for(i=0;i<height;i++){
        //                 for(j=0;j<width;j++){
        //                         //int cur = *curr; 
        //                         //byte in = getClrIndex(image[i][j][0], rgb);
        //                         //image[i][j][0]=rgb[cur].rgbBlue;
        //                         //image[i][j][1]=rgb[cur].rgbGreen;
        //                         //image[i][j][2]=rgb[cur].rgbRed;
        //                         //*curr = in;
        //                         curr++;
        //                 }
        //         }
        // }
}

int main()
{
        byte *pixels;
        int32 width;
        int32 height;
        int32 bytesPerPixel;
        int32 xRes;
        int32 yRes;
        byte *new_pixels;
        RGBQUAD *rgb = NULL;
        ReadImage("./lena/lena.bmp", &pixels, &width, &height,&bytesPerPixel, &xRes, &yRes, &rgb);

        int i = 0, j = 0;
        int totalSize = width*height*3;
        new_pixels = (byte*)malloc(totalSize);
        //char *filename = "./array.txt";
        byte image_array[height][width][3];
        pixToArray(pixels, width, height, rgb, image_array);
        arrayToPix(new_pixels, width, height, NULL, image_array);
        //for(i =0;i<256;i++)
               // printf("%i %i %i\n", rgb[i].rgbRed, rgb[i].rgbGreen,rgb[i].rgbBlue);
        
        WriteImage("imgcol.bmp", new_pixels, width, height, 3,xRes,yRes, NULL);
        printf(", %u width, %u height %u bytesPerPixel %d %d" ,width, height, bytesPerPixel,xRes,yRes);

        free(pixels);
        free(new_pixels);
        return 0;
}