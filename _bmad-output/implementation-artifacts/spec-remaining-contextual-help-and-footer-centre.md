---
title: 'Footer centre HELP + strip Settings/About modal HELP (Wave 3)'
type: 'feature'
created: '2026-10-13'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'a92b2d2fbe2e3208f91742089442a8d163767519'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-header-matrix-modulation-contextual-help.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-section-module-title-contextual-help.md'
  - '{project-root}/_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The footer centre band (MIDI send-queue alert zone) still has no HELP. Separately, Settings and About modals already register furtive HELP; with the main UI dimmed behind those modals the left HELP line is nearly unreadable and looks broken.

**Approach:** Wave 3 — (1) add centre-band hit HELP via the left overlay (DEVICE pattern); (2) remove all contextual HELP registration from Settings and About modals (including prior binds); (3) refresh inventory + Standalone UAT. Display-only; no MIDI queue / PANIC behaviour change. The Settings preference CONTEXTUAL HELP SHOW/HIDE itself stays (it gates main-UI HELP, it is not modal HELP).

## Boundaries & Constraints

**Always:**
- Existing `ContextualHelpBinder` + left-band HELP overlay for main UI; CONTEXTUAL HELP = HIDE still suppresses all furtive HELP including the new centre-band HELP.
- ASCII-only `PREFIX: Sentence.` in `PluginDisplayNames` (hyphen `-`, ellipsis `...`).
- Centre HELP prefix `MIDI:`; transparent hit area over `centreBand` (mirror `deviceHitArea_`); overlay paints on **left** only; red `paintMidiQueuePressureAlert` stays on centre when active (no APVTS sticky write from HELP).
- Settings modal: no `registerContextualHelp` / no binder binds on any Settings control, label, LED, or page (UI / MIDI / AUDIO / PATCH / MUTATOR / MASTER). Remove existing registration and call sites.
- About modal: no contextual HELP on any About control or painted row. Remove existing registration and call sites.
- Inventory: document Settings/About as **no furtive HELP** (modal dimming makes footer HELP unusable); add centre-band row; retire Wave 3 “exhaustive audit” deferral note as resolved for modal policy + centre band.
- Voluntary exclusions (unchanged): empty Patch Manager chrome; sticky left / severity dismiss; Ramp Master Override `M` badge; dialogs / Getting Started; main-UI Wave 1–2 coverage (do not redo).
- Scope keep: one Wave 3 ship (centre HELP + strip modal HELP + inventory) despite prior longer draft.
- Approved centre HELP copy (2026-10-13, human wording):

| Target | Help text |
|--------|-----------|
| Footer centre band | MIDI: Shows a red alert when the MIDI send queue is backed up - click PANIC button to clear stuck notes. |

**Never:**
- Add or keep Settings / About modal HELP binds (expanding mute Settings labels is explicitly rejected).
- Redo Wave 1 / Wave 2 titles, cartouches, bus numbers, logo prefix.
- Footer layout redesign; change MIDI queue pressure threshold, polling, or PANIC.
- Floating JUCE tooltips; sticky PATCH MANAGER / Bank Utility messages.
- Remove or redesign the CONTEXTUAL HELP SHOW/HIDE Settings preference (combo stays; only its own HELP bind goes away with the rest of Settings HELP).
- French in source; Core → GUI dependency.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Centre empty + SHOW | Hover centre band | Left HELP shows approved centre copy; centre empty | N/A |
| Centre alert + SHOW | Hover centre while red alert active | Left HELP same copy; centre red alert still paints | N/A |
| Centre + HIDE | Preference HIDE | No HELP overlay; alert paint unchanged | Clear if overlay was active |
| Leave centre | Mouse exits hit area | HELP clears (binder delay); alert unchanged | Epoch clear |
| Settings open | Hover any Settings control/label | No furtive HELP overlay | N/A |
| About open | Hover any About control/link | No furtive HELP overlay | N/A |
| Main UI after close | Settings/About closed; hover bound main control | HELP works as Waves 1–2 | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.{h,cpp}` — `centreHitArea_` like `deviceHitArea_`; bounds from `centreBand`; bind centre HELP; left overlay paint unchanged.
- `Source/Shared/Definitions/PluginDisplayNames.h` — add `FooterPanel::ContextualHelp` centre constant; remove or leave-unused Settings/About `ContextualHelp` string blocks only if nothing else references them after unbind (prefer delete dead constants).
- `Source/GUI/PluginEditorSettingsOverlay.cpp` — stop calling `SettingsPanel::registerContextualHelp`.
- `Source/GUI/PluginEditorWindows.cpp` — stop calling `AboutPanel::registerContextualHelp`.
- `Source/GUI/Settings/SettingsPanel.{h,cpp}` (+ Setup) — remove binder member / `registerContextualHelp` and all `bind(...)` calls; keep SHOW/HIDE combo wiring elsewhere.
- `Source/GUI/Settings/SettingsMidiPage.{h,cpp}` / `SettingsAudioPage.{h,cpp}` — remove `registerContextualHelp` and binds.
- `Source/GUI/About/AboutPanel.{h,cpp}` — remove binder / `registerContextualHelp` and binds.
- `Source/GUI/Helpers/ContextualHelpBinder.*` — reuse for Footer centre only; no API redesign required.
- `_bmad-output/implementation-artifacts/inventory-contextual-help-messages.md` — §21/§22 = no modal HELP policy; § centre band; keep §23 DEVICE.
- Reuse: Footer DEVICE hit-area + binder pattern.
- Do not change: left sticky writers; queue alert paint/threshold/timers; Wave 1–2 main-UI binds; `wireSettingsContextualHelpCombo` preference persistence.

## Tasks & Acceptance

**Execution:**
- [x] `FooterPanel.{h,cpp}` + `PluginDisplayNames.h` — centre hit area + approved `MIDI:` HELP; HIDE-gated; alert untouched.
- [x] `SettingsPanel*` / `SettingsMidiPage*` / `SettingsAudioPage*` / `PluginEditorSettingsOverlay.cpp` — strip all Settings modal HELP registration.
- [x] `AboutPanel.*` / `PluginEditorWindows.cpp` — strip all About modal HELP registration.
- [x] `PluginDisplayNames.h` — delete orphaned Settings/About ContextualHelp constants if unused after strip.
- [x] `inventory-contextual-help-messages.md` — modal no-HELP policy + centre band + UAT notes.
- [x] Quality — `python3 Scripts/quality/lint_touched.py` on touched C++; build smoke.

**Acceptance Criteria:**
- Given CONTEXTUAL HELP = SHOW and centre band empty, when hovering the centre band, then left HELP shows the approved centre copy.
- Given a MIDI queue pressure alert is active, when hovering the centre band, then the red alert remains on centre and left HELP shows the same copy (no APVTS sticky write).
- Given CONTEXTUAL HELP = HIDE, when hovering the centre band, then no HELP overlay appears.
- Given Settings modal open, when hovering any Settings control or label, then no furtive HELP appears in the footer.
- Given About modal open, when hovering any About element, then no furtive HELP appears in the footer.
- Given Settings/About closed, when hovering a Wave 1–2 main-UI target, then HELP still works.
- Given inventory updated, when reading Settings/About sections, then they state no modal furtive HELP and the centre-band row matches the approved copy.

### Review Findings

- [x] [Review][Defer] No automated check that FooterPanel binds centreHitArea_ to kCentreBand (hover path) [FooterPanel.cpp:441] — deferred: CONVENTIONS §8.5 forbids GUI component unit tests; copy pins + Standalone UAT remain the covering checks (reconfirmed this adversarial review).

Rejected:
- Inventory Wave 3 / §23–§24 section order awkward — low: unlikely everyday harm; reordering is doc hygiene only.
- Centre HELP vs live alert wording differ ("MIDI send queue" / "PANIC button") — false: human-approved centre copy intentionally describes the zone; alert string pre-exists and was out of Wave 3 change scope.
- Centre HELP vs Header PANIC HELP describe PANIC differently — false: different targets (zone vs button); Header copy unchanged by design.
- ContextualHelpOverlayTests includes Settings/About headers for static_assert — low: intentional Wave 3 compile guard; everyday risk low (prior triage already rejected broader API-shape hardening).
- Spec status done + review_loop_iteration 0 / empty Spec Change Log — false: rejected; fix would edit the spec under review.
- Removing Settings/About ContextualHelp constants loses in-UI explanations — false: intentional product decision (modal HELP forbidden); inventory documents no-modal policy.

## Implementation Notes

- Centre/device hit registration unified as `registerBandContextualHelp()`; `updateBandHitAreaBounds()` sizes both hit areas.
- Settings/About `ContextualHelp` string namespaces deleted with binder registration; SHOW/HIDE preference IDs unchanged.
- `ContextualHelpOverlayTests`: Wave 3 centre/DEVICE copy pins; compile-time `static_assert` that Settings/About/MIDI/AUDIO pages have no `registerContextualHelp`. Review patch narrowed the test so it does not claim Footer HIDE wiring.
- Inventory §24 for centre band; deferred-work notes unbound-audit remainder + no GUI bind unit test.
- Standalone UAT (hover centre with live alert, modal smoke) remains human.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Centre HELP duplicates live alert framing when red alert painted | false | Human-approved centre copy intentionally describes the alert zone; coexistence left HELP + centre paint is frozen I/O. |
| `MIDI:` prefix shared with Master MIDI module HELP | false | Locked prefix convention (cartouche / module / centre queue zone); not a defect. |
| Spec Verification omits running unit suite | false | Rejected: fix would be editing this build's Verification section. |
| Exhaustive unbound-control audit closed in inventory though only modal+centre shipped | medium | Intent resolved modal policy + centre only; broader audit still open → **defer** (deferred-work). |
| Spec `status: done` with empty triage at review start | false | Process glitch; status corrected to `in-review` before triage. |
| No positive test that FooterPanel binds `centreHitArea_` | medium | Real CI gap; CONVENTIONS §8.5 forbids GUI unit tests → **defer**. |
| `static_assert` only catches `registerContextualHelp` name | low | Rejected: everyday risk low; broader API-shape guards add complexity without named caller harm. |
| Centre band lacked numbered inventory peer to §23 DEVICE | low | **patch** — inventory now has §24 Footer centre band. |
| Copy says "PANIC button" without Header location | false | Human-approved wording; Header PANIC affordance unchanged. |
| Code Map mentions SettingsPanel Setup overstatement | false | Rejected: fix would edit this build's Code Map. |
| Edge Case Hunter: empty finding list | false | No edge-case defects filed. |
| Verification Gap: centre bind has no automated consumer check | medium | Same as bind-registration gap → **defer** (deferred-work). |
| Verification Gap: wave3 HIDE test implied Footer centre coverage | medium | **patch** — narrowed to copy-only `wave3CentreBandHelpCopyConstantsMatchSpec`; HIDE remains in `hideClearsOverlayDetailPolicy`. |
| Verification Gap Other: local smoke may skip tests | false | Rejected: would edit Verification; CI `build-and-test` still runs Matrix-Control_Tests. |

## Design Notes

Modal HELP is a product rejection: dimmed footer makes HELP look broken. Do not “fix readability” by painting HELP elsewhere — remove binds.

Centre hover must not fight the red alert: HELP stays on the **left** overlay; centre only gains a transparent hit child.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched `Source/**/*.cpp|h`

**Manual checks (Standalone UAT):**
- Hover centre empty → left HELP; leave → clear.
- Queue alert if reproducible → red centre + hover HELP left; PANIC unchanged.
- Open Settings → hover several rows/tabs → no HELP; close → main-UI HELP still works.
- Open About → hover links/rows → no HELP.
- CONTEXTUAL HELP HIDE → centre HELP off; sticky left unchanged.
