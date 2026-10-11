#include <juce_core/juce_core.h>

#include "GUI/Helpers/ComboBoxPopupAutoWidth.h"

class ComboBoxPopupAutoWidthTests : public juce::UnitTest
{
public:
    ComboBoxPopupAutoWidthTests() : juce::UnitTest("ComboBoxPopupAutoWidth") {}

    void runTest() override
    {
        testHardCeiling();
        testShortNamesKeepClosedWidth();
        testGrowRightWhenRoomOnRight();
        testGrowLeftNearRightEdge();
        testClampToHardCeiling();
        testClampToFreeSideSpace();
        testFloorWhenHardCeilingBelowClosed();
        testFloorWhenSideCapBelowClosed();
        testResolveIsPureAndDeterministic();
    }

private:
    void testHardCeiling()
    {
        beginTest("hardCeiling - min of host fraction and design max");

        using namespace TSS::ComboBoxPopupAutoWidth;

        expectWithinAbsoluteError(hardCeiling(1000.0f, 1.0f), 480.0f, 0.01f);
        expectWithinAbsoluteError(hardCeiling(400.0f, 1.0f), 360.0f, 0.01f);
        expectWithinAbsoluteError(hardCeiling(1000.0f, 2.0f), 900.0f, 0.01f);
        expectWithinAbsoluteError(hardCeiling(0.0f, 1.0f), kDesignMaxWidth, 0.01f);
    }

    void testShortNamesKeepClosedWidth()
    {
        beginTest("resolve - short labels stay at closed width");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 40.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 500.0f,
            .freeLeft = 200.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 140.0f, 0.01f);
        expect(result.growRight);
    }

    void testGrowRightWhenRoomOnRight()
    {
        beginTest("resolve - long label grows right when freeRight fits");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 300.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 400.0f,
            .freeLeft = 80.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 311.0f, 0.01f);
        expect(result.growRight);
    }

    void testGrowLeftNearRightEdge()
    {
        beginTest("resolve - prefers left grow when right side is tighter");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 300.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 160.0f,
            .freeLeft = 400.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 311.0f, 0.01f);
        expect(! result.growRight);
    }

    void testClampToHardCeiling()
    {
        beginTest("resolve - clamps to hard ceiling");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 900.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 800.0f,
            .freeLeft = 100.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 480.0f, 0.01f);
        expect(result.growRight);
    }

    void testClampToFreeSideSpace()
    {
        beginTest("resolve - clamps to free space on chosen side");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 500.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 200.0f,
            .freeLeft = 180.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 200.0f, 0.01f);
        expect(result.growRight);
    }

    void testFloorWhenHardCeilingBelowClosed()
    {
        beginTest("resolve - floors at closed when hardCeiling is tighter");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 40.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 100.0f,
            .freeRight = 500.0f,
            .freeLeft = 200.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 140.0f, 0.01f);
        expect(result.growRight);
    }

    void testFloorWhenSideCapBelowClosed()
    {
        beginTest("resolve - floors at closed when sideCap is tighter");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const auto result = resolve({
            .closedWidth = 140.0f,
            .longestLabelWidth = 40.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 100.0f,
            .freeLeft = 80.0f,
        });

        expectWithinAbsoluteError(result.contentWidth, 140.0f, 0.01f);
        expect(result.growRight);
    }

    void testResolveIsPureAndDeterministic()
    {
        beginTest("resolve - pure and deterministic for the same inputs");

        using namespace TSS::ComboBoxPopupAutoWidth;

        const ResolveArgs args{
            .closedWidth = 140.0f,
            .longestLabelWidth = 220.0f,
            .sidePadding = 11.0f,
            .hardCeiling = 480.0f,
            .freeRight = 400.0f,
            .freeLeft = 100.0f,
        };

        const auto first = resolve(args);
        const auto second = resolve(args);
        auto updated = args;
        updated.longestLabelWidth = 50.0f;
        const auto afterInputChange = resolve(updated);

        expectWithinAbsoluteError(first.contentWidth, second.contentWidth, 0.01f);
        expect(first.growRight == second.growRight);
        expectWithinAbsoluteError(first.contentWidth, 231.0f, 0.01f);
        expectWithinAbsoluteError(afterInputChange.contentWidth, 140.0f, 0.01f);
        expect(afterInputChange.growRight);
    }
};

static ComboBoxPopupAutoWidthTests comboBoxPopupAutoWidthTests;
