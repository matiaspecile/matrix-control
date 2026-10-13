---
title: 'Fix Windows Standalone compile after title-bar recentre'
type: 'bugfix'
created: '2026-10-13'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After pulling main, the Windows Debug Standalone target fails to compile because `juce::Component::SafePointer` is expanded incorrectly when JUCE’s `Component` macro is active.

**Approach:** Use bare `Component::SafePointer` (same pattern as `MatrixControlStandaloneApp.cpp`), then rebuild Windows Debug until Standalone and related targets are clean of project errors and warnings.

</frozen-after-approval>

## Implementation Notes

- Root cause: with `Component` `#define`d to `juce::Component`, unqualified expansion of `juce::Component::SafePointer` becomes `juce::juce::Component::SafePointer` and MSVC fails on Standalone.
- Fix: local `push_macro` / `#undef Component` / `pop_macro` around `juce::Component::SafePointer` (same restore-flag pattern as `StandaloneTitleBarDoubleClick.h`), so the header does not depend on include order.
- Also cleared six MSVC C4458 warnings in unit tests (`name` shadowing `juce::UnitTest` member) by renaming locals (`formattedName`, `patchName`, `fileName`, `path`).
- Verified 2026-10-13: `cmake --build --preset windows-debug --target Matrix-Control_Standalone Matrix-Control_Tests Matrix-Control` → exit 0, 0× `warning C*` / `error C*`. Full preset can still fail on Dropbox `CopyToArtefactsDir` file lock (unrelated). `python Scripts/quality/lint_touched.py --base main` OK.

## Review Triage Log

- Spec status still `in-progress` while notes claim green — false after finalize (`status: done`).
- Frozen Intent omits test C4458 renames — false: Approach already requires clean of errors and warnings; renames are the warning cleanup.
- Missing Tasks & Acceptance — false: oneshot route omits those sections by design.
- Empty `context: []` — rejected low: oneshot with local Code Map in notes is enough.
- Implementation Notes not reproducible — patch: added preset, targets, date.
- Comment “matches App.cpp” overclaim — rejected low; notes now describe the TitleBarDoubleClick restore-flag pattern instead.
- Header depends on ambient `Component` macro — patch: self-contained push/undef/pop around `juce::Component::SafePointer`.
- Spec silent on Standalone-only `Component` macro invariant — rejected low: fix is header-local and portable.
- Spec silent on which targets were cleaned — patch: recorded in Implementation Notes.
- Unicode em dash in header comment — rejected low: matches sibling Standalone comments; not UI display strings.
- Curly apostrophe in frozen Problem — rejected: frozen human-owned; cosmetic only.
- Spec untracked vs code modified — false after single commit including the artifact.
