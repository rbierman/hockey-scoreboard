#include "ScoreboardController.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <blend2d.h>

ScoreboardController::ScoreboardController(DoubleFramebuffer& dfb) : dfb(dfb) {
    loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
}

void ScoreboardController::loadFont(const char* path) {
    BLResult err = fontFace.createFromFile(path);
    if (err) {
        std::cerr << "Failed to load font: " << path << " (err=" << err << ")" << std::endl;
        // Try a fallback font
        err = fontFace.createFromFile("/usr/share/fonts/noto/NotoSans-Regular.ttf");
        if (err) {
            std::cerr << "Failed to load fallback font. Text will not be rendered." << std::endl;
            return;
        }
    }
    font.createFromFace(fontFace, 80.0f); // Fixed font size
}


void ScoreboardController::render() {
    const int w = dfb.getWidth();
    const int h = dfb.getHeight();

    // Create a BLImage that references our pixel buffer.
    BLImage image;
    BLResult err = image.createFromData(
        w, h, BL_FORMAT_PRGB32,
        dfb.getBackData(),          // Raw pixel data
        w * 4,                      // Stride in bytes
        BL_DATA_ACCESS_RW           // Read/write access
    );
    if (err) {
        std::cerr << "Failed to create Blend2D image from data: " << err << std::endl;
        return;
    }

    // Create a rendering context
    BLContext ctx(image);

    // --- Drawing starts here ---

    // Clear the background
    ctx.fillAll(BLRgba32(0x00000000));

    // Draw border
    ctx.setStrokeStyle(BLRgba32(0xFFFF0000)); // Bright blue (RGBA)
    ctx.setStrokeWidth(3.0); // 3-pixel wide border
    ctx.strokeRect(0, 0, w, h); // Rectangle covering the whole image

    // Set the fill style for the text
    ctx.setFillStyle(BLRgba32(0xFFFFFFFF)); // White

    // Hardcoded positions for 384x160 display
    // These values are eyeballed and might need adjustment after seeing the output
    double textY = 100.0; // Baseline Y position

    // Draw the home score
    std::string homeScoreStr = std::to_string(homeScore);
    ctx.fillUtf8Text(BLPoint(20, textY), font, homeScoreStr.c_str());

    // Draw the away score
    std::string awayScoreStr = std::to_string(awayScore);
    // This is a rough right-alignment. Actual alignment might need manual offset for font.
    // Assuming 2 digits for score max
    ctx.fillUtf8Text(BLPoint(w - 100, textY), font, awayScoreStr.c_str());

    // Draw the time (centered)
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << timeMinutes << ":"
               << std::setfill('0') << std::setw(2) << timeSeconds;
    std::string timeStr = timeStream.str();
    // This is a rough center alignment. Actual alignment might need manual offset for font.
    ctx.fillUtf8Text(BLPoint(w / 2 - 80, textY), font, timeStr.c_str());

    // --- Drawing ends here ---
    ctx.end();
}

void ScoreboardController::setHomeScore(int score) {
    homeScore = score;
}

void ScoreboardController::setAwayScore(int score) {
    awayScore = score;
}

void ScoreboardController::setTime(int minutes, int seconds) {
    timeMinutes = minutes;
    timeSeconds = seconds;
}