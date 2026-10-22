---
title: 'Shared Looks and control/display widgets off local colour'
type: 'refactor'
created: '2026-10-22'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'f1f7556884ddae9631cf5bdebfc31c6ab572bb08'
context:
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/target-model.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/brownfield.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/1-structured-colour-substrate.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Story 1 left a structured colour substrate, but shared control/display widgets still keep durable local hex or borrowed Looks (ReadOnlyValueField defaults, LED max via Slider text, Logo hover White). Black↔Cream cannot recolour those slots cleanly.

**Approach:** On `feature/skins-colour-model-ssot`, finish CAP-4 for gated shared Looks/widgets and the CAP-5 widget layer (LED max pastille + already-mapped secondary solid). Wire remaining widget locals through Skin keys / LookBuilders; leave chrome panels, dialogs, and the broad alpha-as-darken sweep to stories 3–5.

## Boundaries & Constraints

**Always:**
- Structured `(ObjectType, Property, State)` remains mapping SSOT; Looks resolve via Skin getters (typed helpers OK if they wrap structured keys).
- Gated shared control/display widgets paint durable colours from Looks / Skin only — no lasting `0xAARRGGBB` defaults or `juce::Colours::*` in those widgets.
- LED / PeakIndicator: max-on colour from Skin map (dedicated `kLed` object); LED runtime level modulates alpha in paint; PeakIndicator level tracks fill height (solid max colour).
- PatchNameDisplay secondary mutator stays solid mapped (`kSecondaryText` / `kRed800`); do not reintroduce LookBuilder `withAlpha` for secondary.
- Primary PatchNameDisplay idle/hover alpha stays paint-owned.
- English-only source; `lint_touched.py` clean on touched C++; compile with project preset.
- Good-enough Black/Cream parity on touched widgets only.

**Never:**
- Stories 3–5 scope: Header/Footer/body chrome, Patch/Master/Shared edit panels, Dialogs/wizards, Settings/About close chrome.
- Broad CAP-5 opaque alpha-as-darken sweep (ModulationBusCell drag tint, popup disabled text alpha, Footer overlays) — story 5.
- Touch `LockDimmingFilm` or Settings/About close-cross hover pale.
- Author editor, IPC, Skin Studio, Duo/Monochrome skins, Tailwind hex.
- Require zero colour under `Source/GUI/Tests/**` / `Tests/`.
- Commit / push / PR unless Guillaume asks.
- Start stories 3–5 unless explicitly requested.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| ReadOnlyValueField default | Constructed with Skin; no `setColours` | Fill/text from `kReadOnlyValueField` map (Grey700/Grey200) | Missing key → existing Skin miss path |
| ReadOnlyValueField override | Caller `setColours` (e.g. error red) | Override wins until next defaulting path | N/A |
| LED at level L | Skin set; level in 0…1 | Max colour from `kLed` map; paint uses `max.withAlpha(L)` | level 0 → off plate only |
| PeakIndicator at level L | Skin set; level in 0…1 | Max colour from `kLed` map solid; fill height tracks L (not alpha) | level 0 → off plate only |
| Skin Black→Cream | Same widgets after switch | Colours come from Cream map columns for migrated slots | N/A |
| PatchName secondary | Mutator secondary line | Solid `secondaryText` from Look (no LookBuilder alpha) | N/A |
| PatchName primary idle | Non-edit primary line | Paint-owned alpha on Look text unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Looks/LookBuilders.{h,cpp}`, `WidgetLooks.h` — Add `ReadOnlyValueFieldLook` / `LedLook` (or equivalent) builders from structured keys; keep PatchName secondary solid; Logo hover via Skin state (no raw `ColourChart::kWhite`).
- `Source/GUI/Skins/SkinValues.h`, `SkinColoursWidgetsDisplays.h` (+ Controls if needed), `Skin.cpp` — Add `SkinObjectType::kLed` (+ keys: off background, border, max fill); ensure ReadOnly keys already present stay wired; optional PatchName caret ink / DarkPanel text hover if missing.
- `Source/GUI/Widgets/ReadOnlyValueField.h` — Drop hex defaults; load fill/text from Skin (Look or `getColour`) in ctor/`setSkin`; keep `setColours` override.
- `Source/GUI/Widgets/Led.{h,cpp}`, `PeakIndicator.{h,cpp}` — Stop borrowing Button+Slider Looks for durable colours; use Led Look / Skin max pastille; LED alpha dim in paint; Peak height fill.
- `Source/GUI/Widgets/Logo.cpp` — Hover text from Skin/Look (mapped White), not `ColourChart::kWhite`.
- `Source/GUI/Widgets/PatchNameDisplayPaint.cpp` — Caret ink via Look/Skin (palette absolute), not ad-hoc chart dig; leave primary idle/hover alpha.
- Already clean (verify only): Button, Toggle, Slider, ComboBox paint, NumberBox paint, Label/headers, EnvelopeDisplay, TrackGeneratorDisplay, PatchName chrome+secondary.
- Leave alone this story: `ModulationBusCell.cpp` drag alpha, `PopupMenuRenderer` disabled alpha, `HeaderLogoPopupMenu` section alpha, Header/Footer/BodyPanel, Dialogs/Settings/About, `LockDimmingFilm`, transparent editor clears, Tests/Dev.
- Quality: `Scripts/quality/lint_touched.py`; CMake only if new `.cpp`.

**Reuse:** Story 1 maps, `kReadOnlyValueField` keys, PatchName `kSecondaryText` → `kRed800`, LookBuilder injection pattern.

**Do not change:** Typography (Orbitron ≠ PatchName); panel layout; stories 3–5 breadth.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinValues.h` + `SkinColoursWidgetsDisplays.h` + `Skin.cpp` -- Add `kLed` object and Black/Cream keys for off plate, border, max-on fill (match today’s emerald max); wire any missing Logo hover / caret ink keys -- CAP-4/5 substrate extension.
- [x] `Source/GUI/Looks/WidgetLooks.h` + `LookBuilders.{h,cpp}` -- Add Look structs/builders for ReadOnlyValueField and Led; route Logo hover and PatchName caret through Skin/Looks; confirm secondary solid unchanged -- CAP-4 injection.
- [x] `Source/GUI/Widgets/ReadOnlyValueField.h` -- Remove local hex defaults; initialise from Skin/Look -- CAP-4 widget.
- [x] `Source/GUI/Widgets/Led.{h,cpp}` + `PeakIndicator.{h,cpp}` -- Paint max colour from Led Look/Skin; LED keeps runtime alpha dim; Peak keeps height fill -- CAP-5 LED policy.
- [x] `Source/GUI/Widgets/Logo.cpp` + `PatchNameDisplayPaint.cpp` -- Drop remaining durable ColourChart digs in scope -- CAP-4 inventory.
- [x] Build + `Scripts/quality/lint_touched.py` -- Compile and lint green on touched C++ -- gate.

**Acceptance Criteria:**
- Given gated shared control/display widgets after story 2, when inspecting durable paint, then colours resolve via Looks/Skin structured keys (no lasting hex defaults / `juce::Colours::*` in those widgets).
- Given ReadOnlyValueField without `setColours`, when painted under Black or Cream, then fill/text match `kReadOnlyValueField` map entries.
- Given Led at full level, when painted, then the lit colour is the Skin-mapped max pastille; partial levels only change alpha in paint.
- Given PeakIndicator at a partial level, when painted, then the lit colour is the solid Skin-mapped max pastille and fill height tracks the level (not alpha).
- Given PatchNameDisplay Look build, when secondary text is used, then it remains solid mapped (no LookBuilder `withAlpha` for secondary).
- Given primary PatchName idle/hover, when painted, then paint-owned alpha behaviour is unchanged.
- Given story 2 complete, when checking scope, then Header/Footer/edit panels/Dialogs/Settings/About and deferred alpha hotspots were not migrated; LockDimmingFilm untouched.
- Given touched C++, when building macos-debug-arm64 and running `lint_touched.py`, then both succeed.

## Implementation Notes

- Added `SkinObjectType::kLed` + `SkinColourProperty::kCaret`; DarkPanel text Hover → White; PatchName caret → Black; Led off/border/max-on (Button-off + Slider emerald).
- LookBuilders: `readOnlyValueFieldLookFromSkin`, `ledLookFromSkin`; PatchName Look carries `caretInk`; secondary remains solid `kRed800`.
- ReadOnlyValueField loads fill/text from Skin on ctor/`setSkin`; `setColours` override kept.
- Led uses `maxOn.withAlpha(level)`; PeakIndicator uses solid `maxOn` with height fill (unchanged ballistics).
- Logo hover via DarkPanel Text Hover (no ColourChart dig); PatchName caret ink via Look.
- Verified: macos-debug-arm64 build, `lint_touched.py` OK, SkinColourSubstrate tests green (incl. paint coverage for Led/Peak/Logo/ReadOnly setSkin).
- Human decision 2026-10-22: PeakIndicator keeps height fill (not LED-style paint alpha); frozen matrix/AC split accordingly.
- Review patches: ReadOnly defaults via `getColour` (no LookBuilders include); paint tests for Led/Peak/Logo/setSkin; test target links Label/Led/Logo/PeakIndicator.

## Spec Change Log

- 2026-10-22 — Human chose PeakIndicator height ballistics (option 1). Split frozen I/O matrix LED vs Peak; AC updated so Peak does not require paint alpha.

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| BH: story status still in-progress with tasks done | false | Spec already advanced to `in-review` then `done` after verification; stale diff snapshot. |
| BH: frozen matrix/AC omit Logo hover and caret rows | false | Reject: fix would only edit this build's spec; Intent/tasks already cover both slots. |
| BH: setSkin wipes setColours / contract unspecified | false | Frozen matrix: override wins until next defaulting path; Design Notes document reload on `setSkin`. Test gap handled as VG setSkin patch. |
| BH: PeakIndicator no paint/geometry assert | medium → patch | Merged with VG Led/Peak paint gap; paint asserts added. |
| BH: ReadOnly Cream paint unchecked | false | Black/Cream map columns identical (Grey700/Grey200); Cream paint would not diverge. |
| BH: Logo hover ad-hoc getColour vs caret Look field | low → reject | Uneven pattern is real but everyday harm low; Logo consumer assert added instead of new Look field. |
| BH: ReadOnlyValueField includes LookBuilders.h | low → patch | Switched defaults to `ISkin::getColour`; LookBuilders include removed. |
| BH: Peak vs Led off-plate bounds diverge | defer | Pre-existing geometry; colour story did not introduce it. Logged in deferred-work. |
| BH: Verification Commands omit unit-test run | false | Reject: fix would only edit this build's spec; tests ran in verification. |
| BH: Manual Black↔Cream spot-check on invariant pastilles | false | Reject: edit-spec-only; pastilles intentionally skin-invariant absolutes. |
| EC: (none) | — | Empty finding set. |
| VG: Led/Peak level encoding only Colour::withAlpha | medium → patch | Pre-verified; paint asserts on Led/Peak after setSkin + partial level. |
| VG: Logo hover no consumer assert | medium → patch | Pre-verified; Logo setHighlighted paint/text assert vs DarkPanel Hover. |
| VG: PatchName caret paint unchecked | defer | Look↔Skin mapped; one-field pass-through. Logged in deferred-work. |
| VG: ReadOnly setSkin re-default untested | medium → patch | Pre-verified; setColours → setSkin → paint restores map default. |
| CR2 BH: Led paint-test `friend` for partial level | decision → keep | Human 2026-10-22 option 1: accept `friend` seam; do not add product API or timer-driven test. |
| CR2 VG: ReadOnly text ink unasserted in paint | medium → patch | Pre-verified; fill-only assert with empty text_; extend setText + sample text ink. |

## Design Notes

**LED encoding:** New `SkinObjectType::kLed` (shared by PeakIndicator). Map off background + border to today’s Button off colours; max-on fill to today’s Slider `textEnabled` emerald (same Black/Cream columns). Led paint keeps `max.withAlpha(level)`; PeakIndicator keeps solid `max` with height fill. Do not invent a second parallel LED colour system.

**ReadOnlyValueField:** Constructor / `setSkin` loads Background + Text from Skin. `setColours` remains for caller overrides (error red, dialog-specific). Migrating Settings/Dialog call sites that still pass `ColourChart` literals is story 4; widget defaults must not stay local hex.

**Defer (story 5 CAP-5 sweep):** ModulationBusCell drag Blue/Red `withAlpha(0.75f)`, PopupMenu disabled `withAlpha(0.5f)`, HeaderLogoPopupMenu section-header alpha. Not “local hex defaults”; policy sweep belongs with gate scan.

**Already done (story 1):** LookBuilders structured resolve for existing Looks; PatchName secondary solid. Story 2 verifies and does not reopen.

**Caret / Logo:** Prefer Skin-mapped absolute pastilles (`kBlack`, `kWhite`) into Look fields over widget-level `ColourChart::` digs so skin switch stays one hop.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched `Source/**/*.h` / `.cpp`

**Manual checks (if no CLI):**
- Grep Widgets for lasting `0xff` defaults and durable `ColourChart::` digs outside deferred list / transparent clears.
- Spot-check Black↔Cream: ReadOnlyValueField defaults, LED at full, Logo hover, PatchName secondary still readable solid.

### Review Findings

- [x] [Review][Decision] Accept Led paint-test `friend` seam for partial level, or replace it? — Resolved 2026-10-22: keep `friend` (option 1). Public `setLevel` snaps any positive input to full on; substrate paint test may write `displayedLevel_` via `friend` to pin alpha encoding without timer.
- [x] [Review][Patch] Assert ReadOnlyValueField text ink after Skin default and after setSkin reset [Tests/Unit/SkinColourSubstrateTests.cpp:265] — Fixed 2026-10-22: `setText` + assert `textColour_` (via test `friend`) after construct, setColours override, and setSkin reset to mapped Grey200.
- [x] [Review][Defer] PatchName caret paint path has no pixel assert [Source/GUI/Widgets/PatchNameDisplayPaint.cpp:177] — deferred: already logged in deferred-work.md; Look↔Skin mapping pinned; paint is one-field pass-through.

#### Rejected

- BH: unused `ReadOnlyValueFieldLook` in production widget — low; intentional post-review `getColour` path; Skin map remains SSOT; deleting or wiring Look is design churn without everyday harm.
- BH: frozen matrix/AC omit Logo hover and caret rows — false; Intent/tasks cover both; fix would only edit this spec.
- BH: Verification Commands omit unit-test run — false; tests ran in verification; fix would only edit this spec.
- BH: Logo hover ad-hoc `getColour` vs caret Look field — low; prior triage; Logo consumer assert already added.
- BH: `setSkin` wipes `setColours` / contract unspecified — false; frozen matrix + Design Notes define override until next defaulting path.
- BH: CMake test target adds `Label.cpp` unexplained — false; required to link `Logo`→`Label`; noted in Implementation Notes.
- BH: ReadOnly/Led/Peak/Logo paint asserts only Black not Cream — false; Black/Cream columns identical for these pastilles.
- BH: `review_loop_iteration: 0` with status done — false; metadata-only; fix would only edit this spec.
- EC: caret ink invisible if `look_.caretInk` unset — false; ctor requires Look; production/LookBuilder always fill `caretInk`.
- EC: `paintPixel` zero-sized / OOB — low; helper only called with sized bounds and in-range coords; guards add test complexity for unreachable paths.
