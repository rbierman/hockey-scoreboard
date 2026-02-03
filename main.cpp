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
#include "ResourceLocator.h" // Include ResourceLocator

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

    ResourceLocator resourceLocator; // Create ResourceLocator instance
    ScoreboardController scoreboard(dfb, resourceLocator); // Pass ResourceLocator to constructor
    scoreboard.setHomeTeamName("MAMBAS"); // Set initial home team name
    scoreboard.setAwayTeamName("ICEBREAKERS"); // Set initial away team name

    std::cout << "Starting scoreboard loop..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribScore(0, 9); // For scores
    std::uniform_int_distribution<> distribShots(0, 50); // For shots
    std::uniform_int_distribution<> distribPenalty(0, 5); // For penalty minutes (0-5 min)
    std::uniform_int_distribution<> distribPeriod(1, 3); // For period (1-3)


    int currentHomeScore = 22;
    int currentAwayScore = 15;
    int currentMinutes = 1;
    int currentSeconds = 0;
    int currentHomeShots = 0;
    int currentAwayShots = 0;
    int currentPenalty1Minutes = 0;
    int currentPenalty2Minutes = 0;
    int currentPeriod = 1;


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
                currentHomeScore = distribScore(gen);
                currentAwayScore = distribScore(gen);
                currentHomeShots = distribShots(gen);
                currentAwayShots = distribShots(gen);
                currentPenalty1Minutes = distribPenalty(gen);
                currentPenalty2Minutes = distribPenalty(gen);
                currentPeriod = distribPeriod(gen);
            }
        } else {
            currentSeconds--;
        }

        scoreboard.setHomeScore(currentHomeScore);
        scoreboard.setAwayScore(currentAwayScore);
        scoreboard.setTime(currentMinutes, currentSeconds);
        scoreboard.setHomeShots(currentHomeShots);
        scoreboard.setAwayShots(currentAwayShots);
        scoreboard.setPenalty1Minutes(currentPenalty1Minutes);
        scoreboard.setPenalty2Minutes(currentPenalty2Minutes);
        scoreboard.setCurrentPeriod(currentPeriod);

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