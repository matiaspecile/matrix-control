---
organization: Ten Square Software
project: Matrix-Control
title: Title-Bar Double-Click Recentre
author: BMad Agent
type: feature
created: '2026-10-12'
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 'fd38a0feb3299acb27521ff89288f9f123de0d4e'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-standalone-quit-and-window-placement.md'
  - '{project-root}/_bmad-output/implementation-artifacts/deferred-work.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Double-clicking the native title bar does not recentre the Matrix-Control Standalone window. That behaviour was deferred as Lot 3 from the Standalone quit/placement Build (2026-10-09) and remains open.

**Approach:** On title-bar double-click in Standalone (macOS, Windows, Linux), centre the window on the nearest display’s user area, reusing Standalone placement helpers where they fit, without weakening quit gating or multi-monitor drag.

## Boundaries & Constraints

**Always:**
- Standalone only; gesture on macOS, Windows, and Linux.
- Keep OS-native title bar and `setConstrainer(nullptr)` (multi-monitor drag).
- Recentre uses the nearest display’s `userBounds` (same nearest-display idea as launch ensure; not full screen behind dock/taskbar).
- On macOS, this gesture recentres instead of the OS zoom/maximize-on-double-click behaviour.
- Quit path (`closeButtonPressed` → `systemRequestedQuit` → session close gate) stays unchanged.
- Launch-only ensure stays launch-only — do not re-run it from every `resized`.

**Never:**
- Host/DAW title-bar control for VST3/AU plugin editors (host owns chrome; out of product promise).
- Recolour or redesign native title bar; re-enable `decoratorConstrainer`.
- Change Alt+F4 / Ctrl+Q / session-close confirmation behaviour.
- Fight intentional multi-monitor placement except on the explicit double-click gesture.
- Centre on primary or under-cursor display instead of nearest-to-window.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy path | Standalone; title-bar double-click; displays present | Window centred on nearest display `userBounds` (size unchanged) | N/A |
| Single display | Window off-centre | Centres on that display user area | N/A |
| Multi-monitor | Window mostly on secondary | Centres on that nearest display user area | N/A |
| macOS | Title-bar double-click | Centres (does not leave OS zoom as the product behaviour) | N/A |
| No displays | Empty `Displays` list | No-op (leave bounds) | Same as launch ensure |
| Unknown frame | Native frame top unknown (`<= 0`) | No-op until peer reports chrome | Same as launch ensure |
| Plugin in DAW | Host title-bar double-click | Unchanged host behaviour (out of scope) | N/A |

</frozen-after-approval>

## Code Map

- `Source/Standalone/MatrixControlStandaloneFilterWindow.h` — native title bar; launch ensure; quit routing — add or call on-demand recentre from title-bar double-click hook (platform-appropriate).
- `Source/Standalone/StandaloneWindowPlacement.h` — `ensureClientBoundsTitleBarOnScreen`, clamp, nearest-display helpers — reuse centre math; add explicit “centre client bounds on preferredLimits” helper (launch ensure only centres as last-resort fallback).
- `Source/Standalone/MatrixControlStandaloneApp.cpp` — quit/placement lifecycle — do not change gate; optional wire if hook lives at app/window peer level.
- `Source/Standalone/StandaloneQuitCommands.h` — quit keys only — leave alone.
- `Tests/Unit/StandaloneWindowPlacementTests.cpp` — extend for unconditional centre-on-display helper matrix.
- `CMakeLists.txt` — Standalone target sources if a new `.cpp` is required for platform hooks.
- `Source/GUI/PluginEditor*.cpp` — `setSize` / `syncStandaloneWindowSize` only — no host title-bar API; do not invent plugin recentre.
- Deferred source: `_bmad-output/implementation-artifacts/deferred-work.md` (2026-10-09 Build split) + Never Lot 3 in `spec-standalone-quit-and-window-placement.md`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Standalone/StandaloneWindowPlacement.h` (+ tests) — Add pure helper to centre client bounds on a display `userBounds` (size preserved); unit-test single/multi/empty/unknown-frame cases — shared geometry SSOT
- [x] `Source/Standalone/MatrixControlStandaloneFilterWindow.h` (and platform hook file if needed) — On title-bar double-click (macOS/Windows/Linux), centre via helper + nearest display; keep constrainer null and native title bar — product gesture
- [x] `CMakeLists.txt` — Register any new Standalone-only source — build wiring
- [ ] Manual UAT — Standalone Debug on macOS/Windows/Linux: double-click title bar recentres; drag to secondary then recentre; quit/close gate still works — human smoke
- [x] `python Scripts/quality/lint_touched.py` — clean on touched C++ under `Source/` / `Tests/`

**Acceptance Criteria:**
- Given Standalone on macOS, Windows, or Linux with a known native frame, when the user double-clicks the title bar, then the window is centred on the nearest display user area without changing size.
- Given the window on a secondary monitor, when the user double-clicks the title bar, then it recentres on that nearest display (not forced to primary).
- Given macOS Standalone, when the user double-clicks the title bar, then the product behaviour is recentre (not leaving OS zoom as the delivered behaviour).
- Given plugin-in-host, when the user double-clicks the host title bar, then Matrix-Control does not claim or alter that behaviour.
- Given Alt+F4 / Ctrl+Q / title-bar Close with at-risk session, when quitting, then the existing session close gate still runs unchanged.

## Implementation Notes

- Added `centreClientBoundsOnDisplayUserArea` (unconditional centre; size preserved; empty preferredLimits / unknown frame top → no-op).
- Platform hook `StandaloneTitleBarDoubleClick.{h,cpp}`:
  - **Windows:** HWND subclass; `WM_NCLBUTTONDBLCLK` on caption → recentre (consumes message; OS maximize suppressed).
  - **macOS:** peer-level replace of JUCE `NSWindow` delegate method `windowShouldZoom:toFrame:` for the Standalone window — invoke recentre and return `NO` so OS zoom/maximize is not the product behaviour (native title bar + `setConstrainer(nullptr)` kept). System Settings “Minimize” on title-bar double-click is outside this hook (zoom/maximize AC only).
  - **Linux:** poll global cursor + button against the title-bar drag strip (decoration events never reach JUCE). Standalone is non-resizable and JUCE does not advertise `_NET_WM_ACTION_MAXIMIZE_*`; common WMs usually will not maximize. **Residual risk for Manual UAT / review:** a WM that still maximizes on title-bar double-click cannot be suppressed atomically from the client without fighting the compositor; the poller will recentre afterward but a brief maximize flash is possible.
- `MatrixControlStandaloneFilterWindow::recentreOnNearestDisplayUserArea` uses nearest-display + centre helper; launch ensure path unchanged; constrainer stays null; quit path untouched.
- Verified: `linux-debug` Standalone + `StandaloneWindowPlacement` tests (0 failures); `lint_touched.py` OK. Manual UAT still human.

## Spec Change Log


## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Deferred Lot 3 evidence still said “later branch” | medium | Confirmed stale evidence line; **patched** (rewrote evidence to cite this spec). |
| Spec status / Manual UAT vs “Resolved” wording clash | false | Status is workflow (`in-review`); Manual UAT intentionally open; deferred summary “Resolved” means code shipped for review, not human UAT done. |
| Code Map omits new StandaloneTitleBarDoubleClick files | false | Rejected — fix would only edit this build’s spec. |
| macOS Minimize preference missing from frozen matrix/AC | false | Frozen Always targets zoom/maximize; Minimize is OS-owned — **deferred** as residual. |
| Windows maximize suppress under-specified in matrix | false | Rejected — fix would only edit this build’s spec; code consumes `WM_NCLBUTTONDBLCLK`. |
| Centre without re-ensure can leave title bar off userBounds | medium | Confirmed for oversized windows; **patched** (`centreThenEnsureClientBoundsTitleBarOnScreen`). |
| Centre helper comment oversold “always” | low | Confirmed; **patched** (comment tightened). |
| macOS class-wide zoom swizzle never restored on detach | low | Standalone-only process; callback map clears on detach; rejected as low + non-trivial restore complexity. |
| `#else` poller on non-Win/Mac (incl. theoretical iOS) | low | Confirmed structure risk; **patched** (`#elif JUCE_LINUX` + no-op elsewhere). |
| Linux hard-coded 500 ms / 6 px double-click | low | Real polish gap; **deferred**. |
| Linux strip includes window-control hit targets | medium | Real; **deferred** (WM metrics). |
| No automated hook attach/gesture tests | medium | Confirmed Standalone `.cpp` not in Tests; **deferred** (Manual UAT / Core-only policy). |
| Empty Spec Change Log / Review Triage on new Lot 3 | false | Process noise; this triage log is the review trail. |
| Re-attach on every `visibilityChanged` show | false | Required so Windows/macOS hooks install after peer exists; ctor attach often no-ops. |
| Linux poller misses sub-frame button down | medium | Real polling limit; **deferred**. |
| Linux WM maximize before poller → size not preserved | medium | Real on some WMs; **deferred** (documented residual). |
| `getMillisecondCounter` zero skips first-click pair | low | Confirmed edge; **patched** (`hasLastClick_`). |
| `setBounds` inside `WM_NCLBUTTONDBLCLK` | medium | Nested non-client dispatch risk; **patched** (`callAsync`). |
| Windows unhook restores even if another subclass replaced ours | medium | Classic subclass bug; **patched** (restore only if current is still our proc). |
| macOS detach peer-null misses callback erase | medium | Confirmed; **patched** (store `nsWindow` key at attach). |
| Stale deferred evidence (Edge, deletion) | medium | Same as row 1; **patched**. |
| Linux size-stable AC vs WM maximize (claim) | medium | Same as maximize residual; **deferred**. |
| VG: recentre composition untested at call-site boundary | medium | Confirmed; **patched** (`recentreClientBoundsOnNearestDisplayUserArea` + secondary/oversized tests). |
| VG: title-bar hooks outside automated path | medium | Same as hook-test gap; **deferred**. |

## Design Notes

Native title bar means JUCE component mouse events do not reach the chrome. Expect a small platform hook per OS (Windows non-client double-click, macOS title-bar double-click / zoom override, Linux WM equivalent) that calls shared centre math — do not switch back to JUCE custom title bar.

Launch `ensureClientBoundsTitleBarOnScreen` centres only when clamp fails; double-click needs an unconditional centre path so a valid off-centre window still moves.

Naming note for humans: this Lot 3 is unrelated to Dirty/unsaved UX “chantier 3” (session close warning), already shipped separately.

## Verification

**Commands:**
- `cmake --build --preset <platform-debug>` — Standalone (+ unit tests target) builds
- `ctest` / unit-test binary including `StandaloneWindowPlacementTests` — centre helper cases pass
- `python Scripts/quality/lint_touched.py` — clean on touched C++

**Manual checks (if no CLI):**
- Standalone on each OS: title-bar double-click recentres; multi-monitor smoke; quit/close still gated when at-risk.

### Review Findings

- [x] [Review][Patch] Windows async recentre callback can run after the Standalone window is destroyed (`StandaloneTitleBarDoubleClick.cpp` callAsync copies `std::function` closing over raw `this`; `SafePointer` / lifetime guard missing at FilterWindow attach) [Source/Standalone/StandaloneTitleBarDoubleClick.cpp:48]
- [x] [Review][Patch] Product recentre composition helper `recentreClientBoundsOnNearestDisplayUserArea` is never invoked by unit tests (only lower centre / centreThenEnsure helpers); a no-op body would still pass the suite [Tests/Unit/StandaloneWindowPlacementTests.cpp]
- [x] [Review][Patch] Deferred-work Lot 3 “Resolved” summary misstates macOS as title-bar strip poll; shipped path is `windowShouldZoom:toFrame:` [_bmad-output/implementation-artifacts/deferred-work.md:2504]

#### Rejected

- Spec status / Manual UAT / triage wording clash — rejected: fix would only edit this spec under review.
- Frozen Intent still says Lot 3 “remains open” — rejected: fix would only edit this spec under review.
- I/O matrix omits oversized / already-centred / Linux maximize — rejected: fix would only edit this spec under review.
- Code Map omits `StandaloneTitleBarDoubleClick` / optional App wire — rejected: fix would only edit this spec under review.
- Empty Spec Change Log — rejected: process noise; Review Triage Log is the trail.
- macOS sync recentre inside zoom delegate vs Windows `callAsync` — false: AppKit expects a synchronous zoom answer; no demonstrated crash on the sync path.
- `visibilityChanged` re-attach tears down Linux poller / half-gesture — low: rare hide/show mid-gesture; “already attached” short-circuit adds complexity for little everyday gain.
- Linux WM maximize before recentre / size-stable AC — already deferred in `deferred-work.md` for this spec; out of this review’s action scope.
- Linux poller misses sub-frame presses — already deferred.
- Linux strip includes window-control hit targets — already deferred.
- Linux hard-coded double-click interval/distance — already deferred.
- macOS System Settings Minimize-on-double-click — already deferred.
- Native title-bar hooks have no automated tests — already deferred (Manual UAT / Core-only Tests policy).
- Ensure-after-centre can leave oversized windows uncentred vs frozen happy-path wording — rejected: intentional `centreThenEnsure` (title-bar usable); fixing the wording would edit the frozen matrix.
- Unknown native frame → gesture no-op — false: same intentional guard as launch ensure.
- Empty `preferredLimits` still reaching ensure — low: exotic empty display `userBounds`; unlikely in everyday Standalone use.
- Windows unhook erases state while another WNDPROC sits above ours — low: no second subclass in this Standalone; keeping the map until topmost again adds complexity.
