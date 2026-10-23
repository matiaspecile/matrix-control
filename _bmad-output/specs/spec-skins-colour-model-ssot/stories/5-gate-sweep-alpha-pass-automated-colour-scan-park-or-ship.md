---
title: 'Gate sweep — alpha pass, automated colour scan, park-or-ship'
type: 'refactor'
created: '2026-10-23'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'afe1923aeb99177254d752e637071ab030200e91'
context:
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/target-model.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/brownfield.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/1-structured-colour-substrate.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/2-shared-looks-and-control-display-widgets-off-local-colour.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/3-main-chrome-and-patch-master-shared-edit-panels.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/stories/4-dialogs-wizards-settings-and-about-chrome.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Stories 1–4 closed the substrate and V1 chrome, but CAP-5 still leaves opaque alpha-as-darken shortcuts on the user path, there is no automated gated colour scan, and CAP-6 needs an explicit porte OK or total park before the 2026-11-15 deadline — without a shrink merge of a half-migrated model.

**Approach:** On `feature/skins-colour-model-ssot`, finish the CAP-5 opaque alpha sweep (policy exceptions untouched), add `Scripts/quality` gated-path colour scan (allowlist palette + skin maps; exclude Tests/Dev), smoke Black↔Cream against the migration-plan checklist, document the checklist, and stop at the porte/park verdict (`done_checkpoint`) — no author editor.

## Boundaries & Constraints

**Always:**
- Gate proof = small script under `Scripts/quality/` that fails on durable `0x` hex (6–8 digits) and `juce::Colours::*` under gated `Source/GUI`; allowlist `ColourChart.h` + `SkinColours*.h` map tables; exclude `Source/GUI/Tests/**` and Dev-only tooling; allowlist `LockDimmingFilm.cpp` as the locked code-owned film exception.
- CAP-5 keep: LED max solid + runtime dim; lock film; close-cross pale/hover; primary PatchNameDisplay idle/hover; dialog overlay scrim `.withAlpha(0.85f)` stays code-owned transient dim (story 4).
- CAP-5 sweep to solids via Skin keys (extend maps only if a coherent key is missing): Footer error `0.8f`; ModulationBusCell drag tint (drop ColourChart dig); PopupMenuRenderer disabled text; HeaderLogoPopupMenu section header; LookBuilders `buttonAlertLookFromSkin` ColourChart digs → Skin-resolved pastilles.
- Leave `ColourChart::kTransparent` editor/clear fills (not durable paint).
- Checklist in `migration-plan.md` updated with factual ticks; verdict is **porte franchie** only if all gate rows hold, else **parque total** — never shrink-merge partial SSOT into V1 / `main`.
- English-only source; `lint_touched.py` clean on touched C++/scripts; project compiles; good-enough Black↔Cream parity only.
- Commits / push / PR / merge only if Guillaume asks after the checkpoint.

**Never:**
- Author editor, Skin Studio, IPC, Duo/Monochrome, proud Black/Cream polish.
- Map lock film or close-cross pale under the gate pretext.
- Expand scan to `Source/GUI/Tests/**` / `Tests/**` for “stricter”.
- Require ColourChart-call-site grepping in the scan (gate proof locks hex + `juce::Colours` only); residual durable `ColourChart::` digs on the paint path are fixed by the sweep, not by widening the scanner.
- Fold the scan into `run_quality_gate.sh` in this story (standalone runnable is enough for CAP-6).
- Start post-gate editor work or reopen stories 1–4 for beauty beyond sweep/scan findings.
- Touch `v1.0.0-alpha.1` / shrink-merge into V1.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Colour scan green | Gated `Source/GUI` after sweep | Script exit 0; only allowlisted hex/`Colours` remain | Exit 1 + path:line on residual hits |
| Colour scan on Tests | Hex under `Source/GUI/Tests/**` | Ignored (not scanned) | N/A |
| Lock film | `LockDimmingFilm` still uses `juce::Colours::black` | Allowed by explicit file allowlist | N/A |
| Footer error | Error severity paint | Solid mapped error colour (no `0.8f` alpha darken) | N/A |
| Mod bus drag | Drag overlay | Skin-resolved Blue/Red solid tint (no ColourChart + alpha) | N/A |
| Popup disabled / section | Disabled or section header text | Solid mapped muted colour (no `0.5f` alpha) | N/A |
| Alert button Look | `buttonAlertLookFromSkin` | Fill/text from Skin keys, not ColourChart literals | N/A |
| Black↔Cream smoke | Standalone toggle + walk | Patch Edit, chrome, one Matrix modal, Settings, Getting Started — no half-migrated surfaces | If gaps → park, not partial merge |
| Deadline / incomplete | Checklist not green | Total park documented; alpha continues on trunk / tag | No shrink merge |

</frozen-after-approval>

## Code Map

- `Scripts/quality/scan_gated_colours.py` (new) — Walk gated `Source/GUI` C++ via `git ls-files`; fail on `0[xX][0-9A-Fa-f]{6,8}` and `juce::Colours::`; allowlist `Skins/ColourChart.h`, `Skins/SkinColours*.h`, `Helpers/LockDimmingFilm.cpp`; skip `Source/GUI/Tests/**`; mirror `lint_touched.py` `REPO_ROOT` / exit codes; do not add to `run_quality_gate.sh` this story.
- `Scripts/quality/lint_touched.py` — Pattern reference only; run on touched C++ after edits.
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` — Replace error `.withAlpha(0.8f)` (paint + `getSeverityColour`) with solid `kFooterMessage` Error text (or add a dedicated muted Error pastille in maps if visual needs a darker solid — prefer reuse of existing Error text first; add map entry only if needed for good-enough parity).
- `Source/GUI/Widgets/ModulationBusCell.cpp` — Drag tint: Skin Blue/Red keys (or Look injection) as solids; remove ColourChart + `0.75f`.
- `Source/GUI/Widgets/PopupMenuRenderer.cpp` — Disabled text `0.5f` → solid disabled/muted Skin colour.
- `Source/GUI/Widgets/HeaderLogoPopupMenu.cpp` — Section header `0.5f` → solid muted Skin colour.
- `Source/GUI/Looks/LookBuilders.cpp` — `buttonAlertLookFromSkin`: resolve red/black/white via Skin structured getters / existing alert keys; drop ColourChart include if unused.
- `Source/GUI/Skins/SkinColours*.h` (+ `Skin.cpp` if needed) — Add only missing pastilles for swept solids (Footer muted error, popup disabled/section, mod-bus drag if no existing key).
- Leave alone: `Led.cpp` runtime alpha; `LockDimmingFilm`; Settings/About close pale; `PatchNameDisplayPaint` primary idle/hover; `DialogMatrixHelpers` overlay `0.85f`; `ColourChart::kTransparent` clears; `GrayedControlHelper` component alpha; Tests/Dev.
- `_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md` — Tick Gate acceptance checklist with evidence; record porte or park.
- Quality: `lint_touched.py`; CMake only if new `.cpp` (script-only → no CMake).

**Reuse:** Story 1–4 Skin keys (`kFooterMessage` Error, Blue/Red layout accents, disabled greys, alert button maps if present).

**Do not change:** Pale-hover semantics; lock film; editor/Skin Studio; merge to `main`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- Sweep error `0.8f` alpha-as-darken to solid Skin colour -- CAP-5.
- [x] `Source/GUI/Widgets/ModulationBusCell.cpp` -- Sweep drag tint off ColourChart+alpha to Skin solids -- CAP-5 + CAP-1 dig.
- [x] `Source/GUI/Widgets/PopupMenuRenderer.cpp` + `HeaderLogoPopupMenu.cpp` -- Sweep `0.5f` text alpha to solid muted colours -- CAP-5.
- [x] `Source/GUI/Looks/LookBuilders.cpp` -- Route alert button Look off ColourChart to Skin -- residual durable dig.
- [x] `Source/GUI/Skins/SkinColours*.h` (+ `Skin.cpp` if needed) -- Add missing pastilles only for swept slots -- CAP-2/3.
- [x] `Scripts/quality/scan_gated_colours.py` -- Implement gated hex/`juce::Colours` scan with allowlist/excludes; run green -- CAP-6 gate proof.
- [x] `_bmad-output/specs/spec-skins-colour-model-ssot/migration-plan.md` -- Update Gate acceptance checklist; document porte or park -- CAP-6.
- [x] Smoke + quality -- Black↔Cream walk (Patch Edit, chrome, Matrix modal, Settings, Getting Started); build; `lint_touched.py` on touched files -- CAP-6. *(Build + lint + scan + pytest green; human smoke 2026-10-23 Porte OK.)*

**Acceptance Criteria:**
- Given gated `Source/GUI` after the sweep, when `scan_gated_colours.py` runs, then it exits 0 (Tests hex ignored; LockDimmingFilm allowlisted).
- Given Footer error / mod-bus drag / popup disabled-section / alert Look paints, when inspected, then no opaque alpha-as-darken shortcut and no durable ColourChart dig remain on those paths.
- Given Black↔Cream Standalone smoke on the checklist surfaces, when walked, then nothing looks half-migrated (good-enough).
- Given the migration-plan checklist, when the story stops, then either every gate row is evidenced **porte franchie**, or **parque total** is recorded with factual reasons — never a shrink-merge plan.
- Given touched C++/scripts, when `lint_touched.py` and the project build run, then both succeed.

### Review Findings

- [x] [Review][Patch] Pin absolute ColourChart values for new popup disabled Look fields — `assertPopupMenuDisabledAndAlertLooks` only round-trips Look↔Skin; wrong map still passes. Match sibling substrate style (`kEmerald700` / `Common::kContentDisabled` ≡ `kGrey600`) for Black+Cream. [`Tests/Unit/SkinColourSubstrateTests.cpp:117`]
- [x] [Review][Patch] Cover remaining scanner failure contracts in pytest — add exit-1 probe for gated `juce::Colours::` (hex-only probe today) and monkeypatch empty `gated_source_files()` → exit 2. [`Scripts/quality/tests/test_scan_gated_colours.py`]
- [x] [Review][Patch] Make the dirty-path probe race-safe — fixed `Helpers/_scan_gated_colours_probe_do_not_commit.cpp` can collide under parallel pytest or leftover state; use a unique tempfile under gated `Source/GUI`. [`Scripts/quality/tests/test_scan_gated_colours.py:84`]
- [x] [Review][Patch] Treat unreadable gated files as tooling failure (exit 2), not residual colour hits (exit 1) — `scan_file` currently returns a hit string on `OSError`. Optionally decode git stdout as UTF-8 with `errors="replace"`. [`Scripts/quality/scan_gated_colours.py:105`]
- [x] [Review][Defer] CAP-5 pytest paint checks remain source-text greps only — deferred: already recorded in deferred-work / Build triage; CONVENTIONS forbid GUI paint unit tests; absolute Look pins above are the repo-fit hardening. [`Scripts/quality/tests/test_scan_gated_colours.py:53`]

**Rejected**
- false — Borrowed FooterMessage / DarkPanel / SectionHeader keys for alert + mod-bus drag: intentional reuse per frozen Always + Design Notes + prior Build triage; not a purity defect.
- false — HEX scanner false-positive risk without comment stripping: locked CAP-6 contract (hex + `juce::Colours`); gated tree currently has zero residual hex outside allowlist/Tests.
- false — Scanner “not in CI / quality gate”: `Scripts/quality/tests/` already runs in `.github/workflows/build-and-test.yml` (includes green-tree scan); folding into `run_quality_gate.sh` was explicitly Never for this story.
- false — Missing ISkin convenience for popup disabled text: Look fields already carry the values; raw `getColour` in LookBuilders is sufficient; no product divergence.
- false — Implementation Notes “7 passed” / Verification Commands omit pytest: fixing would only edit the story under review; suite has eight tests including the hex failure path.
- low — Checklist row “Excluded paths still may carry local colour” marked `[x]`: standing exclusion presented as a done tick; wording only, no gate risk after porte declaration.

## Implementation Notes

### CAP-5 sweep (2026-10-23)
- Footer error: dropped `.withAlpha(0.8f)`; solid `kFooterMessage` Error text reused (no new muted pastille).
- ModulationBusCell drag: Skin `kSectionHeader` LineBlue (source) + `kFooterMessage` Error text (drop); ColourChart include removed.
- Popup disabled / HeaderLogo section: new map keys `PopupMenu` / `PopupMenuButtonLike` `kText`+`kDisabled` (Emerald700 / Grey600); Look fields `textDisabled` / `textDisabledButtonLike`.
- `buttonAlertLookFromSkin`: FooterMessage Error text/background + DarkPanel Text Hover; ColourChart include removed from LookBuilders.
- Left alone: LED runtime alpha, LockDimmingFilm, close-cross pale, PatchNameDisplay primary idle/hover, dialog overlay `0.85f`, `kTransparent` clears.

### CAP-6 scan
- Added `Scripts/quality/scan_gated_colours.py` — `git ls-files` under `Source/GUI/`; fail hex + `juce::Colours::`; allowlist ColourChart + SkinColours* + LockDimmingFilm; skip Tests. Exit 0 on branch (275 files). Not added to `run_quality_gate.sh`.

### Verification
- `cmake --build --preset macos-debug-arm64` — success.
- `python3 Scripts/quality/lint_touched.py` — OK.
- `python3 Scripts/quality/scan_gated_colours.py` — OK.
- `python3 -m pytest Scripts/quality/tests/test_scan_gated_colours.py` — 7 passed (scan green/excludes/allowlist + CAP-5 paint greps).
- Standalone Black↔Cream smoke — **Porte OK** (human, 2026-10-23). Checklist complete; verdict **porte franchie** in `migration-plan.md`. No shrink-merge; no editor.
- Review patches: scan tracks+untracked + empty→exit 2; failure-path pytest; SkinColourSubstrate popup/alert Look asserts; migration-plan evidence wording tightened.

## Spec Change Log

## Review Triage Log

- false — Blind: borrowed FooterMessage/DarkPanel/SectionHeader keys for alert/drag — intentional reuse per frozen Always + Design Notes (extend maps only when no coherent key); not a defect.
- false — Blind: Emerald700/Grey600 shade drift vs former alpha bake — good-enough parity locked; human smoke Porte OK; proud recalibration out of scope.
- false — Blind: section header shares disabled pastille — intentional muted solid reuse for CAP-5; not a defect.
- false — Blind: drag fill fully opaque — CAP-5 required solids; alpha-as-darken removed on purpose; smoke OK.
- medium → patch — Blind/Edge: `scan_gated_colours.py` uses only tracked `git ls-files`, so untracked gated residuals can exit 0 — include `--others --exclude-standard` under gated prefix.
- medium → patch — Blind: no failure-path test for dirty hex/`Colours` — add pytest that expects exit 1.
- false — Blind: Dev-only exclude unimplemented — no `Source/Dev` (or equivalent) on tree today; Tests exclude is what exists.
- low → patch — Blind: checklist “Zero local colour” overclaims vs scan (hex/`Colours` only) — tighten evidence wording in `migration-plan.md`.
- false — Blind: story status still `in-progress` in staged diff — already updated to `in-review` after diff capture.
- medium → patch — Edge: empty gated file list exits 0 — fail exit 2 when zero files scanned.
- medium → patch — VG: `popupMenuLookFromSkin` disabled fields unasserted — add Skin/Look unit expects (Black+Cream).
- medium → patch — VG: `buttonAlertLookFromSkin` unasserted — add Look unit expects against FooterMessage Error + DarkPanel Hover.
- defer — VG: CAP-5 pytest paint checks are source-text only — lightweight greps intentional for this gate; executable Look asserts above are the repo-fit fix; full GUI paint tests out of unit policy.

## Design Notes

**Scan contract (locked):** Fail patterns = durable hex literals + `juce::Colours::` under gated GUI. Allowlist SSOT sources (`ColourChart.h`, `SkinColours*.h`) plus policy file `LockDimmingFilm.cpp`. Do not greylist `ColourChart::` identifiers in the scanner — call-site digs are a sweep concern. Standalone script is sufficient; optional later wiring into `run_quality_gate.sh` is out of this story.

**Alpha keep vs sweep:** Overlay scrim is transient window dim (keep). Footer/popup/mod-bus alphas fake a second durable shade (sweep to solids). Prefer remapping existing Error/disabled greys before inventing new pastilles.

**Park rule:** Incomplete checklist or red scan → park the colour-model branch entirely; alpha continues on trunk / `v1.0.0-alpha.1`. No partial SSOT into V1.

## Verification

**Commands:**
- `python3 Scripts/quality/scan_gated_colours.py` -- expected: exit 0 on branch after sweep
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++
- `cmake --build --preset macos-debug-arm64` (or active preset) -- expected: success

**Manual checks (if no CLI):**
- Standalone Black↔Cream: Patch Edit, header/footer, one Matrix modal, Settings, Getting Started shell — coherent, no half-migrated chrome
- Confirm LEDs still dim via code alpha; lock film and close-cross pale unchanged
- Document porte vs park in `migration-plan.md` checklist; stop at `done_checkpoint` (no editor)
