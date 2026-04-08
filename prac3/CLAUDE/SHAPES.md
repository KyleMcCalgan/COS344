# SHAPES.md — Shape Catalogue

This file is a living catalogue of every 3D shape in the scene. Update it whenever a shape
is added, modified, or removed. This is the source of truth for the shape count (need ≥20).

---

## Shape Count Summary

| Category | Required | Current count |
|---|---|---|
| Cuboids | ≥1 | 0 |
| Triangular Prisms | ≥1 | 0 |
| Cones (≥8 base vertices) | ≥1 | 0 |
| Cylinders (≥8 circle vertices) | ≥3 | 0 |
| **Total distinct 3D shapes** | **≥20** | **0** |

---

## Shape Building Conventions

All shapes are built manually from first principles. No GLM shape generators.

### Vertex layout
Each shape is defined as a list of 3D vertices (x, y, z in local space). Faces are specified
as triangles (for solid GL_TRIANGLES rendering) and edges (for wireframe GL_LINES rendering).

### Per-shape data stored in code
```
vertices[]     — flat float array of (x,y,z) positions
triIndices[]   — indices for GL_TRIANGLES (solid)
lineIndices[]  — indices for GL_LINES (wireframe edges)
color          — vec3 RGB colour for this shape
VAO, VBO, EBO  — OpenGL handles
```

### Coordinate system
- Origin at the centre of the course base.
- Y-axis is up.
- The scene is viewed from a fixed perspective camera (isometric-ish angle).

---

## Mandatory Shapes Checklist

### Cylinders (need ≥3, each with ≥8 circle vertices)
- [ ] **Rotor axle** — connects rotor to windmill body. Metallic colour.
- [ ] **Golf hole** — circular hole cutout in the course surface. Distinct dark colour.
- [ ] **Third cylinder** — TBD (can be decorative or part of windmill).

### Specific shape requirements
- [ ] At least one **Cuboid** (e.g. windmill body sections, course walls, starting mat)
- [ ] At least one **Triangular Prism** (e.g. windmill roof)
- [ ] At least one **Cone** with ≥8 base vertices (e.g. windmill cap, decorative tree)

---

## Shape Catalogue

> Entries are added here as shapes are implemented.

### [TEMPLATE — copy this block for each shape]
```
ID:          SHAPE-XXX
Name:        Descriptive name
Type:        Cuboid | Triangular Prism | Cone | Cylinder | Other
Part of:     Windmill body | Windmill rotor | Course | Decoration
Colour:      RGB (r, g, b) and name
File:        Which .cpp file defines this shape's geometry
Transform:   Local offset/scale from origin before scene transform
Status:      Not started | In progress | Done
Notes:       Any quirks, decisions, or issues
```

---

<!-- Add shape entries below this line -->
