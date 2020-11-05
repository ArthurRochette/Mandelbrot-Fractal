#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <math.h>
#include <vector>
#include <complex>
#include <fstream>

using namespace std;

int windowSize = 300;
float zoom = 1;
float camerax = 0, cameray = 0;

bool modif = true;

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

int main(int argc, char *argv[])
{
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Image image;
    sf::Vector2i mouseposfirst, mousepossecond;
    int nbpoints = 10000;
    float buff = 0, buff2 = 0;
    bool followmouse = false;
    cout << "modif reel (0 by default)" << endl;
    cin >> buff;
    cout << "modif imag (0 by default)" << endl;
    cin >> buff2;
    if (argv[1] == "-image")
    {
        ofstream my_image("Mandelbrot.ppm");
        if (my_image.is_open())
        {
            my_image << "P3\n"
                     << windowSize << " " << windowSize << " 255\n";
            for (int i = 0; i < windowSize; i++)
            {
                for (int a = 0; a < windowSize; a++)
                {
                    my_image << mandelBrotColor(a, i, buff, buff2) << ' ' << 0 << ' ' << 0 << "\n";
                }
            }
            my_image.close();
        }
    }

    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Mandelbrot");
    sf::Uint8 pixels[windowSize * windowSize * 4]; //mon buffer image

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
            for (int i = 0; i < windowSize; i++)
            {
                for (int a = 0; a < windowSize; a++)
                {

                    pixels[(i + a * windowSize) * 4] = mandelBrotColor(a, i, buff, buff2); //R
                    pixels[(i + a * windowSize) * 4 + 1] = 0;                              //G
                    pixels[(i + a * windowSize) * 4 + 2] = 0;                              //B
                    pixels[(i + a * windowSize) * 4 + 3] = 255;                            //A
                }
            }
            modif = false;
            cout << zoom << endl;
        }

        //Creer image correspondant au tableau de pixels

        image.create(windowSize, windowSize, pixels);

        texture.loadFromImage(image);
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}