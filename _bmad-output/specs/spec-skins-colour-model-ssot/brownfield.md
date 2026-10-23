# Brownfield — current colour / skin model

Lean inventory of what Matrix-Control already has. Downstream builds sanitize and complete this; they do not invent a parallel system.

## Target vs today

Gate target = structured `(ObjectType, Property, State)` + shade-named palette (`target-model.md`). Below is **brownfield starting point** to migrate off — not the end state.

## Layers today

| Layer | Role today | Primary paths |
|-------|------------|---------------|
| **Palette** | Named `#AARRGGBB` (`kGreen1`, `kDarkGrey1`, …; role leak: `kContextualHelpChrome`) | `Source/GUI/Skins/ColourChart.h` |
| **Token ids** | Flat `SkinColourId` (object+property+state baked into one name) | `Source/GUI/Skins/SkinValues.h` |
| **Mappings** | Per-element `ColourElement { name, blackVariant, creamVariant }` tables | `Source/GUI/Skins/SkinColours*.h` via `SkinColours.h` |
| **Skin runtime** | Loads variant maps into `std::map<SkinColourId, Colour>`; state helpers | `Source/GUI/Skins/Skin.{h,cpp}`, `ISkin` / `ISkinColours` / `ISkinFonts` |
| **Looks** | Construction-time colour (+ font) packages for generic widgets | `Source/GUI/Looks/LookBuilders.*`, `WidgetLooks.h`, `TypographyStyles.*` |
| **Injection** | Panels/factories call Look builders with active `ISkin`; Blue/Orange via widget `ColourVariant` | Panels under `Source/GUI/Panels/`, Settings, Dialogs, widgets |

## Variants

- `Skin::ColourVariant::{Black, Cream}` — two mapping columns only.
- Combo ids: `SkinComboBoxItemId::{kBlack, kCream}`.
- Not greenfield: both skins ship and are alpha/V1-viable as-is.

## Palette (`ColourChart`)

Named entries include neutrals (`kBlack`…`kLightGrey3`, `kCream`, `kContextualHelpChrome`), accents (`kRed`, `kOrange`, `kYellow`, `kBlue`), greens (`kGreen1`…`kGreen4`), `kTransparent`.

## Token coverage (`SkinColourId`)

Already covers: header/body/footer panels; section/module/group labels; buttons/toggles/sliders/combos/popup menus (incl. button-like); number boxes; envelope / PatchNameDisplay / Track Generator displays. Sentinel: `kNumColours`.

Gaps relative to V1 user GUI gate: modal/dialog chrome tokens (border, title band), window close-cross colours, some field defaults that still hardcode hex (see below).

## Fonts (skin, not colour — do not invent)

| API | Typeface | Use |
|-----|----------|-----|
| `getBaseFont` / Bold | PT Sans Narrow | Default UI |
| `getBrandFontBold` | Orbitron Bold | Brand only |
| `getModalBodyFont` / Bold | Montserrat | Modal body |
| PatchNameDisplay | Typography → PT Sans Narrow + chart red text | **Not** Orbitron |

## Known local colours outside palette / tokens (gated path)

Hardcoded hex or `juce::Colours::*` found under user GUI (not exhaustive forever — re-scan at gate):

| Location | Issue |
|----------|-------|
| `Dialogs/DialogMatrixHelpers.h` | `kDialogBorderColour` `0xff5E5E5E`; `kModalTitleBandColour` `0xff000000` |
| `Settings/SettingsWindow.cpp`, `About/AboutWindow.cpp` | Close cross `0xff9A131D` (+ hover alpha) |
| `Widgets/ReadOnlyValueField.h` | Default fill/text `0xff404040` / `0xffb9b9b9` |
| `Helpers/LockDimmingFilm.cpp` | `juce::Colours::black.withAlpha(...)` |
| `About/AboutPanel.h` | Idle/hover `juce::Colours::grey` / `orange` |

## Alpha patterns (policy locked)

| Pattern | Examples | Gate rule |
|---------|----------|-----------|
| LED brightness | `Led.cpp` alpha on active colour | Palette holds **solid max-brightness** colour; code may modulate alpha toward off |
| Lock dimming film | `LockDimmingFilm.cpp` | **Code-owned** — not a skin token; leave alone |
| Close-cross hover pale | Settings/About window close | **Code-owned** hover dim — leave alone (durable cross colour still migrates to palette/token) |
| Secondary mutator labels | `LookBuilders` `secondaryText = text.withAlpha(0.50f)` (Mxx-Ryy / Compare line) | **Replace** with dedicated solid palette entry + `SkinColourId` (or successor) mapping |
| Other opaque alpha-as-darken | Footer error `.withAlpha(0.8f)`; dialog scrim; popup disabled text; etc. | Prefer solids when background known (CAP-5 sweep) |
| Primary PatchNameDisplay idle/hover | paint affordance alphas on main name | Assumed paint-owned for this gate unless later remapped |

## Explicitly out of gate inventory

- `Source/GUI/Tests/**` (local hex / checkerboards OK for Dev tools)
- Unit tests under `Tests/`
- Future `#if DEV` author tooling (must not pollute V1 user path)

## Brief Étape 2 alignment

Historical Skin Studio brief already named the target concepts (palette → token → skin → Look → orchestrator). Matrix-Control partially implements them; this refactor makes the gated path consistent and complete without requiring Skin Studio Lot C.
