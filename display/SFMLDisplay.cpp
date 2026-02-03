//
// Created by ron on 2026-02-01.
//

#include "SFMLDisplay.h"
#include "DoubleFramebuffer.h"
#include <iostream>
#include <SFML/Window/Event.hpp>


SFMLDisplay::SFMLDisplay(DoubleFramebuffer& buffer)
    : IDisplay(buffer),
      texture(sf::Vector2u(static_cast<unsigned int>(dfb.getWidth()), static_cast<unsigned int>(dfb.getHeight()))),
      sprite(texture)
{
    auto w = static_cast<unsigned int>(dfb.getWidth());
    auto h = static_cast<unsigned int>(dfb.getHeight());

    window.create(sf::VideoMode({w * 2, h * 2}), "Preview");

    sprite.setScale({2.0f, 2.0f});

    std::cout << "SFML initialized at " << w << "x" << h << std::endl;
}

void SFMLDisplay::output() {
    if (!window.isOpen()) {
        return; // Don't do anything if the window is closed
    }

    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
    }

    // We only pull the data that is marked as "Front"
    texture.update(dfb.getFrontData());

    window.clear();
    window.draw(sprite);
    window.display();
}

bool SFMLDisplay::isOpen() const {
    return window.isOpen();
}