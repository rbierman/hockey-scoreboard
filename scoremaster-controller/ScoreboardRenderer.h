#pragma once

#include <blend2d.h>
#include "display/DoubleFramebuffer.h"
#include "ResourceLocator.h"
#include "ScoreboardState.h"
#include "IRenderer.h"

class ScoreboardRenderer : public IRenderer {
public:
    explicit ScoreboardRenderer(DoubleFramebuffer& dfb, const ResourceLocator& resourceLocator, const ScoreboardState& state);

    void render() const override;

private:
    DoubleFramebuffer& dfb;
    const ResourceLocator& _resourceLocator;
    const ScoreboardState& state;

    BLFontFace fontFace;
    BLFont font;
    BLFont shotsFont;
    BLFont periodFont;
    BLFont penaltyFont;
    BLFont labelFont;
    BLFont teamNameFont;
    BLFont goalFont;

    BLRgba32 colorWhite{255, 255, 255};
    BLRgba32 colorOrange{255, 170, 51};
    BLRgba32 colorRed{255, 0, 0};

    void loadFont(const char* path);
    void renderGoalCelebration(BLContext& ctx, const ScoreboardState& state) const;
};
