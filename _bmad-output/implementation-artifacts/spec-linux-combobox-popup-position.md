---
title: 'Linux ComboBox popup position in plugin hosts'
type: 'bugfix'
created: '2026-10-12'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '34b5f68ba117b6e787c026fa4db022279f06f179'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Bitwig Studio on Linux (VST3), Matrix-Control ComboBox dropdowns open near the centre of the plugin editor instead of under (or near) the clicked combo, which breaks spatial UX. Standalone is the non-regression reference; `.syx` drag-and-drop under Wayland/Bitwig stays out of scope.

**Approach:** Anchor popups in the plugin editor’s local coordinate space (relative to the top-level component that parents the menu) instead of converting through unreliable global/screen positions, so lists open next to the combo in embedded hosts without Bitwig-only magic offsets or artificial recentring.

</frozen-after-approval>

## Implementation Notes

- Root cause: `PopupMenuPositioner` converted anchor points via `localPointToGlobal` then subtracted `topLevel->getScreenBounds().getPosition()`. Popups are parented with `addAndMakeVisible` on the editor peer, so bad global/screen mapping in Bitwig VST3 Linux yields wrong relative X/Y (reads as “centred in the editor”).
- Fix: compute placement with `topLevel.getLocalPoint(&anchor, …)` and clamp against `topLevel->getLocalBounds()`; return those local X/Y directly (no screen round-trip). Same API for all callers (`ScrollablePopupMenu`, `MultiColumnPopupMenu`, `HierarchicalPopupMenu`, `HeaderLogoPopupMenu`, layout re-fit).
- Private helpers now take `topLevel` + `anchor`; `ResolveYArgs::screenBounds` renamed to `hostBounds`.
- No Bitwig-only offset. No popup width / Look / DnD changes. No unit test in `Matrix-Control_Tests` (GUI positioner not linked there).
- Verified: `cmake --build --preset linux-debug --target Matrix-Control_VST3 Matrix-Control_Standalone` succeeded (VST3 copied to `~/.vst3`).
- Review patch: dropped threaded `hostBounds` param; helpers read `topLevel.getLocalBounds()` so `adjustXPosition` stays at 4 parameters.
- Pending human smoke: Bitwig VST3 Linux (Settings port combo + body combo) and Standalone Linux non-regression.

## Review Triage Log

- Five-parameter `adjustXPosition` / threaded `hostBounds` — `medium` / patched: helpers use `topLevel.getLocalBounds()`; `adjustXPosition` has 4 params again.
- Incomplete left/top host clamp (and forced Above/Below) — `medium` / deferred: pre-existing policy; see deferred-work.
- Assert topLevel matches `addAndMakeVisible` peer — `false`: both paths use `anchor.getTopLevelComponent()`.
- Oneshot missing AC / I/O matrix / review order / context links — `false`: oneshot template keeps Intent + Implementation Notes only.
- No recorded Bitwig UAT in spec — `low` / rejected for code path; noted as pending human smoke in Implementation Notes.
- Missing manual checklist for Hierarchical / HeaderLogo / MultiColumn — `low` / rejected: same `calculateDimensions` call site; Bitwig smoke covers shared path.
- No automated positioner test — `medium` / deferred: GUI not in unit-test target; manual smoke is the gate.
- Cross-platform non-regression not spelled out — `false` for this oneshot: Intent targets Linux host; local space is the correct shared model, not a Bitwig-only offset.

