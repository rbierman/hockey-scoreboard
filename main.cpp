#include <iostream>
#include <unistd.h>
#include <vector>
#include <random> // For random numbers for dynamic scores/time

#include "display/DoubleFramebuffer.h"
#include "display/SFMLDisplay.h"
#include "display/ColorLightDisplay.h"
#include "ScoreboardController.h"

int main() {
    int w = 384, h = 160; // Original dimensions, Blend2D will scale later
    // int w = 64, h = 32; // Original dimensions, Blend2D will scale later

    // The single source of truth for the scoreboard's state
    DoubleFramebuffer dfb(w, h);

    // A list of displays to send the framebuffer to
    std::vector<IDisplay*> displays;

    // Preview window
    SFMLDisplay sfml(dfb);
    displays.push_back(&sfml);

    // Hardware display
    ColorLightDisplay cl("enx00e04c68012e", dfb);
    displays.push_back(&cl);

    // Scoreboard controller
    ScoreboardController scoreboard(dfb);

    std::cout << "Starting scoreboard loop..." << std::endl;

    // Random number generation for testing dynamic content
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9);
    std::uniform_int_distribution<> timeDist(0, 59);

    int currentHomeScore = 0;
    int currentAwayScore = 0;
    int currentMinutes = 12;
    int currentSeconds = 0; // Start at 0 to make it tick

    while(sfml.isOpen()) {
        // --- LOGIC ---
        // Update scoreboard state (for demonstration, just change time and scores)
        if (currentSeconds == 0) {
            currentSeconds = 59;
            currentMinutes--;
            if (currentMinutes < 0) {
                currentMinutes = 20; // Reset period time
                currentHomeScore = distrib(gen); // New random scores
                currentAwayScore = distrib(gen);
            }
        } else {
            currentSeconds--;
        }

        scoreboard.setHomeScore(currentHomeScore);
        scoreboard.setAwayScore(currentAwayScore);
        scoreboard.setTime(currentMinutes, currentSeconds);

        // Render the scoreboard to the framebuffer
        scoreboard.render();

        // --- DISPLAY ---
        // Swap buffers to make the drawing visible to the displays
        dfb.swap();

        // Output to all displays
        for (const auto disp : displays) {
            disp->output();
        }

        usleep(1000000); // Update once per second (1,000,000 microseconds)
    }

    return 0;
}