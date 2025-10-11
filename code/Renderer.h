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
#include "ThreadPool.h"

#include <thread>
#include <iostream>

class Renderer
{
private:
    Image *image;
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
    Renderer(const World &world, Image &image) : world(world), image(&image) {}

    void initialize();

    void render();
};

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
    pixelSampler = PixelSampler(camera, viewport, samplesPerPixel, maxDepth, background, image);

    world.setupAccelerationStructure();
}

void Renderer::render()
{
    ThreadPool pool(
        image,
        [this](const Tile &tile)
        {
            std::vector<std::pair<int, int>> pixels;
            for (int j = tile.startCol; j < tile.endCol; j++)
            {
                for (int i = tile.startRow; i < tile.endRow; i++)
                {
                    pixels.emplace_back(i, j); // store as (row, col)
                }
            }

            std::shuffle(pixels.begin(), pixels.end(), rng); // rng is std::mt19937

            for (auto &[i, j] : pixels)
            {
                Color3 pixelColor = pixelSampler.samplePixel(i, j, world);
            }
        },
        world.getCount(), 
        samplesPerPixel, 
        maxDepth 
    );
    pool.setup();
}

#endif