#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <math.h>
#include <complex>
#include <thread>
#include <string.h>

long double WINDOW_X_MAX = 2;
long double WINDOW_X_MIN = -2;
long double WINDOW_Y_MAX = 1.125;
long double WINDOW_Y_MIN = -1.125;

int WINDOW_X = 1920;
int WINDOW_Y = 1080;

int MAX_ITERATION = 100;
int BREAK_VALUE = 8;

const int MAX_THREAD_CONCURRENCY = std::thread::hardware_concurrency();

sf::Vector2i mouseBPress;

/*
    f(z) = z^2 + c where c  = a + b i
    z0 = 0 -> z1 = + c -> z2 = c^2 + c
*/

long double rescale(int invalue, long double maxoutrange, long double minoutrange, long double mininrange, long double maxinrange)
{
    long double x = (invalue - mininrange) / (maxinrange - mininrange);
    return minoutrange + (maxoutrange - minoutrange) * x;
}

void compute_thread(sf::Uint8 *pixels, int id)
{
    int width = (WINDOW_X / MAX_THREAD_CONCURRENCY);
    for (int x = id * width; x < (id + 1) * width; x++)
    {
        for (int y = 0; y < WINDOW_Y; y++)
        {
            long double a = rescale(x, WINDOW_X_MIN, WINDOW_X_MAX, 0, WINDOW_X);
            long double b = rescale(y, WINDOW_Y_MIN, WINDOW_Y_MAX, 0, WINDOW_Y);

            long double fixeda = a;
            long double fixedb = b;

            int i;
            for (i = 0; i < MAX_ITERATION; i++)
            {
                long double tempa = a * a - b * b;
                long double tempb = 2 * a * b;

                a = tempa + fixeda;
                b = tempb + fixedb;

                if (a * a + b * b > BREAK_VALUE)
                {
                    break;
                }
            }

            long double bright = rescale(i, MAX_ITERATION, 0, 0, 1);

            pixels[(x + y * WINDOW_X) * 4 + 0] = bright;
            pixels[(x + y * WINDOW_X) * 4 + 1] = 0;
            pixels[(x + y * WINDOW_X) * 4 + 2] = 0;
            pixels[(x + y * WINDOW_X) * 4 + 3] = 255;
        }
    }
}

void compute(sf::Uint8 *pixels)
{
    int width_thread = WINDOW_X / MAX_THREAD_CONCURRENCY;

    std::thread *threads = new std::thread[MAX_THREAD_CONCURRENCY];
    for (int i = 0; i < MAX_THREAD_CONCURRENCY; i++)
    {
        threads[i] = std::thread(compute_thread, pixels, i);
    }
    for (int i = 0; i < MAX_THREAD_CONCURRENCY; i++)
    {
        threads[i].join();
    }
}

void zoom(long double delta)
{

    long double ampli = 0.01f * (WINDOW_X_MAX - WINDOW_X_MIN);
    std::cout << delta << std::endl;
    WINDOW_X_MAX -= delta * 16 * ampli;
    WINDOW_Y_MAX -= delta * 9 * ampli;
    WINDOW_X_MIN += delta * 16 * ampli;
    WINDOW_Y_MIN += delta * 9 * ampli;

    std::cout << "WINDOW_X_MAX: " << WINDOW_X_MAX << std::endl;
    std::cout << "WINDOW_X_MIN:" << WINDOW_X_MIN << std::endl;
    std::cout << "WINDOW_Y_MAX: " << WINDOW_Y_MAX << std::endl;
    std::cout << "WINDOW_Y_MIN:" << WINDOW_Y_MIN << std::endl;
}

void drag(sf::Vector2i mousPos)
{
    sf::Vector2f mouv(mousPos.x - mouseBPress.x, mousPos.y - mouseBPress.y);

    mouv.x = rescale(mouv.x, (WINDOW_X_MAX - WINDOW_X_MIN), -(WINDOW_X_MAX - WINDOW_X_MIN), -1920, 1920);
    mouv.y = rescale(mouv.y, (WINDOW_Y_MAX - WINDOW_Y_MIN), -(WINDOW_Y_MAX - WINDOW_Y_MIN), -1080, 1080);

    WINDOW_Y_MAX += mouv.y;
    WINDOW_Y_MIN += mouv.y;

    WINDOW_X_MAX += mouv.x;
    WINDOW_X_MIN += mouv.x;

    std::cout << WINDOW_Y_MAX << std::endl;
    std::cout << WINDOW_Y_MIN << std::endl;
}

void console(bool *need_refresh)
{

    while (true)
    {
        std::string commands;
        std::cin >> commands;

        std::istringstream stream(commands);
        std::vector<std::string> tokens;
        std::string token;
        char **endPtr;

        while (stream.good())
        {
            stream >> token;
            std::cout << token << std::endl;
            tokens.push_back(token);
        }
        std::vector<std::string>::iterator it = tokens.begin();

        while (it != tokens.end())
        {
            if ((*it) == "quit")
                exit(0);
            if ((*it) == "iteration")
            {
                std::cout << "acutal iteration: " << MAX_ITERATION << std::endl;
                std::cin >> MAX_ITERATION;
                *need_refresh = true;
            }
            if ((*it) == "break")
            {
                std::cout << "acutal break: " << BREAK_VALUE << std::endl;
                std::cin >> BREAK_VALUE;
                *need_refresh = true;
            }
            it++;
        }
    }
}

int main(int argc, char **argv)
{

    bool need_refresh = true;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "MandelBrot fractal");
    std::thread thread_console(console, &need_refresh);
    thread_console.detach();

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

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type != 11)
            {
                need_refresh = true;
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::MouseWheelMoved)
                    zoom(event.mouseWheel.delta);
                if (event.type == sf::Event::MouseButtonPressed)
                    mouseBPress = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                if (event.type == sf::Event::MouseButtonReleased)
                    drag(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
        }

        if (need_refresh)
        {
            compute(pixels);
            texture->update(pixels);
            need_refresh = false;
            window.clear();
            window.draw(sprite);
        }

        window.display();
    }
    return 0;
}