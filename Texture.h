#ifndef TEXTURE_H
#define TEXTURE_H

#include "Color3.h"
#include "Point3.h"
#include "./ImageLoader/ImageLoader.h"
#include "Interval.h"
#include "Perlin.h"

#include<memory>
#include<cmath>

using std::shared_ptr;
using std::make_shared;

class Texture {
    public:
        virtual ~Texture() = default;
        virtual Color3 value( double u, double v, const Point3 &point ) const = 0;
};

class solidColor : public Texture {
    public:
        solidColor( const Color3 &albedo ) : albedo( albedo ) {}
        solidColor( double red, double green, double blue ) : solidColor( Color3( red, green, blue ) ) {}

        Color3 value( double u, double v, const Point3 &point ) const override {
            return albedo;
        }

    private:
        Color3 albedo;
};

class CheckerTexture : public Texture {
    public:
        CheckerTexture( double scale, shared_ptr< Texture > even, shared_ptr< Texture > odd ) : invertedScale( 1.0 / scale ), even( even ), odd( odd ) {}
        CheckerTexture( double scale, const Color3 &color1, const Color3 &color2 ) : CheckerTexture( scale, make_shared< solidColor >( color1 ), make_shared< solidColor >( color2 ) ) {}

        Color3 value( double u, double v, const Point3 &point ) const override {
            int xInteger = int( std::floor( invertedScale * point.x() ) );
            int yInteger = int( std::floor( invertedScale * point.y() ) );
            int zInteger = int( std::floor( invertedScale * point.z() ) );

            bool isEven = ( xInteger + yInteger + zInteger ) % 2 == 0;

            return isEven ? even -> value( u, v, point ) : odd -> value( u, v, point );
        }

    private:
        double invertedScale;
        shared_ptr< Texture > even;
        shared_ptr< Texture > odd;
};

class ImageTexture : public Texture {
    public:
        ImageTexture( const char* filename ) : image( filename ) {}

        Color3 value( double u, double v, const Point3 &point ) const override {
            if( image.height() <= 0 ) return Color3( 0, 1, 1 );

            u = Interval( 0, 1 ).clamp( u );
            v = 1.0 - Interval( 0, 1 ).clamp( v );

            auto i = int( u * image.width() );
            auto j = int( v * image.height() );
            auto pixel = image.pixelData( i, j );

            auto colorScale = 1.0 / 255.0;
            return Color3( colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2] );
        }

    private:
        ImageLoader image;
};

class PerlinTexture : public Texture {
    public:
        PerlinTexture( double scale, double turbulence, double distortion, Color3 veinColor, Color3 surfaceColor ) : scale( scale ), turbulence( turbulence ), distortion( distortion ), veinColor( veinColor ), surfaceColor( surfaceColor ) { }

        Color3 value( double u, double v, const Point3 &point ) const override {
            // double s = scale * point.z() + distortion * perlinNoise.turbulence( point, turbulence );
            // double t = 0.5 * ( 1 + std::sin( s ) );

            // return ( 1 - t ) * veinColor + t * surfaceColor;

            return Color3( 1, 1, 1 ) * ( perlinNoise.turbulence( point * 0.04, 5 ) );
        }

    private:
        Perlin perlinNoise;
        double scale;
        double turbulence;
        double distortion;

        Color3 veinColor;
        Color3 surfaceColor;
};

#endif