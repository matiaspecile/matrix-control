#pragma once

#include <juce_core/juce_core.h>

namespace TSS::ComboBoxPopupAutoWidth
{
    /** Fraction of host/display width used as the hard ceiling (with design max). */
    inline constexpr float kHostWidthFraction = 0.9f;

    /** Design-space max content width before UI scale (matches Settings long-name ceiling). */
    inline constexpr float kDesignMaxWidth = 480.0f;

    /** Extra horizontal air beyond textLeftPadding (HierarchicalPopupMenu precedent). */
    inline constexpr float kLabelWidthPadding = 8.0f;

    struct ResolveArgs
    {
        float closedWidth = 0.0f;
        float longestLabelWidth = 0.0f;
        float sidePadding = 0.0f;
        float hardCeiling = 0.0f;
        float freeRight = 0.0f;
        float freeLeft = 0.0f;
    };

    struct ResolveResult
    {
        float contentWidth = 0.0f;
        bool growRight = true;
    };

    [[nodiscard]] inline float hardCeiling(float hostWidth, float uiScale) noexcept
    {
        const float scaledDesignMax = kDesignMaxWidth * juce::jmax(0.0f, uiScale);
        if (hostWidth <= 0.0f)
            return scaledDesignMax;

        return juce::jmin(hostWidth * kHostWidthFraction, scaledDesignMax);
    }

    /** Desired content width and grow side: prefer right (left edges aligned), else left. */
    [[nodiscard]] inline ResolveResult resolve(const ResolveArgs& args) noexcept
    {
        const float closed = juce::jmax(1.0f, args.closedWidth);
        const float desired = juce::jmax(closed, args.longestLabelWidth + args.sidePadding);
        const float freeRight = juce::jmax(0.0f, args.freeRight);
        const float freeLeft = juce::jmax(0.0f, args.freeLeft);
        const bool growRight = desired <= freeRight || freeRight >= freeLeft;
        const float sideCap = growRight ? freeRight : freeLeft;
        const float hardCap = juce::jmax(0.0f, args.hardCeiling);
        const float contentWidth = juce::jmax(closed, juce::jmin(desired, hardCap, sideCap));
        return { contentWidth, growRight };
    }
}
