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
double posx = 0, posy = 0, zoom = 1;
struct rgba {
    int r;
    int g;
    int b;
    int a;
};

rgba mandelBrotColor(long double x, long double y) {
    rgba colors;
    int brightness;
    complex<long double> a((long double) (x / (windowSize * zoom)) + posx,
                           (long double) (y / (windowSize * zoom)) + posy);
    complex<long double> z(0, 0);

    int i = 0;
    while (abs(z) < 2 && i <= MAX_ITERATION) {
        z = z * z + a; //z²+c
        i++;
    }

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
        cout << " thread :" << i << "joined" << endl;
        threadlist.at(i)->join();
    }
}

sf::Uint8 *turnRtoRGBA() {
    auto *newArray = new sf::Uint8[windowSize * windowSize * 4];
    for (int x = 0; x < windowSize * windowSize; x++) {
        newArray[x * 4] = redArray[x];//r
        newArray[x * 4 + 1] = 0;//g
        newArray[x * 4 + 2] = 0;//b
        newArray[x * 4 + 3] = 255;//a
    }

    return newArray;
}

void refresh() {
    try {
        createThreadList();
    } catch (std::system_error &error) {
        std::cout << error.code() << " " << error.what() << std::endl;
    }

    try {
        joinThreadList();
    } catch (std::system_error &error) {
        std::cout << error.code() << " " << error.what() << std::endl;
    }
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
        for (int y = 0; y < windowSize * windowSize * 4; y += 4) {
            my_image << (int) newarray[y] << ' ' << (int) newarray[y + 1] << ' ' << (int) newarray[y + 2] << "\n";
        }
        my_image.close();
    }

    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize, 8), "MandelBrot Fractal");
    sf::Texture texture;

    if (!texture.create(windowSize, windowSize)) {
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
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case (sf::Keyboard::Up):
                        if(zoom < 1){
                            posy -= 0.1;
                        }else
                            posy -= 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Down):
                        if(zoom < 1){
                            posy += 0.1;
                        }else
                            posy += 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Right):
                        if(zoom < 1){
                            posx += 0.1;
                        }else
                            posx += 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Left):
                        if(zoom < 1){
                            posx -= 0.1;
                        }else
                            posx -= 0.5 / zoom;
                        break;
                    case (sf::Keyboard::PageUp):
                        if (zoom < 1) {
                            zoom += 0.1;
                        } else {
                            zoom += 0.5 / zoom;
                        }
                        break;
                    case (sf::Keyboard::PageDown):
                        if(zoom < 1){
                            zoom -= 0.1;
                        }else
                            zoom -= 0.5 / zoom;
                        if (zoom < 0)
                            zoom = 0.1;
                        break;
                }
                std::cout << "DEBUG  zoom :" << zoom << " posx: " << posx << " posy: " << posy << endl;
            } else if (event.type == sf::Event::KeyReleased) {
                refresh();
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