#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;

const int windowSize = 800;
const int MAX_ITERATION = 20;
std::mutex m;


const auto processor_count = std::thread::hardware_concurrency();
sf::Uint8 *redArray;

std::vector<std::thread *> threadlist;

struct rgba {
    int r;
    int g;
    int b;
    int a;
};

rgba mandelBrotColor(long double x, long double y, double cr = 0, double cy = 0) {
    rgba colors;
    int brightness;
    complex<double> a(x / (long double) windowSize - cr, y /(long double) windowSize - cy);
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
    return colors;

}

void draw(int index) {
    for (int y = 0; y < windowSize; y++) {
        for (int x = index * (windowSize / processor_count); x < (index + 1) * (windowSize / processor_count); x++) {
            redArray[x + y * windowSize] = mandelBrotColor(x, y).r;
        }
    }
}

void createThreadList() {
    for (int i = 0; i < processor_count; i++) {
        cout << "new thread :" << i << endl;
        threadlist.push_back(new std::thread(draw, i));
    }
}

void joinThreadList() {
    for (int i = 0; i < processor_count; i++) {
        cout << " thread :" << i << "joined"  << endl;
        threadlist.at(i)->join();
    }
}

sf::Uint8 *turnRtoRGBA(){
    auto *newArray = new sf::Uint8[windowSize*windowSize*4];
    for(int x = 0; x < windowSize*windowSize; x++){
        newArray[x*4]=redArray[x];//r
        newArray[x*4+1]=0;//g
        newArray[x*4+2]=0;//b
        newArray[x*4+3]=255;//a
    }

    return newArray;
}


int main(int argc, char *argv[]) {
    redArray = new sf::Uint8[windowSize * windowSize];
    createThreadList();
    joinThreadList();
    ofstream my_image("Mandelbrot.ppm");
    sf::Uint8 *newarray = turnRtoRGBA();

    if (my_image.is_open()) {
        my_image << "P3\n"
                 << windowSize << " " << windowSize << " 255\n";
        for (int y = 0; y < windowSize * windowSize*4; y+=4) {
            my_image << (int)newarray[y] << ' ' << (int)newarray[y+1] << ' ' << (int)newarray[y+2]  <<  "\n";
        }
        my_image.close();
    }

    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize,8), "MandelBrot Fractal");
    sf::Texture texture;

    if(!texture.create(windowSize, windowSize)){
        cerr << "erreur creating texture" << endl;
        return -1;
    }
    cout << "taille" << texture.getSize().x  << ":" << texture.getSize().y  << endl;
    cout << "taille w" << window.getSize().x << ":" << window.getSize().y << endl;
    sf::Sprite sprite(texture);

    while (window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
                return 0;
            }
        }
        sf::Uint8 *ptr = turnRtoRGBA();
        texture.update(ptr);
        window.clear(sf::Color::White);
        window.draw(sprite);
        window.display();
    }
    return 0;

}