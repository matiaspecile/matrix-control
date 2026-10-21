#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Generic UI kind addressed by the skin colour map (no patch or master object types). */
    enum class SkinObjectType
    {
        kHeaderPanel,
        kBodyPanel,
        kFooterPanel,
        kDarkPanel,
        kFooterMessage,
        kSectionHeader,
        kModuleHeader,
        kGroupLabel,
        kLabel,
        kHorizontalSeparator,
        kButton,
        kToggle,
        kSlider,
        kSliderButtonLike,
        kComboBox,
        kComboBoxButtonLike,
        kPopupMenu,
        kPopupMenuButtonLike,
        kNumberBox,
        kEnvelopeDisplay,
        kTrackGeneratorDisplay,
        kPatchNameDisplay,
        kDialog,
        kWindowClose,
        kReadOnlyValueField
    };

    /** What is painted for an object (intent WHAT, not HOW). */
    enum class SkinColourProperty
    {
        kBackground,
        kBorder,
        kText,
        kSecondaryText,
        kLine,
        kLineBlue,
        kLineOrange,
        kTrack,
        kValueBar,
        kFocusBorder,
        kTriangle,
        kSeparator,
        kScrollbar,
        kEnvelope,
        kShaper,
        kEditorBackground,
        kEditorSelectionBackground,
        kEditorText,
        kTitleBand,
        kGlyph
    };

    /** Named visual variant for a (object, property) pair. */
    enum class SkinColourState
    {
        kDefault,
        kOff,
        kOn,
        kHover,
        kClicked,
        kDisabled,
        kFocus,
        kInfo,
        kWarning,
        kError
    };

    /** SSOT mapping address: (ObjectType, Property, State) → palette entry. */
    struct SkinColourKey
    {
        SkinObjectType object = SkinObjectType::kButton;
        SkinColourProperty property = SkinColourProperty::kBackground;
        SkinColourState state = SkinColourState::kDefault;

        constexpr bool operator==(const SkinColourKey& other) const noexcept
        {
            return object == other.object
                && property == other.property
                && state == other.state;
        }

        constexpr bool operator<(const SkinColourKey& other) const noexcept
        {
            if (object != other.object)
                return object < other.object;
            if (property != other.property)
                return property < other.property;
            return state < other.state;
        }
    };

    constexpr SkinColourKey skinColourKey(SkinObjectType object,
                                          SkinColourProperty property,
                                          SkinColourState state = SkinColourState::kDefault) noexcept
    {
        return SkinColourKey { object, property, state };
    }
}
