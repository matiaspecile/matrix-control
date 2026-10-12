#include <juce_gui_basics/juce_gui_basics.h>

#include "Standalone/StandaloneQuitCommands.h"
#include "Standalone/StandaloneWindowPlacement.h"

namespace
{
class QuitCommandTarget final : public juce::ApplicationCommandTarget
{
public:
    int quitPerformCount = 0;

    ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }

    void getAllCommands (juce::Array<juce::CommandID>& commands) override
    {
        commands.add (juce::StandardApplicationCommandIDs::quit);
    }

    void getCommandInfo (juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
    {
        if (commandID == juce::StandardApplicationCommandIDs::quit)
        {
            result.setInfo ("Quit", "Quits the application", "Application", 0);
            result.defaultKeypresses.add (juce::KeyPress ('q', juce::ModifierKeys::commandModifier, 0));
        }
    }

    bool perform (const InvocationInfo& info) override
    {
        if (info.commandID != juce::StandardApplicationCommandIDs::quit)
            return false;

        ++quitPerformCount;
        return true;
    }
};
} // namespace

class StandaloneWindowPlacementTests : public juce::UnitTest
{
public:
    StandaloneWindowPlacementTests() : juce::UnitTest ("StandaloneWindowPlacement") {}

    void runTest() override
    {
        testValidSecondaryMonitorUnchanged();
        testBadSavedYClampedIntoUserArea();
        testFarOffscreenClampedOntoPreferredDisplay();
        testEmptyUserAreasLeaveBounds();
        testUnknownFrameTopLeavesBounds();
        testCentreOnSingleDisplayPreservesSize();
        testCentreOnSecondaryPreferredLimits();
        testCentreEmptyPreferredLimitsLeavesBounds();
        testCentreUnknownFrameTopLeavesBounds();
        testCentreThenEnsureSecondaryNotPrimary();
        testCentreThenEnsureOversizedTitleBarIntersects();
        testRecentreClientBoundsOnNearestDisplayUserArea();
        testWindowsAltF4QuitKey();
        testStandardApplicationQuitKeypress();
        testBindStandaloneQuitCommandsWiresStandardKeypress();
        testFindNearestDisplayForBounds();
    }

private:
    void testValidSecondaryMonitorUnchanged()
    {
        beginTest ("valid multi-monitor position stays put");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> secondary { 1920, 0, 1920, 1080 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add ({ 0, 0, 1920, 1080 });
        areas.add (secondary);

        const juce::Rectangle<int> client { 2200, 120, 1200, 800 };
        const auto next = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            client, frame, areas, secondary);

        expect (next == client);
    }

    void testBadSavedYClampedIntoUserArea()
    {
        beginTest ("bad saved Y clamps so title bar meets user area");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add (primary);

        // Client Y so frame top (title bar) sits above the usable area.
        const juce::Rectangle<int> client { 100, -40, 1200, 800 };
        const auto next = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            client, frame, areas, primary);

        const auto strip = MatrixStandalone::titleBarDragStrip (next, frame);
        expect (MatrixStandalone::titleBarStripIntersectsAnyUserArea (strip, areas));
        expect (next.getY() != client.getY());
    }

    void testFarOffscreenClampedOntoPreferredDisplay()
    {
        beginTest ("far off-screen window clamps onto preferred display user area");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add (primary);

        const juce::Rectangle<int> client { -4000, -3000, 1200, 800 };
        const auto next = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            client, frame, areas, primary);

        const auto strip = MatrixStandalone::titleBarDragStrip (next, frame);
        expect (MatrixStandalone::titleBarStripIntersectsAnyUserArea (strip, areas));
        expect (primary.contains (strip.getTopLeft()) || strip.intersects (primary));
    }

    void testEmptyUserAreasLeaveBounds()
    {
        beginTest ("empty display list leaves bounds unchanged");

        const juce::Rectangle<int> client { 10, 20, 100, 80 };
        const auto next = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            client, {}, {}, { 0, 0, 1920, 1080 });

        expect (next == client);
    }

    void testUnknownFrameTopLeavesBounds()
    {
        beginTest ("unknown native frame top does not false-accept or clamp");

        const juce::BorderSize<int> unknownFrame {};
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add (primary);

        const juce::Rectangle<int> client { 100, -40, 1200, 800 };
        const auto next = MatrixStandalone::ensureClientBoundsTitleBarOnScreen (
            client, unknownFrame, areas, primary);

        expect (next == client);
    }

    void testCentreOnSingleDisplayPreservesSize()
    {
        beginTest ("centre helper moves off-centre window onto display centre");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        const juce::Rectangle<int> client { 40, 60, 1200, 800 };

        const auto next = MatrixStandalone::centreClientBoundsOnDisplayUserArea (client, frame, primary);

        expect (next.getWidth() == client.getWidth());
        expect (next.getHeight() == client.getHeight());
        expect (next != client);

        const auto full = frame.addedTo (next);
        expect (full.getCentre() == primary.getCentre());
    }

    void testCentreOnSecondaryPreferredLimits()
    {
        beginTest ("centre helper uses preferred display, not primary");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> secondary { 1920, 0, 1920, 1080 };
        const juce::Rectangle<int> client { 100, 80, 1200, 800 };

        const auto next = MatrixStandalone::centreClientBoundsOnDisplayUserArea (client, frame, secondary);
        const auto full = frame.addedTo (next);

        expect (full.getCentre() == secondary.getCentre());
        expect (next.getWidth() == client.getWidth());
        expect (next.getHeight() == client.getHeight());
    }

    void testCentreEmptyPreferredLimitsLeavesBounds()
    {
        beginTest ("centre helper no-ops on empty preferredLimits");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> client { 40, 60, 1200, 800 };

        expect (MatrixStandalone::centreClientBoundsOnDisplayUserArea (client, frame, {}) == client);
    }

    void testCentreUnknownFrameTopLeavesBounds()
    {
        beginTest ("centre helper no-ops when native frame top is unknown");

        const juce::BorderSize<int> unknownFrame {};
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        const juce::Rectangle<int> client { 40, 60, 1200, 800 };

        expect (MatrixStandalone::centreClientBoundsOnDisplayUserArea (client, unknownFrame, primary)
                == client);
    }

    void testCentreThenEnsureSecondaryNotPrimary()
    {
        beginTest ("centreThenEnsure uses nearest secondary, not primary");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const juce::Rectangle<int> primary { 0, 0, 1920, 1080 };
        const juce::Rectangle<int> secondary { 1920, 0, 1920, 1080 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add (primary);
        areas.add (secondary);

        const juce::Rectangle<int> clientOnSecondary { 2200, 120, 1200, 800 };
        expect (MatrixStandalone::indexOfNearestUserAreaByCentreDistance (areas, clientOnSecondary) == 1);

        const auto next = MatrixStandalone::centreThenEnsureClientBoundsTitleBarOnScreen (
            clientOnSecondary, frame, areas, secondary);
        const auto full = frame.addedTo (next);

        expect (full.getCentre() == secondary.getCentre());
        expect (full.getCentre() != primary.getCentre());
        expect (next.getWidth() == clientOnSecondary.getWidth());
        expect (next.getHeight() == clientOnSecondary.getHeight());
        expect (MatrixStandalone::titleBarStripIntersectsAnyUserArea (
            MatrixStandalone::titleBarDragStrip (next, frame), areas));
    }

    void testCentreThenEnsureOversizedTitleBarIntersects()
    {
        beginTest ("centreThenEnsure clamps after centre when strip would miss user area");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        // Tall user area with a top inset so a tall centred window can leave the strip above it.
        const juce::Rectangle<int> user { 0, 100, 800, 400 };
        juce::Array<juce::Rectangle<int>> areas;
        areas.add (user);

        const juce::Rectangle<int> client { 0, 0, 700, 600 };
        const auto centredOnly = MatrixStandalone::centreClientBoundsOnDisplayUserArea (client, frame, user);
        const auto afterEnsure = MatrixStandalone::centreThenEnsureClientBoundsTitleBarOnScreen (
            client, frame, areas, user);

        expect (centredOnly.getWidth() == client.getWidth());
        expect (afterEnsure.getWidth() == client.getWidth());
        expect (MatrixStandalone::titleBarStripIntersectsAnyUserArea (
            MatrixStandalone::titleBarDragStrip (afterEnsure, frame), areas));
    }

    void testRecentreClientBoundsOnNearestDisplayUserArea()
    {
        beginTest ("recentre composition uses nearest display (live / empty Displays)");

        const juce::BorderSize<int> frame { 30, 8, 8, 8 };
        const auto& displays = juce::Desktop::getInstance().getDisplays();

        if (displays.displays.isEmpty())
        {
            const juce::Rectangle<int> client { 40, 60, 1200, 800 };
            expect (MatrixStandalone::recentreClientBoundsOnNearestDisplayUserArea (client, frame, displays)
                    == client);
            return;
        }

        const auto* primary = displays.getPrimaryDisplay();
        expect (primary != nullptr);

        if (primary == nullptr)
            return;

        const auto user = primary->userBounds.toNearestInt();
        // Off-centre but still on the primary user area so nearest stays primary.
        const juce::Rectangle<int> client { user.getX() + 40, user.getY() + 60, 400, 300 };
        const auto next = MatrixStandalone::recentreClientBoundsOnNearestDisplayUserArea (client, frame, displays);

        expect (next.getWidth() == client.getWidth());
        expect (next.getHeight() == client.getHeight());
        expect (next != client);

        juce::Array<juce::Rectangle<int>> userAreas;
        for (const auto& d : displays.displays)
            userAreas.add (d.userBounds.toNearestInt());

        expect (MatrixStandalone::titleBarStripIntersectsAnyUserArea (
            MatrixStandalone::titleBarDragStrip (next, frame), userAreas));

        const auto full = frame.addedTo (next);
        expect (full.getCentre() == user.getCentre());
    }

    void testWindowsAltF4QuitKey()
    {
        beginTest ("Alt+F4 matches Windows standalone quit key");

        expect (MatrixStandalone::isWindowsAltF4QuitKey (
            juce::KeyPress (juce::KeyPress::F4Key, juce::ModifierKeys::altModifier, 0)));
        expect (! MatrixStandalone::isWindowsAltF4QuitKey (
            juce::KeyPress (juce::KeyPress::F4Key, juce::ModifierKeys::noModifiers, 0)));
        expect (! MatrixStandalone::isWindowsAltF4QuitKey (
            juce::KeyPress ('q', juce::ModifierKeys::commandModifier, 0)));
    }

    void testStandardApplicationQuitKeypress()
    {
        beginTest ("Ctrl/Cmd+Q matches JUCE Quit default keypress");

        expect (MatrixStandalone::isStandardApplicationQuitKeypress (
            juce::KeyPress ('q', juce::ModifierKeys::commandModifier, 0)));
        expect (! MatrixStandalone::isStandardApplicationQuitKeypress (
            juce::KeyPress ('q', juce::ModifierKeys::noModifiers, 0)));
    }

    void testBindStandaloneQuitCommandsWiresStandardKeypress()
    {
        beginTest ("bind wires Quit keypress, first target, and invokeDirectly");

        QuitCommandTarget target;
        juce::ApplicationCommandManager commandManager;
        juce::Component keyRoot;
        keyRoot.addToDesktop (0);

        expect (! MatrixStandalone::quitCommandHasStandardKeypress (commandManager));

        MatrixStandalone::bindStandaloneQuitCommands (commandManager, target, keyRoot);

        expect (MatrixStandalone::quitCommandHasStandardKeypress (commandManager));
        expect (commandManager.getFirstCommandTarget (juce::StandardApplicationCommandIDs::quit) == &target);
        expect (commandManager.invokeDirectly (juce::StandardApplicationCommandIDs::quit, false));
        expect (target.quitPerformCount == 1);

        keyRoot.removeFromDesktop();
    }

    void testFindNearestDisplayForBounds()
    {
        beginTest ("nearest user area by centre distance (headless-safe)");

        juce::Array<juce::Rectangle<int>> areas;
        expect (MatrixStandalone::indexOfNearestUserAreaByCentreDistance (areas, { 0, 0, 10, 10 }) == -1);

        areas.add ({ 0, 0, 1920, 1080 });
        areas.add ({ 1920, 0, 1920, 1080 });

        expect (MatrixStandalone::indexOfNearestUserAreaByCentreDistance (
                    areas, { 100, 100, 120, 80 })
                == 0);
        expect (MatrixStandalone::indexOfNearestUserAreaByCentreDistance (
                    areas, { 2200, 100, 120, 80 })
                == 1);
        expect (MatrixStandalone::indexOfNearestUserAreaByCentreDistance (
                    areas, { -80000, -80000, 200, 150 })
                == 0);

        // Live Displays path only when the host exposes a monitor (skipped on headless Linux CI).
        const auto& displays = juce::Desktop::getInstance().getDisplays();

        if (displays.displays.isEmpty())
        {
            expect (MatrixStandalone::findNearestDisplayForBounds (displays, { 0, 0, 10, 10 }) == nullptr);
            return;
        }

        const auto* primary = displays.getPrimaryDisplay();
        expect (primary != nullptr);

        if (primary == nullptr)
            return;

        const auto user = primary->userBounds.toNearestInt();
        const juce::Rectangle<int> onPrimary { user.getX() + 40, user.getY() + 40, 120, 80 };
        expect (MatrixStandalone::findNearestDisplayForBounds (displays, onPrimary) == primary);

        const juce::Rectangle<int> farOffScreen { -80000, -80000, 200, 150 };
        expect (MatrixStandalone::findNearestDisplayForBounds (displays, farOffScreen) != nullptr);
    }
};

static StandaloneWindowPlacementTests standaloneWindowPlacementTests;
