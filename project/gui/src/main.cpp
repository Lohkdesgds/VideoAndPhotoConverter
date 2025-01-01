#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include <test.h>

int main()
{
    std::cout << "Hello world" << std::endl;
    hello_world();

    sf::RenderWindow window(sf::VideoMode({200, 200}), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}