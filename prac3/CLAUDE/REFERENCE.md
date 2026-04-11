# REFERENCE.md — Windmill Reference & Planning

This file documents the chosen real-world mini-golf windmill reference, the planning
annotations, and the mapping from real-world shapes to 3D geometry. It must be updated
once a reference photo is chosen, and the planning photo must be included in the submission.

---

## Status

**Reference photo chosen:** ✅ YES — `windmill.jpg` in project root.
**Annotated photo:** ❌ NOT DONE — required for 6 marks, must be created before submission.

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

**Photo filename:** `windmill.jpg`
**Description:** Classic mini-golf windmill. Blue body with grey/beige tower sections, red boundary fence. Rotor has 4 blades — each blade is a narrow white spine with progressively wider red cross-bars (ladder pattern) and a wide red triangular paddle at the tip. Red square hub at centre. Roof is a dark triangular prism. Overall style closely matches our implementation.

---

## Shape Mapping

| Real-world part | 3D primitive | Implemented |
|---|---|---|
| Lower tower body | Cuboid (split around ball gap) | ✅ |
| Upper tower body | Cuboid (narrower) | ✅ |
| Blue side trim strips | 4 thin cuboids | ✅ |
| Roof | Triangular Prism | ✅ |
| Rotor axle | Cylinder (Z-axis, 12 segs) | ✅ |
| Rotor hub | Cuboid (red square) | ✅ |
| Blade spine | Thin white cuboid | ✅ |
| Blade cross-bars (×8) | Red cuboids, progressive width | ✅ |
| Blade paddle tip | Red triangular prism | ✅ |
| Ball passage gap | Void in lower tower | ✅ |
| Windows | Flat cuboids on tower face | ⬜ Planned |

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
| Windmill tower (beige) | Cream/beige | (0.85, 0.82, 0.69) | #D9D1B0 |
| Blue trim strips | Royal blue | (0.23, 0.43, 0.71) | #3B6EB5 |
| Roof | Dark grey | (0.38, 0.38, 0.38) | #616161 |
| Blade spine | White | (1.0, 1.0, 1.0) | #FFFFFF |
| Blade bars + paddle | Red | (0.87, 0.20, 0.13) | #DE3321 |
| Axle | Metallic grey | (0.67, 0.67, 0.67) | #ABABAB |
| Grass | Green | (0.18, 0.65, 0.18) | #2EA62E |
| Boundary walls | Red | (0.78, 0.15, 0.15) | #C72626 |
| Starting mat | Yellow | (0.93, 0.83, 0.10) | #EDD41A |
| Background | Sky blue | (0.53, 0.81, 0.98) | #87CEF9 |
| Hole walls | Light grey | (0.72, 0.72, 0.72) | #B8B8B8 |
| Hole floor | Dark grey | (0.20, 0.20, 0.20) | #333333 |
