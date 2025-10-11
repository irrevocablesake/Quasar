#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <vector>
#include <cstdint>
#include "Color3.h"

using std::vector;

class Image
{
public:
    double aspectRatio;

    int width;
    int height;
    vector<std::uint8_t> pixels;

    Image() : width(640), height(480), aspectRatio(double(16.0) / double(9.0)) {
        pixels = vector< std::uint8_t> ( width * height * 4.0, 0 );
    }
    Image(const int &width, const double &aspectRatio) : width(width), aspectRatio(aspectRatio)
    {
        if (width < 1)
        {
            throw std::invalid_argument("Image Width must be atleast 1");
        }

        this->height = width / aspectRatio;

        pixels = vector< std::uint8_t> ( this->height * width * 4.0, 0 );

        if (this->height < 1)
        {
            throw std::invalid_argument("Image Height must be atleast 1");
        }
    }

    void setPixel(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b)
    {
        int index = ( y * width + x ) * 4;
        pixels[ index + 0 ] = r;
        pixels[ index + 1 ] = g;
        pixels[ index + 2 ] = b;
        pixels[ index + 3 ] = 255;
    }
};

#endif