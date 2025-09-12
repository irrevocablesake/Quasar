#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <cstdlib>
#include <iostream>
#include <string>

using std::string;

class ImageLoader {
    public:
        ImageLoader() {}
        ImageLoader( const char* fileName ){
            string filename = string( fileName );
            auto imageDirectory = getenv("RAYTRACING");

            if( imageDirectory && load( string( imageDirectory ) + "/" + filename )) return;
            if( load("images/" + filename )) return;

            std::cerr << "ERROR: Could not load image file '" << filename << "'.\n";
        }

        ~ImageLoader() {
            delete[] bdata;
            STBI_FREE( fdata );
        }

        bool load( const string &path ){   
            auto n = bytesPerPixel;
            
            fdata = stbi_loadf( path.c_str(), &imageWidth, &imageHeight, &n, bytesPerPixel );
            if( fdata == nullptr ) return false;

            bytesPerScanline = imageWidth * bytesPerPixel;
            convertToBytes();

            return true;
        }

        int width() const {
            return ( fdata == nullptr ) ? 0 : imageWidth;
        }

        int height() const {
            return ( fdata == nullptr ) ? 0 : imageHeight;
        }

        const unsigned char* pixelData( int x, int y ) const {
            static unsigned char magenta[] = { 255, 0, 255 };
            if( bdata == nullptr ) return magenta;

            x = clamp( x, 0, imageWidth );
            y = clamp( y, 0, imageHeight );

            return bdata + y * bytesPerScanline + x*bytesPerPixel;
        }

    private:
        const int bytesPerPixel = 3;
        float *fdata = nullptr;
        unsigned char *bdata = nullptr;
        int imageWidth = 0;
        int imageHeight = 0;
        int bytesPerScanline = 0;

        static int clamp( int x, int low, int high ){
            if( x < low ) return low;
            if( x < high ) return x;
            return high - 1;
        }

        static unsigned char floatToBytes( float value ){
            if( value <= 0.0 ){
                return 0;
            }
            if( 1.0 <= value ){
                return 255;
            }
            return static_cast< unsigned char >( 256.0 * value );
        }

        void convertToBytes(){
            int totalBytes = imageWidth * imageHeight * bytesPerPixel;
            bdata = new unsigned char[ totalBytes ];

            auto *bptr = bdata;
            auto *fptr = fdata;

            for( auto i = 0; i < totalBytes; i++, fptr++, bptr++ ){
                *bptr = floatToBytes( *fptr );
            }
        }
};

#endif