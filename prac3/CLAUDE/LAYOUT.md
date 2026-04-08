# LAYOUT.md — Scene Layout Reference

This document describes the exact spatial layout of the mini-golf course and windmill
from two perspectives: front view (looking north) and top-down view (bird's eye).
It is the authoritative reference for reconstructing the 3D scene in OpenGL.

Read this before building any geometry. Every dimension and relationship described
here must be honoured in the 3D implementation.

---

## Coordinate System Convention

- **X-axis** — horizontal, positive pointing right (east)
- **Y-axis** — vertical, positive pointing up
- **Z-axis** — depth, positive pointing toward the viewer (south)

The scene origin (0, 0, 0) sits at the **centre of the windmill base**, at ground level.
All positions below are expressed relative to this origin.

---

## 1. Top-Down Layout (Bird's Eye View)

```
                        N  (far end, away from player)
                        |
          +-------------+-------------+
          |                           |
          |       WIDE PUTTING AREA   |
          |                           |
          |   (hole is up here, ~2/3  |
          |    of the way north)      |
          |                           |
          +----+               +------+
               |   WINDMILL    |
               | [  ] gap [ ]  |   <- windmill straddles the junction
               +----+     +----+
                    |     |
                    |     |        <- narrow channel walls
                    |     |
               +----+     +----+
               |               |
               |    NARROW     |
               |   APPROACH    |
               |    CHANNEL    |
               |               |
               +---[START MAT]-+
                        |
                        S  (player start, near end)
```

### Section Descriptions

#### Narrow Approach Channel (south section)
- A tall, skinny rectangle when viewed from above.
- The player starts at the south end on the starting mat.
- The channel is roughly **1/3 the width** of the wide area.
- It runs from the starting mat northward until it meets the windmill.
- Boundary walls run along the left (west) and right (east) sides.
- A closing wall caps the south end (behind the starting mat).

#### Windmill Junction
- The windmill base **straddles the boundary** between the narrow channel and
  the wide putting area.
- The windmill tower footprint is **wider than the channel** but **narrower than
  the wide area**, so it creates two small gaps — one on the left and one on the
  right — where the channel wall meets the wide area wall.
- These gaps are intentional: a ball could theoretically pass through the side
  gaps, but the intended path is through the **gap cut into the windmill base**.
- The windmill gap runs north-south through the centre of the lower tower body,
  aligned with the channel.

#### Wide Putting Area (north section)
- A large square/rectangle when viewed from above.
- Approximately **2.5–3x the width** of the narrow channel.
- The golf hole (cylinder) sits roughly **2/3 of the way north** from the
  junction, centred left-right.
- Boundary walls run along the left (west), right (east), and north ends.
- The south edge of the wide area is open at the windmill junction (no wall
  across the full width — the windmill body and the two side gap openings
  occupy this boundary).

### Top-Down Dimensions (in OpenGL world units)

Use these as the working proportions. Scale the whole scene uniformly if needed.

| Element | Width (X) | Depth (Z) | Notes |
|---|---|---|---|
| Narrow channel (interior) | 1.4 | 3.5 | Interior grass width |
| Narrow channel walls | 0.2 thick | 3.5 | Left and right walls |
| Wide putting area (interior) | 4.0 | 3.5 | Interior grass |
| Wide area walls | 0.2 thick | 3.5 | Left, right, north walls |
| Windmill tower footprint | 1.1 | 1.0 | Lower body top-down footprint |
| Windmill base offset from origin | 0.0 | 0.0 | Tower centred at scene origin |
| Side gap width (each side) | 0.15 | — | Between windmill edge and wide-area wall |
| Golf hole position | X=0.0 | Z=-2.2 | Centred, north part of wide area |
| Golf hole radius | 0.18 | — | Cylinder, >=8 vertices |
| Starting mat position | X=0.0 | Z=+3.8 | South end of channel |
| Starting mat size | 0.9 wide | 0.5 deep | Flat cuboid, slightly raised |

### Wall Layout (top-down, listed by segment)

Going clockwise from the south-west corner:

1. **South cap wall** — runs east-west across the south end of the channel
2. **West channel wall** — runs north from south cap to windmill SW corner
3. **West junction step** — short east-west segment from channel west wall to wide area west wall (this is the "shoulder" where narrow meets wide)
4. **West wide-area wall** — runs north from west junction step to north cap
5. **North cap wall** — runs east-west across the far north end
6. **East wide-area wall** — runs south from north cap to east junction step
7. **East junction step** — short east-west segment, mirror of west
8. **East channel wall** — runs south from east junction step to south cap

The windmill sits in the gap between the channel walls and the wide-area walls
at the junction. No wall segment closes off this gap — the windmill body itself
acts as the barrier (except for the two small side gaps and the centre ball gap).

---

## 2. Front View Layout (looking north, from the south)

```
                    ^  Y (up)
                    |

         +----------+----------+
        /            (roof)             \
       /          Triangular Prism       \
      /_______________________________ ___\
      |  blue |                   | blue  |
      |  trim |   UPPER TOWER     | trim  |
      |       |     (cuboid)      |       |
      |_______|___________________|_______|
      |  blue |                   | blue  |
      |  trim |   LOWER TOWER     | trim  |
      |       |                   |       |
      |       |___[  GAP  ]_______|       |
      |       |   (ball gap)      |       |
      |_______|___________________|_______|
      |                                   |
      |         COURSE SURFACE            |
      +-----------------------------------+
                                         -> X (right/east)

      [rotor assembly on front face of upper tower, protruding toward viewer]
```

### Windmill — Front View Breakdown

#### Lower Tower Body
- **Shape:** Cuboid
- **Colour:** Beige `(0.85, 0.82, 0.69)`
- **Width (X):** 1.1 units
- **Height (Y):** 1.8 units
- **Depth (Z):** 1.0 units
- **Y position:** base sits at Y=0.0 (ground level), top at Y=1.8
- **Ball gap:** centred horizontally, starts at Y=0.0, height=0.45, width=0.55
  - The gap is a void — do not emit the front/back face triangles in the gap region
  - Render the left wall, right wall, top face, and floor of the gap as separate quads
  - The gap aligns with the channel so a ball rolling north passes straight through

#### Upper Tower Body
- **Shape:** Cuboid
- **Colour:** Beige `(0.85, 0.82, 0.69)`
- **Width (X):** 0.9 units (narrower than lower)
- **Height (Y):** 1.4 units
- **Depth (Z):** 0.9 units
- **Y position:** sits directly on top of lower tower, base at Y=1.8, top at Y=3.2
- Centred on X=0

#### Blue Trim Strips (x4 cuboids — 2 per tower section)
- **Shape:** Thin cuboid
- **Colour:** Blue `(0.23, 0.43, 0.71)`
- **Width (X):** 0.1 units
- **Same height and depth as the tower section they belong to**
- Flush against the left and right faces of each tower section
- Lower-left, lower-right, upper-left, upper-right = 4 separate cuboids

#### Roof
- **Shape:** Triangular Prism
- **Colour:** Dark grey `(0.38, 0.38, 0.38)`
- **Base width (X):** matches upper tower width = 0.9
- **Height (Y):** 0.8 units (peak to base)
- **Depth (Z):** matches upper tower depth = 0.9
- **Y position:** base at Y=3.2 (top of upper tower), peak at Y=4.0
- Front and back faces are triangles; left and right faces are rectangles
- Peak is centred at X=0

#### Rotor Axle
- **Shape:** Cylinder, >=8 vertices per circle
- **Colour:** Metallic grey `(0.67, 0.67, 0.67)`
- **Radius:** 0.08 units
- **Length (Z):** 0.35 units — protrudes out from the front face of the upper tower
- **Centre position:** X=0, Y=2.5 (mid-height of upper tower), Z=-0.45 (front face of upper tower) to Z=-0.80 (protruding toward viewer)
- The axle is horizontal, pointing along the Z-axis (toward the viewer from the front)

#### Rotor Hub
- **Shape:** Cuboid (square face-on)
- **Colour:** Red `(0.87, 0.20, 0.13)`
- **Size:** 0.28 x 0.28 x 0.12 (W x H x D)
- **Position:** centred at X=0, Y=2.5, Z=-0.80 (at the protruding tip of the axle)
- This is the pivot point for all blade rotations

#### Blades (x4 cuboids + x4 triangular tips)
- **Blade body shape:** Flat cuboid
- **Blade tip shape:** Triangular prism (pointed end cap on each blade)
- **Colour:** Red `(0.87, 0.20, 0.13)` for both body and tip
- **Blade body size:** 0.22 wide x 1.1 long x 0.06 deep
- **Blade tip size:** triangular prism, base 0.22 wide, height 0.18, depth 0.06 — sits at the far end of each blade body
- **Pivot:** all blades rotate around the hub centre (X=0, Y=2.5, Z=-0.80)
- **Static angular offsets** (before any spin is applied):
  - Blade 1: 45° (top-right when viewed from front)
  - Blade 2: 135° (top-left)
  - Blade 3: 225° (bottom-left)
  - Blade 4: 315° (bottom-right)
- Each blade's local origin is at the hub centre; the blade body extends outward

#### Construction note on blade tips
- The triangular tip is a separate triangular prism appended to the far end of each blade body.
- Its base aligns flush with the outer end face of the blade body.
- Its point faces away from the hub.
- In practice, build one blade+tip unit in local space (hub at origin, blade extending up),
  then rotate copies to the four angular positions.

---

## 3. Course — Front View

```
      |  wall  |                          |  wall  |
      |  (red) |      GREEN SURFACE       |  (red) |
      |        |                          |        |
      +--------+--------------------------+--------+
               (seen from front, wide area visible
                behind and around windmill base)
```

### Course Geometry (front view)

The course surface is a single flat green cuboid slab. From the front, the viewer sees:
- The wide putting area behind the windmill, green surface
- The narrow channel approaching from in front of the windmill (toward the viewer/south)
- Red boundary walls on the left and right of both sections
- The starting mat as a yellow flat slab at the near (south) end of the channel

Because the course is an irregular L/T shape (not a simple rectangle), it is built from
**two separate green cuboid slabs** joined at the junction:

| Slab | Width (X) | Depth (Z) | Height (Y) | Position |
|---|---|---|---|---|
| Channel slab | 1.4 | 3.5 | 0.1 | Z centre = +1.75 (south of origin) |
| Wide area slab | 4.0 | 3.5 | 0.1 | Z centre = -1.75 (north of origin) |

Both slabs sit at Y=-0.1 to Y=0.0 (ground level, top face is at Y=0).

### Boundary Walls (front view, all red cuboids)

All walls are 0.2 units thick and 0.4 units tall (Y=0.0 to Y=0.4).

| Wall segment | Width (X) or Depth (Z) | Notes |
|---|---|---|
| South cap | 1.8 wide (X) | Closes south end of channel |
| West channel wall | 3.5 deep (Z) | Left side of channel |
| East channel wall | 3.5 deep (Z) | Right side of channel |
| West junction step | 1.3 wide (X) | Shoulder from channel to wide area (west side) |
| East junction step | 1.3 wide (X) | Shoulder from channel to wide area (east side) |
| West wide wall | 3.5 deep (Z) | Left side of wide area |
| East wide wall | 3.5 deep (Z) | Right side of wide area |
| North cap | 4.4 wide (X) | Closes north end of wide area |

---

## 4. Key Spatial Relationships to Preserve in 3D

These are the relationships that must hold in the final render. If any are wrong,
the scene will not read correctly.

1. **Windmill base is at ground level** — the bottom of the lower tower sits at Y=0,
   flush with the top of the course slab.

2. **Ball gap is aligned with the channel** — the gap in the lower tower runs along Z
   (north-south), centred at X=0, and its floor is at Y=0 (ground level).
   A ball rolling north along the channel would pass straight through.

3. **The windmill straddles the junction** — the windmill footprint extends slightly into
   both the channel zone (south/+Z) and the wide area zone (north/-Z).
   Approximately 40% of the tower depth is in the channel, 60% in the wide area.

4. **Side gaps are tight** — the east and west edges of the windmill lower tower are
   close to the wide-area side walls, leaving only ~0.15 unit gaps on each side.
   These are intentional but narrow.

5. **Rotor faces south** — the axle points along -Z (toward the viewer when looking north).
   The blades spin in the XY plane as seen from the south.

6. **Hole is in the north section** — the golf hole cylinder is in the wide putting area,
   not near the windmill. It sits at approximately Z=-2.2 from the origin.

7. **Starting mat is at the far south** — at approximately Z=+3.8, the southern-most
   point of the channel.

8. **Roof peak is the highest point** — at Y=4.0. The blades at full extension reach
   approximately Y=3.8 from the hub, putting blade tips near Y=3.8 above ground.
   Ensure the camera is positioned to see the full height.

---

## 5. Suggested Build Order for 3D Implementation

Build and verify each component before moving to the next. This order minimises
debugging complexity.

1. Course channel slab (green cuboid) — verify ground plane is correct
2. Course wide area slab (green cuboid) — verify join with channel
3. All boundary walls (red cuboids, 8 segments) — verify top-down shape matches layout
4. Starting mat (yellow cuboid) — south end of channel
5. Golf hole (dark cylinder, >=8 verts) — north of wide area
6. Lower tower body (beige cuboid, with gap void)
7. Upper tower body (beige cuboid)
8. Blue trim strips (4 thin cuboids)
9. Roof (triangular prism)
10. Rotor axle (cylinder, >=8 verts)
11. Rotor hub (red cuboid)
12. One blade + tip (cuboid + triangular prism) — verify in isolation
13. Remaining 3 blades + tips (rotate copies of step 12)
14. Decorative trees / third cylinder (cones + cylinder outside walls)
15. Wireframe index buffers for all shapes above
16. Rotor spin animation
17. Scene-level transformations (WASDQE rotations, IJKLOU translations)
