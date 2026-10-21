#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"

using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;

class SkinColourSubstrateTests : public juce::UnitTest
{
public:
    SkinColourSubstrateTests()
        : juce::UnitTest("SkinColourSubstrate")
    {
    }

    void runTest() override
    {
        beginTest("lookBuilders_resolveStructuredKeysForBothSkins");

        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::Black);
        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("skinMaps_blackAndCreamColumnsDiverge");

        assertBodyBackgroundColumns();
        assertCreamDivergentLabelText();
        assertDarkPanelTextInvariant();

        beginTest("patchNameDisplay_secondaryTextIsSolidMappedColour");

        assertSecondarySolid(TSS::Skin::ColourVariant::Black);
        assertSecondarySolid(TSS::Skin::ColourVariant::Cream);

        beginTest("gapKeys_existForBlackAndCream");

        assertGapKeys(TSS::Skin::ColourVariant::Black);
        assertGapKeys(TSS::Skin::ColourVariant::Cream);

        beginTest("footerMessage_severityKeysMatchChartForBothSkins");

        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::Black);
        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::buttonLookFromSkin(*skin);
        const auto mapped = skin->getColour(SkinObjectType::kButton,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOff);
        expect(look.backgroundOff == mapped);
        expect(look.backgroundOff == juce::Colour(ColourChart::kBlack));
    }

    void assertBodyBackgroundColumns()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expect(black->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey900));
        expect(cream->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream));
    }

    void assertCreamDivergentLabelText()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kGrey950));
        expect(blackText != creamText);
    }

    void assertDarkPanelTextInvariant()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kGrey200));
        expect(creamText != cream->getColour(SkinObjectType::kLabel, SkinColourProperty::kText));
    }

    void assertSecondarySolid(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::patchNameDisplayLookFromSkin(*skin);
        const auto mapped = skin->getColour(SkinObjectType::kPatchNameDisplay,
                                            SkinColourProperty::kSecondaryText);
        expect(look.secondaryText == mapped);
        expect(look.secondaryText == juce::Colour(ColourChart::kRed800));
        expect(look.secondaryText.getFloatAlpha() == 1.0f);
    }

    void assertGapKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);

        expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kBorder)
               == juce::Colour(ColourChart::kGrey525));
        expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kTitleBand)
               == juce::Colour(ColourChart::kBlack));
        expect(skin->getColour(SkinObjectType::kWindowClose, SkinColourProperty::kGlyph)
               == juce::Colour(ColourChart::kRed900));
        expect(skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey700));
        expect(skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText)
               == juce::Colour(ColourChart::kGrey200));
    }

    void assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kInfo)
               == juce::Colour(ColourChart::kGrey500));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kWarning)
               == juce::Colour(ColourChart::kOrange));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kRed500));
    }
};

static SkinColourSubstrateTests skinColourSubstrateTests;
