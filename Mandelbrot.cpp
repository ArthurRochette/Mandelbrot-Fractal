#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <complex>
#include <fstream>

using namespace std;

int imgSize = 1000;


int mandelBrotColor(int x, int y, float cr = 0, float cy =0)
{
    complex<float> a((float)x / imgSize - cr, y / (float)imgSize - cy);
    complex<float> z(0, 0);
    
    int i = 0;
    while (abs(z) < 2 && i <= 20){
        z = z * z *z + z*z+ a; //z³+z²+c
        i++;
    }
    if (i < 20)
        return (255*i)/20;
    else
        return 0;
}


int main()
{
    int nbpoints = 10000;
    float buff = 0, buff2 = 0;
    cout << "modif reel (0 by default)"<<endl;
    cin >> buff;
    cout << "modif imag (0 by default)" << endl;
    cin >> buff2;
    cout << "Image Size (square, 1000 by default):"<<endl;
    cin >> imgSize;

    ofstream my_image("Mandelbrot.ppm");
    if (my_image.is_open())
    {
        my_image << "P3\n"<< imgSize << " " << imgSize << " 255\n";
        for (int i = 0; i < imgSize; i++)
        {
            for (int a = 0; a < imgSize; a++)
            {
                int color = mandelBrotColor(i, a, buff, buff2);
                my_image << color << ' ' << 0 << ' ' << 0 << "\n";
            }
        }
        my_image.close();
    }
    return 0;
}


