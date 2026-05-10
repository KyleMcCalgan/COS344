# Feature: Transformations

> Scene rotations, light translation, and the reset command.

---

## Scene Rotation

The entire scene (sphere + plane, but NOT the camera) rotates when W/A/S/D/E/Q are pressed.

### Implementation

- Maintain three float accumulators: `sceneRotX`, `sceneRotY`, `sceneRotZ` (in degrees or radians).
- On each relevant key press, increment or decrement the appropriate accumulator by a small step
  (e.g. 5 degrees per press).
- Build the scene model matrix each frame by composing rotation matrices from these accumulators:
  ```
  modelMatrix = rotateZ(sceneRotZ) * rotateY(sceneRotY) * rotateX(sceneRotX)
  ```
- Upload `modelMatrix` as a uniform to the vertex shader.
- **No GLM matrix functions** — use Prac 1 matrix utilities.

### Rotation Matrix Construction (from Prac 1)

Each rotation matrix is a 4×4 matrix. Compose by multiplying:
- `rotateX(angle)` — rotation around X axis.
- `rotateY(angle)` — rotation around Y axis.
- `rotateZ(angle)` — rotation around Z axis.

The **order of multiplication** matters. Since the spec says each key rotates "the scene" around
a fixed axis (not cumulative), applying rotations in a fixed order (e.g. X then Y then Z) is
acceptable and avoids gimbal lock issues for the purposes of this practical.

### Important Rule (Rubric Penalty)

**Do not reset the scene to the origin between applying different rotations.**
Each new rotation key press should accumulate on top of the existing rotation. Moving the scene
back to centre before rotating costs 1 mark per transformation.

---

## Light Translation

The light moves in the **scene's local coordinate system** — not the global world axes.

### Implementation

- Maintain `lightLocalPos` as a `vec3` (starts at `(0, 0, 0)` — sphere centre).
- On each arrow key / `<` / `>` press, add a small delta (e.g. 0.1 units) to the appropriate
  local axis component.
- The light's world position for the shader is: `lightWorldPos = sceneModelMatrix * vec4(lightLocalPos, 1.0)`.
  - This automatically makes the light move relative to the rotated scene.
- Pass `lightWorldPos` as a uniform to the fragment shader each frame.

### Why Local Space Matters

If the scene has been rotated (e.g. 45° around Y), pressing the right arrow key should move
the light "rightward relative to the scene", not rightward in global X. Using the model matrix
to transform the local position handles this correctly without extra logic.

---

## Key Bindings (Transformations)

| Key | Action | Axis/Direction |
|-----|--------|----------------|
| `W` | Scene rotate | +X axis |
| `S` | Scene rotate | -X axis |
| `A` | Scene rotate | +Y axis |
| `D` | Scene rotate | -Y axis |
| `E` | Scene rotate | +Z axis |
| `Q` | Scene rotate | -Z axis |
| Up arrow | Light translate | +local Y |
| Down arrow | Light translate | -local Y |
| Left arrow | Light translate | -local X |
| Right arrow | Light translate | +local X |
| `>` | Light translate | +local Z |
| `<` | Light translate | -local Z |

---

## Reset Command (`Space`)

Resets all transformation and scene state to initial values.

### What Gets Reset

| State Variable | Reset Value |
|----------------|-------------|
| `sceneRotX` | 0.0 |
| `sceneRotY` | 0.0 |
| `sceneRotZ` | 0.0 |
| `lightLocalPos` | (0.0, 0.0, 0.0) |
| `sphereSubdivision` | default (e.g. 16) |
| `planeResolution` | default (e.g. 8) |
| `floorColourIndex` | 0 |
| `ballColourIndex` | 0 |
| `lightColourIndex` | 0 |
| `ballAlpha` | 0.5 |
| `colourTexEnabled` | false |
| `displacementTexEnabled` | false |
| `alphaTexEnabled` | false |
| `wireframeMode` | false |

### Mesh Regeneration on Reset

If `sphereSubdivision` or `planeResolution` changed since startup, the reset must regenerate
the meshes at their default values. Call the same regenerate functions used by the subdivision keys.

---

## MVP Matrix Construction

Each frame, the vertex shader receives a combined MVP matrix:

```
MVP = projectionMatrix * viewMatrix * modelMatrix
```

Where:
- `projectionMatrix` — perspective projection, built once at startup (or on window resize).
- `viewMatrix` — camera look-at matrix, fixed (camera does not move).
- `modelMatrix` — changes every frame based on `sceneRotX/Y/Z`.

All three are built using Prac 1 matrix utilities. Upload as a single `uniform mat4 MVP` to the
vertex shader. Optionally pass `modelMatrix` separately for world-space lighting calculations.

---

## Key Decisions to Log in `progress.md`

- [ ] Rotation step size per key press (degrees).
- [ ] Light translation step size per key press (world units).
- [ ] Rotation composition order (X, Y, Z vs Y, X, Z etc.).
- [ ] Camera position and orientation.
- [ ] Perspective projection FOV and near/far planes.
