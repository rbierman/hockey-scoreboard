#pragma once

#include <blend2d.h>
#include "display/DoubleFramebuffer.h"

class ScoreboardController {
public:
    explicit ScoreboardController(DoubleFramebuffer& dfb);

    void render();

    // Scoreboard state management methods
    void setHomeScore(int score);
    void setAwayScore(int score);
    void setTime(int minutes, int seconds);

private:
    DoubleFramebuffer& dfb;

    // Scoreboard state
    int homeScore = 0;
    int awayScore = 0;
    int timeMinutes = 12;
    int timeSeconds = 34;

    BLFontFace fontFace;
    BLFont font;

    void loadFont(const char* path);
};
