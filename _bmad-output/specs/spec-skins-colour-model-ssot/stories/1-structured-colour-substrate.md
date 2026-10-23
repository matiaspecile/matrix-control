---
title: 'Structured colour substrate — palette rename, keys, Black/Cream maps'
type: 'refactor'
created: '2026-10-21'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ab0d62c736f0824b88c1a0baa0edfda985cf0ae0'
context:
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/target-model.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/brownfield.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Black and Cream already ship, but durable colours still hang on a flat `SkinColourId` enum, role-leaking palette names, and leftover local hex. That model cannot host a future authoring editor or a clean November gate.

**Approach:** On branch `feature/skins-colour-model-ssot`, make `(ObjectType, Property, State) → palette` the mapping SSOT: rename `ColourChart` to shade/absolute `k*` names (same hex), rebuild Black/Cream tables on structured keys, and resolve Looks through that API. Add keys/pastilles for known gaps; stop at this substrate checkpoint (stories 2–5 later).

## Boundaries & Constraints

**Always:**
- Structured `(ObjectType, Property, State)` is the mapping authority; flat `SkinColourId` must not remain SSOT (derived/internal bridge OK only if temporary and non-authoritative).
- Palette: `k` prefix; shades like `kEmerald950` / `kGrey900` / `kRed500`; absolutes `kTransparent` / `kBlack` / `kWhite` / `kCream`; Matrix-Control hex only; no UI-role names in palette.
- `buttonLike` (and parallel looks) = separate object type; Patch/Master Blue/Orange accents = LookBuilders/orchestrator injection only — no `patch*` / `master*` object types.
- Wire `Skin` + `LookBuilders` so existing Looks resolve structured keys; UI may keep compiling via Looks.
- Add palette entries and map keys for: dialog border/title, durable close cross, ReadOnlyValueField fill/text, PatchNameDisplay secondary mutator solid (replace LookBuilder `withAlpha(0.50f)`).
- English-only source; `lint_touched.py` clean on touched C++; compile with project preset.
- Stop after story 1 validation (`done_checkpoint`); do not start stories 2–5.

**Never:**
- Migrate all panels / full V1 GUI off local colour (stories 2–5).
- Author editor, IPC, Skin Studio Lot C, Duo/Monochrome skins, Tailwind hex borrow.
- Skin tokens for lock dimming film or close-cross hover pale.
- Require zero colour under Tests / Dev tools for this story.
- Commit / push / PR unless Guillaume asks.
- Invent product typography (Orbitron ≠ PatchNameDisplay; PatchName stays PT Sans Narrow + chart red / `kRed500`).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Resolve known Look slot | Active Black or Cream skin; LookBuilder requests e.g. button background off | Returns palette `juce::Colour` from structured map for that skin | Missing key → fail loudly in Debug (jassert / logged); no silent white-as-SSOT for new API |
| Switch skin variant | Same structured key after Black→Cream (or reverse) | Colour comes from that skin’s map column only | N/A |
| Secondary mutator text | `patchNameDisplayLookFromSkin` | `secondaryText` is a solid mapped colour (no `withAlpha` shortcut) | N/A |
| Gap keys present | Maps queried for dialog border/title, close cross, read-only field, secondary text | Keys exist for Black and Cream → palette entries | Call sites may still use local hex until stories 2/4 — keys ready, not all wired |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/ColourChart.h` — rename all entries to shade/absolute names; keep hex; retire `kContextualHelpChrome` → grey shade; add pastilles for dialog border (`0xff5E5E5E`) and close cross (`0xff9A131D`) if not already representable.
- `Source/GUI/Skins/SkinValues.h` — replace flat `SkinColourId` authority with ObjectType / Property / State (and any thin key type); do not leave flat enum as SSOT.
- `Source/GUI/Skins/SkinColoursCommon.h` + `SkinColours.h` + `SkinColoursPanels.h` / `SkinColoursWidgetsLayout.h` / `SkinColoursWidgetsControls.h` / `SkinColoursWidgetsSelection.h` / `SkinColoursWidgetsDisplays.h` — rebuild tables as structured key → black/cream palette refs; extend for gap keys + secondary mutator.
- `Source/GUI/Skins/Skin.{h,cpp}`, `ISkin.h`, `ISkinColours.h`, `SkinHelpers.h` — load maps; `getColour(object, property, state)`; migrate state helpers off flat ids.
- `Source/GUI/Looks/LookBuilders.{h,cpp}`, `WidgetLooks.h` — resolve Looks via structured getters; PatchNameDisplay secondary solid from map.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` — mechanical chart rename for former `kContextualHelpChrome` (no new role name).
- Gap call sites (keys only this story; paint migrate later): `DialogMatrixHelpers.*`, `SettingsWindow.cpp`, `AboutWindow.cpp`, `ReadOnlyValueField.h`.
- Leave alone: `LockDimmingFilm.cpp` (code-owned); Tests / Dev colour literals; CMake unless new `.cpp` added.
- Quality: `Scripts/quality/lint_touched.py`; `CMakeLists.txt` `PLUGIN_SOURCES` if needed.

**Reuse:** existing `ColourElement` black/cream columns and LookBuilder injection shape; Blue/Orange section lines stay injection.

**Do not change:** typography (`TypographyStyles`, Orbitron brand-only); panel layout; stories 2–5 migration breadth.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/ColourChart.h` (+ FooterPanel chart call) -- Rename palette to locked shade/absolute names; same hex; add missing pastilles for dialog border and close-cross red; remove role names -- CAP-1 substrate.
- [x] `Source/GUI/Skins/SkinValues.h` (+ new key headers if clearer) -- Introduce ObjectType / Property / State as mapping SSOT; demote or remove flat `SkinColourId` authority -- CAP-2.
- [x] `Source/GUI/Skins/SkinColours*.h` -- Rewrite Black/Cream maps as structured key → palette; cover today’s slots + gap keys + secondary mutator solid -- CAP-2/CAP-3.
- [x] `Source/GUI/Skins/Skin.{h,cpp}`, `ISkinColours.h` -- Implement structured `getColour`; adapt helpers; maps load from new tables -- CAP-3 runtime.
- [x] `Source/GUI/Looks/LookBuilders.cpp` -- Resolve all existing Look builders through structured keys; secondary mutator solid from map (no `withAlpha(0.50f)`) -- CAP-4 injection path for substrate.
- [x] Build + `Scripts/quality/lint_touched.py` on touched C++ -- Compile and lint green; halt at story 1 checkpoint -- done_checkpoint.

**Acceptance Criteria:**
- Given the dedicated branch after story 1, when reviewing the colour model, then `(ObjectType, Property, State)` maps are the SSOT and flat `SkinColourId` is not the authority.
- Given Black or Cream active, when LookBuilders build Looks for existing widgets, then colours resolve via structured Skin getters from that skin’s map.
- Given PatchNameDisplay Look build, when secondary text is set, then it uses a solid mapped colour (not primary×0.5 alpha).
- Given the substrate maps, when inspecting keys for dialog border/title, close cross, ReadOnlyValueField, and secondary mutator, then Black and Cream entries exist pointing at named palette colours.
- Given touched C++ sources, when building the macOS debug preset and running `lint_touched.py`, then both succeed.
- Given story 1 complete, when checking scope, then panels/dialogs are not fully migrated and stories 2–5 were not started.

### Review Findings

- [x] [Review][Patch] Substrate tests leave Look↔Skin and map invariants under-pinned — LookBuilder/NumberBox asserts are absolute-only (bypass Skin still green); FooterMessage Info/Warning/Error and DarkPanel Cream text have no Skin asserts; standard Slider emerald Skin load is Black-only [`Tests/Unit/SkinColourSubstrateTests.cpp`, `NumberBoxDisabledBackgroundColourTests.cpp`, `SliderButtonLikeColourTests.cpp`]
- [x] [Review][Patch] English-only comment slip: `SkinColoursCommon.h` still says French “fond” [`Source/GUI/Skins/SkinColoursCommon.h:20`]
- [x] [Review][Patch] Stale palette role name in comment: HeaderPanel still says “LightGrey1” after rename to `kGrey300` [`Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp:49`]
- [x] [Review][Defer] Companion `code-map.md` Token ids row still lists flat `SkinColourId` — deferred: companion doc drift; update when refreshing navigation map for stories 2+ (not a runtime defect)

Rejected:
- Gap paints still local hex while maps hold keys — false: intentional story 1 checkpoint (keys ready, paints in stories 2/4).
- Miss DBG lacks object/property/state — low reject: only hits broken maps; not everyday.
- No full-table map walk in tests — reject: CAP-6 / story 5 scan territory; excess for substrate checkpoint.
- `setColour` lacks triple overload — low reject: unused on product path.
- Default `SkinColourKey` lands on button background — low reject: zero-init path not used by maps/helpers.
- Blue/Orange line colours in maps contradict “injection only” — false: no `patch*`/`master*` types; LookBuilders still inject into Looks from structured Section/Module keys; alert stays ColourChart injection.
- Public `Hoover` helper spelling vs map `Hover` — low reject: pre-existing public API; rename churn out of scope.
- Story Verification Commands omit unit tests / metadata `review_loop_iteration: 0` / Implementation Notes “half-true” — reject: fix would edit the spec under review; notes match mechanical flat→structured call-site rewires with gap hex left local.
- ReadOnly error red unmapped — false: story gap keys are fill/text only.
- Uneven `kGrey525`/`kGrey350` stops — low reject: intentional pastilles for locked hex; not everyday harm.
- FooterMessage chrome reuses Info text state — low reject: remapped correctly; vocabulary polish not required for story 1.

## Implementation Notes

- Palette renamed: `kEmerald*`, `kGrey*`, `kRed500`/`kRed800`/`kRed900`, `kGrey525` (dialog border), `kGrey350` (ex-help chrome). Flat `SkinColourId` removed entirely.
- Maps use `ColourElement { SkinColourKey, black, cream }`; Skin stores `std::map<SkinColourKey, Colour>`; miss → `jassertfalse` + DBG + white fallback.
- LookBuilders on structured getters; PatchNameDisplay secondary = `kRed800` solid (both skins).
- Gap keys in maps; DialogMatrixHelpers / Settings/About close / ReadOnlyValueField paints still local hex (stories 2–4).
- Added `Tests/Unit/SkinColourSubstrateTests.cpp` for matrix coverage (Look resolve both skins, secondary solid, gap keys).
- Verified: macos-debug-arm64 build, lint_touched OK, SkinColourSubstrate + SliderButtonLike + NumberBox colour tests pass.
- Review patches: dropped unused SkinColourState usings; hardened substrate/NumberBox/Slider tests (absolute chart, Cream divergence, Skin map load).
- Code-review patches: Look↔Skin asserts (button/NumberBox); FooterMessage + DarkPanel Skin asserts; Cream emerald slider Skin load; English “fill” comment; HeaderPanel `kGrey300` comment.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| BH: unused `using SkinColourState` in ~17 TUs | low → patch | Confirmed: many files import the using but never reference `SkinColourState::`. Dead usings from mechanical migration. |
| BH: miss path returns white after jassert | false | Frozen I/O allows jassert/logged loud fail in Debug; white is Release fallback, not map SSOT. |
| BH: DBG miss lacks (object,property,state) | low → reject | Real but only hits broken maps; enriching DBG is nice-to-have, not everyday. |
| BH: ReadOnlyValueField error red unmapped | false | Story intent lists fill/text gap keys only; error role deferred with call-site migration. |
| BH: device fields still use ColourChart literals | false | Design Notes: gap paints stay local until stories 2/4; chart rename only. |
| BH: Footer/overlay/modbus alphas remain | false | Story 1 locked secondary solid only; CAP-5 opaque sweep is story 5. |
| BH: close-cross / dialog border still local hex | false | Intentional: keys in maps, paints in stories 2/4. |
| BH: `buttonAlertLookFromSkin` uses ColourChart | false | Alert chrome is injection (same class as zone accents), not a map SSOT slot. |
| BH: ModuleHeader focus borrows NumberBox text | defer | Pre-existing cross-object borrow preserved; not introduced as new SSOT debt this story. |
| BH: tests miss Cream divergence / full map | medium → patch | Merged with VG Cream + circular Look asserts (same root: thin substrate tests). |
| BH: `setColour` lacks triple overload | low → reject | Asymmetry is real but unused in product path; no everyday harm. |
| BH: `ISkin.h` / `SkinHelpers.h` untouched | false | Neither file referenced flat `SkinColourId`; colour API lives on `ISkinColours`. |
| BH: Hoover helper names vs Hover Common rename | low → reject | Public helper spelling pre-dates story; renaming public API is non-trivial churn. |
| BH: FooterPanel `.withAlpha(0.0f)` dead paint | defer | Pre-existing no-op path; not caused by colour SSOT rewrite. |
| EC: (none) | — | Empty finding set. |
| VG: LookBuilder assert Look↔getColour circular | medium → patch | Pre-verified: need absolute ColourChart expects on representative Looks. |
| VG: NumberBox/Slider static tables skip Skin load | medium → patch | Pre-verified: add Skin-resolved asserts for disabled NumberBox + enabled green slider. |
| VG: Cream-only columns never asserted | medium → patch | Pre-verified: assert body `kCream` vs Black `kGrey900` (+ one cream text/border). |

## Design Notes

**Encoding (agent choice):** Prefer three `enum class` families (`SkinObjectType`, `SkinColourProperty`, `SkinColourState`) plus a compile-time or init-time map keyed by that triple (or a packed internal id derived from the triple). Public Skin API must take the structured triple (or a named structured key type). Do not keep a hand-maintained flat `SkinColourId` list as the place humans add colours.

**Palette rename map (hex unchanged):**
- Greens `kGreen1…4` → `kEmerald950`, `kEmerald900`, `kEmerald700`, `kEmerald400`
- `kRed` → `kRed500`; keep `kOrange` / `kYellow` / `kBlue` as family accents (shade suffix only if already multi-stop)
- Dark/light greys → homogeneous `kGrey950`…`kGrey100` stops we actually use
- `kContextualHelpChrome` → grey shade only (same `0xFF888888`)
- Absolutes unchanged names: `kTransparent`, `kBlack`, `kWhite`, `kCream`
- New pastilles for hex absent from chart: dialog border `0xff5E5E5E`, close cross `0xff9A131D` (shade names, not role names)

**Secondary solid:** For each skin, bake a solid that matches today’s appearance (primary text over display background at ~50% opacity) into the map; do not leave runtime `withAlpha`.

**Gap wiring:** Story 1 adds keys + pastilles and uses secondary in LookBuilders. Do not rewrite DialogMatrixHelpers / Settings / About / ReadOnlyValueField paints here — stories 2 and 4 own those call sites.

**buttonLike:** Separate object type rows in maps (mirror today’s `*ButtonLike*` slots).

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched `Source/**/*.h` / `.cpp`

**Manual checks (if no CLI):**
- Grep: no remaining role palette name `kContextualHelpChrome`; no `SkinColourId` used as the place new durable colours are defined.
- Spot-check Black↔Cream in Standalone: existing Looks still paint (parity good-enough); secondary mutator line still readable without looking “half transparent hack”.
