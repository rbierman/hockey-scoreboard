#include "GoalCelebrationRenderer.h"
#include <iostream>
#include <chrono>

GoalCelebrationRenderer::GoalCelebrationRenderer(DoubleFramebuffer& dfb, const ResourceLocator& resourceLocator, const ScoreboardController& controller)
    : dfb(dfb), _resourceLocator(resourceLocator), controller(controller) {
    
    BLResult err = fontFace.createFromFile((_resourceLocator.getFontsDirPath() + "/digital-7 (mono).ttf").c_str());
    if (err) {
        std::cerr << "Failed to load font for GoalCelebrationRenderer" << std::endl;
    }
    titleFont.createFromFace(fontFace, 60.0f);
    playerFont.createFromFace(fontFace, 30.0f);
}

void GoalCelebrationRenderer::render() const {
    const int w = dfb.getWidth();
    const int h = dfb.getHeight();
    const ScoreboardState& state = controller.getState();

    BLImage image;
    image.createFromData(w, h, BL_FORMAT_PRGB32, dfb.getBackData(), w * 4, BL_DATA_ACCESS_RW);
    BLContext ctx(image);

    ctx.clearAll();

    // 1. Render Player Image (Full Height: 160px)
    const auto& imageData = controller.getGoalPlayerImageData();
    if (!imageData.empty()) {
        BLImage playerImg;
        if (playerImg.readFromData(imageData.data(), imageData.size()) == BL_SUCCESS) {
            double targetH = (double)h; // h is 160
            double scale = targetH / playerImg.height();
            double targetW = playerImg.width() * scale;
            
            double imgX = (w - targetW) / 2.0;
            double imgY = 0.0;
            
            ctx.blitImage(BLRect(imgX, imgY, targetW, targetH), playerImg);
        }
    }

    // 2. Render "GOAL!" text (Blinking at top left)
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    bool showGoal = (ms / 500) % 2 == 0; // 500ms blink rate

    if (showGoal) {
        ctx.setFillStyle(colorRed);
        std::string goalText = "GOAL!";
        ctx.fillUtf8Text(BLPoint(10.0, 50.0), titleFont, goalText.c_str());
    }

    // 3. Render Player Name and Number (Bottom right)
    if (state.goalEvent.playerNumber > 0 || !state.goalEvent.playerName.empty()) {
        std::string playerNum = "#" + std::to_string(state.goalEvent.playerNumber);
        std::string playerName = state.goalEvent.playerName;
        
        BLGlyphBuffer gb;
        BLTextMetrics tmNum, tmName;
        
        gb.setUtf8Text(playerNum.c_str(), playerNum.length());
        playerFont.getTextMetrics(gb, tmNum);
        
        gb.clear();
        gb.setUtf8Text(playerName.c_str(), playerName.length());
        playerFont.getTextMetrics(gb, tmName);

        double padding = 10.0;
        double currentX = w - padding;

        // Draw Number in Orange (rightmost)
        currentX -= tmNum.advance.x;
        ctx.setFillStyle(colorOrange);
        ctx.fillUtf8Text(BLPoint(currentX, h - 10.0), playerFont, playerNum.c_str());

        // Draw Name in White (left of number)
        if (!playerName.empty()) {
            currentX -= (tmName.advance.x + 10.0); // 10px spacing
            ctx.setFillStyle(colorWhite);
            ctx.fillUtf8Text(BLPoint(currentX, h - 10.0), playerFont, playerName.c_str());
        }
    }

    ctx.end();
}
