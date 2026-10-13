---
title: 'Section / module / group-label contextual HELP (Wave 2) + logo prefix'
type: 'feature'
created: '2026-10-13'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '6b866261d887ee7cf0d4320095e3a01c0d72cbea'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-header-matrix-modulation-contextual-help.md'
  - '{project-root}/_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Hovering section titles, most module titles, and some Patch Manager chrome still yields no footer HELP (Wave 1 deferred this). The logo HELP line still uses the obsolete `EDIT:` prefix even though the logo sits outside the EDIT cartouche.

**Approach:** Wave 2 — bind `SectionHeader` / unbound `ModuleHeader` titles (and reuse existing Patch Name display HELP on its title), update already-bound Internal/Computer titles and Browser/Memory/Storage group labels to the approved table, retarget logo + logo-menu HELP prefixes to `MATRIX-CONTROL:`, and align the contextual-help inventory. Keep the existing footer HELP overlay; no floating JUCE tooltips.

## Boundaries & Constraints

**Always:**
- Footer HELP via existing `ContextualHelpBinder` + footer overlay; Settings CONTEXTUAL HELP = HIDE still suppresses all help.
- ASCII-only help copy in `PluginDisplayNames` (hyphen `-`, ellipsis `...`).
- Prefix style: `PREFIX: Sentence.` (no space before colon).
- Logo face + logo-menu HELP (`SETTINGS` / About / Skin / UI Scale) use prefix `MATRIX-CONTROL:` (not `EDIT:`).
- Child I/C/P (and other bound children) keep their own HELP when the pointer is on those controls (binder uses nested mouse listeners).
- PATCH NAME module title uses the same string as the Patch Name display hover.
- Approved HELP copy (2026-10-13, human-refined):

| Target | Help text |
|--------|-----------|
| Logo | MATRIX-CONTROL: Opens the logo menu for SETTINGS, About, Skin, and UI Scale. |
| SETTINGS (logo menu) | MATRIX-CONTROL: Opens SETTINGS (MIDI ports, standalone audio, paths, warnings, master utility, shortcuts). |
| About (logo menu) | MATRIX-CONTROL: Shows product version, links, and credits. |
| Skin (logo menu) | MATRIX-CONTROL: Chooses the visual skin for the editor. |
| UI Scale (logo menu) | MATRIX-CONTROL: Sets the user interface scale. |
| PATCH EDIT | PATCH EDIT: Edits the current patch and shows envelopes, Track Generator, and patch name. |
| MATRIX MODULATION | MATRIX MODULATION: Routes modulation sources to destinations across ten buses. |
| PATCH MANAGER | PATCH MANAGER: Navigates banks and patches, stores to memory or disk, imports or exports banks, and inits, copies, or pastes patches. |
| MASTER EDIT | MASTER EDIT: Edits global settings for all patches (Matrix-1000 only). |
| DCO 1 | DCO 1: Oscillator 1 controls for the current patch. |
| DCO 2 | DCO 2: Oscillator 2 controls for the current patch. |
| VCF/VCA | VCF/VCA: Filter and amplifier controls for the current patch. |
| FM/TRACK | FM/TRACK: Frequency modulation and Track Generator controls for the current patch. |
| RAMP/PORTAMENTO | RAMP/PORTAMENTO: Ramp generator and portamento controls for the current patch. |
| ENV 1 | ENV 1: Envelope 1 controls for the current patch. |
| ENV 2 | ENV 2: Envelope 2 controls for the current patch. |
| ENV 3 | ENV 3: Envelope 3 controls for the current patch. |
| LFO 1 | LFO 1: Low-frequency oscillator 1 controls for the current patch. |
| LFO 2 | LFO 2: Low-frequency oscillator 2 controls for the current patch. |
| PATCH NAME | PATCH NAME: Shows the 8-character patch name - double-click to rename when editable. |
| MIDI | MIDI: Global MIDI channel and related controls for the synthesizer. |
| VIBRATO | VIBRATO: Global vibrato controls for the synthesizer. |
| MISC | MISC: Global bend, pressure, tuning, memory protect, plus global Unison and Unison Detune. |
| BANK UTILITY | BANK UTILITY: Selects a bank and copies, pastes, imports, or exports it (Matrix-1000 only). |
| INTERNAL PATCHES | INTERNAL PATCHES: Browses and loads patches from the connected synthesizer memory. |
| COMPUTER PATCHES | COMPUTER PATCHES: Browses and loads patch files from a folder on disk. |
| PATCH MUTATOR | PATCH MUTATOR: Mutates the current patch and browses mutation history. |
| BROWSER (Internal) | INTERNAL PATCHES: Bank and patch number browser for synthesizer memory. |
| MEMORY | INTERNAL PATCHES: Init, copy, paste, and store for the selected memory slot. |
| BROWSER (Computer) | COMPUTER PATCHES: Folder browser for patch files on disk. |
| STORAGE | COMPUTER PATCHES: Open a folder and save the current patch as a .syx file. |

**Never:**
- Classic JUCE floating tooltips as primary UX.
- User-manual rewrite (inventory alignment only).
- Changing control/parameter HELP already shipped except logo/logo-menu prefixes and the title/group rows in the approved table.
- Exhaustive unbound-control audit (Wave 3).
- Behaviour changes to Init / Copy / Paste / bank / mutator actions.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Section title hover | Mouse over PATCH EDIT / MATRIX MODULATION / PATCH MANAGER / MASTER EDIT header | Footer shows approved section HELP | N/A |
| Module title hover | Mouse over unbound module title band (DCO 1, Bank Utility, MIDI, …) | Footer shows approved module HELP | N/A |
| Module I/C/P hover | Mouse over Init/Copy/Paste on a titled header | Existing I/C/P HELP unchanged (not title string) | N/A |
| PATCH NAME title | Mouse over PATCH NAME `ModuleHeader` | Same string as Patch Name display HELP | N/A |
| Already-bound titles/groups | Internal/Computer titles; Browser/Memory/Storage | Footer shows approved revised strings | N/A |
| Logo / logo menu | Mouse over logo or logo-menu items | HELP uses `MATRIX-CONTROL:` prefix (not `EDIT:`) | N/A |
| CONTEXTUAL HELP HIDE | Preference HIDE | No HELP overlay for any Wave 2 target | N/A |

</frozen-after-approval>

## Code Map

- `Source/Shared/Definitions/PluginDisplayNames.h` -- SSOT: add section/module `kTitle` ContextualHelp constants; retarget `HeaderPanel::ContextualHelp` logo + logo-menu strings to `MATRIX-CONTROL:`; revise Internal/Computer/group strings per approved table; update comment that claimed logo stays `EDIT:`.
- `Source/GUI/Helpers/ContextualHelpBindingSupport.h` -- optional `bindModuleHeaderTitle` helper if it reduces duplication; keep I/C/P helpers unchanged.
- `Source/GUI/Widgets/SectionHeader.*` -- paint-only; bind whole component (no new hit widget expected).
- `Source/GUI/Widgets/ModuleHeader.*` -- title band + children; bind header for title HELP; children already win for I/C/P.
- `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.*` -- create binder or register section HELP on `sectionHeader_`.
- `Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.*` -- same for MASTER EDIT.
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.*` -- same for PATCH MANAGER.
- `Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanelContextualHelp.cpp` -- bind `sectionHeader_` (panel already has binder).
- Patch Edit / Master module `registerContextualHelp` files -- bind `moduleHeader_` with new `kTitle` (DCO 1/2, VCF/VCA, FM/TRACK, RAMP/PORTAMENTO, ENV 1-3, LFO 1-2, MIDI, VIBRATO, MISC).
- `PatchNameDisplayPanel.*` -- bind `moduleHeader_` to `kPatchNameDisplay` (or alias `kTitle`).
- `BankUtilityPanel*` / `PatchMutatorPanelContextualHelp.cpp` -- bind module headers with new `kTitle`.
- `InternalPatchesPanel.cpp` / `ComputerPatchesPanelContextualHelp.cpp` -- already bind titles + group labels; update strings to approved table.
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` -- already binds logo; string change only (logo + menu constants).
- `_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md` -- add Wave 2 title/section/group rows; note logo prefix.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` -- Add approved section/module title HELP constants; retarget logo + logo-menu to `MATRIX-CONTROL:`; revise Internal/Computer/group strings -- SSOT copy.
- [x] Section panels (`PatchEditPanel`, `MasterEditPanel`, `PatchManagerPanel`, `MatrixModulationPanelContextualHelp.cpp`) -- Register/bind each `sectionHeader_` -- section title HELP.
- [x] Module panels (Patch Edit + Master + Bank Utility + Patch Mutator + Patch Name display) -- Bind each `moduleHeader_` / `bankUtilityModuleHeader_` with `kTitle` -- module title HELP.
- [x] `InternalPatchesPanel` / `ComputerPatches*` -- Apply approved string revisions (binds already exist) -- group/title copy update.
- [x] `_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md` -- Document Wave 2 titles + logo prefix -- inventory alignment.
- [ ] UAT Standalone -- Smoke hover section/module/group titles + logo + I/C/P still distinct + HIDE preference -- human-visible proof.

**Acceptance Criteria:**
- Given CONTEXTUAL HELP shown, when the user hovers each of the four section titles, then the footer shows the approved section HELP string.
- Given CONTEXTUAL HELP shown, when the user hovers each listed module title (including Bank Utility, Patch Mutator, Patch Name), then the footer shows the approved module HELP string; Patch Name title matches the display HELP.
- Given a module header with I/C/P, when the user hovers Init (or Copy/Paste), then the existing control HELP appears (not the title string).
- Given CONTEXTUAL HELP shown, when the user hovers Browser / Memory / Storage group labels, then the footer shows the approved (kept or revised) group HELP.
- Given CONTEXTUAL HELP shown, when the user hovers the MATRIX-CONTROL logo, then HELP uses the `MATRIX-CONTROL:` prefix (not `EDIT:`).
- Given CONTEXTUAL HELP = HIDE, when hovering any Wave 2 target, then no HELP overlay appears.

## Implementation Notes

- 2026-10-13: Wave 2 HELP strings + binds landed; `bindModuleHeaderTitle` helper added; Patch Edit / Patch Manager section HELP registered lazily on first `resized()` (lint-safe). Build `macos-debug-arm64` + `lint_touched.py` OK. Matrix rows are hover UX — coverage is Standalone UAT (still open), matching Wave 1 pattern; approved copy verified against `PluginDisplayNames.h`.
- 2026-10-13 review patches: `resolveHoveredControl` prefers deepest bound control so ModuleHeader title HELP cannot override I/C/P after clear-delay refresh; Mutator help-copy unit test pins `kTitle` (23 strings); inventory Wave 2 status wording + MIDI/VIBRATO/MISC title rows + Hors perimetre note.
- 2026-10-13 adversarial code-review patches: MIDI title drops "mode"; MISC title uses Unison Detune (not thickness); Wave 2 inventory title SSOT expanded (ENV/LFO distinct + Patch Manager modules); unit test pins 31 Wave 2 logo/section/module/group constants.

## Spec Change Log

- 2026-10-13: Human renegotiated frozen MIDI / MISC module title HELP (adversarial code review) — MIDI: "channel and related controls"; MISC: "Unison Detune" instead of "Unison thickness".

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Inventory per-module tables omit Title rows for Patch Edit/Master modules | low | Wave 2 summary table is title SSOT; expanded per-module rows deferred as doc polish — reject (not everyday user harm) |
| PATCH NAME / MATRIX MODULATION inventory sections lack Title/section rows | low | Covered in Wave 2 summary; reject same as above |
| Wave 2 summary pointed MIDI/VIBRATO/MISC at wrong sections | medium | Real inventory glitch — **patch** applied (explicit title rows) |
| Inventory preface locks Header prefixes without MATRIX-CONTROL | false | Cartouche/logo table already lists MATRIX-CONTROL: (section 1) |
| Hors perimetre still claims empty despite Wave 3 open | low | **patch** applied (Wave 3 note) |
| ModuleHeader + I/C/P bind makes resolveHoveredControl ambiguous | medium | Verified in `ContextualHelpBinder.cpp`: parent `isMouseOver(true)` while over Init; map order could re-show title HELP — **patch** deepest-wins |
| Inconsistent section HELP registration (ctor vs first resized) | low | Both paths bind before user interaction; reject (unify would be non-trivial churn) |
| PATCH NAME title HELP during LOAD PATCH drag chrome | false | Frozen Always requires same string as display HELP, not dynamic title label match |
| PATCH EDIT section copy omits top-row modules | false | Approved frozen copy; renegotiate intent to change |
| STORAGE group copy uses imperative voice | false | Approved frozen copy |
| Spec AC omits logo-menu prefix | false | Reject finding whose fix edits this build's AC; code matches frozen table |
| Inventory marks Wave 2 shipped while UAT open | low | **patch** applied (wording → implemented, UAT pending) |
| Edge: PATCH NAME drag title mismatch | false | Same as Blind PATCH NAME drag claim — intentional per frozen Always |
| Edge: resolveHoveredControl parent over I/C/P | medium | Same root as binder ambiguity — **patch** deepest-wins |
| Verification: Mutator test misses kTitle | medium | Pre-verified gap — **patch** applied (23-string pin); test ran green |
| Verification: no automated registration for new binds | medium (unverified CI style) | **defer** — Standalone UAT / Core-only policy; appended to deferred-work.md |

### Review Findings

- [x] [Review][Patch] MISC / MIDI module title HELP wording vs real controls — Human chose reopen both: MISC title → Unison Detune (not thickness); MIDI title → drop misleading "mode"; sync frozen table in this spec, `PluginDisplayNames.h`, inventory Wave 2 rows, and any copy pins. [Source/Shared/Definitions/PluginDisplayNames.h:999]
- [x] [Review][Patch] Pin Wave 2 HELP constants beyond Mutator kTitle [Tests/Unit/ContextualHelpOverlayTests.cpp:167]
- [x] [Review][Patch] Complete Wave 2 inventory title SSOT (ENV/LFO rows + Patch Manager module titles) [_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md:446]
- [x] [Review][Defer] Deepest-wins resolveHoveredControl untested on clear-refresh path [Source/GUI/Helpers/ContextualHelpBinder.cpp:141] — deferred: Core-only unit-test policy; prior binder hover defer already covers resolveHoveredControl / applyClearIfIdle; UAT remains covering check
- [x] [Review][Defer] New section/module title binds have no registration unit check — deferred: reconfirmed; already logged under Build review of this spec; Core-only policy + Standalone UAT

#### Rejected

- Spec `status: done` while UAT checkbox open — false for code review action (process/doc metadata; UAT already tracked as open task; do not edit frozen AC for this)
- Acceptance Criteria omit explicit logo-menu Given/When/Then — false (fix would edit this build's AC; code matches frozen Always table; prior triage same)
- Lazy `resized()` vs Master ctor registration asymmetry — false (both bind before interactive hover; prior triage rejected unify churn)
- PATCH NAME title bind ownership via parent displays panel — low reject (works; asymmetric ownership is not everyday user harm)
- Per-module inventory tables omit Title rows for Patch Edit/Master; Hors perimetre documents intentional defer — low reject / already scoped as doc polish
- Blind Hunter MISC/MIDI treated as mandatory code defect without renegotiation — superseded by Decision above (strings match frozen table)
