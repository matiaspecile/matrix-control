---
title: 'Dialogs, wizards, Settings, and About chrome'
type: 'refactor'
created: '2026-10-23'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ba9a6f3ddf3e00c54d09fd4da24ff22c60558f5f'
context:
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/target-model.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/brownfield.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/1-structured-colour-substrate.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/2-shared-looks-and-control-display-widgets-off-local-colour.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/3-main-chrome-and-patch-master-shared-edit-panels.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After stories 1–3, modal helpers still paint border/title from local hex, Settings/About close crosses hardcode Red900, device value fields and About accents still dig ColourChart / `juce::Colours`, so Black↔Cream cannot recolour the remaining V1 user chrome cleanly.

**Approach:** On `feature/skins-colour-model-ssot`, wire Dialogs (including Getting Started), Settings, and About durable paints to existing structured Skin keys (`kDialog` border/title, `kWindowClose` glyph, ReadOnlyValueField maps) plus any missing Error/About pastilles; keep close-cross pale/hover contrast code-owned; good-enough parity.

## Boundaries & Constraints

**Always:**
- Mapping SSOT remains `(ObjectType, Property, State)` → pastille; no durable ColourChart / raw hex / `juce::Colours::*` on migrated Dialogs/Settings/About paint paths.
- Modal chrome uses surface/dialog object types already on the substrate (`kDialog`, `kBodyPanel`, `kHeaderPanel`, `kWindowClose`, …) — never `patch*` / `master*`.
- Durable close-cross colour from Skin (`kWindowClose` / `kGlyph`); down/disabled `.withAlpha(0.6f)` and hover cut-out contrast stay code-owned (same behaviour).
- Reuse story 1–3 maps and LookBuilders; extend Skin maps only for gaps (e.g. ReadOnlyValueField error text; About white/orange via existing White/Orange pastilles).
- English-only source; `lint_touched.py` clean on touched C++; compile with project preset.
- Good-enough Black↔Cream parity — no proud polish.

**Never:**
- Story 5: automated gated colour scan, broad CAP-5 opaque alpha-as-darken sweep, park-or-ship checklist.
- Refactor Header/Footer/Patch Edit / Master Edit / Shared (story 3).
- Touch `LockDimmingFilm`; change pale-hover behaviour of close crosses.
- Author editor, Skin Studio, new skins, typography (Orbitron ≠ PatchNameDisplay).
- Require zero colour under `Source/GUI/Tests/**` / `Tests/`.
- Commit / push / PR unless Guillaume asks; do not start story 5 unless explicitly requested.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Modal overlay | Confirm/prompt / Getting Started open | Border + title band from `kDialog` Border/TitleBand; body/overlay/title text from existing Skin keys | Missing key → existing Skin miss path |
| Settings/About close | Idle cross | Glyph from `kWindowClose` / `kGlyph` (Red900) | N/A |
| Settings/About close | Hover / down / disabled | Same interaction as today (hover fill+cut-out; down/disabled alpha) — not skin-mapped pale | N/A |
| Device value field | Searching / connected / not connected | Fill/text from `kReadOnlyValueField`; error text from mapped Error pastille (not ColourChart) | N/A |
| About accents | Title, values, link idle/hover | Durable colours from Skin keys (White/Orange pastilles via existing types); no ColourChart / `juce::Colours` defaults | N/A |
| Skin Black→Cream | Toggle in Standalone | Modal + Settings (+ About) durable colours follow maps | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/DialogMatrixHelpers.{h,cpp}` — Remove `kDialogBorderColour` / `kModalTitleBandColour`; `paintMatrixOverlayChrome` uses `kDialog` Border + TitleBand. Callers that used helper hex (SettingsTabRail, close hover cut-out) switch to Skin TitleBand.
- `Source/GUI/Dialogs/GettingStartedWizardDialogDevice.cpp`, `EpromTypePromptDialog.cpp` — Drop ColourChart in `refreshDeviceValueField`; resolve fill/text/error via Skin / ReadOnlyValueField keys.
- Other `Source/GUI/Dialogs/*` confirm/prompt/progress — Already Skin-based body text; verify no residual ColourChart/hex after helper change.
- `Source/GUI/Settings/SettingsWindow.cpp`, `Source/GUI/About/AboutWindow.cpp` — Close durable colour from `kWindowClose`/`kGlyph`; keep alpha/hover cut-out code path; title-band cut-out ink from Skin `kDialog` TitleBand (same Black).
- `Source/GUI/Settings/SettingsTabRail.cpp` — Selected fill from Skin TitleBand (not helper hex).
- `Source/GUI/Settings/SettingsPanelDevice.cpp` — Same device-field Skin resolution as dialogs.
- `Source/GUI/About/AboutPanel.{h,cpp}` — Replace ColourChart White/Orange and `juce::Colours` link defaults with Skin-resolved pastilles (reuse White e.g. Button Hover text; Orange e.g. Section/Module header Orange — no new About object type unless a gap has no coherent reuse).
- `Source/GUI/Skins/SkinColoursWidgetsDisplays.h` (+ `Skin.cpp` if needed) — Add `kReadOnlyValueField` Text Error → Red500 (story 1 deferred error role to call-site migration). Prefer reuse over new types for About accents.
- Leave alone: `LockDimmingFilm`, Header/Footer/edit panels, story 5 alpha sweep, Tests/Dev, overlay dim `.withAlpha(0.85f)` on Skin body fill.
- Quality: `Scripts/quality/lint_touched.py`; CMake only if new `.cpp`.

**Reuse:** Story 1 `kDialog` / `kWindowClose` / ReadOnly maps; story 2 ReadOnly widget defaults + LookBuilders; modal helpers Skin body/header/button text already wired.

**Do not change:** Pale-hover semantics; panel layout geometry; stories 3/5 breadth.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Dialogs/DialogMatrixHelpers.{h,cpp}` -- Wire border/title to `kDialog` keys; delete local hex constants; update any in-header consumers -- CAP-3/4 modal chrome.
- [x] `Source/GUI/Dialogs/GettingStartedWizardDialogDevice.cpp` + `EpromTypePromptDialog.cpp` -- Device field colours from Skin (incl. error) -- CAP-4 wizard/prompt.
- [x] `Source/GUI/Settings/SettingsWindow.cpp` + `SettingsTabRail.cpp` + `SettingsPanelDevice.cpp` -- Close glyph Skin; rail selected fill Skin; device field Skin -- CAP-4 Settings.
- [x] `Source/GUI/About/AboutWindow.cpp` + `AboutPanel.{h,cpp}` -- Close glyph Skin; About accents from Skin; drop ColourChart / `juce::Colours` durable defaults -- CAP-4 About.
- [x] `Source/GUI/Skins/SkinColoursWidgetsDisplays.h` + `Skin.cpp` (if needed) -- Add ReadOnlyValueField Text Error map; wire putColour -- CAP-2 gap.
- [x] Audit remaining Dialogs/Settings/About under story scope -- No durable ColourChart/hex/`juce::Colours` left on migrated paths -- CAP-5 opaque path for this zone only (not repo-wide scan).
- [x] Build + `Scripts/quality/lint_touched.py` -- Compile and lint green on touched C++ -- gate.

**Acceptance Criteria:**
- Given Matrix modal chrome (confirm/prompt / Getting Started), when inspecting durable border/title/overlay paints, then they resolve via Skin structured keys (no helper hex / ColourChart).
- Given Settings and About close buttons, when idle, then cross colour comes from `kWindowClose`/`kGlyph`; hover/down/disabled pale behaviour remains code-owned and visually equivalent.
- Given Settings rail selected fill and device value fields (Settings + dialogs), when painted, then colours come from Skin keys (including error text), not ColourChart.
- Given About title/values/links, when painted, then durable colours come from Skin keys (no ColourChart / `juce::Colours` defaults).
- Given Black↔Cream toggle, when viewing one Matrix modal + Settings (+ About if touched), then durable colours follow maps with good-enough parity.
- Given story 4 complete, when checking scope, then Header/Footer/edit panels, LockDimmingFilm, and story 5 scan/park work were not started.
- Given touched C++, when building macos-debug-arm64 and running `lint_touched.py`, then both succeed.

### Review Findings

- [x] [Review][Defer] Migrated Dialogs/Settings/About paint digs not observed by automated tests [`DialogMatrixHelpers.cpp` / Settings / About call sites] — deferred: CONVENTIONS forbid GUI paint unit tests; map keys pinned in `SkinColourSubstrateTests`; story 5 gated colour scan owns residual call-site coverage (reconfirmed external code review 2026-10-23; already listed in deferred-work.md).

**Rejected**
- false — Code Map still cites Button Hover for About White while code uses DarkPanel Text Hover — code and Design Notes are the SSOT; fixing only the Code Map example would edit the spec under review (rejected per triage rule).
- false — Contradictory Build triage log entries on About White key — narrative of false-then-patched path; code already on DarkPanel; editing the log is a spec-only fix.
- false — Getting Started Black-lock vs Black↔Cream smoke wording — pre-existing product lock (`skinBlack_` + header contract); durable paints still resolve via Skin; documenting the lock is a spec-only fix.
- false — About/rail semantic Skin borrows without alias token — intentional frozen couplings (DarkPanel White, ModuleHeader Orange, TitleBand rail); About already comments the reuse.
- false — Three identical device-field Skin digs / missing Implementation Notes rule — within WET→DRY tolerance (Build triage); documenting a fourth-site rule is a spec-only fix.
- false — Verification Commands omit SkinColourSubstrate test — Implementation Notes already record the pin gate; adding the command is a spec-only fix.
- false — `review_loop_iteration: 0` with status done — metadata hygiene only; editing frontmatter is a spec-only fix.
- false — Empty Spec Change Log after About White key choice — design shift already in Design Notes / Implementation Notes / triage; changelog fill is a spec-only fix.
- low (rejected) — Overlay chrome comment still says “grey border” after Skin border — cosmetic; everyday readers see the Skin dig on the next line; prior Build triage rejected the same class of comment cleanup.
- false — Close-button paint Release crash without early-return — Window ctors call `setSkin` before `addAndMakeVisible`; same invariant as Build triage; adding a Release guard would add complexity without a demonstrated reachable path.

## Implementation Notes

- Modal chrome: deleted `kDialogBorderColour` / `kModalTitleBandColour`; `paintMatrixOverlayChrome` uses `kDialog` Border + TitleBand. Overlay dim `.withAlpha(0.85f)` left as code-owned (story 5).
- Device fields (Settings + Getting Started + Eprom prompt): fill/text/error via `kReadOnlyValueField` (new Text Error → `kRed500` map + `putColour`).
- Settings/About close: durable glyph from `kWindowClose`/`kGlyph`; title-band cut-out from `kDialog` TitleBand; down/disabled `.withAlpha(0.6f)` and hover fill+cut-out unchanged.
- Settings tab rail selected fill from Skin TitleBand (dropped helper hex include).
- About accents: title/values/link idle White via DarkPanel Text Hover; link hover Orange via ModuleHeader LineOrange; dropped ColourChart / `juce::Colours` defaults on link members.
- Zone audit: no durable ColourChart / hex / `juce::Colours` under `Source/GUI/{Dialogs,Settings,About}/`.
- Review patches: About White → DarkPanel Text Hover; assertGapKeys pins DarkPanel Hover White + ModuleHeader LineOrange.
- Verified: macos-debug-arm64 build, `lint_touched.py` OK, SkinColourSubstrate tests green (incl. ReadOnly error + About-reuse pins).

## Spec Change Log

## Review Triage Log

- false — About White should use DarkPanel Text Hover instead of Button Text Hover — Design Notes allowed Button Hover as reuse example; both map to White today. Routed separately as low patch to prefer documented brand White key.
- low (patched) — About title/value/link idle White via Button Text Hover couples About to button-hover palette; DarkPanel Text Hover is the documented absolute White brand pastille — switch About to that key.
- false — Extract shared helper for three identical device-field Skin digs — third copy is within WET→DRY tolerance; helper would add surface without everyday failure; leave call sites.
- false — ReadOnlyValueFieldLook omits Error after map add — story 2 kept setColours + Skin digs for overrides; Look Error slot not required.
- false — AboutCloseButton/SettingsCloseButton paint null-skin crash — Window ctors call setSkin before addAndMakeVisible; button not in hierarchy while skin_ is null; jassert is enough.
- false — AboutTextLink default-constructed colours until refresh — AboutPanel ctor calls refreshHyperlinkAppearance before addAndMakeVisible; links cannot paint unset.
- false — Settings rail selected fill borrowing kDialog TitleBand — intentional Code Map / Design Notes coupling (same Black title-band pastille).
- false — Black/Cream map columns identical for dialog/close/ReadOnly — intentional good-enough parity; product smoke still valid for half-unthemed leftovers.
- low (rejected) — Extract shared Settings/About close-chrome helper — pre-existing duplication; extract adds surface beyond direct correction.
- low (rejected) — Update comments describing Button hover / grey border — cosmetic; DarkPanel switch comment updated in patch only.
- medium (patched) — assertGapKeys does not pin About-reused White/Orange pastilles — extend pins for DarkPanel Text Hover → White and ModuleHeader LineOrange → Orange (Black+Cream).
- defer — Migrated Dialogs/Settings/About paint call sites not observed by unit tests — map keys pinned; GUI paint unit tests forbidden; story 5 gated scan owns residual path checks.

## Design Notes

**Pre-wired substrate:** Story 1 already maps `kDialog` Border (`kGrey525`), TitleBand (`kBlack`), and `kWindowClose` Glyph (`kRed900`). This story primarily adopts call sites — do not reintroduce helper hex “temporarily”.

**ReadOnly error:** Story 1 intentionally left error red unmapped; story 2 kept `setColours` overrides. Add Text Error → Red500 when migrating the three device-field call sites.

**About accents:** Prefer reusing existing White/Orange pastilles (DarkPanel Text Hover for absolute White; ModuleHeader LineOrange) over a new About object type. Idle credit-link greys already Skin (`kLabel` text).

**Close hover:** Hover fill = durable red; glyph ink = title-band Black (Skin). Alpha pale on down/disabled stays code-owned — do not invent Skin states for pale.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`

**Manual checks (if no CLI):**
- Standalone Black↔Cream: one Matrix modal (border/title), Settings window (close + rail + device field), About (close + accents) — no half-unthemed leftovers from this story.
