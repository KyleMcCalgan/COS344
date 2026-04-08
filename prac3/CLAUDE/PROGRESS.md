# PROGRESS.md — Session Log

This file tracks what has been done each session, the current state of the project,
known issues, and what to tackle next. Update this at the end of every session.

---

## Current Status

**Overall:** ⬜ Not started — setup phase only.

| Area | Status |
|---|---|
| Project scaffold & build | ⬜ Not started |
| Matrix math (from P1) | ⬜ Not started |
| Vertex / Fragment shaders | ⬜ Not started |
| Camera / projection setup | ⬜ Not started |
| Course geometry | ⬜ Not started |
| Windmill geometry | ⬜ Not started |
| Rotor & blades | ⬜ Not started |
| WASDQE rotations | ⬜ Not started |
| IJKLOU translations | ⬜ Not started |
| +/- rotor speed control | ⬜ Not started |
| Wireframe (GL_LINES) | ⬜ Not started |
| Background colour | ⬜ Not started |
| Reference photo & planning | ⬜ Not started |
| Submission packaging | ⬜ Not started |

Status key: ⬜ Not started | 🔄 In progress | ✅ Done | ❌ Broken / needs fix

---

## Session Log

### Session 0 — Project Setup (Pre-coding)
**Date:** 2026-04-08
**What was done:**
- Read the full P3 specification (PDF).
- Created `CLAUDE.md` root guide.
- Created `/CLAUDE/` memory folder with `DECISIONS.md`, `PROGRESS.md`, `SHAPES.md`, `SHADERS.md`, `REFERENCE.md`.
- Reviewed provided starter files: `main.cpp`, `shader.cpp`, `shader.hpp`, `makefile`.
- Documented all spec requirements, constraints, and submission rules in `CLAUDE.md`.
- Made initial architecture decisions (DEC-001 through DEC-005).

**Current issues / blockers:**
- No windmill reference photo chosen yet — needed before windmill geometry can begin in earnest.
- Practical 1 matrix code not yet copied into the project.

**Next session should:**
1. Choose a windmill reference photo and document it in `CLAUDE/REFERENCE.md`.
2. Copy Practical 1 matrix math code into the project, verify it compiles.
3. Set up the vertex and fragment shaders (see `CLAUDE/SHADERS.md`).
4. Set up the camera (projection + view matrix) so there is something on screen.
5. Build the first shape (a cuboid) to verify the pipeline end-to-end.

---

<!-- Add new sessions below this line in the same format -->
