#include <iostream> 
#include <vector> 
#include <cmath> 
#include <assert.h> 
#include <glm/vec3.hpp> // glm::vec3:
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

class Pixel
{
    public:
    uint8_t r,g,b;
    Pixel(uint8_t r, uint8_t g, uint8_t b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Pixel()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    Pixel operator+(const Pixel& rhs)
    {
        Pixel temp;
        temp.r = this->r + rhs.r;
        temp.g = this->g + rhs.g;
        temp.b = this->b + rhs.b;
        return temp;
    }
    Pixel operator-(const Pixel& rhs)
    {
        Pixel temp;
        temp.r = this->r - rhs.r;
        temp.g = this->g - rhs.g;
        temp.b = this->b - rhs.b;
        return temp;
    }
    Pixel operator*(const float& c)
    {
        Pixel temp;
        temp.r = this->r * c;
        temp.g = this->g * c;
        temp.b = this->b * c;
        return temp;
    }
    bool operator==(const Pixel& rhs)
    {
        if(this->r != rhs.r ||
           this->g != rhs.g ||
           this->b != rhs.b )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};


int main(int argc, char** argv)
{
    float EULERS_NUMBER = std::exp(1.0);
    char* inputFileName = NULL;
    char* outputFileName = NULL;
    int kernelSize = 1;
    int radius = -1;
    float sigma = 1;

    if(argc < 2)
    {
        std::cerr << "Enter a file to apply the gaussian filter" << std::endl;
        exit(-1);
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            
            if(strcmp(argv[i], "-i") == 0 )
            {
                if(i + 1 >= argc)
                {
                    std::cerr << "expected filename after -i option" << std::endl;
                    exit(-1);
                }
                inputFileName = strdup(argv[i+1]); 
                if(outputFileName == NULL)
                {
                    outputFileName = new char[strlen(argv[i+1])+64];
                    outputFileName[0] = 'o'; 
                    outputFileName[1] = 'u'; 
                    outputFileName[2] = 't'; 
                    outputFileName[3] = '_'; 
                    strcat(outputFileName, argv[i+1]);
                }

                i++;
            }
            else if(strcmp(argv[i], "--kernel-size") == 0 || strcmp(argv[i], "-k") == 0 )
            {
                if(i + 1 >= argc)
                {
                    std::cerr << "expected kernel size after -k option" << std::endl;
                    exit(-1);
                }
                radius = atoi(argv[i+1]);

                i++;
            }
            else if(strcmp(argv[i], "-o") == 0 )
            {
                if(i + 1 >= argc)
                {
                    std::cerr << "expected filename after -o option" << std::endl;
                    exit(-1);
                }
                if(outputFileName != NULL)
                {
                    free(outputFileName);
                }
                outputFileName = new char[strlen(argv[i+1])+5];
                //strcat(outputFileName, argv[i+1]);
                outputFileName = strdup(argv[i+1]);

                i++;
            }
        }
    }
    
    //std::cout << "input filename: " <<  inputFileName << std::endl;

    //std::cout << "output filename: " << outputFileName << std::endl;
    //std::cout << "radius: " << radius << std::endl;

    //exit(-1);
    //else if (strcmp(argv[1], "--kernel-size") == 0 )
    //{
    //    fileName = strdup(argv[1]); 
    //    outputFileName = new char[strlen(argv[1])+64];
    //    outputFileName[0] = 'o'; 
    //    outputFileName[1] = 'u'; 
    //    outputFileName[2] = 't'; 
    //    outputFileName[3] = '_'; 
    //    strcat(outputFileName, argv[1]);
    //}
    //else if (strcmp(argv[1], "--out") == 0 )
    //{
    //    fileName = strdup(argv[1]); 
    //    outputFileName = new char[strlen(argv[1])+64];
    //    outputFileName[0] = 'o'; 
    //    outputFileName[1] = 'u'; 
    //    outputFileName[2] = 't'; 
    //    outputFileName[3] = '_'; 
    //    strcat(outputFileName, argv[1]);
    //}

    if(inputFileName == NULL)
    {
        std::cerr << "provide an input image"<< std::endl;
        exit(-1);
    }
    
    int width, height, numOfCh;
    Pixel* imgIn = (Pixel*) stbi_load(inputFileName, &width, &height, &numOfCh, 3);
    Pixel* imgOut = new Pixel[width*height*numOfCh];

    std::cout << "input image size = " << width << "x" << height << std::endl;
    std::cout << "number of channels = " << numOfCh << std::endl;
    std::cout << "writing to file = " << outputFileName << "\n" << std::endl;
    

    if(radius == -1)
    {
        radius = 3;
    }
    kernelSize = radius*2 + 1;
    sigma = std::max(radius/2.f, 1.f);

    float** kernel = new float*[kernelSize];
    for(int i = 0; i < kernelSize; i++)
    {
        kernel[i] = new float[kernelSize];
    }


    float twoSigmaSqr = 2*sigma * sigma;
    float sum = 0.f;
    for(int j = -radius; j <= radius; ++j)
    {
        for(int i = -radius; i <= radius; ++i)
        {
            float eMult = pow(EULERS_NUMBER, -(i*i + j*j)/twoSigmaSqr);
            kernel[radius +i][radius+j] = 1.f/(M_PI * twoSigmaSqr) * eMult;
            sum += kernel[radius +i][radius+j];
        }
    }
    std::cout << "sum = " << sum << std::endl;

    float acc = 0.f;
    for(int j = -radius; j <= radius; ++j)
    {
        for(int i = -radius; i <= radius; ++i)
        {
            kernel[radius +i][radius+j] /= sum;
            acc += kernel[radius +i][radius+j];
            //std::cout << kernel[radius +i][radius+j] << ", ";
        }
        
        //std::cout <<std::endl;
    }

    std::cout << "acc = " << acc << std::endl;

    for(int y = radius; y < height -radius; ++y)
    {
        for(int x = radius; x < width -radius; ++x)
        {
            float r = 0.f;
            float g = 0.f;
            float b = 0.f;
            for(int j = -radius; j <= radius; ++j)
            {
                for(int i = -radius; i <= radius; ++i)
                {
                     r += imgIn[(y+j)*width + x + i].r * kernel[radius+i][radius+j]; 
                     g += imgIn[(y+j)*width + x + i].g * kernel[radius+i][radius+j]; 
                     b += imgIn[(y+j)*width + x + i].b * kernel[radius+i][radius+j]; 
                }
            }
            imgOut[(y)*width + x ].r = r;
            imgOut[(y)*width + x ].g = g;
            imgOut[(y)*width + x ].b = b;

        }
    }


    for(int y = 0; y < radius; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            imgOut[(y)*width + x ].r = imgIn[(y)*width + x].r;
            imgOut[(y)*width + x ].g = imgIn[(y)*width + x].g;
            imgOut[(y)*width + x ].b = imgIn[(y)*width + x].b;
        }
    }
    for(int y = 0; y < height; ++y)
    {
        for(int x = 0; x < radius; ++x)
        {
            imgOut[(y)*width + x ].r = imgIn[(y)*width + x].r;
            imgOut[(y)*width + x ].g = imgIn[(y)*width + x].g;
            imgOut[(y)*width + x ].b = imgIn[(y)*width + x].b;
        }
    }

    for(int y = height - radius; y < height; ++y)
    {
        for(int x = 0; x < width; ++x)
        {
            imgOut[(y)*width + x ].r = imgIn[(y)*width + x].r;
            imgOut[(y)*width + x ].g = imgIn[(y)*width + x].g;
            imgOut[(y)*width + x ].b = imgIn[(y)*width + x].b;
        }
    }
    for(int y = 0; y < height; ++y)
    {
        for(int x = width - radius; x < width; ++x)
        {
            imgOut[(y)*width + x ].r = imgIn[(y)*width + x].r;
            imgOut[(y)*width + x ].g = imgIn[(y)*width + x].g;
            imgOut[(y)*width + x ].b = imgIn[(y)*width + x].b;
        }
    }

    int stride = width * sizeof(Pixel);
    //assert(numOfCh == 3);
    stbi_write_png(outputFileName, width, height, 3, imgOut, stride);


    ///////////////////////////
    //sigma = std::max(kernelSize/2.f, 1.f);
    //for(int j = -kernelSize; j <= kernelSize; ++j)
    //{
    //    for(int i = -kernelSize; i <= kernelSize; ++i)
    //    {
    //        int dx = i + kernelSize;
    //        int dy = j + kernelSize;

    //        filterMatrix[dx][dy] = 1.f/(2*M_PI *sigmaSqr)
    //            *pow(EULERS_NUMBER, -(j*j + i*i) / (2.f*sigmaSqr));
    //        std:: cout << filterMatrix[dx][dy] << ", ";
    //        acc += filterMatrix[dx][dy];
    //    }
    //    std::cout << std::endl;
    //}

    //std::cout << "sum: " << acc << std::endl;
    //for(int j = kernelSize; j < y-kernelSize; ++j)
    //{
    //    for(int i = kernelSize; i < x-kernelSize; ++i)
    //    {
    //        if(!(Pixel(255,255,255) == img[j*y + i]))
    //        {
    //            std::cout << "First unequal pixel: "
    //                << i << ", " << j << std::endl;
    //            exit(0);
    //        }
    //        
    //    }
    //}


    return 0;
}
