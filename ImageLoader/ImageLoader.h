#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <cstdlib>
#include <iostream>
#include <string>

using std::string;

//TODO:
//separate loadf + load
//seems liek a good temp solution

class ImageLoader {
    public:
        ImageLoader() {}
        ImageLoader( const unsigned char* data, int size ) {
            loadFromMemory( data, size );
        }

        ImageLoader( const char* fileName ){
            string filename = string( fileName );
            auto imageDirectory = getenv("RAYTRACING");

            if( imageDirectory && load( string( imageDirectory ) + "/" + filename )) return;
            if( load("images/" + filename )) return;

            std::clog << "ERROR: Could not load image file '" << filename << "'.\n";
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
            isFloatData = true;
            convertToBytes(  );

            return true;
        }

        bool loadFromMemory( const unsigned char* data, int size ){

            auto n = bytesPerPixel;

            cdata = stbi_load_from_memory( data, size, &imageWidth, &imageHeight, &n, bytesPerPixel );
            
            if( cdata == nullptr ) return false;

            bytesPerScanline = imageWidth * bytesPerPixel;
            isFloatData = false;

            convertToBytes(  );

            return true;
        }

        int width() const {
           if( fdata == nullptr && cdata == nullptr ) return 0;
           return imageWidth;           
        }

        int height() const {
            if( fdata == nullptr && cdata == nullptr ) return 0;
            return imageHeight;
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
        unsigned char *cdata = nullptr;
        unsigned char *bdata = nullptr;
        int imageWidth = 0;
        int imageHeight = 0;
        int bytesPerScanline = 0;
        bool isFloatData = NULL;

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

            float *fptr = nullptr;
            unsigned char *cptr = nullptr;

            if( isFloatData ){
                fptr = fdata;
            }
            else{
                cptr = cdata;
            }

            for( auto i = 0; i < totalBytes; i++, bptr++ ){
                if( isFloatData ){
                    *bptr = floatToBytes(*fptr);
                    fptr++;
                }
                else{
                    *bptr = *cptr;
                    cptr++;
                }
            }
        }
};

#endif