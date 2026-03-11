#ifndef LAYOUT_H
#define LAYOUT_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <cstdio>
#include "shapes.h"

// ============================================================
//  JSON layout save/load — supports variable-length obstacle arrays
//
//  Format:
//  {
//    "ball":  { "tx": 0.0, "ty": -0.65, "rotation": 0.0, "sx": 1.0, "sy": 1.0 },
//    "hole":  { ... },
//    "bridgeOpen": true,
//    "triangles": [
//      { "tx": -0.2, "ty": -0.35, "rotation": 0.0, "sx": 1.0, "sy": 1.0 },
//      ...
//    ],
//    "rects": [
//      { "tx": -0.25, "ty": 0.35, "rotation": 0.0, "sx": 1.0, "sy": 1.0 },
//      ...
//    ]
//  }
// ============================================================

struct ShapeState { float tx, ty, rotation, sx, sy; };

static ShapeState snapShape(const RenderShape& s) {
    return { s.tx, s.ty, s.rotation, s.sx, s.sy };
}

static std::string shapeJson(const ShapeState& s) {
    char buf[200];
    snprintf(buf, sizeof(buf),
        "{ \"tx\": %.6f, \"ty\": %.6f, \"rotation\": %.6f, \"sx\": %.6f, \"sy\": %.6f }",
        s.tx, s.ty, s.rotation, s.sx, s.sy);
    return buf;
}

inline bool exportLayout(const char* filename,
                         const RenderShape& ball,
                         const RenderShape& hole,
                         const std::vector<RenderShape*>& triObs,
                         const std::vector<RenderShape*>& rectObs,
                         bool bridgeOpen)
{
    std::ofstream f(filename);
    if (!f.is_open()) return false;

    f << "{\n";
    f << "  \"ball\": "  << shapeJson(snapShape(ball))  << ",\n";
    f << "  \"hole\": "  << shapeJson(snapShape(hole))  << ",\n";
    f << "  \"bridgeOpen\": " << (bridgeOpen ? "true" : "false") << ",\n";

    f << "  \"triangles\": [\n";
    for (int i = 0; i < (int)triObs.size(); i++) {
        f << "    " << shapeJson(snapShape(*triObs[i]));
        f << (i + 1 < (int)triObs.size() ? "," : "") << "\n";
    }
    f << "  ],\n";

    f << "  \"rects\": [\n";
    for (int i = 0; i < (int)rectObs.size(); i++) {
        f << "    " << shapeJson(snapShape(*rectObs[i]));
        f << (i + 1 < (int)rectObs.size() ? "," : "") << "\n";
    }
    f << "  ]\n";
    f << "}\n";

    f.close();
    return true;
}

// ---- Parser helpers ----

static bool parseFloat(const std::string& s, const char* key, float& out) {
    std::string k = std::string("\"") + key + "\":";
    size_t pos = s.find(k);
    if (pos == std::string::npos) return false;
    pos += k.size();
    while (pos < s.size() && s[pos] == ' ') pos++;
    out = strtof(s.c_str() + pos, nullptr);
    return true;
}

static bool parseBoolField(const std::string& s, const char* key, bool& out) {
    std::string k = std::string("\"") + key + "\":";
    size_t pos = s.find(k);
    if (pos == std::string::npos) return false;
    pos += k.size();
    while (pos < s.size() && s[pos] == ' ') pos++;
    out = (s.substr(pos, 4) == "true");
    return true;
}

static ShapeState parseShapeBlock(const std::string& block) {
    ShapeState s = {0,0,0,1,1};
    parseFloat(block, "tx",       s.tx);
    parseFloat(block, "ty",       s.ty);
    parseFloat(block, "rotation", s.rotation);
    parseFloat(block, "sx",       s.sx);
    parseFloat(block, "sy",       s.sy);
    return s;
}

// Parse a named single-object field: "key": { ... }
static bool parseNamedShape(const std::string& content, const char* name, ShapeState& out) {
    std::string key = std::string("\"") + name + "\"";
    size_t pos = content.find(key);
    if (pos == std::string::npos) return false;
    size_t open  = content.find('{', pos);
    size_t close = content.find('}', open);
    if (open == std::string::npos || close == std::string::npos) return false;
    out = parseShapeBlock(content.substr(open, close - open + 1));
    return true;
}

// Parse a named array field: "key": [ { ... }, { ... } ]
static std::vector<ShapeState> parseNamedArray(const std::string& content, const char* name) {
    std::vector<ShapeState> result;
    std::string key = std::string("\"") + name + "\"";
    size_t pos = content.find(key);
    if (pos == std::string::npos) return result;
    size_t arrOpen = content.find('[', pos);
    size_t arrClose = content.find(']', arrOpen);
    if (arrOpen == std::string::npos || arrClose == std::string::npos) return result;
    std::string arr = content.substr(arrOpen, arrClose - arrOpen + 1);
    // Walk through { } blocks inside the array
    size_t cur = 0;
    while (true) {
        size_t open = arr.find('{', cur);
        if (open == std::string::npos) break;
        size_t close = arr.find('}', open);
        if (close == std::string::npos) break;
        result.push_back(parseShapeBlock(arr.substr(open, close - open + 1)));
        cur = close + 1;
    }
    return result;
}

static void applyState(RenderShape& s, const ShapeState& st) {
    s.tx = st.tx; s.ty = st.ty;
    s.rotation = st.rotation;
    s.sx = st.sx; s.sy = st.sy;
}

inline bool importLayout(const char* filename,
                         RenderShape& ball,
                         RenderShape& hole,
                         std::vector<RenderShape*>& triObs,
                         std::vector<RenderShape*>& rectObs,
                         bool& bridgeOpen,
                         // spawn callbacks — caller provides these
                         std::function<RenderShape*(float,float)> spawnTri,
                         std::function<RenderShape*(float,float)> spawnRect)
{
    std::ifstream f(filename);
    if (!f.is_open()) return false;
    std::ostringstream ss; ss << f.rdbuf();
    std::string content = ss.str();
    f.close();

    ShapeState ballSt, holeSt;
    if (!parseNamedShape(content, "ball", ballSt)) return false;
    if (!parseNamedShape(content, "hole", holeSt)) return false;
    applyState(ball, ballSt);
    applyState(hole, holeSt);

    parseBoolField(content, "bridgeOpen", bridgeOpen);

    // Clear and rebuild obstacle pools
    for (RenderShape* s : triObs)  delete s;
    for (RenderShape* s : rectObs) delete s;
    triObs.clear(); rectObs.clear();

    for (auto& st : parseNamedArray(content, "triangles")) {
        RenderShape* s = spawnTri(st.tx, st.ty);
        s->rotation = st.rotation; s->sx = st.sx; s->sy = st.sy;
    }
    for (auto& st : parseNamedArray(content, "rects")) {
        RenderShape* s = spawnRect(st.tx, st.ty);
        s->rotation = st.rotation; s->sx = st.sx; s->sy = st.sy;
    }
    return true;
}

#endif
