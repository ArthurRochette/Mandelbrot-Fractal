#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

int windowSizex = 800;
int windowSizey = 800;
const int MAX_ITERATION = 20;
std::mutex m;


const auto processor_count = std::thread::hardware_concurrency();
sf::Uint8 *fractalArray;

std::vector<std::thread *> threadList;

struct rgba {
    int r;
    int g;
    int b;
    int a;
};

rgba mandelBrotColor(long double x, long double y, double cr = 0, double cy = 0) {
    rgba colors;
    int brightness;
    complex<double> a(x / (long double) windowSizex - cr, y / (long double) windowSizey - cy);
    complex<double> z(0, 0);

    int i = 0;
    while (abs(z) < 2 && i <= MAX_ITERATION) {
        z = z * z + a; //z²+c
        i++;
    }
    brightness = (255 * i) / MAX_ITERATION;

    if (i < MAX_ITERATION)
        brightness = (255 * i) / MAX_ITERATION;
    else
        brightness = 0;

    colors.r = brightness;
    if (colors.r > 255) {
        colors.r = 255;
    }
    colors.g = brightness;
    colors.b = (int) sqrt(brightness);
    colors.a = 255;
    return colors;

}

void draw(int index) {

    for (int y = 0; y < windowSizey; y++) {
        for (int x = index * (windowSizex / processor_count); x < (index + 1) * (windowSizex / processor_count); x++) {

            rgba colors = mandelBrotColor(x, y);
            m.lock();
            fractalArray[x * 4 + y * windowSizey * 4] = colors.r;
            fractalArray[x * 4 + y * windowSizey * 4 + 1] = colors.g;
            fractalArray[x * 4 + y * windowSizey * 4 + 2] = colors.b;
            fractalArray[x * 4 + y * windowSizey * 4 + 3] = colors.a;
            m.unlock();
        }
    }
}

void createThreadList() {
    for (int i = 0; i < processor_count; i++) {
        try{
            threadList.push_back(new std::thread(draw, i));
            cout << "new thread :" << i << endl;

        }catch( std::system_error &_err){
            cerr << "create thread :" << i << " errcode "<<_err.code() << " what ?? " << _err.what() << endl;
        }

    }
}

void joinThreadList() {
    for (int i = 0; i < processor_count; i++) {
        try{
            threadList.at(i)->join();
            cout << " thread joined :" << i << endl;

        }catch( std::system_error &_err){
            cerr << "join thread :" << i << " errcode " <<_err.code() << " what ?? " << _err.what() << endl;
        }
    }
}


int main(int argc, char *argv[]) {

    sf::RenderWindow window(sf::VideoMode(windowSizex, windowSizey, 8), "MandelBrot Fractal");
    sf::Texture texture;

    fractalArray = new sf::Uint8[windowSizex * windowSizey * 4];

    createThreadList();
    joinThreadList();


    //picture drawer
/*
    ofstream my_image("Mandelbrot.txt");
    if (my_image.is_open()) {
        my_image << "P3\n"
                 << windowSizex << " " << windowSizey << " 255\n";
        for (int y = 0; y < windowSizex * windowSizey * 4; y += 4) {
            my_image << (int) fractalArray[y] << ' ' << (int) fractalArray[y + 1] << ' ' << (int) fractalArray[y + 2]
                     << "\n";
        }
        my_image.close();
    }
    */

    if (!texture.create(windowSizex, windowSizey)) {
        cerr << "erreur creating texture" << endl;
        return -1;
    }
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            switch (event.type) {
                case sf::Event::Resized:
                    cout << "change" << endl;
                    windowSizex = event.size.width;
                    windowSizey = event.size.height;
                    if (!texture.create(windowSizex, windowSizey)) {
                        cerr << "erreur creating texture" << endl;
                        return -1;
                    }
                    delete(fractalArray);
                    fractalArray = new sf::Uint8[windowSizex * windowSizey * 4];
                    createThreadList();
                    joinThreadList();
                    break;
            }
        }
        texture.update(fractalArray);
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;

}