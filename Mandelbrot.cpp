#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

const int windowSize = 300;
const int MAX_ITERATION = 20;
std::mutex m;


const auto processor_count = 1;
sf::Uint8 *pixels = new sf::Uint8[windowSize * windowSize * 4]; //rgba

std::vector<std::thread *> threadlist;

struct rgba{
    int r;
    int g;
    int b;
    int a;
};

rgba mandelBrotColor(long double x, long double y, double cr = 1, double cy = 1)
{
    rgba colors;
    int brightness;
    complex<long double> a((long double)(x) / (long double)(windowSize - cr), (y ) / (long double)(windowSize - cy));
    complex<long double> z(0, 0);

    int i = 0;
    while (abs(z) < 2 && i <= MAX_ITERATION)
    {
        z = z * z + a; //z²+c
        i++;
    }
    if (i < MAX_ITERATION )
        brightness = (255 * i) / MAX_ITERATION;
    else
        brightness = 0;

    colors.r = brightness;
    if(colors.r > 255){
        colors.r = 255;
    }
    colors.g = 0;
    colors.b = 0;
    colors.a = 255;
    return colors;
}

void draw(int index) {
    for (int y = 0; y < windowSize*4; y++) {
        for (int x = index * (windowSize/processor_count); x < ((windowSize * 4)/processor_count)*index; x++) {
            rgba colors = mandelBrotColor(x,y);
            if(colors.r > 240) cout << "ahoy!!"<<endl;
            m.lock();
            pixels[x + y * windowSize] = colors.r;//r
            pixels[x + y * windowSize +1] = colors.g;//g
            pixels[x + y * windowSize +2 ] = colors.b;//b
            pixels[x + y * windowSize +4] =colors.a;//a
            m.unlock();
        }
    }
}

void createThreadList() {
    for (int i = 0; i < processor_count; i++) {
        threadlist.push_back(new std::thread(draw, i));
        std::cout << "Thread " << i << " created " << endl;
    }
}

void syncThreadList() {
    for (int i = 0; i < processor_count; i++) {
        threadlist.at(i)->join();
        std::cout << "Thread " << i << " joined " << endl;
    }
}


int main(int argc, char *argv[]) {
    cout << "CPU core: " << processor_count << endl;
    createThreadList();

    syncThreadList();
    if(argc == 1){


        ofstream  file;
        file.open("fractal.txt");
        file << "P3\n" << windowSize << " " << windowSize << " 255\n";
        for(int i = 0 ; i < windowSize*windowSize*4; i+=4){
            file << (int)pixels[i] << " " << (int)pixels[i] << " " << (int)pixels[i] << "\n";
        }
        file.close();
        cout << "image done" << endl;
        return 1;
    }

    sf::Texture texture;
    if (!texture.create(windowSize, windowSize)) {
        cerr << "Failed create Texture" << endl;
    }
    sf::Sprite sprite(texture);
    sf::Image image;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Mandelbrot");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        texture.update(pixels);
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;
}