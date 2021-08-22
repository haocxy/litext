#pragma once

#include "core/font_index.h"

#include "layout_config.h"


class RenderConfig {
public:
    RenderConfig() {}

    const HLayoutConfig &hLayout() const {
        return hLayout_;
    }

    void setHLayout(const HLayoutConfig &hLayout) {
        hLayout_ = hLayout;
    }

    const font::FontIndex &font() const {
        return font_;
    }

    void setFont(const font::FontIndex &font) {
        font_ = font;
    }

    int widthLimit() const {
        return widthLimit_;
    }

    void setWidthLimit(int limit) {
        widthLimit_ = limit;
    }

    int heightLimit() const {
        return heightLimit_;
    }

    void setHeightLimit(int limit) {
        heightLimit_ = limit;
    }

private:
    HLayoutConfig hLayout_;
    font::FontIndex font_;
    int widthLimit_ = 0;
    int heightLimit_ = 0;
};

