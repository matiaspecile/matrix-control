---
organization: Ten Square Software
project: Matrix-Control
title: Linux Patch/Master .syx Drag-Drop
author: BMad Agent
type: bugfix
created: '2026-10-12'
updated: '2026-10-12'
status: done
route: dispatch
review_loop_iteration: 0
baseline_commit: 34b5f68ba117b6e787c026fa4db022279f06f179
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-master-file-drag-drop.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On Linux, dragging Patch or Master `.syx` files onto the plugin GUI does not load them, so the documented hot-swap path is broken on this platform.

**Approach:** Ship a Linux text/`file://` fallback into the existing whole-window drop path (same Patch/Master load and PATCH NAME overlay meaning), plus a short Linux note that native Wayland file managers often never deliver DnD to JUCE's X11 peer — Open / Settings remain the guaranteed path. Acceptance surface is Standalone; optional AudioPluginHost smoke is extra, not a gate.

## Decisions

- **Symptom:** NO_OVERLAY — drag shows no PATCH NAME feedback (OS/peer does not reach `FileDragAndDropTarget` in the reported Wayland session).
- **Surface:** STANDALONE_ONLY — fix/verify Standalone; VST3 not an acceptance gate (optional AudioPluginHost smoke allowed).
- **Mitigation:** CODE_PLUS_DOC — `TextDragAndDropTarget` (or equivalent) parsing `file://` / absolute paths into the existing file-drop path when DnD arrives as text; document Wayland→X11 DnD limits and Open/Settings fallback.

## Boundaries & Constraints

**Always:**
- Keep `PluginEditor` as the sole whole-window file/text drop target; reuse `handleSyxFilesDropped` / Master drop / Core assess+load unchanged in product meaning.
- Preserve existing PATCH NAME drag overlay and footer-on-drop-only contract from the done drag-drop specs.
- Linux-first verification on Standalone; macOS/Windows behavior must not regress.
- Document that Ubuntu/GNOME Wayland-only sessions (e.g. 26.04 with no « Ubuntu on Xorg ») may never deliver OS file DnD to the current JUCE X11 build.

**Never:**
- Rewrite Patch/Master validation or Computer Patches / Settings LOAD pipelines.
- Vendor a custom Wayland compositor DnD stack or fork JUCE windowing wholesale.
- Add a second visible drop-zone widget as the primary fix.
- Treat optional AudioPluginHost testing as a merge gate.
- Change macOS/Windows DnD semantics except via shared helpers that stay behavior-neutral there.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Linux Standalone, OS delivers file URIs | One valid Patch `.syx` dragged then dropped | PATCH NAME overlay during drag; drop loads via Computer Patches path | Existing reject footers for bad/bank files |
| Linux Standalone, OS delivers file URIs | One valid Master `.syx` / `.m1km` | Master overlay + Master load / choice dialog as today | Existing Master reject footers |
| Linux DnD arrives as text paths / `file://` | Same files, text payload instead of `files[]` | Treated as file drop when paths parse cleanly | Non-path text ignored; no false load |
| Linux Wayland session, no OS DnD to X11 peer | Drag from native Wayland file manager over Standalone | No overlay (known limit); Linux note + Open/Settings remain available | No silent “success” |
| macOS / Windows | Existing Patch/Master drop | Unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditor.h` + `Source/GUI/PluginEditorFileDragDrop.cpp` — sole `FileDragAndDropTarget`; interest always `!files.isEmpty()`; overlay + Patch/Master drop dispatch. Add `TextDragAndDropTarget` + path/`file://` parse → reuse `filesDropped` / existing handlers.
- Shared path-parse helper (new small unit under `Source/GUI/` or `Source/Shared/` as fits conventions) — parse uri-list / absolute paths; unit-testable without GUI.
- `Source/Core/Services/PatchFileService.*`, `MasterFileAssess.*`, `MasterM1kmCodec.*` — extension/assess SSOT; reuse, do not reimplement.
- `PluginProcessor` drop-load / `loadMasterFromUserFile` — commit paths; reuse.
- Docs: `CONTRIBUTING.md` and/or Linux user note (README / Getting Started) — Wayland DnD limit + Open/Settings.
- JUCE 9 release tree: Xdnd only; MIME prefers UTF8_STRING / text/plain before `text/uri-list`. Upstream `wayland` preview branch exists but is out of scope for this bugfix.
- Done specs: `spec-syx-drag-drop-load.md`, `spec-master-file-drag-drop.md`.
- Do **not** change: Lock film as primary fix; Core→GUI direction; SysEx length contracts.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/PluginEditor.{h}` + `PluginEditorFileDragDrop.cpp` (+ small path-parse helper) — Implement `TextDragAndDropTarget` forwarding parsed paths into existing file-drop path — covers JUCE MIME preference when DnD arrives as text
- [x] `Tests/Unit/...` — Cover parse of `file://`, multi-line uri-list text, reject non-path text — lock helper without GUI
- [x] Docs — Short Linux note: JUCE DnD is X11/Xdnd; native Wayland file managers may not drop onto the X11 window; Open/Settings remain available — sets user expectation
- [ ] Manual Linux Standalone smoke — Attempt Patch/Master `.syx` drag; confirm Open/Settings still work; if a text-delivered drop is reproducible, confirm load — product proof on Standalone

**Acceptance Criteria:**
- Given Linux Standalone when the OS delivers a normal file drag of one valid Patch `.syx`, when dropped, then overlay + load match existing product meaning.
- Given the same for one valid Master `.syx`, when dropped, then Master load path runs (or existing reject if invalid).
- Given DnD payload arrives as text containing `file://` paths, when dropped, then the same load path runs as a normal file drop.
- Given a Wayland session where OS DnD never reaches the peer, when the user reads the Linux note, then the limitation and Open/Settings alternative are explicit.
- Given macOS/Windows, when existing drop flows are unchanged by review, then no intentional behavior change.

## Implementation Notes

- Added header-only `TSS::parseDropTextToFilePaths` / `tryParseDropTextLineToPath` in `Source/GUI/Helpers/DropPathParse.h` (uri-list + absolute paths; reject non-path / non-file URLs / relative paths).
- `PluginEditor` now also implements `juce::TextDragAndDropTarget`; text drag/drop parses to paths then reuses `fileDragEnter` / `fileDragMove` / `fileDragExit` / `filesDropped` (same Patch/Master meaning).
- Unit tests: `DropPathParse` — 0 failures (`Matrix-Control_Tests --category DropPathParse`). Lint OK on touched C++.
- Docs: Linux Wayland/X11 DnD note in `README.md` (build Linux section) and `CONTRIBUTING.md` (Linux contributor note).
- Manual Standalone UAT still required on the target Wayland session (expected: no overlay; Open/Settings still work).
- Parent verification (step-03): DropPathParse 0 failures; PatchManagerActionHandlerDropLoad* + PatchFileServiceAssess + MasterFileAssess 0 failures; lint_touched.py OK vs baseline.

## Spec Change Log

## Review Triage Log

- blind: README omitted text/`file://` fallback — medium — true; route patch (README sentence added).
- blind: Verification still had `<new-helper-filter>` placeholder — medium — true; route patch (DropPathParse category command).
- blind: Manual Standalone smoke unchecked — low — process/UAT remaining for human; not a code defect. Rejected for auto-fix.
- blind: Missing `file://localhost` / single-slash / FILE: casing tests — low — rejected (unlikely everyday vs JUCE URL; not worth expanding surface).
- blind: Missing non-ASCII percent-encoding tests — low — rejected (JUCE URL decode; everyday harm unproven).
- blind: No NUL / space-separated uri-list handling — low/maybe-false — rejected (Xdnd typically newline uri-list; fix adds complexity).
- blind: Matrix omits multi-file text drop — false — text path reuses `filesDropped` which already handles multi; not a gap.
- blind/verification-gap: No automated PluginEditor text→file bridge test — medium (unverified as product break) — route defer (GUI not unit-tested; manual smoke).
- blind: User manual not updated — low — rejected (README + CONTRIBUTING satisfy CODE_PLUS_DOC; manuel is larger docs chore).
- blind: Empty Spec Change / Review Triage logs at in-review — false — process timing; this log is the triage.
- blind: No Windows `file:///C:/` cases — false after Linux-only TextDragAndDropTarget; helper unused off Linux.
- blind: `isInterestedInTextDrag` accepts any absolute path — false — mirrors file-drag interest + BAD FILE overlay contract.
- edge: Angle-bracket uri-list `<file:///…>` rejected — medium — true; route patch (strip brackets + test).
- edge: UTF-8 BOM prefix — low — rejected (unlikely everyday Xdnd payload).
- edge: NUL-separated multi-URI — low — rejected (same as blind NUL).
- edge: `file://` with non-empty host mistaken path — maybe-false — rejected (JUCE `isLocalFile` gate; not demonstrated).
- edge: Space-separated URIs on one line — low — rejected.
- edge: TextDragAndDropTarget on macOS/Windows — high — true vs frozen Linux mitigation / no intentional mac-win change; route patch (`#if JUCE_LINUX`).
- edge claim: macOS/Windows “no behavior change” false — high — same root cause as Linux-only wrap; patched.
- verification-gap Other: Manual smoke unchecked — same as blind UAT note; human walkthrough.

## Design Notes

Observed on Ubuntu 26.04 GNOME: Wayland-only login (no `xsessions`, no « Ubuntu on Xorg » gear). JUCE 9.0.3 release uses X11 peers + Xdnd; native Wayland→XWayland file drags often never invoke `FileDragAndDropTarget` (matches NO_OVERLAY). When Xdnd does fire, JUCE may prefer plain text over `text/uri-list` — text-path fallback is the in-repo lever. Upstream JUCE has a public `wayland` preview (Standalone); plugins still X11 — out of scope here.

## Verification

**Commands:**
- `cmake --build --preset linux-debug` — expected: build succeeds
- `"Builds/Linux/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests" --category DropPathParse` — expected: pass (0 failures)

**Manual checks:**
- Standalone under current Wayland: drag `.syx` (expect no overlay if OS never delivers); confirm Computer Patches Open / Settings Master LOAD.
- If any environment delivers text/`file://` drops, confirm load matches file-drop behavior.
- Optional: AudioPluginHost smoke — informational only.
