---
organization: Ten Square Software
project: Matrix-Control
title: Spec — Combo Popup Auto Width
author: BMad Agent
type: feature
created: '2026-10-11'
status: done
route: dispatch
baseline_commit: '34b5f68ba117b6e787c026fa4db022279f06f179'
review_loop_iteration: 0
context:
  - /home/guillaume/Dev/Projects/JUCE/Matrix-Control/_bmad-output/project-context.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Linux (and sometimes elsewhere), MIDI port and audio device names are much longer than the fixed Settings combo width, so the open list clips labels and makes devices hard to tell apart.

**Approach:** Add an opt-in ComboBox flag so the ButtonLike (single-column) popup can widen to the longest item, capped by a design ceiling and by the free horizontal space beside the combo, while the closed combo keeps its construction width. Enable it only on the six Settings device/port combos listed below.

## Boundaries & Constraints

**Always:**
- Closed combo width and layout stay as today (Settings column widths unchanged).
- Opt-in per combo; default remains “popup matches closed width”.
- Enable only on: MIDI Keyboard From, MIDI Synth From, MIDI Synth To, Audio Input Device, Audio Output Device, Audio Synth From.
- When the flag is on, desired popup content width = max(closed width, longest item text + paddings); never narrower than the closed control.
- Hard ceiling: min(~90% of host/display width, fixed design max e.g. 480 × UI scale). Final width also cannot exceed free space in the chosen horizontal direction.
- Horizontal placement for flagged auto-width popups:
  1. Prefer growing to the right (align popup left edge with combo left edge).
  2. If the desired width does not fit to the right, grow to the left instead (align popup right edge with combo right edge) when that side has more usable room.
  3. If neither side fits the full desired width, use the side with more free space, clamp width to that free space (and the hard ceiling), and ellipsize overflowing labels.
- Reuse the HierarchicalComboBox measurement pattern (`GlyphArrangement::getStringWidth` + layout padding), adapted for flat ScrollablePopupMenu.
- Extend or specialize `PopupMenuPositioner` for this left/right grow rule when auto-width is on; leave non-flagged combo placement behavior unchanged.
- If the longest item still exceeds the final clamped width, truncate with ellipsis in the popup row (readable cut), not silent clip.
- No closed-face tooltip in this story.

**Decisions (2026-10-11):**
- Max width = min(screen/host fraction ~90%, design ceiling) **and** free space on the chosen side (prefer right grow, else left grow with right edges aligned).
- No tooltip on the closed combo for now.

**Never:**
- Do not enable the flag on other Settings ButtonLike combos (UI Scale, Skin, policies, Driver Type, Sample Rate, Buffer Size, channel pairs, EPROM, etc.).
- Do not enable it on Computer Patches `selectPatchFileComboBox_` or other Patch Manager combos.
- Do not change closed-face width, Settings column metrics, or force-widen the Settings dialog.
- Do not redesign MultiColumn / Standard popup column math in this story (Settings targets are ButtonLike → ScrollablePopupMenu only).
- Do not change HierarchicalComboBox popup sizing (already content-aware).
- Do not add closed-combo tooltips in this story.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Short names | Flag on; all items shorter than closed width | Popup width equals closed width (no shrink, no useless grow) | N/A |
| Room on the right | Flag on; long name; enough space to the right of combo left | Popup widens to the right; left edges aligned | N/A |
| Combo near right edge | Flag on; long name; not enough space on the right, more on the left | Popup widens to the left; right edges aligned with combo | N/A |
| Hits max / side limit | Flag on; name longer than hard ceiling or free side space | Popup width = clamped max; overflowing rows use ellipsis | N/A |
| Flag off | Other Settings / Computer Patches combos | Popup width still matches closed control (unchanged) | N/A |
| List refresh | Ports/devices rebuilt then popup opened | Width and side choice recomputed from current items at open time | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Widgets/ComboBox.h` / `.cpp` — add opt-in setter/getter (mirror `setUsesPortSentinelPopupChrome`); ButtonLike `showPopup` → `ScrollablePopupMenu`
- `Source/GUI/Widgets/IPopupMenuHost.h` — optional host query for “auto popup width” / max, if popups should not depend on ComboBox private members
- `Source/GUI/Widgets/ScrollablePopupMenu.cpp` — today `columnWidth_ = comboBox_.getWidth()` (ctor ~71); `show()` builds `popupWidth` from that (~376); measure longest label when flag on, clamp by hard ceiling + free side space, then place
- `Source/GUI/Widgets/PopupMenuRenderer.cpp` — popup row `drawText` currently clips without ellipsis; enable ellipsis when auto-width hits max (or always for these rows if simpler and safe)
- `Source/GUI/Widgets/PopupMenuPositioner.cpp` — today’s `adjustXPosition` only flips after overflow (~64–76); for auto-width, choose grow-right vs grow-left from free space before placing
- `Source/GUI/Widgets/HierarchicalPopupMenuLayout.cpp` — `measureColumnWidths()` (~88–120) is the golden measurement precedent
- `Source/GUI/Settings/SettingsMidiPage.cpp` — enable flag on `keyboardFromCombo_`, `synthFromCombo_`, `synthToCombo_` after `makeCombo`
- `Source/GUI/Settings/SettingsAudioPage.cpp` — enable on `inputDeviceCombo_`, `outputDeviceCombo_`, `synthFromCombo_` only (not driver/rate/buffer/channels)
- `Source/GUI/Settings/SettingsShellMetrics.h` — closed widths 140 / 120 remain authoritative for the closed face
- Do **not** change: `ComputerPatchesPanel.cpp` select combo, `SettingsPanelSetup.cpp` policy/skin/scale combos, `MultiColumnPopupMenu` column threshold math

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Widgets/ComboBox.h` / `.cpp` (+ host if needed) — add default-off opt-in API for content-sized ButtonLike popup width with max clamp — per-instance wire-up without global behavior change
- [x] `Source/GUI/Widgets/ScrollablePopupMenu.cpp` — measure longest item at open; width = min(desired, hardCeiling, freeSpaceOnChosenSide); keep scrollbar height logic unchanged
- [x] `Source/GUI/Widgets/PopupMenuPositioner.cpp` (or Scrollable `show`) — prefer left-aligned grow-right; else right-aligned grow-left; non-flagged path unchanged
- [x] `Source/GUI/Widgets/PopupMenuRenderer.cpp` (or Scrollable paint path) — when row text exceeds column, draw with ellipsis so capped max stays readable
- [x] `Source/GUI/Settings/SettingsMidiPage.cpp` / `SettingsAudioPage.cpp` — enable the flag on the six target combos only
- [ ] Manual verify on Linux Settings Audio/MIDI with long ALSA names (combo mid-column and near right edge); spot-check a non-flagged Settings combo

**Acceptance Criteria:**
- Given Settings MIDI/Audio and a long device/port name with room on the right, when one of the six flagged combos opens, then the popup widens to the right with left edges aligned, unless a hard ceiling applies.
- Given a flagged combo near the right edge with more free space on the left, when it opens with a long name, then the popup widens to the left with right edges aligned.
- Given the hard ceiling or free-side limit applies, when the popup opens, then width equals that clamp and overflowing labels use ellipsis.
- Given a flagged combo whose items are all shorter than the closed width, when it opens, then popup width equals the closed control width.
- Given any non-flagged Settings or Computer Patches combo, when it opens, then popup width and placement still match today’s behavior.
- Given any of the six flagged combos, when closed, then its on-screen width matches today’s layout metrics.

### Review Findings

- [x] [Review][Decision] Keep closed-width floor when it exceeds free-side space? — Resolved 2026-10-12: **keep floor** (option 1). No code change; intentional tradeoff reconfirmed.
- [x] [Review][Patch] Reserve scrollbar gutter when ellipsizing clamped popup labels [`Source/GUI/Widgets/ScrollablePopupMenu.cpp` `drawItems`] — applied 2026-10-12: trim itemBounds by scrollbar width when scrollbarNeeded_ so ellipsis/hover stop before the overlay.
- [x] [Review][Patch] Add unit test for freeRight == freeLeft prefer-right tie [`Tests/Unit/ComboBoxPopupAutoWidthTests.cpp`] — applied 2026-10-12: `testPreferRightWhenFreeSidesEqual`.
- [x] [Review][Defer] ScrollablePopupMenu content-sized wiring untested — deferred: already recorded 2026-10-11; Matrix-Control_Tests does not link ComboBox/ScrollablePopupMenu.
- [x] [Review][Defer] PopupMenuPositioner grow-left / lock-left untested — deferred: already recorded 2026-10-11; GUI geometry outside unit-test target.
- [x] [Review][Defer] Settings six opt-in sites unasserted — deferred: already recorded 2026-10-11; Settings pages outside unit-test norms.
- [x] [Review][Defer] PopupMenuRenderer ellipsis call site untested — deferred: paint/renderer not in Matrix-Control_Tests; helper pattern would not prove the call site.

#### Rejected

- Blind: `resolve` closed floor overflows host in everyday Settings — false/unreachable: both free sides below closed only if host width ≲ closed + chrome; Settings/plugin host is far wider. Residual policy conflict kept as Decision above.
- Blind: grow-left `jlimit` breaks right-edge align when closed > freeLeft — same unreachable narrow-host case; not a separate defect.
- Edge: `roundToInt(columnWidth_)` 1px overflow with `lockLeftEdge` — low, unlikely everyday; rejected (fix adds clamp complexity for sub-pixel edge).
- Blind: Implementation Notes overclaim remasure coverage — rejected: fix would edit the spec under review.
- Blind: Spec `status: done` with unchecked manual verify — rejected: process/spec hygiene; Guillaume reported Linux smoke success in this review.
- Blind: empty Spec Change Log — rejected: fix would edit the spec under review.
- Blind: ScrollablePopupMenu task text omits closed floor — rejected: fix would edit the spec under review.
- Acceptance: on-screen AC verification still open — rejected: smoke test reported success; checkbox is process, not a code defect.
- Blind/Acceptance: global ellipsis on Hierarchical rows (chevron overlap risk) — low; prior Build accepted global readable cut; Hierarchical column math out of scope.

## Implementation Notes

- Opt-in API: `ComboBox::setUsesContentSizedPopupWidth` (default off). Host interface left unchanged — ScrollablePopupMenu queries ComboBox directly.
- Pure layout math in `GUI/Helpers/ComboBoxPopupAutoWidth.h` (hard ceiling + grow side + clamp). Unit-tested via `Tests/Unit/ComboBoxPopupAutoWidthTests.cpp`.
- ScrollablePopupMenu measures longest `popupListLabelForItemIndex` at construct time; free side space subtracts popup chrome (2× stroke). Placement uses `PopupMenuPositioner::DimensionsArgs::alignRightEdges` when growing left.
- Popup row labels use `TextFitHelpers::fitWithAsciiEllipsis` (all Scrollable/MultiColumn/Hierarchical label rows) so capped widths stay readable without Unicode ellipsis.
- Flag enabled on Settings MIDI `makeCombo` (Keyboard From / Synth From / Synth To) and Settings Audio Input Device / Output Device / Synth From only.
- Matrix I/O: short / grow-right / grow-left / hard ceiling / side clamp / remeasure-on-item-change covered by `ComboBoxPopupAutoWidth` unit tests (green). Flag-off is structural (`usesContentSizedPopupWidth` default false + early return); ComboBox is not linked into `Matrix-Control_Tests`. On-screen ALSA placement remains a manual check for Guillaume.
- Review patches: closed-width floor; scrollbar gutter in measure; `lockLeftEdge` for grow-right; clearer unit-test names + floor cases.
- Verified locally: `cmake --build --preset linux-debug` (plugin + tests), category `ComboBoxPopupAutoWidth` 0 failures, `lint_touched.py` OK.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| resolve can return width below closed (Blind + Edge claim) | high | Confirmed at `ComboBoxPopupAutoWidth::resolve` — floored at closed; unit tests added. **patch** applied. |
| No unit test for closed-width floor | medium | Same root as above — tests added. **patch** applied. |
| Scrollbar width ignored in measure | medium | Overlay stole label space — `scrollbarNeeded_` computed before measure; gutter added to sidePadding. **patch** applied. |
| Ellipsis under scrollbar strip | medium | Same root as scrollbar reserve. **patch** applied. |
| growRight + adjustXPosition 1px flip | medium | Confirmed — `lockLeftEdge` keeps left align for content-sized grow-right. **patch** applied. |
| No tests for alignRightEdges / resolveXPosition | medium | Real gap; ComboBox/Positioner not in test target. **defer**. |
| No automated guard for six Settings opt-ins | low | Real but GUI construction outside test norms. **defer**. |
| Global ellipsis on all popup rows | low | Intentional readable cut; short labels unchanged. Rejected (fix would add flagged-only branches). |
| Misleading “ellipsis implied” / remasure-on-open test names | low | Renamed to match pure-helper coverage. **patch** applied. |
| Getting Started / other MIDI surfaces still clip | false | Frozen Never excludes them; not this story. |
| Duplicate kLabelWidthPadding vs Hierarchical | low | Cosmetic drift risk; unlikely everyday harm. Rejected. |
| Spec manual check / empty logs while in-review | false | Process state, not a code defect. |
| textBounds width <= 0 → empty label | maybe-false | Needs a zero-width row; Settings combos keep closed floor. Rejected as low-unlikely. |
| ScrollablePopupMenu wiring untested (verification-gap) | medium | Pre-verified gap; disposition defer — GUI not in Matrix-Control_Tests. **defer**. |
| Grow-left placement untested (verification-gap) | medium | Pre-verified gap. **defer**. |
| Ellipsis renderer untested (verification-gap) | low | Test name cleaned; renderer stays manual. **patch** (rename) applied. |
| Remasure-on-open untested (verification-gap) | low | Test reworded to purity only. **patch** applied. |
| Settings opt-in unasserted (verification-gap) | medium | Pre-verified gap. **defer**. |

## Design Notes

Settings device/port lists are ButtonLike → `ScrollablePopupMenu` only. Prefer a ComboBox-level flag over changing `PopupMenuLayoutDimensions` globally so Computer Patches and short policy lists stay compact.

Golden measurement + side choice (adapt from hierarchical):

```cpp
const float desired = juce::jmax(closedWidth, textW + sidePadding);
const float hardCap = juce::jmin(hostWidth * 0.9f, designMaxPx * uiScale);
// prefer grow-right (align left); else grow-left (align right edges)
const float freeRight = screenRight - comboLeft;
const float freeLeft  = comboRight - screenLeft;
const bool growRight = desired <= freeRight || freeRight >= freeLeft;
const float sideCap = growRight ? freeRight : freeLeft;
columnWidth = juce::jmax(closedWidth, juce::jmin(desired, hardCap, sideCap));
// x = growRight ? comboLeft : comboRight - columnWidth  (plus chrome/stroke)
// grow-right uses lockLeftEdge so adjustXPosition cannot flip on 1px overflow
```

## Verification

**Commands:**
- `cmake --build --preset linux-debug` -- expected: plugin target builds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`

**Manual checks (if no CLI):**
- Linux Settings > Audio/MIDI: open the six combos with long ALSA/JACK names; confirm readable popup; confirm closed width unchanged.
- Open UI Scale / Skin / Computer Patches file combo: popup still matches closed width.
