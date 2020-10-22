#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "image.h"

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


Image ReadImage(const char *fileName)
{
        //Open the file for reading in binary mode
        Image image;
        FILE *imageFile = fopen(fileName, "rb");
        //Read data offset
        int32 dataOffset;
        fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
        fread(&dataOffset, 4, 1, imageFile);
        //Read width
        int w;
        fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
        fread(&w, 4, 1, imageFile);
        //Read height
        
        image.width = w;
        fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
        fread(&image.height, 4, 1, imageFile);
        //Read bits per pixel
        int16 bitsPerPixel;
        fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
        fread(&bitsPerPixel, 2, 1, imageFile);
        image.bytesPerPixel = ((int32)bitsPerPixel) / 8;
        fseek(imageFile, X_RES_OFFSET, SEEK_SET);
        fread(&image.xRes, 4, 1, imageFile);

        fseek(imageFile, Y_RES_OFFSET, SEEK_SET);
        fread(&image.yRes, 4, 1, imageFile);

        //Allocate a pixel array
        if(dataOffset > 54){
                int rgb_offset = 1024; 
                int j = 0;
                image.rgb = (RGBQUAD*)malloc(rgb_offset);
                RGBQUAD* curr = image.rgb;
                fseek(imageFile, RGB_OFFSET, SEEK_SET);
                for(j=0;j<256;j++){
                        fread(curr, 4, 1, imageFile);
                        curr++;
                }
        }
        else{
                image.rgb = NULL;
        }
        //Rows are stored bottom-up
        //Each row is padded to be a multiple of 4 bytes. 
        //We calculate the padded row size in bytes
        int paddedRowSize = (int)(4 * ceil((float)(image.width) / 4.0f))*(image.bytesPerPixel);
        //We are not interested in the padded bytes, so we allocate memory just for
        //the pixel data
        int unpaddedRowSize = (image.width)*(image.bytesPerPixel);
        //Total size of the pixel data in bytes
        int totalSize = unpaddedRowSize*(image.height);
        image.pixels = (byte*)malloc(totalSize);
        //Read the pixel data Row by Row.
        //Data is padded and stored bottom-up
        int i = 0;
        //point to the last row of our pixel array (unpadded)
        byte *currentRowPointer = image.pixels+((image.height-1)*unpaddedRowSize);
        for (i = 0; i < image.height; i++)
        {
                //put file cursor in the next row from top to bottom
	        fseek(imageFile, dataOffset+(i*paddedRowSize), SEEK_SET);
	        //read only unpaddedRowSize bytes (we can ignore the padding bytes)
	        fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
	        //point to the next row (from bottom to top)
	        currentRowPointer -= unpaddedRowSize;
        }
        
        fclose(imageFile);
        printf("Reading Image...\n");
        printf("Image Size: %dKB\n", totalSize/1000);
        printf("Image Type: %s\n",image.bytesPerPixel==1?"Grayscale":"Color");
        printf("Image Height: %d\n", image.height);
        printf("Image Width: %d\n", image.width);
        return image;
}

void WriteImage(const char *fileName, Image image){
        //Open file in binary mode
        FILE *outputFile = fopen(fileName, "wb");
        //write signature
        const char *BM = "BM";
        fwrite(&BM[0], 1, 1, outputFile);
        fwrite(&BM[1], 1, 1, outputFile);
        // printf();
        //Write file size considering padded bytes
        int paddedRowSize = (int)(4 * ceil((float)image.width/4.0f))*image.bytesPerPixel;
        int fileSize = paddedRowSize*image.height + HEADER_SIZE + INFO_HEADER_SIZE;
        fwrite(&fileSize, 4, 1, outputFile);
        //Write reserved
        int32 reserved = 0x0000;
        fwrite(&reserved, 4, 1, outputFile);
        //Write data offset
        int32 dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;
        fwrite(&dataOffset, 4, 1, outputFile);

        //Write size
        int32 infoHeaderSize = INFO_HEADER_SIZE;
        fwrite(&infoHeaderSize, 4, 1, outputFile);
        //Write width and height
        fwrite(&image.width, 4, 1, outputFile);
        fwrite(&image.height, 4, 1, outputFile);
        //Write planes
        int16 planes = 1; //always 1
        fwrite(&planes, 2, 1, outputFile);
        //write bits per pixel
        int16 bitsPerPixel = image.bytesPerPixel * 8;
        fwrite(&bitsPerPixel, 2, 1, outputFile);
        //write compression
        int32 compression = NO_COMPRESION;
        fwrite(&compression, 4, 1, outputFile);
        //write image size (in bytes)
        int32 imageSize = image.width*image.height*image.bytesPerPixel;
        fwrite(&imageSize, 4, 1, outputFile);
        //write resolution (in pixels per meter)
        int32 resolutionX = image.xRes == 0 ? 96:image.xRes;
        int32 resolutionY = image.yRes == 0 ? 96:image.yRes;
        fwrite(&resolutionX, 4, 1, outputFile);
        fwrite(&resolutionY, 4, 1, outputFile);
        //write colors used 
        int32 colorsUsed = MAX_NUMBER_OF_COLORS;
        fwrite(&colorsUsed, 4, 1, outputFile);
        //Write important colors
        int32 importantColors = ALL_COLORS_REQUIRED;
        fwrite(&importantColors, 4, 1, outputFile);
        
        
        // if(image.rgb != NULL){
        //         int j = 0;
        //         for(j = 0;j<256;j++){
        //                 fwrite(&(image.rgb)+j, 4, 1, outputFile);
        //         }
        //     }
        //write data
        int i = 0;
        int unpaddedRowSize = image.width*image.bytesPerPixel;
        for ( i = 0; i < image.height; i++)
        {
                //start writing from the beginning of last row in the pixel array
                int pixelOffset = ((image.height - i) - 1)*unpaddedRowSize;
                fwrite(&image.pixels[pixelOffset], 1, paddedRowSize, outputFile);	
        }
        fclose(outputFile);
        
        
}

/*
int main(){
    Image image;
    image = ReadImage("./lena_2.bmp");
    printf("\n%d %d %d %d",image.height, image.width, image.xRes, image.yRes);
    WriteImage("./lenatest.bmp", image);
}
*/