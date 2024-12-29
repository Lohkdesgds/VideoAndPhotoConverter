#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    std::cout << "Hello world" << std::endl;

    sf::RenderWindow window(sf::VideoMode({200, 200}), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}