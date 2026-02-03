#include <iostream>
#include <unistd.h>
#include <vector>
#include <random>
#include <string>

#include "display/DoubleFramebuffer.h"
#include "display/SFMLDisplay.h"
#include "display/ColorLightDisplay.h"
#include "ScoreboardController.h"
#include "CommandLineArgs.h" // Include the new header

int main(int argc, char* argv[]) {

    CommandLineArgs args(argc, argv); // Create an instance of the new class

    if (args.showHelp()) { // Check if help was requested
        args.printHelp(argv[0]);
        return 0;
    }

    int w = 384, h = 160;

    DoubleFramebuffer dfb(w, h);
    std::vector<IDisplay*> displays; // Use pointers for polymorphic behavior and dynamic allocation

    SFMLDisplay* sfmlDisplay = nullptr;
    if (args.enableSFML()) { // Use getter
        sfmlDisplay = new SFMLDisplay(dfb);
        displays.push_back(sfmlDisplay);
    }

    if (args.enableColorLight()) { // Use getter
        ColorLightDisplay* clDisplay = new ColorLightDisplay(args.colorLightInterface(), dfb); // Use getter
        displays.push_back(clDisplay);
    }

    if (displays.empty()) {
        std::cerr << "No display enabled. Please use -s or -c to enable at least one display." << std::endl;
        return 1;
    }

    ScoreboardController scoreboard(dfb);

    std::cout << "Starting scoreboard loop..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9);

    int currentHomeScore = 0;
    int currentAwayScore = 0;
    int currentMinutes = 12;
    int currentSeconds = 0;

    // Main application loop
    bool running = true;
    while(running) {
        // If SFML display is enabled, its window state controls the loop
        if (sfmlDisplay && !sfmlDisplay->isOpen()) {
            running = false;
        }
        if (!running) break; // Exit if SFML window closed

        // --- LOGIC ---
        if (currentSeconds == 0) {
            currentSeconds = 59;
            currentMinutes--;
            if (currentMinutes < 0) {
                currentMinutes = 20;
                currentHomeScore = distrib(gen);
                currentAwayScore = distrib(gen);
            }
        } else {
            currentSeconds--;
        }

        scoreboard.setHomeScore(currentHomeScore);
        scoreboard.setAwayScore(currentAwayScore);
        scoreboard.setTime(currentMinutes, currentSeconds);

        scoreboard.render();

        // --- DISPLAY ---
        dfb.swap();

        for (IDisplay* disp : displays) {
            disp->output();
        }

        usleep(1000000);
    }

    // Clean up dynamically allocated display objects
    for (IDisplay* disp : displays) {
        delete disp;
    }
    displays.clear();

    return 0;
}
