#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Panels
    {
        inline constexpr ColourElement kHeaderPanelBackground = {
            skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey950,
            ColourChart::kGrey950
        };

        inline constexpr ColourElement kBodyPanelBackground = {
            skinColourKey(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey900,
            ColourChart::kCream
        };

        inline constexpr ColourElement kFooterPanelBackground = {
            skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey950,
            ColourChart::kGrey950
        };

        // Unified chrome grey for Header cartouche badge/frame and Footer HELP chrome.
        inline constexpr juce::uint32 kMainChromeGrey = ColourChart::kGrey300;

        inline constexpr ColourElement kHeaderPanelBorder = {
            skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBorder),
            kMainChromeGrey,
            kMainChromeGrey
        };

        inline constexpr ColourElement kFooterPanelBorder = {
            skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBorder),
            kMainChromeGrey,
            kMainChromeGrey
        };

        // MIDI-queue alert badge cut-out text (fill uses FooterMessage Error text).
        inline constexpr ColourElement kFooterMessageErrorBackground = {
            skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kBackground, SkinColourState::kError),
            ColourChart::kBlack,
            ColourChart::kBlack
        };

        // Modal chrome + durable close-cross glyph (wired by dialogs / Settings / About).
        inline constexpr ColourElement kDialogBorder = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kBorder),
            ColourChart::kGrey525,
            ColourChart::kGrey525
        };

        inline constexpr ColourElement kDialogTitleBand = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kTitleBand),
            ColourChart::kBlack,
            ColourChart::kBlack
        };

        inline constexpr ColourElement kWindowCloseGlyph = {
            skinColourKey(SkinObjectType::kWindowClose, SkinColourProperty::kGlyph),
            ColourChart::kRed900,
            ColourChart::kRed900
        };
    }
}
