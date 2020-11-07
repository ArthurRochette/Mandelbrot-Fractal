#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <math.h>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>
#include <string.h>

using namespace std;

const int windowSize = 300;
float zoom = 1;
float camerax = 0, cameray = 0;
std::mutex m;
bool modif = true;
float buff = 0, buff2 = 0;
clock_t t;

const auto processor_count = std::thread::hardware_concurrency();
std::array<int, windowSize * windowSize> pixelsImage;
std::array<sf::Uint8, windowSize * windowSize * 4> pixels;

int mandelBrotColor(int x, int y, float cr = 0, float cy = 0)
{
    complex<float> a((float)(x + camerax) / (float)windowSize * zoom - cr, (y + cameray) / (float)windowSize * zoom - cy); //*2 dezoom
    complex<float> z(0, 0);

    int i = 0;
    while (abs(z) < 2 && i <= 20)
    {
        z = z * z + a; //z²+c
        i++;
    }
    if (i < 20)
        return (255 * i) / 20;
    else
        return 0;
}

void imageThread(int nmthread)
{

    float redCode;
    int nbrpixel = windowSize / processor_count;
    for (int x = nbrpixel * nmthread; x < nbrpixel * (nmthread + 1); x++)
    {
        for (int y = 0; y < windowSize; y++)
        {
            redCode = mandelBrotColor(x, y, buff, buff2);
            m.lock();

            pixelsImage[x + y * windowSize] = redCode;
            m.unlock();
        }
    }
}

void windowThread(int nmthread)
{
    float red_code;
    int nbrpixel = windowSize / processor_count;

    for (int x = nbrpixel * nmthread; x < nbrpixel * (nmthread + 1); x++)
    {
        for (int y = 0; y < windowSize; y++)
        {
            red_code = mandelBrotColor(x, y, buff, buff2);

            m.lock();

            pixels.at(x + y * windowSize) = red_code;
            pixels.at(x + y * windowSize + 1) = 0;
            pixels.at(x + y * windowSize + 2) = 0;
            pixels.at(x + y * windowSize + 3) = 255;
            m.unlock();
        }
    }
}

int main(int argc, char *argv[])
{
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Image image;
    sf::Vector2i mouseposfirst, mousepossecond;
    int nbpoints = 10000;
    float buff = 0, buff2 = 0;
    bool followmouse = false;
    t = clock();
    std::vector<std::thread> threadlist;

    if (argc > 1)
    {

        if (strcmp(argv[1], "-image") == 0)
        {
            cout << "set to image mod" << endl;
            for (int i = 0; i < processor_count; i++)
            {
                threadlist.emplace_back(imageThread, i);
            }
            for (int i = 0; i < processor_count; i++)
            {
                threadlist.at(i).join();
                cerr << "thread " << i << " joined" << endl;
            }

            ofstream my_image("Mandelbrot.ppm");
            if (my_image.is_open())
            {
                my_image << "P3\n"
                         << windowSize << " " << windowSize << " 255\n";
                for (int y = 0; y < windowSize; y++)
                {
                    for (int x = 0; x < windowSize; x++)
                    {
                        my_image << pixelsImage.at(x + y * windowSize) << ' ' << 0 << ' ' << 0 << "\n";
                    }
                }

                my_image.close();
            }
            t = clock() - t;
            cout << "time : " << t << "ms";
            return 0;
        }
    }

    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Mandelbrot");
    bool dede = true;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.delta == 1)
                {

                    zoom += zoom / 1.5f;
                    modif = true;
                }
                else
                {
                    if (zoom - 1 / zoom <= 0)
                    {
                        zoom = zoom / 1.5f;
                        modif = true;
                    }
                    else
                    {
                        zoom -= zoom / 1.5f;
                        modif = true;
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                {
                    camerax -= 1;
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    camerax += 1;
                }
                else if (event.key.code == sf::Keyboard::Left)
                {
                    cameray -= 1;
                }
                else
                {
                    cameray += 1;
                }
                modif = true;
            }
        }

        if (modif)
        {
            //call thread

            for (int i = 0; i < processor_count; i++)
            {
                threadlist.emplace_back(windowThread, i);
            }
            for (int i = 0; i < processor_count; i++)
            {
                threadlist.at(i).join();
                cerr << "thread " << i << " joined" << endl;
            }
            threadlist.clear();
            modif = false;
        }

        //Creer image correspondant au tableau de pixels

        image.create(windowSize, windowSize, pixels.data());

        texture.loadFromImage(image);
        sprite.setTexture(texture);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;
}