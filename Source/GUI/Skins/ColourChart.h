#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct ColourChart
{
    static constexpr juce::uint32 kTransparent = 0x00000000;
    static constexpr juce::uint32 kBlack       = 0xFF000000;
    static constexpr juce::uint32 kWhite       = 0xFFFFFFFF;
    static constexpr juce::uint32 kCream       = 0xFFE9E8D5;

    static constexpr juce::uint32 kGrey950     = 0xFF151515;
    static constexpr juce::uint32 kGrey900     = 0xFF202020;
    static constexpr juce::uint32 kGrey800     = 0xFF303030;
    static constexpr juce::uint32 kGrey700     = 0xFF404040;
    static constexpr juce::uint32 kGrey600     = 0xFF505050;
    static constexpr juce::uint32 kGrey525     = 0xFF5E5E5E;
    static constexpr juce::uint32 kGrey500     = 0xFF606060;
    static constexpr juce::uint32 kGrey400     = 0xFF707070;
    /** Midway between former HELP #B0B0B0 and sticky INFO #606060 — furtive HELP chrome. */
    static constexpr juce::uint32 kGrey350     = 0xFF888888;
    static constexpr juce::uint32 kGrey300     = 0xFF999999;
    static constexpr juce::uint32 kGrey200     = 0xFFB9B9B9;
    static constexpr juce::uint32 kGrey100     = 0xFFDDDDDD;

    static constexpr juce::uint32 kRed900      = 0xFF9A131D;
    /** Solid match for primary red at ~50% over black (PatchNameDisplay secondary). */
    static constexpr juce::uint32 kRed800      = 0xFF800000;
    static constexpr juce::uint32 kRed500      = 0xFFFF0000;
    static constexpr juce::uint32 kOrange      = 0xFFF87A00;
    static constexpr juce::uint32 kYellow      = 0xFFFFFF00;
    static constexpr juce::uint32 kBlue        = 0xFF004FB0;

    static constexpr juce::uint32 kEmerald950  = 0xFF002D0E;
    static constexpr juce::uint32 kEmerald900  = 0xFF06471C;
    static constexpr juce::uint32 kEmerald700  = 0xFF00785E;
    static constexpr juce::uint32 kEmerald400  = 0xFF00DDAD;
};
