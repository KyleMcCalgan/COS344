#ifndef COLLISION_H
#define COLLISION_H
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////
//circleVsAABB - push a circle out of an axis-aligned box
//returns true if overlap occurred; pushX/pushY carry the displacement
////////////////////////////////////////////////////////////////////////////////////////
static bool circleVsAABB(float bx, float by, float br,
                          float rx, float ry, float rhw, float rhh,
                          float& pushX, float& pushY)
{
    float cx = fmaxf(rx - rhw, fminf(bx, rx + rhw));
    float cy = fmaxf(ry - rhh, fminf(by, ry + rhh));
    float dx = bx - cx, dy = by - cy;
    float d2 = dx*dx + dy*dy;
    if (d2 >= br*br) { pushX = pushY = 0; return false; }
    float d = sqrtf(d2);
    if (d < 1e-6f) { pushX = 0; pushY = br; return true; }
    float ov = br - d;
    pushX = dx/d * ov;
    pushY = dy/d * ov;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
//circleVsCircle - push a circle out of another circle (modifies bx/by in-place)
////////////////////////////////////////////////////////////////////////////////////////
static void circleVsCircle(float& bx, float& by, float br,
                            float ox, float oy, float or_)
{
    float dx = bx - ox, dy = by - oy;
    float d2 = dx*dx + dy*dy;
    float minD = br + or_;
    if (d2 >= minD*minD || d2 < 1e-12f) return;
    float d = sqrtf(d2), ov = minD - d;
    bx += dx/d * ov;
    by += dy/d * ov;
}

////////////////////////////////////////////////////////////////////////////////////////
//circleVsTriangle - push a circle out of a triangle defined by three world-space vertices
//uses closest-point-on-each-edge and an interior winding test
////////////////////////////////////////////////////////////////////////////////////////
static void circleVsTriangle(float& bx, float& by, float br,
                              float ax, float ay,
                              float bvx, float bvy,
                              float cvx, float cvy)
{
    auto closestOnSeg = [](float qx, float qy,
                           float p0x, float p0y, float p1x, float p1y,
                           float& cx, float& cy) {
        float ex = p1x-p0x, ey = p1y-p0y;
        float t = ((qx-p0x)*ex + (qy-p0y)*ey) / (ex*ex + ey*ey + 1e-12f);
        t = fmaxf(0.f, fminf(1.f, t));
        cx = p0x + t*ex; cy = p0y + t*ey;
    };

    float bestDist2 = 1e30f, bestCx = 0, bestCy = 0;
    float segs[3][4] = {
        {ax,ay,bvx,bvy}, {bvx,bvy,cvx,cvy}, {cvx,cvy,ax,ay}
    };
    for (auto& seg : segs) {
        float cx, cy;
        closestOnSeg(bx, by, seg[0],seg[1],seg[2],seg[3], cx, cy);
        float dx = bx-cx, dy = by-cy, d2 = dx*dx+dy*dy;
        if (d2 < bestDist2) { bestDist2 = d2; bestCx = cx; bestCy = cy; }
    }

    auto sign = [](float px,float py,float ax,float ay,float bx,float by){
        return (px-bx)*(ay-by) - (ax-bx)*(py-by);
    };
    bool inside = (sign(bx,by,ax,ay,bvx,bvy) <= 0) &&
                  (sign(bx,by,bvx,bvy,cvx,cvy) <= 0) &&
                  (sign(bx,by,cvx,cvy,ax,ay) <= 0);
    inside = inside || ((sign(bx,by,ax,ay,bvx,bvy) >= 0) &&
                        (sign(bx,by,bvx,bvy,cvx,cvy) >= 0) &&
                        (sign(bx,by,cvx,cvy,ax,ay) >= 0));

    if (!inside && bestDist2 >= br*br) return;

    float dist = sqrtf(bestDist2);
    float dx = bx - bestCx, dy = by - bestCy;
    if (inside || dist < 1e-6f) { dx = 0; dy = 1; dist = 0; }
    float ov = br - dist;
    if (dist > 1e-6f) { bx += dx/dist * ov; by += dy/dist * ov; }
    else              { bx += 0; by += ov; }
}

#endif // COLLISION_H
