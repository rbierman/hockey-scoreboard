#pragma once

#include "IRenderer.h"
#include "display/DoubleFramebuffer.h"
#include "ResourceLocator.h"
#include "ScoreboardController.h"
#include <blend2d.h>

class GoalCelebrationRenderer : public IRenderer {
public:
    explicit GoalCelebrationRenderer(DoubleFramebuffer& dfb, const ResourceLocator& resourceLocator, const ScoreboardController& controller);

    void render() const override;

private:
    DoubleFramebuffer& dfb;
    const ResourceLocator& _resourceLocator;
    const ScoreboardController& controller;

    BLFontFace fontFace;
    BLFont titleFont;
    BLFont playerFont;
    
    BLRgba32 colorWhite{255, 255, 255};
    BLRgba32 colorOrange{255, 170, 51};
    BLRgba32 colorRed{255, 0, 0};
};
