---
organization: Ten Square Software
project: Matrix-Control
title: Inventory — Contextual help messages
author: BMad Agent
status: archived
sources:
  - Desktop draft Matrix-Control-aide-contextuelle-messages.md
created: 2026-09-16
updated: 2026-10-13
---

# Matrix-Control — Aide contextuelle (brouillon de messages)

**But :** valider / corriger les textes avant implémentation.
**Format affiché :** badge `HELP` + texte blanc furtif : `MODULE: message`
**Langue UI :** anglais (ASCII seulement : tiret `-`, ellipsis `...` — pas de tiret cadratin).
**Déjà en production :** table Patch Mutator (reference, ne pas réinventer sauf ajustement volontaire).
**Préfixe Header (locked) :** cartouche — `EDIT:` / `MIDI:` / `AUDIO:` (plus `SESSION`).

Comment editer : change la colonne **Message proposé** ; laisse **Contrôle** tel quel.
Quand c'est prêt, dis-moi dans le chat Build quelles tables tu as touchées (ou renvoie le fichier).

---

## Préfixes Header par cartouche (locked)

| Cartouche | Préfixe | Cible typique |
|-----------|---------|---------------|
| **EDIT** | `EDIT:` | Badge EDIT, UNDO / REDO |
| **MATRIX-CONTROL** (logo) | `MATRIX-CONTROL:` | Logo + menu logo (hors cartouche) |
| **MIDI** | `MIDI:` | Badge MIDI, labels + LEDs FROM KEYBOARD / FROM SYNTH / TO SYNTH, PANIC |
| **AUDIO** | `AUDIO:` | Badge AUDIO, INPUT GAIN (label + slider), peak (Standalone) |

Ancienne option figee `SESSION` : retiree — ne plus l'utiliser pour le Header.

---

## 1. Header (EDIT / MIDI / AUDIO)

| Contrôle | Message proposé |
|----------|-----------------|
| EDIT (badge) | EDIT: Undo and redo the last PATCH or MASTER edit. |
| MIDI (badge) | MIDI: Activity lights for MIDI keyboard (or host in Plugin), synthesizer MIDI in and out, plus PANIC. |
| AUDIO (badge) | AUDIO: Monitors the selected audio input level and sets INPUT GAIN (Standalone). |
| FROM KEYBOARD (label + LED) | MIDI: Lights when MIDI activity arrives from the MIDI keyboard (or host in Plugin). |
| FROM SYNTH (label + LED) | MIDI: Lights when MIDI activity arrives from the synthesizer. |
| TO SYNTH (label + LED) | MIDI: Lights when MIDI activity is sent to the synthesizer. |
| INPUT GAIN | AUDIO: Sets monitoring level for the selected audio input. Choose the listen source in SETTINGS > AUDIO > SYNTH FROM. |
| UNDO | EDIT: Undoes the last PATCH or MASTER edit in this session. |
| REDO | EDIT: Redoes the last undone PATCH or MASTER edit. |
| PANIC | MIDI: Sends Note Offs for held notes, plus All Notes Off and Reset Controllers. |
| LOGO | MATRIX-CONTROL: Opens the logo menu for SETTINGS, About, Skin, and UI Scale. |
| SETTINGS... | MATRIX-CONTROL: Opens SETTINGS (MIDI ports, standalone audio, paths, warnings, master utility, shortcuts). |
| ABOUT... | MATRIX-CONTROL: Shows product version, links, and credits. |
| SKIN | MATRIX-CONTROL: Chooses the visual skin for the editor. |
| UI SCALE | MATRIX-CONTROL: Sets the user interface scale. |
| AUDIO PEAK | AUDIO: Shows the peak level of the monitored audio input. |

---

## 2. DCO 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | DCO 1: Resets this module to its init values. |
| C | DCO 1: Copies this module to the clipboard. |
| P | DCO 1: Pastes compatible clipboard data into this module. |
| FREQUENCY | DCO 1: Sets DCO 1 pitch in semitones. |
| FREQ < LFO 1 | DCO 1: How much LFO 1 modulates DCO 1 frequency. |
| SYNC | DCO 1: Hard-syncs DCO 1 to DCO 2 when enabled. |
| PULSE WIDTH | DCO 1: Sets pulse width for pulse / combination waves. |
| PW < LFO 2 | DCO 1: How much LFO 2 modulates pulse width. |
| WAVE SHAPE | DCO 1: Mixes wave components for the selected wave family. |
| WAVE SELECT | DCO 1: Chooses the DCO 1 waveform family. |
| LEVERS | DCO 1: Enables lever modulation for this DCO. |
| KEYBD/PORTA | DCO 1: Tracks keyboard / portamento for this DCO. |
| KEY CLICK | DCO 1: Adds a short click on key-on for this DCO. |

---

## 3. DCO 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | DCO 2: Resets this module to its init values. |
| C | DCO 2: Copies this module to the clipboard. |
| P | DCO 2: Pastes compatible clipboard data into this module. |
| FREQUENCY | DCO 2: Sets DCO 2 pitch in semitones. |
| FREQ < LFO 1 | DCO 2: How much LFO 1 modulates DCO 2 frequency. |
| DETUNE | DCO 2: Fine detune of DCO 2 relative to DCO 1. |
| PULSE WIDTH | DCO 2: Sets pulse width for pulse / combination waves. |
| PW < LFO 2 | DCO 2: How much LFO 2 modulates pulse width. |
| WAVE SHAPE | DCO 2: Mixes wave components for the selected wave family. |
| WAVE SELECT | DCO 2: Chooses the DCO 2 waveform family (includes Noise). |
| LEVERS | DCO 2: Enables lever modulation for this DCO. |
| KEYBD/PORTA | DCO 2: Tracks keyboard / portamento for this DCO. |
| KEY CLICK | DCO 2: Adds a short click on key-on for this DCO. |

---

## 4. VCF/VCA

| Contrôle | Message proposé |
|----------|-----------------|
| I | VCF/VCA: Resets this module to its init values. |
| DCO 2 \| DCO 1 MIX | VCF/VCA: Balances DCO 1 and DCO 2 into the filter stage. |
| FREQUENCY | VCF/VCA: Sets the filter cutoff frequency. |
| FREQ < ENV 1 | VCF/VCA: How much ENV 1 modulates filter cutoff frequency. |
| FREQ < PRESSURE | VCF/VCA: How much pressure modulates filter cutoff frequency. |
| RESONANCE | VCF/VCA: Sets filter resonance (emphasis at cutoff frequency). |
| VCA 1 VOLUME | VCF/VCA: Sets VCA 1 overall volume. |
| VCA 1 < VELOCITY | VCF/VCA: How much velocity modulates VCA 1. |
| VCA 2 < ENV 2 | VCF/VCA: How much ENV 2 modulates VCA 2. |
| LEVERS | VCF/VCA: Enables lever modulation for filter / amplifier. |
| KEYBD/PORTA | VCF/VCA: Tracks keyboard / portamento for the filter. |

---

## 5. FM/TRACK

| Contrôle | Message proposé |
|----------|-----------------|
| I | FM/TRACK: Resets this module to its init values. |
| VCF FM AMOUNT | FM/TRACK: Sets FM amount into the VCF from DCO 2. |
| FM < ENV 3 | FM/TRACK: How much ENV 3 modulates FM amount. |
| FM < PRESSURE | FM/TRACK: How much pressure modulates FM amount. |
| TRACK POINT 1 | FM/TRACK: Sets Track Generator point 1 level. |
| TRACK POINT 2 | FM/TRACK: Sets Track Generator point 2 level. |
| TRACK POINT 3 | FM/TRACK: Sets Track Generator point 3 level. |
| TRACK POINT 4 | FM/TRACK: Sets Track Generator point 4 level. |
| TRACK POINT 5 | FM/TRACK: Sets Track Generator point 5 level. |
| TRACK INPUT | FM/TRACK: Selects the source that drives the Track Generator. |
| Track Generator (curve) | FM/TRACK: Drag the Track Generator curve points for tracking shape. |

---

## 6. RAMP/PORTAMENTO

| Contrôle | Message proposé |
|----------|-----------------|
| I | RAMP/PORTAMENTO: Resets this module to its init values. |
| RAMP 1 RATE | RAMP/PORTAMENTO: Sets Ramp 1 rise time. |
| RAMP 1 TRIGGER | RAMP/PORTAMENTO: Chooses what starts Ramp 1. |
| RAMP 2 RATE | RAMP/PORTAMENTO: Sets Ramp 2 rise time. |
| RAMP 2 TRIGGER | RAMP/PORTAMENTO: Chooses what starts Ramp 2. |
| PORTAMENTO RATE | RAMP/PORTAMENTO: Sets glide time between notes. |
| PORTA < VELOCITY | RAMP/PORTAMENTO: How much velocity scales portamento rate. |
| PORTA MODE | RAMP/PORTAMENTO: Chooses how portamento glides between notes. |
| LEGATO PORTA | RAMP/PORTAMENTO: Enables legato-only portamento (Unison keyboard modes). |
| KEYBOARD MODE | RAMP/PORTAMENTO: Chooses poly / unison / related keyboard response. |

---

## 7. ENV 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 1: Resets this module to its init values. |
| C | ENV 1: Copies this module to the clipboard. |
| P | ENV 1: Pastes compatible clipboard data into this module. |
| DELAY | ENV 1: Delay before the envelope attack starts. |
| ATTACK | ENV 1: Time to rise from zero to peak. |
| DECAY | ENV 1: Time to fall from peak to sustain. |
| SUSTAIN | ENV 1: Level held while the key is down. |
| RELEASE | ENV 1: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 1: Overall envelope depth. |
| AMP < VELOCITY | ENV 1: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 1: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 1: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 1: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 1: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 8. ENV 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 2: Resets this module to its init values. |
| C | ENV 2: Copies this module to the clipboard. |
| P | ENV 2: Pastes compatible clipboard data into this module. |
| DELAY | ENV 2: Delay before the envelope attack starts. |
| ATTACK | ENV 2: Time to rise from zero to peak. |
| DECAY | ENV 2: Time to fall from peak to sustain. |
| SUSTAIN | ENV 2: Level held while the key is down. |
| RELEASE | ENV 2: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 2: Overall envelope depth. |
| AMP < VELOCITY | ENV 2: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 2: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 2: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 2: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 2: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 9. ENV 3

| Contrôle | Message proposé |
|----------|-----------------|
| I | ENV 3: Resets this module to its init values. |
| C | ENV 3: Copies this module to the clipboard. |
| P | ENV 3: Pastes compatible clipboard data into this module. |
| DELAY | ENV 3: Delay before the envelope attack starts. |
| ATTACK | ENV 3: Time to rise from zero to peak. |
| DECAY | ENV 3: Time to fall from peak to sustain. |
| SUSTAIN | ENV 3: Level held while the key is down. |
| RELEASE | ENV 3: Time to fall to zero after key-up. |
| AMPLITUDE | ENV 3: Overall envelope depth. |
| AMP < VELOCITY | ENV 3: How much velocity scales envelope amplitude. |
| TRIGGER MODE | ENV 3: Chooses how the envelope is triggered. |
| ENVELOPE MODE | ENV 3: Chooses envelope contour behaviour. |
| LFO 1 TRIGGER | ENV 3: Lets LFO 1 retrigger this envelope when enabled. |
| Envelope display | ENV 3: Drag Delay, Attack, Decay, Sustain, and Release on the curve. |

---

## 10. LFO 1

| Contrôle | Message proposé |
|----------|-----------------|
| I | LFO 1: Resets this module to its init values. |
| C | LFO 1: Copies this module to the clipboard. |
| P | LFO 1: Pastes compatible clipboard data into this module. |
| SPEED | LFO 1: Sets LFO 1 rate. |
| SPEED < PRESSURE | LFO 1: How much pressure modulates LFO 1 speed. |
| RETRIGGER POINT | LFO 1: Phase point used when the LFO retriggers. |
| AMPLITUDE | LFO 1: Sets LFO 1 depth. |
| AMP < RAMP 1 | LFO 1: How much Ramp 1 scales LFO 1 amplitude. |
| WAVEFORM | LFO 1: Chooses the LFO 1 waveform. |
| TRIGGER MODE | LFO 1: Chooses how LFO 1 starts and retriggers. |
| LAG | LFO 1: Smooths LFO 1 output changes. |
| SAMPLE INPUT | LFO 1: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED. |

---

## 11. LFO 2

| Contrôle | Message proposé |
|----------|-----------------|
| I | LFO 2: Resets this module to its init values. |
| C | LFO 2: Copies this module to the clipboard. |
| P | LFO 2: Pastes compatible clipboard data into this module. |
| SPEED | LFO 2: Sets LFO 2 rate. |
| SPEED < KEYBD | LFO 2: How much keyboard tracking modulates LFO 2 speed. |
| RETRIGGER POINT | LFO 2: Phase point used when the LFO retriggers. |
| AMPLITUDE | LFO 2: Sets LFO 2 depth. |
| AMP < RAMP 2 | LFO 2: How much Ramp 2 scales LFO 2 amplitude. |
| WAVEFORM | LFO 2: Chooses the LFO 2 waveform. |
| TRIGGER MODE | LFO 2: Chooses how LFO 2 starts and retriggers. |
| LAG | LFO 2: Smooths LFO 2 output changes. |
| SAMPLE INPUT | LFO 2: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED. |

---

## 12. PATCH NAME

| Contrôle | Message proposé |
|----------|-----------------|
| Patch name (display / edit) | PATCH NAME: Shows the 8-character patch name - double-click to rename when editable. |

---

## 13. MATRIX MODULATION

| Contrôle | Message proposé |
|----------|-----------------|
| Column header (# / SOURCE / AMOUNT / DESTINATION) | MATRIX MODULATION: Column guide for bus number, source, amount, and destination. Drag a bus number to reorder. |
| I (section) | MATRIX MODULATION: Resets all modulation buses to init. |
| C (section) | MATRIX MODULATION: Copies all modulation buses to the clipboard. |
| P (section) | MATRIX MODULATION: Pastes compatible clipboard data into all modulation buses. |
| # (bus handle) | MATRIX MODULATION: Drag to reorder this modulation bus. |
| I (per bus) | MATRIX MODULATION: Resets this bus source, amount, and destination. |
| SOURCE | MATRIX MODULATION: Selects the modulation source for this bus. |
| AMOUNT | MATRIX MODULATION: Sets how strongly this bus modulates its destination. |
| DESTINATION | MATRIX MODULATION: Selects what this bus modulates. |

---

## 14. MIDI (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | MIDI: Resets this module to its init values. |
| CHANNEL | MIDI: Sets the synthesizer MIDI channel. |
| MIDI ECHO | MIDI: Echoes incoming MIDI back out when enabled. |
| CONTROLLERS | MIDI: Enables or disables MIDI controller reception. |
| PATCH CHANGES | MIDI: Enables or disables MIDI program changes. |
| PEDAL 1 SELECT | MIDI: Assigns the function for pedal 1. |
| PEDAL 2 SELECT | MIDI: Assigns the function for pedal 2. |
| LEVER 2 SELECT | MIDI: Assigns the function for lever 2. |
| LEVER 3 SELECT | MIDI: Assigns the function for lever 3. |

---

## 15. VIBRATO (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | VIBRATO: Resets this module to its init values. |
| SPEED | VIBRATO: Sets global vibrato rate. |
| WAVEFORM | VIBRATO: Chooses the vibrato waveform. |
| AMPLITUDE | VIBRATO: Sets global vibrato depth. |
| SPEED MOD SOURCE | VIBRATO: Selects what modulates vibrato speed. |
| SPEED MOD AMOUNT | VIBRATO: How much the speed mod source affects vibrato rate. |
| AMP MOD SOURCE | VIBRATO: Selects what modulates vibrato depth. |
| AMP MOD AMOUNT | VIBRATO: How much the amp mod source affects vibrato depth. |

---

## 16. MISC (Master Edit)

| Contrôle | Message proposé |
|----------|-----------------|
| I | MISC: Resets this module to its init values. |
| MASTER TUNE | MISC: Fine-tunes overall instrument pitch. |
| MASTER TRANSPOSE | MISC: Transposes the instrument in semitones. |
| BEND RANGE (+/-) | MISC: Sets pitch-bend range in semitones up and down. |
| UNISON | MISC: Enables Master Unison (can override Patch keyboard mode). |
| VOLUME INVERT | MISC: Inverts volume pedal response when enabled. |
| BANK LOCK | MISC: Locks bank changes on the hardware. |
| MEMORY PROTECT | MISC: Protects synth memory from being overwritten. |

---

## 17. BANK UTILITY

| Contrôle | Message proposé |
|----------|-----------------|
| Title (module header) | BANK UTILITY: Selects a bank and copies, pastes, imports, or exports it (Matrix-1000 only). |
| 0 | BANK UTILITY: Selects Matrix-1000 bank 0 (RAM) and sets it as the copy/paste/import/export target. |
| 1 | BANK UTILITY: Selects Matrix-1000 bank 1 (RAM) and sets it as the copy/paste/import/export target. |
| 2 | BANK UTILITY: Selects Matrix-1000 bank 2 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 3 | BANK UTILITY: Selects Matrix-1000 bank 3 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 4 | BANK UTILITY: Selects Matrix-1000 bank 4 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 5 | BANK UTILITY: Selects Matrix-1000 bank 5 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 6 | BANK UTILITY: Selects Matrix-1000 bank 6 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 7 | BANK UTILITY: Selects Matrix-1000 bank 7 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 8 | BANK UTILITY: Selects Matrix-1000 bank 8 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| 9 | BANK UTILITY: Selects Matrix-1000 bank 9 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1). |
| COPY | BANK UTILITY: Copies the selected bank from the synthesizer to the clipboard. |
| PASTE | BANK UTILITY: Pastes the clipboard bank into the selected destination bank. |
| IMPORT | BANK UTILITY: Imports patch files into the selected RAM bank. |
| EXPORT | BANK UTILITY: Exports the selected bank as SysEx files on disk. |

---

## 18. INTERNAL PATCHES

| Contrôle | Message proposé |
|----------|-----------------|
| Title (module header) | INTERNAL PATCHES: Browses and loads patches from the connected synthesizer memory. |
| BROWSER (group label) | INTERNAL PATCHES: Bank and patch number browser for synthesizer memory. |
| MEMORY (group label) | INTERNAL PATCHES: Init, copy, paste, and store for the selected memory slot. |
| < | INTERNAL PATCHES: Loads the previous patch in the current bank. |
| > | INTERNAL PATCHES: Loads the next patch in the current bank. |
| CURRENT BANK | INTERNAL PATCHES: Selects the current synthesizer bank. |
| CURRENT PATCH | INTERNAL PATCHES: Selects the current patch number in the bank. |
| INIT | INTERNAL PATCHES: Loads the init patch template into the editor and sends it to the synthesizer. |
| COPY | INTERNAL PATCHES: Copies the current patch to the clipboard. |
| PASTE | INTERNAL PATCHES: Pastes a clipboard patch into the editor and sends it to the synthesizer. |
| STORE | INTERNAL PATCHES: Stores the current patch into the selected synthesizer memory slot. |

---

## 19. COMPUTER PATCHES

| Contrôle | Message proposé |
|----------|-----------------|
| Title (module header) | COMPUTER PATCHES: Browses and loads patch files from a folder on disk. |
| BROWSER (group label) | COMPUTER PATCHES: Folder browser for patch files on disk. |
| STORAGE (group label) | COMPUTER PATCHES: Open a folder and save the current patch as a .syx file. |
| < | COMPUTER PATCHES: Loads the previous patch file in the folder. |
| > | COMPUTER PATCHES: Loads the next patch file in the folder. |
| SELECT A PATCH | COMPUTER PATCHES: Chooses which .syx patch file is selected. |
| OPEN | COMPUTER PATCHES: Opens a folder of .syx patch files. |
| SAVE AS | COMPUTER PATCHES: Saves the current patch as a new .syx file. |
| SAVE | COMPUTER PATCHES: Saves over the current .syx file. |

---

## 20. PATCH MUTATOR (déjà en production — reference)

| Contrôle | Message actuel |
|----------|----------------|
| Title (module header) | PATCH MUTATOR: Mutates the current patch and browses mutation history. |
| MODE | PATCH MUTATOR: Sets how far mutations stray - Kindred, Drift, Warp, or Wild. |
| PITCH | PATCH MUTATOR: Controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free. |
| HISTORY | PATCH MUTATOR: Recalls a mutation or retry from this session. |
| MUTATE | PATCH MUTATOR: Creates a new variation from the current recipe and sends it to the synthesizer. |
| RETRY | PATCH MUTATOR: Rolls again from the same mutation root. |
| < | PATCH MUTATOR: Steps backward through session history. |
| > | PATCH MUTATOR: Steps forward through session history. |
| C | PATCH MUTATOR: Compares with the origin patch and locks editing until you click [C] button again. |
| D | PATCH MUTATOR: Deletes the selected history entry. |
| F | PATCH MUTATOR: Flushes the whole session mutation history. |
| E | PATCH MUTATOR: Exports the session mutations as SysEx files. |
| D1 | PATCH MUTATOR: Include DCO 1 module in the mutation recipe. |
| D2 | PATCH MUTATOR: Include DCO 2 module in the mutation recipe. |
| F/A | PATCH MUTATOR: Include VCF/VCA module in the recipe. |
| F/T | PATCH MUTATOR: Include FM/TRACK module in the recipe. |
| R/P | PATCH MUTATOR: Include RAMP/PORTAMENTO module in the recipe. |
| E1 | PATCH MUTATOR: Include ENV 1 module in the recipe. |
| E2 | PATCH MUTATOR: Include ENV 2 module in the recipe. |
| E3 | PATCH MUTATOR: Include ENV 3 module in the recipe. |
| L1 | PATCH MUTATOR: Include LFO 1 module in the recipe. |
| L2 | PATCH MUTATOR: Include LFO 2 module in the recipe. |
| MM | PATCH MUTATOR: Include MATRIX MODULATION module in the recipe. |

---

## 21. SETTINGS — no modal furtive HELP (Wave 3 policy)

Settings is a modal over a dimmed main UI. Footer HELP is nearly unreadable behind that dimming, so **no furtive HELP** is registered on any Settings control, label, LED, or page (UI / MIDI / AUDIO / PATCH / MUTATOR / MASTER).

The CONTEXTUAL HELP SHOW/HIDE preference combo remains: it gates main-UI HELP only; it does not receive its own HELP bind.

---

## 22. ABOUT — no modal furtive HELP (Wave 3 policy)

About is a modal over a dimmed main UI. **No furtive HELP** on any About control or painted row (same readability policy as Settings).

---

## Wave 2 — Section headers and module title bands (2026-10-13, implemented — Standalone UAT pending)

Logo / logo-menu prefix retarget: `MATRIX-CONTROL:` (see section 1).

| Target | Message proposé |
|--------|-----------------|
| PATCH EDIT (section header) | PATCH EDIT: Edits the current patch and shows envelopes, Track Generator, and patch name. |
| MATRIX MODULATION (section header) | MATRIX MODULATION: Routes modulation sources to destinations across ten buses. |
| PATCH MANAGER (section header) | PATCH MANAGER: Navigates banks and patches, stores to memory or disk, imports or exports banks, and inits, copies, or pastes patches. |
| MASTER EDIT (section header) | MASTER EDIT: Edits global settings for all patches (Matrix-1000 only). |
| DCO 1 (module title) | DCO 1: Oscillator 1 controls for the current patch. |
| DCO 2 (module title) | DCO 2: Oscillator 2 controls for the current patch. |
| VCF/VCA (module title) | VCF/VCA: Filter and amplifier controls for the current patch. |
| FM/TRACK (module title) | FM/TRACK: Frequency modulation and Track Generator controls for the current patch. |
| RAMP/PORTAMENTO (module title) | RAMP/PORTAMENTO: Ramp generator and portamento controls for the current patch. |
| ENV 1 (module title) | ENV 1: Envelope 1 controls for the current patch. |
| ENV 2 (module title) | ENV 2: Envelope 2 controls for the current patch. |
| ENV 3 (module title) | ENV 3: Envelope 3 controls for the current patch. |
| LFO 1 (module title) | LFO 1: Low-frequency oscillator 1 controls for the current patch. |
| LFO 2 (module title) | LFO 2: Low-frequency oscillator 2 controls for the current patch. |
| PATCH NAME (module title) | PATCH NAME: Shows the 8-character patch name - double-click to rename when editable. |
| MIDI (module title) | MIDI: Global MIDI channel and related controls for the synthesizer. |
| VIBRATO (module title) | VIBRATO: Global vibrato controls for the synthesizer. |
| MISC (module title) | MISC: Global bend, pressure, tuning, memory protect, plus global Unison and Unison Detune. |
| BANK UTILITY (module title) | BANK UTILITY: Selects a bank and copies, pastes, imports, or exports it (Matrix-1000 only). |
| INTERNAL PATCHES (module title) | INTERNAL PATCHES: Browses and loads patches from the connected synthesizer memory. |
| COMPUTER PATCHES (module title) | COMPUTER PATCHES: Browses and loads patch files from a folder on disk. |
| PATCH MUTATOR (module title) | PATCH MUTATOR: Mutates the current patch and browses mutation history. |

---

## Wave 3 — modal HELP policy (2026-10-13)

Modal HELP (Settings / About): removed — see §21 / §22.

Standalone UAT: Settings/About open → no HELP; CONTEXTUAL HELP HIDE → centre HELP off; main-UI Wave 1–2 HELP still works after closing modals; hover §23 / §24.

---

## Hors perimetre (volontaire)

- Empty Patch Manager chrome; sticky left / severity dismiss; Ramp Master Override `M` badge; dialogs / Getting Started.
- Per-module inventory tables for Patch Edit / Master control rows still omit a dedicated Title row; Wave 2 summary above is the title SSOT until a doc pass expands those tables.
- Exhaustive unbound-control audit beyond Waves 1–3 centre/modals — tracked in deferred-work (Wave 3 review).

---

## 23. DEVICE (footer, bande droite)

| Controle | Message propose |
|----------|-----------------|
| DEVICE (+ modele + version EPROM) | DEVICE: Shows the connected synthesizer model and EPROM version, or connection status. |

---

## 24. Footer centre band (MIDI queue alert zone)

| Controle | Message propose |
|----------|-----------------|
| Centre band (empty or red alert) | MIDI: Shows a red alert when the MIDI send queue is backed up - click PANIC button to clear stuck notes. |

