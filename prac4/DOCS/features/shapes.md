# Feature: Shapes

> Sphere (golf ball) and Plane (floor) — generation, subdivision, and runtime adjustment.

---

## Sphere

### Generation Strategy: UV Sphere (Stacks × Slices)

The sphere is generated using the standard latitude/longitude (stacks × slices) approach.

- **Stacks** = horizontal rings from top pole to bottom pole.
- **Slices** = vertical segments around the circumference.
- Both controlled by a single integer `subdivisionLevel`. For example, `subdivisionLevel = 16`
  means 16 stacks and 16 slices. Increasing/decreasing this one value changes both.
- Alternatively, stacks and slices can be tracked separately — document which is chosen in `progress.md`.

### Vertex Data Per Vertex

Each vertex stores:
- Position `(x, y, z)` — computed from polar coordinates.
- Normal `(nx, ny, nz)` — for a unit sphere, normal equals position (normalised).
- UV `(u, v)` — `u = slice / totalSlices`, `v = stack / totalStacks`.

### Triangle Generation

For each quad formed by adjacent stacks/slices, emit two triangles. Handle poles separately
(they produce single triangles, not quads) to avoid degenerate geometry.

### Wireframe Index Buffer

After generating the triangle indices, derive an additional edge index buffer:
- For each triangle, extract 3 edges as line pairs.
- Deduplicate shared edges so each edge draws once.
- Store as a separate EBO used only in wireframe mode.

### Runtime Adjustment

- Two keys (e.g. `I` / `K`) increase/decrease `subdivisionLevel`.
- A minimum of 3 prevents degenerate geometry.
- On change: delete old VAO/VBO/EBO, regenerate vertex + index data, upload to GPU.

### Positioning

- Sphere centred at world origin `(0, 0, 0)`.
- Rests on the plane, so the plane is positioned at `Y = -radius`.

---

## Plane (Floor)

### Generation Strategy: Grid of Quads

The plane is a flat N × N grid of quads lying in the XZ plane (Y = -radius of sphere).

- `planeResolution` = N controls how many quads along each axis.
- Total quads = N × N. Total triangles = 2 × N × N.
- Increasing N adds more vertices, which:
  - Improves the appearance of the light gradient on the floor (more vertices = smoother Phong).
  - Satisfies the spec requirement to observe the effect of vertex count on render quality.

### Vertex Data Per Vertex

Each vertex stores:
- Position `(x, 0, z)` — evenly spaced across the plane's extent. Y is fixed at `-radius`.
- Normal `(0, 1, 0)` — flat plane, all normals point up.
- UV `(u, v)` — normalised position across the grid.

### Wireframe Index Buffer

Same approach as sphere — derive edge pairs from triangle indices, deduplicate, store as EBO.

### Runtime Adjustment

- Two keys (e.g. `O` / `L`) increase/decrease `planeResolution`.
- Minimum of 1 (a single quad).
- On change: delete old buffers, regenerate, upload.

### Plane Extent

- Choose a fixed world-space extent, e.g. `-5.0` to `5.0` in X and Z.
- The sphere (radius ~1.0) sits in the centre, touching the plane at the bottom.

---

## Key Decisions to Log in `progress.md`

- [ ] Chosen key bindings for sphere and plane subdivision.
- [ ] Whether stacks and slices are tied to one variable or tracked separately.
- [ ] Default subdivision value for sphere and default resolution for plane.
- [ ] Sphere radius (e.g. 1.0).
- [ ] Plane extent (e.g. ±5.0 in X and Z).
