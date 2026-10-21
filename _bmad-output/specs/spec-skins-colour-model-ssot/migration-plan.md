# Migration plan — zones, parity, gate, risks

## Definition of done (product)

By **2026-11-15**, on a **dedicated branch**, Matrix-Control paints the **entire V1 user GUI** from the cleaned colour model: closed palette (shade names) + **structured** `(ObjectType, Property, State)` → palette maps for Black/Cream + Look injection. A user switching Black ↔ Cream sees a coherent, recognisable UI with **no local colour leftovers** on that path. **Tests** and **Dev tools** are out of the gate. If the date slips: **total park** (no partial SSOT merge into V1, no eating alpha calendar); resume 2027; alpha may continue on current trunk skins.

## Good-enough parity

| Allowed | Not required |
|---------|----------------|
| Micro visual gaps vs today’s trunk if the SSOT is coherent | Pixel-identical match to pre-migration frames |
| Unifying borders/greys that already diverge between call sites | Per-pixel browser/HTML fidelity |
| Recognisable Black and Cream identities | “Proud” contrast polish of Black / Cream (post-gate) |

Acceptance cue: side-by-side Black and Cream on Patch Edit + chrome + one modal + Settings — nothing looks accidentally unthemed or half-migrated.

## Zone order (suggested)

Migrate and close each zone before declaring the gate. Order favours shared chrome first so leaf panels inherit tokens.

1. **Model substrate** — palette rename (`kEmerald*`, `kGrey*`, `kRed500`, …); structured key API; migrate maps off flat `SkinColourId` SSOT; inventory baseline (`brownfield.md` + fresh scan). Patch/Master = injection only.
2. **Looks / shared widgets** — LookBuilders resolve structured keys; control/display widgets (incl. buttonLike as separate type).
3. **Main chrome** — Header/Footer/body as surface object types; logo / brand labels.
4. **Patch Edit + Master Edit + Shared / Patch Manager** — panels via Looks; Blue/Orange accents via injection.
5. **Dialogs + wizards** — modal chrome keys, confirm/prompt dialogs, Getting Started wizard.
6. **Settings + About** — windows, tab rail, pages, close controls.
7. **Gate sweep** — automated colour scan; alpha-as-darken pass; Black↔Cream smoke.

## Gate acceptance checklist (2026-11-15)

All must hold on the dedicated branch:

- [ ] **CAP-1…CAP-4** behaviours demonstrable in Standalone: skin switch recolours gated UI via maps/Looks only.
- [ ] **Zero local colour** on V1 user GUI path: no new/remaining ad-hoc `0xAARRGGBB` / `juce::Colours::*` durable paints outside palette+tokens (see open question for automated vs manual proof).
- [ ] **CAP-5** alpha policy: LED max solid + runtime dim; lock film + close-cross hover dim untouched as code-owned; PatchNameDisplay secondary mutator labels on solid mapped colour; other opaque alpha-as-darken swept to solids.
- [ ] **Automated colour scan** green on gated `Source/GUI` (excludes `Source/GUI/Tests/**` and equivalent Dev-only paths) — see script note in `code-map.md`.
- [ ] **Parity** good-enough for Black and Cream on: Patch Edit, header/footer, at least one Matrix modal, Settings, Getting Started wizard shell.
- [ ] **Excluded** paths still may carry local colour: `Source/GUI/Tests/**`, `Tests/**`, any future Dev-only tooling not on the user path.
- [ ] Project compiles; relevant tests pass; `Scripts/quality/lint_touched.py` clean on touched C++.
- [ ] **No shrink merge** plan if incomplete — either gate met or park.

## Alpha policy (locked 2026-10-21)

| Effect | Skin model? | Rule |
|--------|-------------|------|
| LEDs | Yes (max colour) | Solid palette colour at full brightness; code alphas down toward off |
| Lock UI film | No | Leave; no skin token |
| Close-cross hover pale | No (effect) | Leave hover dim in code; durable cross colour still SSOT |
| Mutator secondary line (Mxx-Ryy / Compare) | Yes | New solid token + Black/Cream maps; remove `withAlpha` shortcut in LookBuilder |

## Gate proof (locked)

Ship a **small automated check** (script under `Scripts/quality/` or equivalent, wired so the gate/build review can run it) that fails on residual local `0xAARRGGBB` / `juce::Colours::*` durable paints under gated `Source/GUI`, allowing `ColourChart.h` / skin map tables as SSOT sources and excluding Tests/Dev tools. Exact patterns and allowlist live with the script; this companion only requires that the check exists and is part of CAP-6 success.

## Suggested story slices (for optional `stories.yaml`)

Independently reviewable; execution top→bottom. Checkpoint flags set with Guillaume at breakdown time.

| Slice | Intent |
|-------|--------|
| 1 — Substrate | Structured keys + palette rename + Black/Cream maps; local-hex key gaps |
| 2 — Shared Looks/widgets | LookBuilders + control/display widgets off local colour |
| 3 — Main chrome + edit panels | Surfaces + Patch/Master/Shared via injection |
| 4 — Dialogs/wizards/Settings/About | Modal chrome + Settings/About close path |
| 5 — Gate sweep | Alpha pass, automated scan, Black↔Cream smoke, park-or-ship |

## Risks / debt traps

| Risk | Mitigation |
|------|------------|
| Incomplete inventory → false “done” | Gate re-scan; prefer small automated check excluding Tests |
| Perfectionism “zero colour including Tests/Dev” | Explicitly excluded from CAP-6 |
| `#if DEV` pollution on user path “until editor” | Forbidden; editor is post-gate |
| Scope creep into author editor / Skin Studio | Non-goals; forge rejected editor-before-SSOT |
| Slip past 15 Nov with half SSOT | Total park only — no shrink merge |
| Aesthetic rabbit hole (Black contrast, Cream beyond unify) | Post-gate; good-enough only here |

## Planning note (checked 2026-10-21)

Matrix-Control PRD / epics / sprint-status do **not** assume Skin Studio Lot C. The historical Skin Studio brief still describes the old Studio calibration path; optional archive/supersede note there is enough — **no** `bmad-correct-course` required for the plugin PRD.
