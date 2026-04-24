# Part 4 Screenshots — what to capture

**Scene:** `scene/selfCollision.json`

**Default params** (already in the JSON, shown in the GUI):
- damping: 0.2
- density: 150
- ks: 5000
- thickness: 0.0095
- gravity: (0, -9.8, 0)

Launch with:

```
./clothsim -f ../scene/selfCollision.json
```

In the GUI: Appearance panel top-left, select shading mode **Normal** (the shaded default that works without Phong etc.). Press P to play / pause. Press R to restart.

## 7 screenshots required by the rubric

All filenames below go directly in `hw4/images/part4/`. Use native OS screenshot tool — no hotkey in-app.

### A. Time progression (3 images, default params)

Restart (R) then unpause (P) and count seconds. Pause (P) and screenshot at roughly these moments.

1. **`self_early.png`** — early self-collision
   - ~1–2 seconds after release.
   - Cloth has fallen partway, first folds on itself visible (the cloth starts folding like an accordion).
2. **`self_mid.png`** — mid fall
   - ~3–5 seconds.
   - Cloth has substantial folding, some of it hitting the ground plane.
3. **`self_rest.png`** — resting state
   - ~10–15 seconds.
   - Cloth is flattened on the ground but folds are still visible; slight residual bounciness is OK (spec says so).

### B. Density variation (2 images)

Restart each time. Change density in the GUI before pressing P.

4. **`self_density_1.png`** — density = 1 (very light cloth)
   - Pause at ~3 seconds. Should flutter a lot, fold softly.
5. **`self_density_500.png`** — density = 500 (heavy cloth)
   - Pause at ~3 seconds. Should fall faster, stretch more, fewer sharp folds.

### C. ks variation (2 images)

Restart each time. Change ks in the GUI before pressing P. Keep density at default 150.

6. **`self_ks_500.png`** — ks = 500 (weak springs)
   - Pause at ~3 seconds. Cloth stretches way more, looks "droopy", bigger folds.
7. **`self_ks_50000.png`** — ks = 50000 (stiff springs)
   - Pause at ~3 seconds. Cloth looks rigid, small tight folds, barely deforms.

## How to screenshot on macOS

- `Cmd+Shift+4`, then drag the region around the simulator window (exclude the title bar if you want).
- Or `Cmd+Shift+4` then Space then click the window.
- Save to Desktop by default; drop into `hw4/images/part4/` with the filenames above.

## On Windows

- Snipping Tool (`Win+Shift+S`) to capture a region.

## Sanity checks after running

- Cloth should **fold on itself** rather than clip through.
- At rest it may continue to slowly flatten due to lack of damped spring forces — that's expected per spec, don't worry about it.
- If the cloth explodes, thickness is likely wrong or `hash_position` is returning the same key for everything. Should not happen with the code I wrote — but if it does, check that `num_width_points` / `num_height_points` haven't been set to 0 somewhere.
