#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <vector>
#include "Color3.h"

using std::vector;

class Image
{
public:
    double aspectRatio;

    int width;
    int height;
    vector<vector<Color3>> pixels;

    Image() : width(640), height(480), aspectRatio(double(16.0) / double(9.0)) {
        pixels = vector< vector< Color3 >> ( height, vector< Color3 >( width ));
    }
    Image(const int &width, const double &aspectRatio) : width(width), aspectRatio(aspectRatio)
    {
        if (width < 1)
        {
            throw std::invalid_argument("Image Width must be atleast 1");
        }

        this->height = width / aspectRatio;

        pixels = vector< vector< Color3 >> ( this->height, vector< Color3 >( width ));

        if (this->height < 1)
        {
            throw std::invalid_argument("Image Height must be atleast 1");
        }
    }

    void setPixel(int x, int y, Color3 pixelColor)
    {
        pixels[x][y] = pixelColor;
    }

    Color3 getPixel(int x, int y)
    {
        return pixels[x][y];
    }

    void render()
    {
        std::clog << "\n\nGenerating Image...\n";

        std::cout << "P3\n"
                  << width << ' ' << height << '\n'
                  << "255\n";

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Color3 pixelColor = getPixel( i, j );
                std::cout << pixelColor.r() << ' ' << pixelColor.g() << ' ' << pixelColor.b() << '\n';
            }
            std::clog << "\rProgress: " << int( ( i / float( height - 1 ) ) * 100 ) << " %   " << std::flush;
        }
    }
};

#endif