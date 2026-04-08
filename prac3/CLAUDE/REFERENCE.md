# REFERENCE.md — Windmill Reference & Planning

This file documents the chosen real-world mini-golf windmill reference, the planning
annotations, and the mapping from real-world shapes to 3D geometry. It must be updated
once a reference photo is chosen, and the planning photo must be included in the submission.

---

## Status

**Reference photo chosen:** ❌ NO — must be chosen before significant windmill geometry is built.

---

## Instructions (to complete this section)

1. Find a photo of a real-world **miniature golf course windmill** (not a farm windmill).
   - Good sources: Google Images "mini golf windmill obstacle", Pinterest, course websites.
   - Choose one that is clearly a mini-golf windmill with a gap for the ball.
   - Save the photo as `reference_windmill.jpg` (or similar) in the project root.

2. Annotate the photo with outlines showing which 3D primitive maps to which part
   (like Figure 17 in the spec — draw boxes around sections labelling them as cuboids,
   prisms, cones, cylinders, etc.).
   - Save annotated version as `reference_windmill_annotated.jpg`.

3. Fill in the sections below.

---

## Chosen Windmill

**Photo filename:** *(not yet chosen)*
**Source URL:** *(not yet chosen)*
**Description:** *(not yet chosen)*

---

## Shape Mapping (fill in once reference is chosen)

Map each visible part of the real windmill to a 3D primitive:

| Real-world part | 3D primitive | Notes |
|---|---|---|
| Main tower body | Cuboid(s) | Stacked/tapered sections |
| Roof / cap | Triangular Prism or Cone | Depends on windmill style |
| Rotor axle | Cylinder | Metallic colour, connects hub to tower |
| Rotor hub | Cylinder or Cuboid | Centre of the spinning assembly |
| Blades (×4+) | Cuboid / flat prism | Long thin rectangles, angled |
| Ball passage gap | (negative space) | Gap through the tower base |
| *(add more rows as needed)* | | |

---

## Planning Photo Requirements (from spec §4.7)

The spec requires submitting:
- A reference photo of the real-world windmill.
- An annotated/augmented version showing the planning of the 3D low-polygon rendering
  (similar to Figure 17 in the PDF — outlines drawn over the photo).

These photos must be:
- Included in the submission archive (zip).
- Shown to the tutor during the demo.
- Worth **6 marks** — 0 if missing.

---

## Colour Palette (fill in once reference is chosen)

Derive the colour palette from the reference photo:

| Element | Real colour | OpenGL RGB | Hex |
|---|---|---|---|
| Windmill tower | — | — | — |
| Roof | — | — | — |
| Blades | — | — | — |
| Axle | Metallic grey | (0.7, 0.7, 0.75) | #B2B2BF |
| Grass | Green | (0.2, 0.7, 0.2) | #33B233 |
| Boundary walls | — | — | — |
| Starting mat | — | — | — |
| Background | — | — | — |
| Golf hole | Dark | (0.1, 0.1, 0.1) | #1A1A1A |
