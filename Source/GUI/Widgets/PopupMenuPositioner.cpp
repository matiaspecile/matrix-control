#include "PopupMenuPositioner.h"
#include "ComboBox.h"

namespace TSS
{
    PopupMenuDimensions PopupMenuPositioner::calculateDimensions(
        const ComboBox& comboBox,
        int popupWidth,
        int popupHeight,
        int /* borderThickness */)
    {
        return calculateDimensions(static_cast<const juce::Component&>(comboBox),
                                   DimensionsArgs{
                                       .popupWidth = popupWidth,
                                       .popupHeight = popupHeight,
                                       .verticalMargin = comboBox.getScaledVerticalMargin(),
                                       .placement = comboBox.getPopupVerticalPlacement()});
    }

    PopupMenuDimensions PopupMenuPositioner::calculateDimensions(
        const juce::Component& anchor,
        const DimensionsArgs& args)
    {
        auto* topLevelComponent = anchor.getTopLevelComponent();
        if (topLevelComponent == nullptr)
            return { args.popupWidth, args.popupHeight, 0, 0, false };

        // Parent popups with addAndMakeVisible on the editor peer. Use top-level local
        // coordinates (getLocalPoint) — not screen space — so embedded Linux hosts with
        // unreliable global positions (e.g. Bitwig VST3) still anchor under the combo.
        const auto belowPosition = positionBelow(*topLevelComponent, anchor, args.verticalMargin);
        bool opensAbove = false;
        const int resolvedY = resolveYPosition(*topLevelComponent,
                                               anchor,
                                               ResolveYArgs{
                                                   .desiredYBelow = belowPosition.getY(),
                                                   .popupHeight = args.popupHeight,
                                                   .verticalMargin = args.verticalMargin,
                                                   .placement = args.placement},
                                               opensAbove);
        const int resolvedX = resolveXPosition(*topLevelComponent,
                                               anchor,
                                               ResolveXArgs{
                                                   .desiredX = belowPosition.getX(),
                                                   .popupWidth = args.popupWidth,
                                                   .alignRightEdges = args.alignRightEdges,
                                                   .lockLeftEdge = args.lockLeftEdge});

        return { args.popupWidth, args.popupHeight, resolvedX, resolvedY, opensAbove };
    }

    juce::Point<int> PopupMenuPositioner::positionBelow(
        const juce::Component& topLevel,
        const juce::Component& anchor,
        int verticalMargin)
    {
        return topLevel.getLocalPoint(&anchor,
                                      juce::Point<int>(0, anchor.getHeight() + verticalMargin));
    }

    juce::Point<int> PopupMenuPositioner::positionAbove(
        const juce::Component& topLevel,
        const juce::Component& anchor,
        int popupHeight,
        int verticalMargin)
    {
        return topLevel.getLocalPoint(&anchor,
                                      juce::Point<int>(0, -popupHeight - verticalMargin));
    }

    int PopupMenuPositioner::adjustXPosition(
        const juce::Component& topLevel,
        const juce::Component& anchor,
        int desiredX,
        int popupWidth)
    {
        const auto hostBounds = topLevel.getLocalBounds();
        if (desiredX + popupWidth > hostBounds.getRight())
        {
            const auto anchorRight = topLevel.getLocalPoint(
                                         &anchor,
                                         juce::Point<int>(anchor.getWidth(), 0))
                                         .getX();
            return anchorRight - popupWidth;
        }

        return desiredX;
    }

    int PopupMenuPositioner::resolveXPosition(
        const juce::Component& topLevel,
        const juce::Component& anchor,
        const ResolveXArgs& args)
    {
        const auto hostBounds = topLevel.getLocalBounds();

        if (args.alignRightEdges)
        {
            const auto anchorRight = topLevel.getLocalPoint(
                                         &anchor,
                                         juce::Point<int>(anchor.getWidth(), 0))
                                         .getX();
            const int rightAlignedX = anchorRight - args.popupWidth;
            const int minX = hostBounds.getX();
            const int maxX = hostBounds.getRight() - args.popupWidth;
            if (maxX < minX)
                return minX;

            return juce::jlimit(minX, maxX, rightAlignedX);
        }

        if (args.lockLeftEdge)
            return args.desiredX;

        return adjustXPosition(topLevel, anchor, args.desiredX, args.popupWidth);
    }

    int PopupMenuPositioner::resolveYPosition(
        const juce::Component& topLevel,
        const juce::Component& anchor,
        const ResolveYArgs& args,
        bool& opensAbove)
    {
        const auto hostBounds = topLevel.getLocalBounds();

        switch (args.placement)
        {
            case PopupVerticalPlacement::Above:
                opensAbove = true;
                return positionAbove(topLevel, anchor, args.popupHeight, args.verticalMargin).getY();

            case PopupVerticalPlacement::Below:
                opensAbove = false;
                return args.desiredYBelow;

            case PopupVerticalPlacement::Auto:
            default:
                if (args.desiredYBelow + args.popupHeight > hostBounds.getBottom())
                {
                    opensAbove = true;
                    return positionAbove(topLevel, anchor, args.popupHeight, args.verticalMargin).getY();
                }

                opensAbove = false;
                return args.desiredYBelow;
        }
    }
}
