# COS344 Practical 2 — Mini Golf
**Student:** u24648826

## Build & Run
```
make                                  # compile
./main                                # run
./main --import layout.json           # load a saved layout
make import FILE=CoolLayout.json      # load via make (FILE= is required)
```

> **Note:** When using `make import`, you must provide the filename via `FILE=<name>`.
> Omitting `FILE=` will cause the program to launch without importing anything.

---

## Scene Contents

### Required Visual Elements
| Element | Implementation |
|---|---|
| Concrete floor | Grey rectangle behind the grass |
| Grass area | Green rectangle |
| Borders (N/S/E/W) | Four dark brown rectangles |
| Starting pad | Maroon rectangle at bottom |
| River / stream | Blue rectangle + two blue triangles forming bank cutouts |
| Bridge | Two brown ramp planks that animate open/closed (P key) |
| Golf hole | Pink high-poly circle (60 segments) |
| Golf ball | White low-poly circle (8 segments) with 5 dark soccer-ball spots |

### Obstacles (3 distinct types)
| Type | Shape | Colour | Selectable |
|---|---|---|---|
| Obstacle 1 | Orange triangles (x2 default, spawnable) | Orange | Key 2 |
| Obstacle 2 | Purple rectangles (x2 default, spawnable) | Purple | Key 3 |
| Obstacle 3 | Grey hexagonal log stumps (x2) floating in river | Grey | No |

### Colours (11 distinct)
Charcoal background, grey floor, green grass, dark brown borders, maroon start pad, blue river, orange obstacles, purple obstacles, grey logs, pink hole, white ball.

---

## Controls

### Selection
| Key | Action |
|---|---|
| `1` | Select ball |
| `2` | Select next orange triangle |
| `3` | Select next purple rectangle |
| `4` | Select hole |
| `0` | Deselect all |
| `Ctrl + 1/2/3/4` | Add to / toggle multi-selection |

### Transforms (apply to selected object)
| Key | Action |
|---|---|
| `W/A/S/D` | Translate |
| `Q/E` | Rotate (5° per press) |
| `+/-` | Scale up/down (10% per press) |

### Other
| Key | Action |
|---|---|
| `Enter` | Toggle wireframe mode |
| `P` | Toggle bridge open/closed |
| `R` | Reset scene to default |
| `T` | Spawn new orange triangle |
| `Y` | Spawn new purple rectangle |
| `Delete` | Remove selected obstacle(s) |
| `F5` | Export layout to `layout.json` |
| `Escape` | Quit |

---

## Bonus Features Implemented
- **Multi-select** — hold Ctrl while pressing selection keys to build a multi-selection; transforms apply to all selected shapes simultaneously
- **Bounds checking** — ball is clamped to the grass area and cannot leave
- **Ball collision** — push-out collision response against river banks, purple rect obstacles, and orange triangle obstacles
- **Dynamic spawn/delete** — T/Y spawn new obstacles at the origin; Delete removes selected ones
- **Soccer ball detail** — five dark spots on the ball that move and scale with it
- **Save layout** — F5 exports ball, hole, and all obstacle positions to `layout.json`
- **Load layout** — `--import <file>` command-line argument restores a saved layout on startup
