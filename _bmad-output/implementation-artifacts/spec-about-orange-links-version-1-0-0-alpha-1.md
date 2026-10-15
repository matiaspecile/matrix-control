---
title: 'About orange links and version 1.0.0-alpha.1'
type: 'chore'
created: '2026-10-15'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/CONTRIBUTING.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** About modal hyperlink hover uses a dark red with poor contrast; product still reports `0.3.0-alpha` while the next milestone should display `1.0.0-alpha.1` with an up-to-date release date.

**Approach:** Recolor About link hover to the same orange as Master Edit header lines; bump CMake version SSOT to `1.0.0` with prerelease suffix `alpha.1`, align Luthier and the pinned PluginVersion test, and refresh release date via CMake configure-time build date.

</frozen-after-approval>

## Implementation Notes

- About link hover: replaced hardcoded dark red `0xff9A131D` with `ColourChart::kOrange` (same token as Master Edit header lines). Idle colours unchanged (white for value links, label colour for BMAD credit).
- Left `AboutTextLink` member default as `juce::Colours::orange` (constructor always overrides via `refreshHyperlinkAppearance`); avoided pulling `ColourChart.h` into the header after Blind Hunter.
- Version SSOT: `project(Matrix-Control VERSION 1.0.0)`; `MATRIX_CONTROL_PRERELEASE_SUFFIX` default `"alpha.1"` → display `1.0.0-alpha.1`.
- Release date: refreshed via CMake `string(TIMESTAMP … UTC)` at reconfigure (no hardcoded date; TIMESTAMP line unchanged by design).
- Aligned `.luthier.json` `projectVersion` to `1.0.0`; pinned `PluginVersionTests` expectation to `1.0.0-alpha.1`.
- Did not create/push git tag `v1.0.0-alpha.1` (out of scope unless asked).
- Verified: reconfigure with `-DMATRIX_CONTROL_PRERELEASE_SUFFIX=alpha.1`, PluginVersion tests 0 failures, `lint_touched.py --base HEAD` OK, plugin Debug build OK.

## Review Triage Log

- CMake CACHE default change may leave stale local `alpha` — **low rejected**: pinned PluginVersion test fails if suffix wrong; same pattern as prior bumps; prior deferred-work already covers PRERELEASE footgun.
- Release date “not in diff” because TIMESTAMP line untouched — **false**: date is configure-time by design; reconfigure refreshed it.
- Consuming CMake `1.0.0` while still alpha contradicts policy — **false**: human asked for display `1.0.0-alpha.1`.
- Host/Standalone bare `1.0.0` vs About `1.0.0-alpha.1` — **defer** (already tracked for 0.2.0 dual-surface).
- Luthier `projectVersion` without prerelease — **false**: matches prior 0.3.0 convention.
- ColourChart include in AboutPanel.h for redundant default — **patch applied**: removed include; default left as `juce::Colours::orange`.
- Close cross still red while links orange — **false**: intentional; user asked to recolor links only.
- User-manual Version-Produit still stale — **defer** (docs drift).
- CHANGELOG empty / no Unreleased stub — **defer** (changelog hygiene).
- Spec oneshot missing AC / still in-progress mid-flight — **false**: Finalize sets `done`; oneshot omits AC by design.
- PluginVersionTests does not pin a calendar date — **false**: pinning would fail daily; ISO format already asserted.
