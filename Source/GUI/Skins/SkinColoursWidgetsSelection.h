#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace ComboBox
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder),
                ColourChart::kEmerald900,
                ColourChart::kEmerald900
            };

            // Standard combo border reuses ButtonLike border pastilles (same hex as before).
            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        namespace ComboBoxButtonLike
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kTriangle),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBoxButtonLike, SkinColourProperty::kTriangle, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        namespace PopupMenu
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            // Solid muted emerald replacing former text.withAlpha(0.5f) on disabled items.
            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover),
                Common::kContentHover,
                Common::kContentHover
            };

            // Standard scrollbar reuses ButtonLike scrollbar pastille (same hex as before).
            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };
        }

        namespace PopupMenuButtonLike
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kBorder),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kSeparator),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            // Solid muted grey replacing former text.withAlpha(0.5f) on disabled / section headers.
            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenuButtonLike, SkinColourProperty::kScrollbar),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };
        }
    }
}
