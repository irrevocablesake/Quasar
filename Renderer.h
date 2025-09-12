#ifndef RENDERER_H
#define RENDERER_H

#include "Vector3.h"
#include "Point3.h"
#include "Color3.h"
#include "Camera.h"
#include "Ray.h"
#include "Image.h"
#include "Viewport.h"
#include "World.h"
#include "Interval.h"
#include "IntersectionManager.h"
#include "PixelSampler.h"

#include <thread>
#include <iostream>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

class Renderer
{
private:
    Image image;
    Viewport viewport;
    Camera camera;
    World world;
    PixelSampler pixelSampler;

    std::vector<std::atomic<int>> threadProgress;

public:
    int samplesPerPixel = 10;
    int maxDepth = 10;
    float vFOV = 90;
    Point3 lookFrom{0, 0, 0};
    Point3 lookAt{0, 0, -1};
    Vector3 vUp{0, 1, 0};
    float defocusAngle = 1.0;
    float focusDistance = 1.0;
    Color3 background = Color3(1.0, 0.0, 0.0);

public:
    Renderer(const World &world, const Image &image) : world(world), image(image) {}

    void initialize();

    void render();
    void writePixelColor(const Color3 &pixelColor, int i, int j);
    void showProgress( int threadCount );
};

inline double linearToGamma(double linearComponent)
{
    if (linearComponent > 0)
    {
        return std::sqrt(linearComponent);
    }

    return 0;
}

void Renderer::writePixelColor(const Color3 &pixelColor, int i, int j)
{
    double rNormalized = pixelColor.r();
    double gNormalized = pixelColor.g();
    double bNormalized = pixelColor.b();

    double rGammaCorrected = linearToGamma(rNormalized);
    double gGammaCorrected = linearToGamma(gNormalized);
    double bGammaCorrected = linearToGamma(bNormalized);

    static const Interval intensity(0.000, 1.000);
    int rChannel = 255 * intensity.clamp(rGammaCorrected);
    int gChannel = 255 * intensity.clamp(gGammaCorrected);
    int bChannel = 255 * intensity.clamp(bGammaCorrected);

    image.setPixel(i, j, Color3(rChannel, gChannel, bChannel));
}

void Renderer::initialize()
{

    camera.vFOV = vFOV;
    camera.lookFrom = lookFrom;
    camera.lookAt = lookAt;
    camera.vup = vUp;

    camera.updatePosition();
    camera.updateOrientation();

    camera.defocusAngle = defocusAngle;

    if (focusDistance <= 0)
    {
        camera.focusDistance = (lookFrom - lookAt).length();
    }
    else
    {
        camera.focusDistance = focusDistance;
    }

    viewport = Viewport(image, camera);
    pixelSampler = PixelSampler(camera, viewport, samplesPerPixel, maxDepth, background);

    world.setupAccelerationStructure();
}

void Renderer::showProgress( int threadCount ){
    bool done = false;
    
    float totalProgress = 0.0;

    while( !done ){
        done = true;

        for( int i = 0; i < threadCount; i++ ){
            int p = threadProgress[i].load( std::memory_order_relaxed );
            totalProgress = totalProgress + p;
            if( p < 100 ) done = false;
        }

        totalProgress = totalProgress / float( threadCount );
        std::clog << "\rProgress: " << totalProgress << " %    " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        totalProgress = 0;
    }
}

void Renderer::render()
{
    std::clog << "Raycasting Scene...\n";

    auto startTime = Clock::now();

    int numThreads = std::thread::hardware_concurrency();
    int rowsPerThread = image.height / numThreads;

    threadProgress = std::vector<std::atomic<int>>(numThreads);

    for( int i = 0; i < numThreads; i++ ){
        threadProgress[i].store( 0 );
    }

    std::vector<std::thread> threads;

    std::clog << "\nSpawning " << numThreads << " Threads \n";

    for (int k = 0; k < numThreads; k++){
        int startY = k * rowsPerThread;
        int endY = (k == numThreads - 1) ? image.height : startY + rowsPerThread;

        threads.emplace_back( [this, startY, endY, k ](){
            int totalRows = endY - startY;
            for (int j = startY; j < endY; j++){
                for (int i = 0; i < image.width; i++){
                    Color3 pixelColor = pixelSampler.samplePixel(j, i, world);
                    writePixelColor( pixelColor, j, i );
                }

                int percent = (( j - startY + 1 ) * 100 ) / totalRows;
                threadProgress[k].store( percent, std::memory_order_relaxed );
            }
        });
    }

    showProgress( numThreads );

    for (auto &thread : threads)
    {
        thread.join();
    }

    image.render();

    auto endTime = Clock::now();
    auto duration = std::chrono::duration_cast< std::chrono::seconds>( endTime - startTime ).count();

    int minutes = duration / 60;
    int seconds = duration % 60;

    std::clog << "\n\nDone in " << minutes << " minutes " << seconds << " seconds.";
}

#endif