# Part 5 Screenshots — what to capture

All shader code is in `shaders/` and has been written. After rebuild:

```
cd build && make -j
./clothsim -f ../scene/sphere.json
./clothsim -f ../scene/pinned4.json    # for some screenshots where flat cloth looks better
```

In the GUI, top-left **Appearance** dropdown selects the shader. The ones we care about:
- Normal (the default fallback — not used for Part 5 screenshots)
- Diffuse
- Phong
- Texture
- Bump
- Displacement
- Mirror

Save screenshots directly into `hw4/images/part5/` with the exact filenames below.

---

## Task 2 — Blinn-Phong components (4 screenshots)

The full shader lives in `shaders/Phong.frag`. To isolate each component, open that file and swap the active `L = ...` line as noted in the inline comments. Rebuild after each swap (`make -j`; shaders are loaded at program start).

Scene: `sphere.json` (looks best — specular highlights are obvious on the sphere itself).

1. **`phong_ambient.png`** — edit Phong.frag, set `vec3 L = L_a;`, rebuild, screenshot sphere scene after cloth settles.
2. **`phong_diffuse.png`** — set `vec3 L = L_d;`, rebuild, screenshot.
3. **`phong_specular.png`** — set `vec3 L = L_s;`, rebuild, screenshot. You should see only bright highlights on cloth+sphere surfaces facing the light halfway vector.
4. **`phong_full.png`** — set `vec3 L = L_a + L_d + L_s;` (default), rebuild, screenshot.

**After you are done, make sure Phong.frag is left set to `L = L_a + L_d + L_s;`.**

---

## Task 3 — Texture mapping with custom texture (1 screenshot)

Spec requires you modify a file in `textures/`. Easiest path:

1. Pick any image you like (meme, photo, pattern). Resize to square, save as PNG.
2. Overwrite `textures/texture_1.png` with it. Keep the same filename.
3. Rebuild is NOT required (textures are loaded at runtime), just restart the simulator.
4. Run `./clothsim -f ../scene/pinned4.json`, select **Texture** shader. After cloth settles, screenshot.

**File:** `texture_custom.png`

---

## Task 4 — Bump & Displacement (3 + 4 screenshots)

Use the same NON-DEFAULT texture for all 7 of these so the comparison is fair. Spec explicitly says: "BUT choose one that's not the default texture_2.png."

Recommended: use `texture_3.png` or `texture_4.png` by renaming one to `texture_2.png` temporarily, OR edit the shaders to sample `u_texture_3` instead. Easiest: copy `texture_3.png` over `texture_2.png` (back up the original first!).

```
cp textures/texture_2.png textures/texture_2.png.bak
cp textures/texture_3.png textures/texture_2.png
```

### Same-shader, same-texture set (3 shots)

Sphere scene, default coarseness (no `-o/-a` flags):

5. **`bump_cloth.png`** — `./clothsim -f ../scene/pinned4.json`, select **Bump**, settle, screenshot.
6. **`bump_sphere.png`** — `./clothsim -f ../scene/sphere.json`, select **Bump**, settle, screenshot.
7. **`displacement_sphere.png`** — `./clothsim -f ../scene/sphere.json`, select **Displacement**, settle, screenshot.

### Sphere-coarseness comparison (4 shots)

Spec: "Compare how your the two shaders react to the sphere by changing the sphere mesh's coarseness by using -o 16 -a 16 and then -o 128 -a 128."

8. **`bump_sphere_16.png`** — `./clothsim -f ../scene/sphere.json -o 16 -a 16`, **Bump**, screenshot.
9. **`bump_sphere_128.png`** — `./clothsim -f ../scene/sphere.json -o 128 -a 128`, **Bump**, screenshot.
10. **`disp_sphere_16.png`** — `./clothsim -f ../scene/sphere.json -o 16 -a 16`, **Displacement**, screenshot.
11. **`disp_sphere_128.png`** — `./clothsim -f ../scene/sphere.json -o 128 -a 128`, **Displacement**, screenshot.

**After:** restore the texture: `cp textures/texture_2.png.bak textures/texture_2.png`

---

## Task 5 — Mirror (2 screenshots)

12. **`mirror_cloth.png`** — `./clothsim -f ../scene/pinned4.json`, select **Mirror**, settle, screenshot. Cloth should reflect the cubemap skybox.
13. **`mirror_sphere.png`** — `./clothsim -f ../scene/sphere.json`, select **Mirror**, settle, screenshot.

---

## Total: 13 screenshots

File list for reference:
- phong_ambient.png
- phong_diffuse.png
- phong_specular.png
- phong_full.png
- texture_custom.png
- bump_cloth.png
- bump_sphere.png
- displacement_sphere.png
- bump_sphere_16.png
- bump_sphere_128.png
- disp_sphere_16.png
- disp_sphere_128.png
- mirror_cloth.png
- mirror_sphere.png

(14 actually — I miscounted. 4 Phong + 1 Texture + 3 bump/disp same-tex + 4 coarseness + 2 mirror = 14.)

## After all screenshots are captured

Push them to the webpage repo and I'll wire them into `index.html` with the Part 5 prose.
