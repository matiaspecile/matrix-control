# Code map and Suggested Review Order

Entry points for a future `/bmad-build` on **model refactor only**. Prefer extending existing layers over parallel colour systems.

## Suggested Review Order

1. **Palette + structured keys + maps** — `ColourChart.h` (rename), replace flat `SkinColourId` SSOT with ObjectType/Property/State + skin maps (`SkinValues.h` / `SkinColours*.h` / `Skin.cpp` as migration surface).
2. **Look injection** — `LookBuilders.cpp` / `WidgetLooks.h` resolve structured keys; typography unchanged.
3. **Shared widgets paint** — controls and displays that still hold local defaults or alpha-as-darken.
4. **Chrome panels** — Header, Footer, MainComponent / body backgrounds.
5. **Product panels** — Patch Edit, Master Edit, Shared / Patch Manager modules.
6. **Dialogs + wizards** — `DialogMatrixHelpers.*`, confirm/prompt dialogs, Getting Started.
7. **Settings + About** — window chrome, close button colours, pages/rail.
8. **Gate scan** — run the automated gated-path colour check; CMake only if new `.cpp` added.

## Code map

### Skin substrate

| Area | Paths | Role |
|------|-------|------|
| Palette | `Source/GUI/Skins/ColourChart.h` | Closed named colours |
| Token ids | `Source/GUI/Skins/SkinValues.h` | `SkinColourId` |
| Map tables | `Source/GUI/Skins/SkinColours.h` + `SkinColoursCommon.h`, `SkinColoursPanels.h`, `SkinColoursWidgetsLayout.h`, `SkinColoursWidgetsControls.h`, `SkinColoursWidgetsSelection.h`, `SkinColoursWidgetsDisplays.h` | `ColourElement` black/cream |
| Runtime | `Source/GUI/Skins/Skin.{h,cpp}`, `ISkin.h`, `ISkinColours.h`, `ISkinFonts.h`, `SkinHelpers.h` | Load maps, getters, fonts |
| Skin switch / prefs | `Source/GUI/PluginEditorSkinScale.cpp` (and Settings skin row wiring) | Active variant |

### Looks

| Area | Paths | Role |
|------|-------|------|
| Builders | `Source/GUI/Looks/LookBuilders.{h,cpp}` | Skin → Look |
| Look structs | `Source/GUI/Looks/WidgetLooks.h` | Paint contracts |
| Typography | `Source/GUI/Looks/TypographyStyles.{h,cpp}` | Font styles (PatchName ≠ Orbitron) |

### Known local-colour hotspots (migrate)

| Area | Paths |
|------|-------|
| Modal chrome | `Source/GUI/Dialogs/DialogMatrixHelpers.{h,cpp}` |
| Close cross | `Source/GUI/Settings/SettingsWindow.cpp`, `Source/GUI/About/AboutWindow.cpp` |
| Field defaults | `Source/GUI/Widgets/ReadOnlyValueField.h` |
| About accents | `Source/GUI/About/AboutPanel.{h,cpp}` |
| Lock film | `Source/GUI/Helpers/LockDimmingFilm.cpp` |
| Alpha-as-darken samples | `FooterPanel.cpp`, `PatchNameDisplayPaint.cpp`, `ModulationBusCell.cpp`, `LookBuilders.cpp` (secondary text) |

### Zone roots (consume Looks)

| Zone | Paths |
|------|-------|
| Shell | `Source/GUI/MainComponent.cpp`, `PluginEditor*.cpp` |
| Header / Footer | `Source/GUI/Panels/MainComponent/HeaderPanel/`, `FooterPanel/` |
| Patch / Master / Shared | `Source/GUI/Panels/MainComponent/BodyPanel/` |
| Settings | `Source/GUI/Settings/` |
| Dialogs / wizard | `Source/GUI/Dialogs/` |
| About | `Source/GUI/About/` |

### Explicitly not for this build

| Area | Paths | Note |
|------|-------|------|
| Dev / visual tests | `Source/GUI/Tests/` | Out of gate |
| Unit tests colour | `Tests/` | Out of gate unless helper extraction |
| Skin Studio HTML | Separate repo / frozen | Non-goal |
| Author editor / IPC | None yet | Post-gate |

### Quality

| Concern | Path |
|---------|------|
| Lint gate | `Scripts/quality/lint_touched.py` |
| Gated colour scan (to add) | Prefer `Scripts/quality/` sibling (e.g. scan local hex / `juce::Colours` under gated `Source/GUI`, allowlist `ColourChart` + `SkinColours*`, exclude `Source/GUI/Tests/**`) — exact name chosen at build |
| Sources list | `CMakeLists.txt` `PLUGIN_SOURCES` |
| Conventions | `CONVENTIONS.md` |

### CAP-5 secondary label hotspot

| Concern | Path |
|---------|------|
| Mutator secondary `withAlpha(0.50f)` | `Source/GUI/Looks/LookBuilders.cpp` (`patchNameDisplayLookFromSkin`) |
| Secondary paint consumer | `Source/GUI/Widgets/PatchNameDisplayPaint.cpp` |
| LED max + alpha dim | `Source/GUI/Widgets/Led.cpp` |
| Lock film (leave) | `Source/GUI/Helpers/LockDimmingFilm.cpp` |
