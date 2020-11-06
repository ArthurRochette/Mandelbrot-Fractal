#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <math.h>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>


using namespace std;

const int windowSize = 300;
float zoom = 1;
float camerax = 0, cameray = 0;
std::mutex m;
bool modif = true;
float buff = 0, buff2 = 0;
clock_t t;

const auto processor_count = std::thread::hardware_concurrency();
std::array<int, windowSize*windowSize> pixels;

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
    
    int redCode;
    int nbrpixel = windowSize / processor_count;
    for (int x = nbrpixel * nmthread; x < nbrpixel * (nmthread+1); x++)
    {
        for (int y = 0; y < windowSize; y++)
        {
            redCode = mandelBrotColor(x, y, buff, buff2);
            m.lock();
            
            pixels[x+y*windowSize] = redCode;
            m.unlock();
        }
    }
}

int main(int argc, char *argv[])
{
    t = clock();
    std::vector<std::thread> threadlist;
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
        for (int y =0; y < windowSize; y++)
            {
                for (int x = 0; x < windowSize; x++)
                {
                    my_image << pixels.at(x + y * windowSize) << ' ' << 0 << ' ' << 0 << "\n";
                }
            }

        my_image.close();
    }
    t = clock() - t;
    cout << "time : " << t << "ms";
    return 0;
}