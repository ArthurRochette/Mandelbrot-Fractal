#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <math.h>
#include <complex>

#define WINDOW_X_MAX 2
#define WINDOW_X_MIN -2
#define WINDOW_Y_MAX 1.125
#define WINDOW_Y_MIN -1.125

int WINDOW_X = 1920;
int WINDOW_Y = 1080;

int MAX_ITERATION = 100;
int BREAK_VALUE = 16;

/*
    f(z) = z^2 + c where c  = a + b i
    z0 = 0 -> z1 = + c -> z2 = c^2 + c
 */

float rescale(int invalue, float maxoutrange, float minoutrange, float mininrange, float maxinrange)
{
    float x = (invalue-mininrange) / (maxinrange-mininrange);
    return minoutrange + (maxoutrange-minoutrange) * x;
}

void compute(sf::Uint8 *pixels)
{

    for (int x = 0; x < WINDOW_X; x++)
    {
        
        for (int y = 0; y < WINDOW_Y; y++)
        {
            float a = rescale(x,WINDOW_X_MIN, WINDOW_X_MAX, WINDOW_X,0);
            float b = rescale(y, WINDOW_Y_MIN,WINDOW_Y_MAX,WINDOW_Y,0);

            float fixeda = a;
            float fixedb = b;

            int i;
            for (i = 0; i < MAX_ITERATION; i++)
            {
                float tempa = a * a - b * b;
                float tempb = 2 * a * b;

                a = tempa + fixeda;
                b = tempb + fixedb;

                if (a*a+b*b > BREAK_VALUE)
                {
                    break;
                }
            }

            float bright = rescale(i, MAX_ITERATION,0,0,1);
            
            pixels[(x + y * WINDOW_X) * 4 + 0] = bright;
            pixels[(x + y * WINDOW_X) * 4 + 1] = 0;
            pixels[(x + y * WINDOW_X) * 4 + 2] = 0;
            pixels[(x + y * WINDOW_X) * 4 + 3] = 255;
        }
    }
}

int main(int argc, char **argv)
{

    std::cout << "hello world" << std::endl;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "MandelBrot fractal");

    sf::Uint8 pixels[1920 * 1080 * 4];
    for (int i = 0; i < 1920 * 1080 * 4; i += 4)
    {
        pixels[i + 0] = 220;
        pixels[i + 1] = 40;
        pixels[i + 2] = 150;
        pixels[i + 3] = 255;
    }
    sf::Texture *texture = new sf::Texture;

    if (!texture->create(1920, 1080))
        return -1;

    sf::Sprite sprite(*texture);
    compute(pixels);
    texture->update(pixels);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
    return 0;
}