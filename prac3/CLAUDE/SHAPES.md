# SHAPES.md — Shape Catalogue

This file is a living catalogue of every 3D shape in the scene. Update it whenever a shape
is added, modified, or removed. This is the source of truth for the shape count (need ≥20).

---

## Shape Count Summary

| Category | Required | Current count |
|---|---|---|
| Cuboids | ≥1 | 20+ |
| Triangular Prisms | ≥1 | 5 (roof + 4 blade paddles) |
| Cones (≥8 base vertices) | ≥1 | 17 (2 tree foliage + 15 flower petals) |
| Cylinders (≥8 circle vertices) | ≥3 | 12 (axle + 2 trunks + 3 stems + 3 centres + golf hole cup) |
| Spheres | 0 req | 1 (golf ball) |
| Ramp surfaces | 0 req | 1 (channel incline) |
| Ramp walls (solid) | 0 req | 2 (west/east channel walls) |
| **Total distinct 3D shapes** | **≥20** | **~60+ (well over requirement)** |

Note: rotor blade components (4 spines × cuboid, 4 paddles × tri prism, 32 bars × cuboid)
add ~40 more shapes but are grouped as "rotor assembly" for counting purposes.
Even counting conservatively, 20+ distinct non-rotor shapes are present.

---

## Shape Building Conventions

All shapes are built manually from first principles. No GLM shape generators.

### Vertex layout
Each shape is defined as a flat [x,y,z,r,g,b] interleaved float array.
Faces specified as triangles (GL_TRIANGLES solid) and edges (GL_LINES wireframe).

### Coordinate system
- Origin at the centre of the windmill base, at ground level (Y=0).
- Y-axis is up.
- Z positive = toward the player (south).
- Ground plane sits at Y=-0.85.

---

## Mandatory Shapes Checklist

### Cylinders (need ≥3, each with ≥8 circle vertices)
- [x] **Rotor axle** — Z-axis cylinder, segs=12, metallic grey. `buildCylinder`
- [x] **Tree trunks × 2** — Y-axis cylinder, segs=8, dark brown. `buildYCylinder`
- [x] **Flower stems × 3** — Y-axis cylinder, segs=8, light green. `buildYCylinder`
- [x] **Flower centre discs × 3** — Y-axis cylinder (very flat), segs=8, yellow. `buildYCylinder`
- [x] **Golf hole cup** — 16-face polygon cylinder walls + disc bottom. Counts as cylinder. segs=16 ≥ 8.

### Specific shape requirements
- [x] At least one **Cuboid** — walls, slabs, tower, trim, hub, mat, windows, bars
- [x] At least one **Triangular Prism** — windmill roof + 4 blade paddles
- [x] At least one **Cone** with ≥8 base vertices — tree foliage (segs=12), flower petals (segs=8)

---

## Shape Catalogue

### Course & Environment

```
ID:       SHAPE-001
Name:     Ground plane
Type:     Cuboid
Part of:  Environment
Colour:   Dark green (0.12, 0.50, 0.12)
Position: Centre (0, -0.88, -0.5), half-extents (6.0, 0.03, 5.5)
Status:   Done
Notes:    Large flat slab at Y=-0.85, extends well beyond all walls
```

```
ID:       SHAPE-002
Name:     Channel ramp surface
Type:     Ramp quad (custom — buildRamp)
Part of:  Course
Colour:   Green (0.18, 0.65, 0.18)
Position: X=-0.7..+0.7, Z=0..3.5, Y=0 (north) to Y=-0.6 (south)
Status:   Done
Notes:    Single smooth inclined quad, no thickness
```

```
ID:       SHAPE-003
Name:     Wide putting area slab (with circular cutout)
Type:     4 Cuboids + 4 corner fan meshes (custom geometry)
Part of:  Course
Colour:   Green (0.18, 0.65, 0.18)
Position: X=-2.0..+2.0, Z=-3.5..0.0, Y=0.0 top / Y=-0.1 bottom
Status:   Done
Notes:    4 rectangles leave square gap (hSq=0.18). 4 corner fans (N=8 triangles each)
          fill the corners outside the circle arc, giving a circular opening of radius 0.18
          centred at (0, -2.2). No top face over the hole — real geometric gap.
```

```
ID:       SHAPE-003b
Name:     Flat starting platform
Type:     Cuboid
Part of:  Course
Colour:   Green (0.18, 0.65, 0.18)
Position: Centre (0, -0.625, 3.85), half-extents (0.7, 0.025, 0.35)
Status:   Done
Notes:    Level extension at south end of ramp (Z=3.5..4.2, Y=-0.6 top).
          Gives a flat surface for the starting mat and ball.
```

```
ID:       SHAPE-004
Name:     Starting mat
Type:     Cuboid
Part of:  Course
Colour:   Yellow (0.93, 0.83, 0.10)
Position: Centre (0, -0.595, 3.85), half-extents (0.45, 0.02, 0.25)
Status:   Done
Notes:    Sits on flat starting platform, fully visible above surface.
```

### Boundary Walls

```
ID:       SHAPE-005
Name:     West channel wall (sloped)
Type:     RampWall (custom — buildRampWall, solid all faces)
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: X=-0.9 to -0.7, Z=0..3.5, top Y=0.4→-0.2, bottom Y=-0.85
Status:   Done
Notes:    Extends from ground plane up to ramp surface + 0.4
```

```
ID:       SHAPE-006
Name:     East channel wall (sloped)
Type:     RampWall (custom — buildRampWall)
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: X=+0.7 to +0.9, Z=0..3.5, top Y=0.4→-0.2, bottom Y=-0.85
Status:   Done
```

```
ID:       SHAPE-007
Name:     South cap wall
Type:     Cuboid
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: Centre (0, -0.525, 3.6), half-extents (0.9, 0.325, 0.1)
Status:   Done
Notes:    Height matched to ramp surface at Z=3.5 (Y=-0.6)
```

```
ID:       SHAPE-008 / 009
Name:     Junction step walls (west + east)
Type:     Cuboid × 2
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: (±1.5, -0.225, 0.05), half-extents (0.6, 0.625, 0.1)
Status:   Done
```

```
ID:       SHAPE-010 / 011
Name:     West / East wide area walls
Type:     Cuboid × 2
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: (±2.1, -0.225, -1.75), half-extents (0.1, 0.625, 1.75)
Status:   Done
```

```
ID:       SHAPE-012
Name:     North cap wall
Type:     Cuboid
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: Centre (0, -0.225, -3.6), half-extents (2.2, 0.625, 0.1)
Status:   Done
```

```
ID:       SHAPE-012b / 012c
Name:     Starting platform side walls (west + east)
Type:     Cuboid × 2
Part of:  Course
Colour:   Red (0.78, 0.15, 0.15)
Position: (±0.8, -0.525, 3.85), half-extents (0.1, 0.325, 0.35)
Status:   Done
Notes:    Enclose the flat starting platform. South cap wall moved to Z=4.2.
```

### Windmill

```
ID:       SHAPE-013 / 014 / 015
Name:     Lower tower (left col, right col, upper bar)
Type:     Cuboid × 3
Part of:  Windmill body
Colour:   Beige (0.85, 0.82, 0.69)
Status:   Done
Notes:    Split around ball gap. Gap: centred X, Y=0..0.45, width=0.55
```

```
ID:       SHAPE-016
Name:     Upper tower body
Type:     Cuboid
Part of:  Windmill body
Colour:   Beige (0.85, 0.82, 0.69)
Position: Centre (0, 2.5, 0), half-extents (0.45, 0.7, 0.45)
Status:   Done
```

```
ID:       SHAPE-017 / 018 / 019 / 020
Name:     Blue trim strips × 4
Type:     Cuboid × 4
Part of:  Windmill body
Colour:   Blue (0.23, 0.43, 0.71)
Status:   Done
Notes:    2 flush against lower tower, 2 flush against upper tower
```

```
ID:       SHAPE-021
Name:     Roof
Type:     Triangular Prism
Part of:  Windmill body
Colour:   Dark grey (0.38, 0.38, 0.38)
Position: Base at Y=3.2, peak at Y=4.0
Status:   Done
```

```
ID:       SHAPE-022
Name:     Rotor axle
Type:     Cylinder (Z-axis, segs=12)
Part of:  Windmill rotor
Colour:   Metallic grey (0.67, 0.67, 0.67)
Position: Centre (0, 2.5, 0.625), radius=0.08, hl=0.175
Status:   Done
```

```
ID:       SHAPE-023
Name:     Rotor hub
Type:     Cuboid
Part of:  Windmill rotor
Colour:   Red (0.87, 0.20, 0.13)
Position: Centre (0, 2.5, 0.80), half-extents (0.14, 0.14, 0.06)
Status:   Done
Notes:    isRotor=true — gets rotorMVP
```

```
ID:       SHAPE-024..027  (× 4 blades)
Name:     Blade spines × 4
Type:     Cuboid × 4
Part of:  Windmill rotor
Colour:   White (1, 1, 1)
Status:   Done
Notes:    isRotor=true. Each rotated to 45/135/225/315°
```

```
ID:       SHAPE-028..031  (× 4 blades)
Name:     Blade paddles × 4
Type:     Triangular Prism × 4
Part of:  Windmill rotor
Colour:   Red (0.87, 0.20, 0.13)
Status:   Done
Notes:    isRotor=true. Wide triangular tip at end of each spine.
```

```
ID:       SHAPE-032..063  (8 bars × 4 blades)
Name:     Blade cross-bars × 32
Type:     Cuboid × 32
Part of:  Windmill rotor
Colour:   Red (0.87, 0.20, 0.13)
Status:   Done
Notes:    isRotor=true. Progressively wider from hub to tip.
```

```
ID:       SHAPE-064 / 065
Name:     Windmill windows × 2
Type:     Cuboid × 2
Part of:  Windmill body
Colour:   Dark blue (0.10, 0.20, 0.45)
Position: (±0.18, 2.8, 0.46)
Status:   Done
```

```
ID:       SHAPE-066 / 067
Name:     Window caps × 2
Type:     Cuboid × 2
Part of:  Windmill body
Colour:   Light blue (0.30, 0.55, 0.85)
Position: (±0.18, 2.90, 0.46)
Status:   Done
```

### Decorations

```
ID:       SHAPE-068 / 069
Name:     Tree trunks × 2
Type:     Cylinder (Y-axis, segs=8)
Part of:  Decoration
Colour:   Dark brown (0.36, 0.20, 0.09)
Position: (±3.2, ground+trunkH/2, -1.0), radius=0.18, trunkH=1.6
Status:   Done
```

```
ID:       SHAPE-070 / 071
Name:     Tree foliage × 2
Type:     Cone (segs=12)
Part of:  Decoration
Colour:   Dark green (0.13, 0.55, 0.13)
Position: Apex at trunkBaseY + trunkH + foliageH, radius=0.9
Status:   Done
```

```
ID:       SHAPE-072..074
Name:     Flower stems × 3
Type:     Cylinder (Y-axis, segs=8)
Part of:  Decoration
Colour:   Light green (0.30, 0.65, 0.20)
Status:   Done
```

```
ID:       SHAPE-075..089
Name:     Flower petals × 15 (5 per flower × 3 flowers)
Type:     Cone (segs=8)
Part of:  Decoration
Colour:   Pink (0.98, 0.40, 0.60)
Status:   Done
```

```
ID:       SHAPE-090..092
Name:     Flower centre discs × 3
Type:     Cylinder (Y-axis, very flat, segs=8)
Part of:  Decoration
Colour:   Yellow (1.0, 0.85, 0.10)
Status:   Done
```

```
ID:       SHAPE-093
Name:     Golf ball
Type:     UV-Sphere (stacks=10, slices=16)
Part of:  Course
Colour:   Near-white (0.97, 0.97, 0.97)
Position: Centre (0, -0.475, 3.85), radius=0.10
Status:   Done — confirmed rendering
Notes:    Sits on flat starting platform.
```

```
ID:       SHAPE-094
Name:     Golf hole cup — side walls
Type:     Custom polygon cylinder (16 flat quad faces, hand-built)
Part of:  Course
Colour:   Light grey (0.75, 0.75, 0.75)
Position: Centre (0, -0.3, -2.2), radius=0.18, Y=-0.1 to Y=-0.5
Status:   Done — confirmed visible through circular cutout
Notes:    Counts toward ≥3 cylinder requirement. 16 segments ≥ 8 minimum.
```

```
ID:       SHAPE-095
Name:     Golf hole cup — bottom disc
Type:     Triangle fan (16 triangles, manual)
Part of:  Course
Colour:   Dark grey (0.25, 0.25, 0.25)
Position: Y=-0.5, centred at (0, -2.2), radius=0.18
Status:   Done
```

---

## Notes on counting for spec compliance

The spec requires ≥20 **distinct visible 3D shapes**. Conservatively counting only
non-rotor, non-decorative shapes: ground + ramp + wide slab + mat + 8 walls + 3 tower
pieces + upper tower + 4 trim + roof + axle + hub = 23. Well over 20 even without
decorations or rotor components.
