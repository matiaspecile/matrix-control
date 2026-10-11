#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ComboBox;

    enum class PopupVerticalPlacement
    {
        Auto,
        Above,
        Below
    };

    struct PopupMenuDimensions
    {
        int width = 0;
        int height = 0;
        int x = 0;
        int y = 0;
        bool opensAbove = false;
    };

    class PopupMenuPositioner
    {
    public:
        struct DimensionsArgs
        {
            int popupWidth = 0;
            int popupHeight = 0;
            int verticalMargin = 0;
            PopupVerticalPlacement placement = PopupVerticalPlacement::Auto;
            /** When true, align popup right edge with anchor right (grow-left auto-width). */
            bool alignRightEdges = false;
            /** When true, keep left edges aligned (no overflow flip to right-align). */
            bool lockLeftEdge = false;
        };

        static PopupMenuDimensions calculateDimensions(
            const ComboBox& comboBox,
            int popupWidth,
            int popupHeight,
            int borderThickness);

        static PopupMenuDimensions calculateDimensions(
            const juce::Component& anchor,
            const DimensionsArgs& args);

    private:
        struct ResolveYArgs
        {
            int desiredYBelow = 0;
            int popupHeight = 0;
            juce::Rectangle<int> screenBounds;
            int verticalMargin = 0;
            PopupVerticalPlacement placement = PopupVerticalPlacement::Auto;
        };

        static juce::Point<int> positionBelow(
            const juce::Component& anchor,
            int verticalMargin);

        static juce::Point<int> positionAbove(
            const juce::Component& anchor,
            int popupHeight,
            int verticalMargin);

        static int adjustXPosition(
            const juce::Component& anchor,
            int desiredX,
            int popupWidth,
            const juce::Rectangle<int>& screenBounds);

        struct ResolveXArgs
        {
            int desiredX = 0;
            int popupWidth = 0;
            juce::Rectangle<int> screenBounds;
            bool alignRightEdges = false;
            bool lockLeftEdge = false;
        };

        static int resolveXPosition(
            const juce::Component& anchor,
            const ResolveXArgs& args);

        static int resolveYPosition(
            const juce::Component& anchor,
            const ResolveYArgs& args,
            bool& opensAbove);
    };
}
