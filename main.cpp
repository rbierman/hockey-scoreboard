#include <iostream>
#include <vector>
#include <string>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>

#include "display/DoubleFramebuffer.h"
#include "display/SFMLDisplay.h"
#include "display/ColorLightDisplay.h"
#include "ScoreboardController.h"
#include "ScoreboardRenderer.h"
#include "CommandLineArgs.h"
#include "ResourceLocator.h"

void handleInput(sf::RenderWindow& window, ScoreboardController& scoreboard) {
    while (const std::optional event = window.pollEvent()) {
        if (event->getIf<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::Space:
                    scoreboard.toggleClock();
                    break;
                case sf::Keyboard::Key::C:
                    scoreboard.setClockMode(ClockMode::Clock);
                    break;
                case sf::Keyboard::Key::S:
                    scoreboard.setClockMode(ClockMode::Stopped);
                    break;
                case sf::Keyboard::Key::R:
                    scoreboard.setClockMode(ClockMode::Running);
                    break;
                case sf::Keyboard::Key::Num1:
                    scoreboard.addHomeScore(1);
                    break;
                case sf::Keyboard::Key::Num2:
                    scoreboard.addAwayScore(1);
                    break;
                case sf::Keyboard::Key::Num3:
                    scoreboard.addHomeShots(1);
                    break;
                case sf::Keyboard::Key::Num4:
                    scoreboard.addAwayShots(1);
                    break;
                case sf::Keyboard::Key::H:
                    scoreboard.addHomePenalty(120, 22);
                    break;
                case sf::Keyboard::Key::A:
                    scoreboard.addAwayPenalty(120, 33);
                    break;
                case sf::Keyboard::Key::P:
                    scoreboard.nextPeriod();
                    break;
                default:
                    break;
            }
        }
    }
}

int main(int argc, char* argv[]) {

    CommandLineArgs args(argc, argv);

    if (args.showHelp()) {
        args.printHelp(argv[0]);
        return 0;
    }

    int w = 384, h = 160;

    DoubleFramebuffer dfb(w, h);
    std::vector<IDisplay*> displays;

    SFMLDisplay* sfmlDisplay = nullptr;
    if (args.enableSFML()) {
        sfmlDisplay = new SFMLDisplay(dfb);
        displays.push_back(sfmlDisplay);
    }

    if (args.enableColorLight()) {
        ColorLightDisplay* clDisplay = new ColorLightDisplay(args.colorLightInterface(), dfb);
        displays.push_back(clDisplay);
    }

    if (displays.empty()) {
        std::cerr << "No display enabled. Please use -s or -c to enable at least one display." << std::endl;
        return 1;
    }

    ResourceLocator resourceLocator;
    ScoreboardController scoreboard;
    ScoreboardRenderer renderer(dfb, resourceLocator);

    scoreboard.setHomeTeamName("MAMBAS");
    scoreboard.setAwayTeamName("BREAKERS");
    scoreboard.setClockMode(ClockMode::Stopped);

    std::cout << "Starting scoreboard loop..." << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "SIMULATOR CONTROLS:" << std::endl;
    std::cout << "  [Space] Toggle Start/Stop" << std::endl;
    std::cout << "  [1]     Home Score+" << std::endl;
    std::cout << "  [2]     Away Score+" << std::endl;
    std::cout << "  [3]     Home Shots+" << std::endl;
    // Align columns for readability
    std::cout << "  [4]     Away Shots+" << std::endl;
    std::cout << "  [H]     Home Penalty (2:00)" << std::endl;
    std::cout << "  [A]     Away Penalty (2:00)" << std::endl;
    std::cout << "  [P]     Next Period" << std::endl;
    std::cout << "  [C]     Clock Mode (System Time)" << std::endl;
    std::cout << "  [R]     Resume Running" << std::endl;
    std::cout << "  [S]     Stop Clock" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    sf::Clock clock;

    // Main application loop
    bool running = true;
    while(running) {
        if (sfmlDisplay) {
            if (!sfmlDisplay->isOpen()) {
                running = false;
            } else {
                handleInput(sfmlDisplay->getWindow(), scoreboard);
            }
        }
        
        if (!running) break;

        // --- LOGIC ---
        sf::Time elapsed = clock.restart();
        scoreboard.update(elapsed.asSeconds());

        // --- RENDER ---
        renderer.render(scoreboard.getState());

        // --- DISPLAY ---
        dfb.swap();

        for (IDisplay* disp : displays) {
            disp->output();
        }

        // Avoid pegged CPU
        sf::sleep(sf::milliseconds(10));
    }

    for (IDisplay* disp : displays) {
        delete disp;
    }
    displays.clear();

    return 0;
}
