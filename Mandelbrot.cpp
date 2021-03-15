#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <complex>
#include <fstream>
#include <thread>
#include <mutex>
#include <string.h>

using namespace std;

int windowSizeX = 800;
int windowSizeY = 800;
int MAX_ITERATION = 20;
std::mutex m;


const auto processor_count = std::thread::hardware_concurrency();
sf::Uint8 *redArray;

std::vector<std::thread *> threadlist;
double posx = -2.2, posy = -1.3, zoom = 0.3;
struct rgba {
    int r;
    int g;
    int b;
    int a;
};

rgba mandelBrotColor(long double x, long double y) {
    rgba colors;
    int brightness;
    complex<long double> a((long double) (x / (windowSizeX * zoom)) + posx,
                           (long double) (y / (windowSizeY * zoom)) + posy);
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
    for (int y = 0; y < windowSizeY; y++) {
        for (int x = index * (windowSizeX / processor_count); x < (index + 1) * (windowSizeX / processor_count); x++) {
            redArray[x + y * windowSizeX] = mandelBrotColor(x, y).r;
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
    auto *newArray = new sf::Uint8[windowSizeX * windowSizeY * 4];
    for (int x = 0; x < windowSizeX * windowSizeY; x++) {
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
    if(argc > 1){
        cout << "sup"<<endl;
        if(strcmp(argv[1],"ppm")==0){
            cout << "image size x:" << endl;
            cin >> windowSizeX;
            cout << "image size y:"  << endl;
            cin >> windowSizeY;


            redArray = new sf::Uint8[windowSizeX * windowSizeY];
            createThreadList();
            joinThreadList();
            ofstream my_image("Mandelbrot.ppm");
            sf::Uint8 *newarray = turnRtoRGBA();

            if (my_image.is_open()) {
                my_image << "P3\n"
                         << windowSizeX << " " << windowSizeY << " 255\n";
                for (int y = 0; y < windowSizeX * windowSizeY * 4; y += 4) {
                    my_image << (int) newarray[y] << ' ' << (int) newarray[y + 1] << ' ' << (int) newarray[y + 2] << "\n";
                }
                my_image.close();
            }
            return 1;
        }
    }
    cout << "MandelBrot fractal" << endl;
    cout << "By @ArthurSenpaii github" << endl;
    cout << endl << "Enter window dimension x" << endl;
    cin >> windowSizeX;
    cout << endl << "Enter window dimension y" << endl;
    cin >> windowSizeY;
    sf::RenderWindow *window;
    if(sf::VideoMode::getDesktopMode().width == windowSizeX && sf::VideoMode::getDesktopMode().height == windowSizeY){

        window = new sf::RenderWindow(sf::VideoMode(sf::Style::Fullscreen, 8), "MandelBrot Fractal");
    }else{
        window = new sf::RenderWindow(sf::VideoMode(windowSizeX, windowSizeY, 8), "MandelBrot Fractal");
    }

    redArray = new sf::Uint8[windowSizeX * windowSizeY];
    createThreadList();
    joinThreadList();
    ofstream my_image("Mandelbrot.ppm");
    sf::Uint8 *newarray = turnRtoRGBA();

    sf::Texture texture;

    if (!texture.create(windowSizeX, windowSizeY)) {
        cerr << "erreur creating texture" << endl;
        return -1;
    }
    sf::Sprite sprite(texture);

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return 0;
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case (sf::Keyboard::Up):
                        if (zoom < 1) {
                            posy -= 0.1;
                        } else
                            posy -= 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Down):
                        if (zoom < 1) {
                            posy += 0.1;
                        } else
                            posy += 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Right):
                        if (zoom < 1) {
                            posx += 0.1;
                        } else
                            posx += 0.5 / zoom;
                        break;
                    case (sf::Keyboard::Left):
                        if (zoom < 1) {
                            posx -= 0.1;
                        } else
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
                        if (zoom < 1) {
                            zoom -= 0.1;
                        } else
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
        window->clear(sf::Color::White);
        window->draw(sprite);
        window->display();
    }
    return 0;

}