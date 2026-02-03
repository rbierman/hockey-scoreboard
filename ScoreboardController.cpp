#include "ScoreboardController.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <blend2d.h>
#include <string> // Ensure string is included for std::string functions

ScoreboardController::ScoreboardController(DoubleFramebuffer& dfb, const ResourceLocator& resourceLocator)
    : dfb(dfb), _resourceLocator(resourceLocator) {
    loadFont((_resourceLocator.getFontsDirPath() + "/DS-DIGI.TTF").c_str());
}

void ScoreboardController::loadFont(const char* path) {
    BLResult err = fontFace.createFromFile(path);

    font.createFromFace(fontFace, 75.0f); // Main score/time font size
    shotsFont.createFromFace(fontFace, 28.0f); // Smaller font for SOG and penalties
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

    // Clear the background to a dark blue
    ctx.clearAll();
    // ctx.fillAll(fillAllBLRgba32(0xFF000000));

    // Draw border
    ctx.setStrokeStyle(BLRgba32(0xFFFF0000)); // Bright blue (ABGR)
    ctx.setStrokeWidth(3.0); // 3-pixel wide border
    ctx.strokeRect(0, 0, w, h); // Rectangle covering the whole image

    // Set the fill style for the text
    ctx.setFillStyle(BLRgba32(0xFFFFFFFF)); // White

    // Hardcoded positions for 384x160 display
    double penaltyTextY = 150.0; // Baseline Y position for penalties

    BLGlyphBuffer gb; // Initialize glyph buffer once

    // --- Team Names ---
    ctx.setFillStyle(BLRgba32(0xFFFFFFFF)); // White for team names

    // Calculate teamNameTextY based on font metrics (ascent only)
    BLFontMetrics teamNameFontMetrics = shotsFont.metrics(); // Assuming shotsFont for team names
    double teamNameTextY = teamNameFontMetrics.ascent; // Using ascent directly

    // Home Team Name
    ctx.fillUtf8Text(BLPoint(2, teamNameTextY), shotsFont, homeTeamName.c_str());

    // Away Team Name (right-aligned)
    gb.setUtf8Text(awayTeamName.c_str(), awayTeamName.length());
    BLTextMetrics tmAwayName; // Use a distinct variable name
    shotsFont.getTextMetrics(gb, tmAwayName);
    gb.clear();

    double awayTeamNameWidth = tmAwayName.advance.x; // Use advance.x for width
    ctx.fillUtf8Text(BLPoint(w - awayTeamNameWidth - 2, teamNameTextY), shotsFont, awayTeamName.c_str());

    // Calculate Y position for the line below team names
    double lineBelowTeamNamesY = 28;

    // Draw horizontal line below team names
    ctx.setStrokeStyle(BLRgba32(0xFFFF0000)); // Bright blue (ABGR)
    ctx.setStrokeWidth(1.0); // 1-pixel wide line
    ctx.strokeLine(0, lineBelowTeamNamesY, w, lineBelowTeamNamesY);


    // --- Scores ---
    // Draw the home score
    BLFontMetrics fontMetrics = font.metrics(); // Assuming shotsFont for team names
    double mainTextY = lineBelowTeamNamesY + fontMetrics.ascent - 6;
    std::string homeScoreStr = std::to_string(homeScore);
    ctx.fillUtf8Text(BLPoint(2, mainTextY), font, homeScoreStr.c_str());

    // Draw the away score
    std::string awayScoreStr = std::to_string(awayScore);
    BLTextMetrics tmAwayScore{}; // For away score, using main font
    gb.setUtf8Text(awayScoreStr.c_str(), awayScoreStr.length());
    font.getTextMetrics(gb, tmAwayScore);
    gb.clear();
    double awayScoreWidth = tmAwayScore.advance.x; // Use advance.x for width
    ctx.fillUtf8Text(BLPoint(w - awayScoreWidth - 4 , mainTextY), font, awayScoreStr.c_str());

    // Draw the time (centered)
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << timeMinutes << ":"
               << std::setfill('0') << std::setw(2) << timeSeconds;
    std::string timeStr = timeStream.str();
    BLTextMetrics tmTime{};
    gb.setUtf8Text(timeStr.c_str(), timeStr.length());
    font.getTextMetrics(gb, tmTime);
    gb.clear();
    double timeWidth = tmTime.advance.x; // Use advance.x for width
    ctx.setFillStyle(BLRgba32(0xFF0000FF)); // Bright red (ABGR)
    ctx.fillUtf8Text(BLPoint(w / 2 - timeWidth / 2, mainTextY), font, timeStr.c_str());

    // Draw horizontal line below the score / time
    double lineBelowScoreTimeY = 94;
    ctx.setStrokeStyle(BLRgba32(0xFFFF0000)); // Bright blue (ABGR)
    ctx.setStrokeWidth(1.0); // 1-pixel wide line
    ctx.strokeLine(0, lineBelowScoreTimeY, w, lineBelowScoreTimeY);


    // --- Shots on Goal ---
    double sogTextY = lineBelowScoreTimeY + 2 + teamNameFontMetrics.ascent;
    ctx.setFillStyle(BLRgba32(0xFFCCCCCC)); // Lighter gray for SOG text

    // Home SOG
    std::string homeShotsStr = "SOG: " + std::to_string(homeShots);
    ctx.fillUtf8Text(BLPoint(2, sogTextY), shotsFont, homeShotsStr.c_str()); // 60 for number + padding


    // Period label and number (centered)
    std::string periodStr = "PER: " + std::to_string(currentPeriod);
    BLTextMetrics tmPeriod{};
    gb.setUtf8Text(periodStr.c_str(), periodStr.length());
    shotsFont.getTextMetrics(gb, tmPeriod);
    gb.clear();
    double periodWidth = tmPeriod.advance.x; // Use advance.x for width
    ctx.fillUtf8Text(BLPoint(w / 2 - periodWidth / 2, sogTextY), shotsFont, periodStr.c_str());

    // Away SOG number
    std::string awayShotsStr = "SOG: " + std::to_string(awayShots);
    BLTextMetrics tmAwayShots;
    gb.setUtf8Text(awayShotsStr.c_str(), awayShotsStr.length());
    shotsFont.getTextMetrics(gb, tmAwayShots);
    gb.clear();
    double awayShotsWidth = tmAwayShots.advance.x; // Use advance.x for width
    ctx.fillUtf8Text(BLPoint(w - awayShotsWidth - 2, sogTextY), shotsFont, awayShotsStr.c_str());

    // Draw horizontal line below the score / time
    double lineBelowSOGY = 125;
    ctx.setStrokeStyle(BLRgba32(0xFFFF0000)); // Bright blue (ABGR)
    ctx.setStrokeWidth(1.0); // 1-pixel wide line
    ctx.strokeLine(0, lineBelowSOGY, w, lineBelowSOGY);

    // --- Penalties ---
    ctx.setFillStyle(BLRgba32(0xFFFFAA00)); // Orange for penalties

    // Penalty 1 (left)
    std::string p1Str = "P1: " + std::to_string(penalty1Minutes);
    ctx.fillUtf8Text(BLPoint(20, penaltyTextY), shotsFont, p1Str.c_str());

    // Penalty 2 (right-aligned)
    std::string p2Str = "P2: " + std::to_string(penalty2Minutes);
    BLTextMetrics tmP2;
    gb.setUtf8Text(p2Str.c_str(), p2Str.length());
    shotsFont.getTextMetrics(gb, tmP2);
    gb.clear();
    double p2Width = tmP2.advance.x; // Use advance.x for width
    ctx.fillUtf8Text(BLPoint(w - p2Width - 20, penaltyTextY), shotsFont, p2Str.c_str());

    // --- Period ---
    ctx.setFillStyle(BLRgba32(0xFFFFFFFF)); // White for period text

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

void ScoreboardController::setHomeShots(int shots) {
    homeShots = shots;
}

void ScoreboardController::setAwayShots(int shots) {
    awayShots = shots;
}

void ScoreboardController::setPenalty1Minutes(int minutes) {
    penalty1Minutes = minutes;
}

void ScoreboardController::setPenalty2Minutes(int minutes) {
    penalty2Minutes = minutes;
}

void ScoreboardController::setCurrentPeriod(int period) {
    currentPeriod = period;
}

void ScoreboardController::setHomeTeamName(const std::string& name) {
    homeTeamName = name;
}

void ScoreboardController::setAwayTeamName(const std::string& name) {
    awayTeamName = name;
}
