#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::SkinObjectType;
using TSS::SkinColourProperty;
using TSS::SkinColourState;

class SliderButtonLikeColourTests : public juce::UnitTest
{
public:
    SliderButtonLikeColourTests()
        : juce::UnitTest("SliderButtonLikeColour")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("buttonLikeEnabled_matchesApprovedGreys");

        expect(SliderButtonLike::kTrack.blackVariant == ColourChart::kBlack);
        expect(SliderButtonLike::kTrack.creamVariant == ColourChart::kBlack);
        expect(SliderButtonLike::kFocusBorder.blackVariant == ColourChart::kGrey800);
        expect(SliderButtonLike::kFocusBorder.creamVariant == ColourChart::kGrey800);
        expect(SliderButtonLike::kValueBar.blackVariant == ColourChart::kGrey600);
        expect(SliderButtonLike::kValueBar.creamVariant == ColourChart::kGrey600);
        expect(SliderButtonLike::kText.blackVariant == ColourChart::kGrey200);
        expect(SliderButtonLike::kText.creamVariant == ColourChart::kGrey200);

        beginTest("standardSliderEnabled_stillGreen");

        expect(Slider::kTrack.blackVariant == ColourChart::kEmerald950);
        expect(Slider::kFocusBorder.blackVariant == ColourChart::kEmerald900);
        expect(Slider::kValueBar.blackVariant == ColourChart::kEmerald700);
        expect(Slider::kText.blackVariant == ColourChart::kEmerald400);

        beginTest("skinLoaded_standardSliderEnabledMatchesEmeraldStops");

        assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant::Black);
        assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("sharedDisabledTokens_unchanged");

        expect(Slider::kValueBarDisabled.blackVariant == ColourChart::kGrey700);
        expect(Slider::kValueBarDisabled.creamVariant == ColourChart::kGrey700);

        beginTest("buttonLikeLookFromSkin_mapsApprovedGreys");

        assertButtonLikeLook(TSS::Skin::ColourVariant::Black);
        assertButtonLikeLook(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        expect(skin->getSliderTrackColour(true) == juce::Colour(ColourChart::kEmerald950));
        expect(skin->getSliderValueBarColour(true) == juce::Colour(ColourChart::kEmerald700));
        expect(skin->getSliderTextColour(true) == juce::Colour(ColourChart::kEmerald400));
    }

    void assertButtonLikeLook(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::sliderLookButtonLikeFromSkin(*skin);

        expect(look.trackEnabled == juce::Colour(ColourChart::kBlack));
        expect(look.focusBorder == juce::Colour(ColourChart::kGrey800));
        expect(look.valueBarEnabled == juce::Colour(ColourChart::kGrey600));
        expect(look.textEnabled == juce::Colour(ColourChart::kGrey200));

        // Disabled roles reuse the shared slider tokens; editor stays high-contrast white.
        expect(look.trackDisabled == skin->getSliderTrackColour(false));
        expect(look.backgroundDisabled == skin->getColour(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled));
        expect(look.valueBarDisabled == skin->getSliderValueBarColour(false));
        expect(look.textDisabled == skin->getSliderTextColour(false));
        expect(look.editorText == skin->getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText));
        expect(look.editorCaret == skin->getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText));
    }
};

static SliderButtonLikeColourTests sliderButtonLikeColourTests;
