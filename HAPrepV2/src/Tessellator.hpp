#ifndef TESSELLATOR_HPP
#define TESSELLATOR_HPP

#include <vector>
#include "Face.hpp"

// Triangulate a polygon with holes using bridge-edge merge + ear-clipping.
// Returns a flat list of 2D triangle vertices: [v0, v1, v2, v0, v1, v2, ...]
// outer must be CCW; each hole must be CW.
std::vector<Vec2> tessellate(const Contour& outer,
                              const std::vector<Contour>& holes);

float signedArea(const Contour& c);

#endif // TESSELLATOR_HPP
