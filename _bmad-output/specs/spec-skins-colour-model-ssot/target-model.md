# Target model — palette → structured keys → skins → injection

Product-facing contract for the colour model. Aligned with Skin Studio concepts (palette-blind + object × property × state); implemented in Matrix-Control C++ for Black/Cream. No Skin Studio Lot C / JSON editor in this gate.

## Concepts

| Concept | Role | Rules |
|---------|------|-------|
| **Palette** | Closed set of named opaque colours | Single SSOT (`ColourChart`). Hex owned by Matrix-Control. Names: `k` + family + shade (`kEmerald950`, `kGrey900`, `kRed500`) or absolutes (`kTransparent`, `kBlack`, `kWhite`, `kCream`). **Never** encode UI role in the palette name. |
| **Object type** | Generic UI kind (`button`, `slider`, `headerPanel`, …) | No `patch*` / `master*` object types. Surfaces (header/footer/body/modal chrome) are object types too. Parallel looks (e.g. button-like) = **separate** object type. |
| **Property** | What is painted (`background`, `border`, `text`, `valueBar`, `fill`, …) | Stable ids; intent is WHAT, not HOW. |
| **State** | Named visual variant (`off`, `on`, `hover`, `clicked`, `disabled`, `info`/`warning`/`error`, or `default` when none) | Only states that exist for that object type — no empty grid bloat. |
| **Mapping key** | `(ObjectType, Property, State)` | SSOT address for durable colour. Flat `SkinColourId` is **not** the model SSOT (may exist only as derived/internal if useful). |
| **Skin** | Full map: every required key → exactly one palette entry | Black and Cream only in this gate. |
| **Look** | Snapshot injected into a generic widget at construction | Widgets paint from Look only. |
| **Orchestrator / builders** | Know product zones; choose which Look / accent to inject | Patch vs Master, Blue vs Orange section lines = **injection**, not object types. |

## Data flow (logical)

```
ColourChart (palette pastilles)
    ↑ referenced by
Skin maps: (ObjectType, Property, State) → palette entry   [per Black / Cream]
    ↑ resolved by
Skin runtime getColour(object, property, state)
    ↑ read by
LookBuilders / ISkin helpers
    ↑ injected into
Generic widgets & surface panels (paint from Look only)
```

## Palette naming (locked)

- Prefix `k` everywhere (project convention).
- Shades: `kFamilyNNN` (950 dark → lower numbers lighter); only stops we use.
- Accents example mapping from today’s hex (names illustrative; hex unchanged unless a later story retunes):  
  `kGreen1…4` → `kEmerald950`, `kEmerald900`, `kEmerald700`, `kEmerald400`  
  `kRed` → `kRed500`  
  `kDarkGrey*` / `kLightGrey*` → `kGrey950`…`kGrey100` (homogeneous Grey family; **not** copied Tailwind greys)  
  Retire role names such as `kContextualHelpChrome` → a Grey shade only.
- Absolutes: `kTransparent`, `kBlack`, `kWhite`, `kCream`.

## Invariants

1. **Generic widgets** — no Patch/Master knowledge inside widget paint.
2. **One mapping hop** — key → palette entry; no ad-hoc hex in widgets.
3. **Two skins at gate** — Black and Cream maps complete for all keys used on the V1 user GUI path.
4. **Fonts unchanged** — Orbitron brand-only; PatchNameDisplay PT Sans Narrow + chart red (`kRed500`).
5. **No author editor required** — C++ tables OK; same concepts must stay editor-ready.
6. **Inventory honesty** — only real (object, property, state) slots; Prefer two object types over hidden flags for divergent looks.

## Allowed inside the gate

- Rename palette; introduce structured key API; migrate tables off flat `SkinColourId` SSOT.
- Add palette entries / keys for local-hex gaps and secondary mutator solid.
- Unify divergent borders onto one pastille when good-enough.
- LED: max-brightness pastille + runtime alpha dim.
- Keep thin `ISkin` helpers if they resolve to structured keys.

## Forbidden inside the gate

- New skins (Duo, Monochrome, …).
- `patch*` / `master*` object types.
- Parallel colour systems; `#if DEV` colour literals on the user path.
- Deferring structure-B to “after flat migration” — structured keys are in-scope for this gate.
- Skin Studio HTML / pixel-near browser acceptance.
- Skin tokens for lock film or close-cross hover pale.
