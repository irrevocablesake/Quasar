#ifndef PIXELSAMPLER_H
#define PIXELSAMPLER_H

#include <cstdint>

#include "Vector3.h"
#include "Point3.h"
#include "Ray.h"
#include "Color3.h"
#include "Viewport.h"
#include "Camera.h"
#include "IntersectionManager.h"
#include "World.h"
#include "Material.h"
#include "constants.h"

#include "random.h"

class PixelSampler{
    public:
        int samplesPerPixel;
        int maxDepth;
        Color3 background;
        Image *image;

    public:
        double average;
        Viewport viewport;
        Camera camera;

        PixelSampler() {} 
        PixelSampler( Camera &camera, Viewport &viewport, int samplesPerPixel, int maxDepth, Color3 &background, Image *image ) : camera( camera ), viewport( viewport ), samplesPerPixel( samplesPerPixel ), maxDepth( maxDepth ), background( background ), image( image ) {
            average = 1 / double( samplesPerPixel );
        }

        Color3 samplePixel( int i, int j, World &world );
        Vector3 sample();
        
        Color3 processPixelColor( Ray &ray, World &world, int maxDepth );
        void writePixelColor(const Color3 &pixelColor, int i, int j );
};

inline double linearToGamma(double linearComponent)
{
    if (linearComponent > 0)
    {
        return std::sqrt(linearComponent);
    }

    return 0;
}

void PixelSampler::writePixelColor(const Color3 &pixelColor, int i, int j )
{
    double rNormalized = pixelColor.r();
    double gNormalized = pixelColor.g();
    double bNormalized = pixelColor.b();

    double rGammaCorrected = linearToGamma(rNormalized);
    double gGammaCorrected = linearToGamma(gNormalized);
    double bGammaCorrected = linearToGamma(bNormalized);

    static const Interval intensity(0.000, 1.000);
    std::uint8_t rChannel = ( std::uint8_t ) 255 * intensity.clamp(rGammaCorrected);
    std::uint8_t gChannel = ( std::uint8_t ) 255 * intensity.clamp(gGammaCorrected);
    std::uint8_t bChannel = ( std::uint8_t ) 255 * intensity.clamp(bGammaCorrected);

    image -> setPixel(i, j, rChannel, gChannel, bChannel);
}

Color3 PixelSampler::processPixelColor( Ray &ray, World &world, int maxDepth ){

    if( maxDepth <= 0 ){
        return Color3( 0, 0, 0 );
    }

    IntersectionManager intersectionManager;

    bool hit = world.raycast( ray, Interval( 0.001, INF ), intersectionManager );

    if( hit ){

        if( auto solidMaterial = dynamic_cast<Solid*>(intersectionManager.material.get()) ){
            return solidMaterial -> getAlbedo();
        }

        if (dynamic_cast<Normal*>(intersectionManager.material.get()) ) {
            return 0.5 * Color3(
                intersectionManager.normal.x() + 1,
                intersectionManager.normal.y() + 1,
                intersectionManager.normal.z() + 1
            );
        }

        Ray scattered;
        Color3 attenuation;

        Color3 emittedColor = intersectionManager.material -> emitted( intersectionManager.u, intersectionManager.v, intersectionManager.point );

        if( intersectionManager.material -> scatter( ray, attenuation, scattered, intersectionManager ) ){
            Color3 value = processPixelColor( scattered, world, maxDepth - 1 );
            
            return emittedColor + ( attenuation * value );
        }
        return emittedColor;
    }

    return background;
}

Vector3 PixelSampler::sample() {
    return Vector3( generateRandomNumber() - 0.5, generateRandomNumber() - 0.5, 0 );
}

Color3 PixelSampler::samplePixel( int i, int j, World &world ){
    Color3 pixelColor( 0, 0, 0 );
    
    for( int count = 0; count < samplesPerPixel; count++ ){
        Vector3 offset = sample();
        Point3 pixelSample = viewport.pixel00Location + ( ( i + offset.x() ) * viewport.pixelDeltaHeight ) + ( ( j + offset.y() ) * viewport.pixelDeltaWidth );

        Point3 origin = ( camera.defocusAngle <= 0 ) ? camera.position : camera.defocusDiskSample();
        double rayTime = generateRandomNumber();
        
        Ray ray( origin, ( pixelSample - origin ), rayTime );

        pixelColor += processPixelColor( ray, world, maxDepth );
        writePixelColor(pixelColor * ( 1.0 / ( float ) ( count + 1 ) ), j, i );
    }

    return ( pixelColor * average );
}

#endif