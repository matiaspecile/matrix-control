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

        // Gap keys for stories 2/4 call-site migration (maps ready; paints stay local for now).
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
