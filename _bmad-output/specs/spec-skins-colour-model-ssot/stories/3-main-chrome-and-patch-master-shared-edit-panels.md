---
title: 'Main chrome and Patch/Master/Shared edit panels'
type: 'refactor'
created: '2026-10-22'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '8954be4ff15e763512216dd61927e22599bf520c'
context:
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/target-model.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/brownfield.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/1-structured-colour-substrate.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/2-shared-looks-and-control-display-widgets-off-local-colour.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After stories 1–2, Header/Footer still dig ColourChart for cartouche, HELP chrome, and MIDI-queue alert badges, while Patch Edit / Master Edit / Shared already mostly paint via structured Skin keys and injected Looks. Zone 3–4 of the migration plan is not closed for the primary product shell.

**Approach:** On `feature/skins-colour-model-ssot`, finish main chrome and Patch/Master/Shared panels so durable colours resolve only via structured `(ObjectType, Property, State)` maps and LookBuilders injection (Blue/Orange accents stay injection — no `patch*` / `master*` object types). Good-enough Black↔Cream parity; unify already-divergent chrome greys when SSOT-coherent.

## Boundaries & Constraints

**Always:**
- Structured keys remain mapping SSOT; chrome surfaces use existing surface object types (`kHeaderPanel`, `kFooterPanel`, `kBodyPanel`, `kFooterMessage`, `kDarkPanel`, …) plus any missing property keys needed for cartouche / HELP / alert badge slots.
- Header, Footer, body column fills, and BodyPanel product panels under Patch Edit / Master Edit / Shared / Patch Manager: no durable `ColourChart` digs, raw hex, or `juce::Colours::*` on the migrated paint path.
- Blue/Orange section/module/Matrix Mod accents continue via LookBuilders + `ColourVariant` / construction context — never new product-zone object types.
- Reuse story 2 Looks/builders; extend Skin maps only for chrome gaps.
- English-only source; `lint_touched.py` clean on touched C++; compile with project preset.
- Good-enough parity: micro-gaps OK if SSOT-coherent (including unifying Header cartouche Grey300 vs Footer HELP Grey350 onto one mapped chrome grey when that improves coherence).

**Never:**
- Story 4: `Dialogs/`, Getting Started wizard, `Settings/`, `About/`.
- Story 5: gated automated colour scan, broad CAP-5 opaque alpha-as-darken sweep (Footer error `.withAlpha(0.8f)`, ModulationBusCell drag tint, popup disabled alpha).
- Touch `LockDimmingFilm` or close-cross hover pale.
- Invent `patch*` / `master*` object types; harden accents into a single pastille without injection context.
- Author editor, Skin Studio, Duo/Monochrome, proud Black/Cream polish, typography (Orbitron ≠ PatchNameDisplay).
- Require zero colour under `Source/GUI/Tests/**` / `Tests/`.
- Commit / push / PR unless Guillaume asks; do not start stories 4–5 unless explicitly requested.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Header cartouche | Skin set; paint cartouche | Badge/frame from mapped Header chrome key (not ColourChart) | Missing key → existing Skin miss path |
| Footer HELP | Contextual help active | HELP badge/detail chrome from mapped Footer/surface key | Sticky error path unchanged |
| Footer MIDI alert | Queue pressure alert active | Badge fill/text from Skin keys (e.g. FooterMessage Error + absolute Black), not ColourChart | Inactive → no badge |
| Body columns | Patch/Master/Shared paint | Background from `kBodyPanel` background; widgets via injected Looks | N/A |
| Accent zones | Patch Blue / Master Orange headers | Same visual accents via Look `ColourVariant` injection | No new object types |
| Skin Black→Cream | Toggle in Standalone | Chrome + Patch Edit durable colours follow Cream maps | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h`, `SkinColoursPanels.h` (+ Layout if FooterMessage reuse), `Skin.cpp` — Add Header/Footer chrome keys for cartouche / HELP / alert badge slots (reuse `kFooterMessage` Error + Black absolute when enough); map Black/Cream; wire `putColour`. Prefer one chrome-grey pastille for cartouche+HELP if unifying.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` — Drop `ColourChart` cartouche dig; resolve via Skin/Look; keep badge text cut-out via `kHeaderPanel` background.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` — Drop ColourChart HELP + MIDI alert digs; remove dead transparent `kBodyPanel.withAlpha(0.0f)` band fill; leave error `.withAlpha(0.8f)` for story 5.
- `Source/GUI/MainComponent.cpp` — Verify shell fill uses structured surface keys; do not invent body-vs-header visual retune unless a gap is unthemed.
- `Source/GUI/Panels/MainComponent/BodyPanel/**` — Audit: already no ColourChart; keep `fillAll(kBodyPanel)`; ensure section/module headers use LookBuilders + Blue/Orange injection; fix residual structured digs that bypass existing Looks when a Look field already carries the colour (e.g. MasterOverrideBadge Orange line → section/module Look line; bank-marker focus tint may stay Skin key if no Look field).
- `Source/GUI/Looks/LookBuilders.{h,cpp}`, `WidgetLooks.h` — Reuse only; add panel Look structs only if chrome paint needs injected snapshots (prefer Skin keys for simple surface chrome).
- Leave alone: `LockDimmingFilm`, Dialogs/Settings/About, ModulationBusCell drag alpha, Footer error alpha, Tests/Dev.
- Quality: `Scripts/quality/lint_touched.py`; CMake only if new `.cpp`.

**Reuse:** Story 1 surface types + FooterMessage/DarkPanel maps; story 2 LookBuilders; `ColumnLayout` / `ColourVariant` accent injection.

**Do not change:** Typography; panel layout geometry; stories 4–5 breadth.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinValues.h` + `SkinColoursPanels.h` (+ related) + `Skin.cpp` -- Add/map Header/Footer chrome keys for cartouche, HELP, MIDI-alert badge; unify divergent chrome greys when SSOT-coherent -- CAP-2/3 chrome gaps.
- [x] `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` -- Remove ColourChart; paint cartouche from Skin/Look -- CAP-3/4 chrome.
- [x] `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- Remove ColourChart HELP/MIDI digs; drop dead alpha-0 band fill; keep error alpha for story 5 -- CAP-3/4 chrome.
- [x] `Source/GUI/MainComponent.cpp` + BodyPanel shell -- Confirm structured surface fills; no new local colour -- CAP-3.
- [x] `Source/GUI/Panels/MainComponent/BodyPanel/**` -- Audit Patch/Master/Shared; route residual accent digs through Looks/injection where Looks already exist; no ColourChart/hex; no patch/master object types -- CAP-4 panels.
- [x] Build + `Scripts/quality/lint_touched.py` -- Compile and lint green on touched C++ -- gate.

**Acceptance Criteria:**
- Given Header cartouche and Footer HELP/MIDI alert paints, when inspecting durable colours, then they resolve via Skin structured keys (no ColourChart / hex / `juce::Colours::*` in those call sites).
- Given Patch Edit, Master Edit, and Shared / Patch Manager panels, when inspecting durable paint, then colours come from Skin keys and/or injected Looks only (no local ColourChart/hex on that path).
- Given Blue vs Orange zone headers, when painted, then accents still come from LookBuilders / `ColourVariant` injection without `patch*` / `master*` object types.
- Given Black↔Cream toggle in Standalone, when viewing chrome + Patch Edit at minimum, then durable colours follow skin maps with good-enough parity (no half-unthemed chrome leftovers from this story).
- Given story 3 complete, when checking scope, then Dialogs/Settings/About, LockDimmingFilm, Footer error alpha, and ModulationBusCell drag tint were left for later stories.
- Given touched C++, when building macos-debug-arm64 and running `lint_touched.py`, then both succeed.

### Review Findings

- [x] [Review][Patch] Substrate chrome assert should pin `kMainChromeGrey` instead of a hard-coded Grey300 literal [`Tests/Unit/SkinColourSubstrateTests.cpp:317`]
- [x] [Review][Patch] Remove duplicate `#include <memory>` in RampPortamentoPanel header [`Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.h:9`]
- [x] [Review][Defer] Substrate tests do not observe Header/Footer/MasterOverrideBadge paint adoption [`Tests/Unit/SkinColourSubstrateTests.cpp:314`] — deferred: CONVENTIONS forbid GUI paint unit tests; same gap already tracked for story 5 gated scan / manual Standalone smoke (reconfirmed by external code review)

**Rejected**

- false — `RampPortamentoPanel::setSkin` name-hides non-virtual `BaseModulePanel::setSkin` so a `BaseModulePanel*` call would skip badge Look refresh — actual `propagateSkin` callers pass concrete `RampPortamentoPanel*`; template binds that type; badge refresh is reached on Black↔Cream.
- false — `kFooterMessageErrorBackground` living under `Panels` while FooterMessage text keys live under `Widgets` — story Code Map placed new chrome/alert map slots in `SkinColoursPanels.h`; intentional, not a misplaced key.
- false — Footer MIDI alert uses Error `kText` as badge fill and Error `kBackground` as cut-out — matches historical Red/Black paint arg order and sticky severity fill pattern; already dismissed in Build triage.
- rejected (edit-spec) — Implementation Notes still say `SectionHeaderLook::lineOrange` while code uses `ModuleHeaderLook` — fixing would only edit the story under review.
- rejected (edit-spec) — Manual Verification omits MIDI-queue alert / MasterOverrideBadge / HELP after Grey350→Grey300 unify — Grey350→Grey300 is an approved good-enough decision; expanding Verification is story-doc only.
- rejected (edit-spec / process) — `review_loop_iteration: 0` and empty Spec Change Log despite prior Build triage — process metadata, not a product defect.
- rejected (low) — `assertMainChromeKeys` also asserts ModuleHeader Look orange under a “mainChrome” name — cosmetic naming; everyday failure signal remains clear enough without a rename pass.

## Implementation Notes

- Added panel chrome maps: `kHeaderPanel`/`kFooterPanel` `kBorder` → unified `kGrey300`; `kFooterMessage` `kBackground` `kError` → `kBlack` (MIDI-alert badge cut-out). Wired in `Skin.cpp`.
- Header cartouche + Footer HELP paint via Skin border keys (no ColourChart). MIDI alert fill/text via FooterMessage Error text + Error background.
- Removed dead Footer band `withAlpha(0.0f)` fills; left sticky-error `.withAlpha(0.8f)` for story 5.
- MainComponent / BodyPanel columns already used structured `fillAll`; no ColourChart/hex under MainComponent panels after audit.
- MasterOverrideBadge Orange now reads `SectionHeaderLook::lineOrange` (LookBuilders injection); body cut-out stays `kBodyPanel` background. Bank-marker focus tints stay Skin NumberBox focus keys (no Look field).
- Substrate tests assert chrome keys + ModuleHeader Look orange parity. Verified: macos-debug-arm64 build, `lint_touched.py` OK, SkinColourSubstrate green.
- Review patches: shared `kMainChromeGrey` pastille; RampPortamentoPanel `setSkin` refreshes MasterOverrideBadge via `ModuleHeaderLook`; substrate asserts Header border == Footer border.

## Spec Change Log

## Review Triage Log

- false — MIDI alert uses FooterMessage Error text as badge fill and Error background as cut-out text — matches historical Red/Black paint arg order and sticky severity pattern (fill = severity colour); not an inverted runtime defect.
- false — Black and Cream map the same Grey300 chrome borders by design; Cream half-theming cannot appear on those new keys.
- false — Empty Spec Change Log / Triage before this pass was process timing, not a product defect.
- false — Task lines naming `SkinValues.h` / MainComponent “confirm” describe audit/reuse of existing `kBorder` property and pre-clean shell; reject fixing by editing the build spec.
- false — Footer Info Grey500 vs HELP Border Grey300 are distinct roles (detail text vs badge chrome); pre-existing product look, not a unify miss.
- false — SectionHeader vs ModuleHeader Orange pastilles are identical today; switched to ModuleHeaderLook in patch for hierarchy, not because colours diverged.
- low (patched) — duplicated Header/Footer Grey300 map pairs without shared pastille — fixed via `kMainChromeGrey`.
- low (patched) — redundant `WidgetLooks.h` include — removed.
- low (patched) — substrate missing Header==Footer border invariant — assert added.
- medium (patched) — MasterOverrideBadge Look/skin not refreshed on panel `setSkin` after Look cache — RampPortamentoPanel overrides `setSkin` and refreshes badge.
- defer — `buttonAlertLookFromSkin` still digs ColourChart Red/Black while Footer MIDI alert uses Skin keys — pre-existing LookBuilder hotspot; Header PANIC shares that Look.
- defer (verification-gap) — no automated observation of Header cartouche / Footer HELP / Footer MIDI alert / MasterOverrideBadge paint call sites — CONVENTIONS forbid GUI paint unit tests; map keys pinned; story 5 gated scan owns residual path checks.

## Design Notes

**Surface paint:** Widgets stay Look-only. Simple panel `fillAll` may use `Skin::getColour` on surface keys (already SSOT). Add dedicated panel Look structs only if chrome multi-slot paint benefits from one injected snapshot.

**Chrome greys:** Cartouche used `kGrey300`, HELP used `kGrey350`. Prefer one mapped chrome-grey pastille for both when unifying improves SSOT coherence (good-enough); do not chase pixel identity.

**Accents:** MasterOverrideBadge Orange dig is already structured (`kSectionHeader` / `kLineOrange`); prefer reading the same pastille via an injected Look field when one exists, without creating master-specific types.

**Deferred alpha:** Footer sticky-error `.withAlpha(0.8f)` and ModulationBusCell drag tint remain story 5 CAP-5 sweep.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`

**Manual checks (if no CLI):**
- Standalone: Black↔Cream on Header cartouche, Footer HELP (if enabled), body columns, Patch Edit section/module accents Blue; Master Edit Orange accents still correct.
