#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#define SFML_STATIC

#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <random>
#include <mutex>
#include "Image.h"

#include <chrono>

using Clock = std::chrono::high_resolution_clock;

std::random_device randomDevice;
std::mt19937 rng(randomDevice());

struct Tile
{
    int startRow;
    int endRow;
    int startCol;
    int endCol;
};

class ThreadPool
{
public:
    ThreadPool(Image *image, std::function<void(const Tile &)> processTile, int primitiveCount, int samplesPerPixel, int maxDepth) : image(image), processTile(processTile), primitiveCount(primitiveCount), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth) {}

    void displayThread()
    {
        sf::RenderWindow window(sf::VideoMode::getDesktopMode(), ("Quasar - RayTracing Engine"));
        window.setPosition(sf::Vector2i(0, 0));

        sf::Texture texture({(unsigned int)image->width, (unsigned int)image->height});

        sf::Sprite sprite(texture);

        sf::Font font;
        if (!font.openFromFile("./font/AntonRegular.ttf"))
        {
            std::clog << "Failed to load font file\n";
        }

        sf::Text pixelSize(font, L"Width: " + std::to_wstring(image->width) + L" Height: " + std::to_wstring(image->height), 16);
        pixelSize.setFillColor(sf::Color::White);
        pixelSize.setPosition(sf::Vector2f(10, 10));

        sf::Text sppText(font, L"Samples Per Pixel: " + std::to_wstring(samplesPerPixel), 16);
        sppText.setFillColor(sf::Color::White);
        sppText.setPosition(sf::Vector2f(10, 30));

        sf::Text maxDepthText(font, L"Max Depth: " + std::to_wstring(maxDepth), 16);
        maxDepthText.setFillColor(sf::Color::White);
        maxDepthText.setPosition(sf::Vector2f(10, 50));

        sf::Text primitiveCountText(font, L"Primitive Count: " + std::to_wstring(primitiveCount), 16);
        primitiveCountText.setFillColor(sf::Color::White);
        primitiveCountText.setPosition(sf::Vector2f(10, 70));

        sf::Text text(font, L"Tiles: " + std::to_wstring(tilesDone.load()) + L" / " + std::to_wstring(tiles.size()), 16);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(10, 90));

        sf::Text timeTaken(font, L"Time Taken: X minutes X seconds ", 16);
        timeTaken.setFillColor(sf::Color::White);
        timeTaken.setPosition(sf::Vector2f(10, 110));

        const float winW = sf::VideoMode::getDesktopMode().size.x;
        const float winH = sf::VideoMode::getDesktopMode().size.y;

        float scaleX = winW / image->width;
        float scaleY = winH / image->height;
        float scale = std::min(scaleX, scaleY);

        sprite.setScale(sf::Vector2f(scale, scale));

        float offsetX = (winW - image->width * scale) / 2.f;
        float offsetY = (winH - image->height * scale) / 2.f;
        sprite.setPosition(sf::Vector2f(offsetX, offsetY));

        while (window.isOpen())
        {
            while (auto event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    window.close();
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
                {
                    std::clog << "Saving Image...\n";

                    sf::Texture imageTexture({(unsigned int)image->width, (unsigned int)image->height});
                    imageTexture.update(image->pixels.data());
                    sf::Image screenshot = imageTexture.copyToImage();

                    if (!screenshot.saveToFile("Render/render.png"))
                    {
                        std::clog << "Failed to save Image\n";
                    }
                    else
                    {
                        std::clog << "Image saved as Render/render.png";
                    }
                }
            }

            texture.update(image->pixels.data());
            if (tilesDone.load() == tiles.size())
            {
                text.setString(L"Tiles: " + std::to_wstring(tilesDone.load()) + L" / " + std::to_wstring(tiles.size()) + L" Render Done!");
                timeTaken.setString(L"Time Taken: " + std::to_wstring(minutes) + L" minutes " + std::to_wstring(seconds) + L" seconds");
            }
            else
            {
                text.setString(L"Tiles: " + std::to_wstring(tilesDone.load()) + L" / " + std::to_wstring(tiles.size()));
            }

            window.clear();
            window.draw(sprite);
            window.draw(primitiveCountText);
            window.draw(text);
            window.draw(timeTaken);
            window.draw(sppText);
            window.draw(maxDepthText);
            window.draw(pixelSize);
            window.display();
        }
    }

    void setup()
    {
        auto startTime = Clock::now();

        std::thread guiThread(&ThreadPool::displayThread, this);

        threadCount = std::thread::hardware_concurrency() - 1;
        tileCount = threadCount * multiplier;
        // tileCount = 1 ;
        // threadCount = 1;

        int tilesX = std::ceil(std::sqrt(tileCount * image->aspectRatio));
        int tilesY = (tileCount + tilesX - 1) / tilesX;

        int baseW = image->width / tilesX;
        int baseH = image->height / tilesY;
        int remW = image->width % tilesX;
        int remH = image->height % tilesY;

        for (int ty = 0; ty < tilesY; ty++)
        {
            int startRow = ty * baseH + std::min(ty, remH);
            int endRow = startRow + baseH + (ty < remH ? 1 : 0);

            for (int tx = 0; tx < tilesX; tx++)
            {
                int startCol = tx * baseW + std::min(tx, remW);
                int endCol = startCol + baseW + (tx < remW ? 1 : 0);
                tiles.push_back({startRow,
                                 endRow,
                                 startCol,
                                 endCol});
            }
        }

        std::shuffle(tiles.begin(), tiles.end(), rng);
        std::atomic<int> grabTile(0);

        for (int i = 0; i < threadCount; i++)
        {
            workers.emplace_back(
                [this, &grabTile]()
                {
                    while (true)
                    {
                        int idx = grabTile.fetch_add(1);
                        if (idx >= tiles.size())
                            break;

                        Tile tile = tiles[idx];
                        processTile(tile);

                        tilesDone.fetch_add(1, std::memory_order_relaxed);
                    }
                });
        }

        for (auto &thread : workers)
        {
            thread.join();
        }

        auto endTime = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

        minutes = duration / 60;
        seconds = duration % 60;

        guiThread.join();
    }

private:
    int threadCount;
    int tileCount;
    int multiplier = 64;
    int primitiveCount;
    int samplesPerPixel;
    int maxDepth;

    int minutes;
    int seconds;

    std::vector<Tile> tiles;
    std::vector<std::thread> workers;
    std::function<void(const Tile &)> processTile;
    Image *image;

    std::atomic<int> tilesDone{0};
};

#endif