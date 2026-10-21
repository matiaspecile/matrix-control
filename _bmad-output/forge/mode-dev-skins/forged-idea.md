# Forged idea — skins V1 / modèle SSOT

## Locked

- **Pivot** : pas de GUI Matrix-Control pixel-près dans Skin Studio HTML pour V1.
- **Chantier prioritaire** : refonte **modèle** dans Matrix-Control — palette SSOT, mappings, états ; zéro couleur locale sur le chemin GUI utilisateur V1 (Patch Edit, chrome, dialogs, Settings, wizards). Tests / outils Dev exclus de la porte.
- **Ordre** : modèle d’abord → éditeur auteur ensuite → peaufinage Black/Cream (contrastes ; Cream au-delà du fond) → contributeurs Dev = après V1.
- **Branche dédiée** ; alpha possible sur trunk actuel si dérapage.
- **Deadline modèle** : **2026-11-15** prêt et fonctionnel. Sinon **parque total** (pas de shrink merge, pas de glissement alpha). Reprise 2027.
- **Parité pendant migration** : « assez bonne » — micro-écarts OK si cohérence SSOT (unifier divergences déjà là) ; pas d’identité pixel stricte.
- **Skin Studio** : **gelé** (pas Lot C). Réutiliser plus tard sidebar / PhotoCraft. Plus de double entretien HTML↔JUCE pour V1.
- **Forme éditeur** (in-app flag compile vs app télécommande IPC) : **reportée** après porte modèle.
- **Noël** : viser modèle + éditeur + Black/Cream « fier » si vitesse OK ; filet = modèle seul ou parque + Black/Cream actuels (déjà viables alpha/V1).
- **Calendrier produit** : post-15 nov → ~2–3 sem. alpha → ~3–4 sem. bêta → Noël.

## Rejected

- Pousser Skin Studio vers GUI complète / pixel-près navigateur.
- Éditeur live sur modèle sale (raccourci avant SSOT).
- Shrink merge d’un SSOT partiel dans V1 si deadline ratée.
- Allonger la phase modèle en mangeant l’alpha.
- Figer in-app vs télécommande avant le modèle.
- Contributeurs infographie comme exigence V1.

## Why (downstream)

- Besoin réel = juger le **vrai** paint JUCE ; mock HTML plafonne ; double entretien coûte cher.
- Éditeur ne remplace pas la refonte modèle (déjà Étape 2 du brief Skin Studio).
- Black/Cream shippables sans outil auteur → Noël ≠ otage de l’éditeur.

## Next

- **`bmad-spec`** : refonte modèle SSOT Matrix-Control (porte 15 nov, parité assez bonne, hors éditeur).
- `bmad-correct-course` : seulement si gel Skin Studio doit être formalisé dans un planning/PRD versionné ailleurs.
- Pas de `bmad-build` éditeur tant que la porte modèle n’est pas franchie.
