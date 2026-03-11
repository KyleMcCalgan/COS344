#ifndef LAYOUT_H
#define LAYOUT_H

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include "shapes.h"

// ============================================================
//  Simple JSON-like layout save/load
//  Format (human-readable, hand-parsed):
//  {
//    "ball":  { "tx": 0.0, "ty": -0.65, "rotation": 0.0, "sx": 1.0, "sy": 1.0 },
//    "hole":  { "tx": 0.0, "ty":  0.65, "rotation": 0.0, "sx": 1.0, "sy": 1.0 },
//    "obs1a": { ... },
//    "obs1b": { ... },
//    "obs2a": { ... },
//    "obs2b": { ... },
//    "bridgeOpen": true
//  }
// ============================================================

struct Layout {
    struct ShapeState {
        float tx, ty, rotation, sx, sy;
    };
    ShapeState ball, hole, obs1a, obs1b, obs2a, obs2b;
    bool bridgeOpen;
};

static std::string shapeToJson(const char* name, const Layout::ShapeState& s, bool last = false) {
    char buf[256];
    snprintf(buf, sizeof(buf),
        "  \"%s\": { \"tx\": %.6f, \"ty\": %.6f, \"rotation\": %.6f, \"sx\": %.6f, \"sy\": %.6f }%s\n",
        name, s.tx, s.ty, s.rotation, s.sx, s.sy, last ? "" : ",");
    return buf;
}

inline bool exportLayout(const char* filename,
                         const RenderShape& ball,
                         const RenderShape& hole,
                         const RenderShape& obs1a,
                         const RenderShape& obs1b,
                         const RenderShape& obs2a,
                         const RenderShape& obs2b,
                         bool bridgeOpen)
{
    std::ofstream f(filename);
    if (!f.is_open()) return false;

    auto snap = [](const RenderShape& s) -> Layout::ShapeState {
        return { s.tx, s.ty, s.rotation, s.sx, s.sy };
    };

    f << "{\n";
    f << shapeToJson("ball",  snap(ball));
    f << shapeToJson("hole",  snap(hole));
    f << shapeToJson("obs1a", snap(obs1a));
    f << shapeToJson("obs1b", snap(obs1b));
    f << shapeToJson("obs2a", snap(obs2a));
    f << shapeToJson("obs2b", snap(obs2b));
    f << "  \"bridgeOpen\": " << (bridgeOpen ? "true" : "false") << "\n";
    f << "}\n";
    f.close();
    return true;
}

// Minimal JSON float/bool parser — no dependencies
static bool parseField(const std::string& line, const char* key, float& out) {
    std::string k = std::string("\"") + key + "\":";
    size_t pos = line.find(k);
    if (pos == std::string::npos) return false;
    pos += k.size();
    while (pos < line.size() && (line[pos] == ' ')) pos++;
    out = strtof(line.c_str() + pos, nullptr);
    return true;
}

static bool parseBool(const std::string& line, const char* key, bool& out) {
    std::string k = std::string("\"") + key + "\":";
    size_t pos = line.find(k);
    if (pos == std::string::npos) return false;
    pos += k.size();
    while (pos < line.size() && line[pos] == ' ') pos++;
    out = (line.substr(pos, 4) == "true");
    return true;
}

// Parse a shape block like: "name": { "tx": ..., "ty": ..., ... }
// Reads the entire file content and finds the named block.
static bool parseShape(const std::string& content, const char* name, Layout::ShapeState& s) {
    std::string key = std::string("\"") + name + "\"";
    size_t start = content.find(key);
    if (start == std::string::npos) return false;
    size_t open = content.find('{', start);
    size_t close = content.find('}', open);
    if (open == std::string::npos || close == std::string::npos) return false;
    std::string block = content.substr(open, close - open + 1);

    // Default values in case a field is missing
    s = {0,0,0,1,1};
    parseField(block, "tx",       s.tx);
    parseField(block, "ty",       s.ty);
    parseField(block, "rotation", s.rotation);
    parseField(block, "sx",       s.sx);
    parseField(block, "sy",       s.sy);
    return true;
}

inline bool importLayout(const char* filename, Layout& out) {
    std::ifstream f(filename);
    if (!f.is_open()) return false;

    std::ostringstream ss;
    ss << f.rdbuf();
    std::string content = ss.str();
    f.close();

    if (!parseShape(content, "ball",  out.ball))  return false;
    if (!parseShape(content, "hole",  out.hole))  return false;
    if (!parseShape(content, "obs1a", out.obs1a)) return false;
    if (!parseShape(content, "obs1b", out.obs1b)) return false;
    if (!parseShape(content, "obs2a", out.obs2a)) return false;
    if (!parseShape(content, "obs2b", out.obs2b)) return false;

    out.bridgeOpen = true; // default
    // Find bridgeOpen outside any shape block
    size_t pos = content.rfind("\"bridgeOpen\"");
    if (pos != std::string::npos) {
        std::string tail = content.substr(pos);
        parseBool(tail, "bridgeOpen", out.bridgeOpen);
    }
    return true;
}

inline void applyLayout(const Layout& layout,
                        RenderShape& ball,
                        RenderShape& hole,
                        RenderShape& obs1a,
                        RenderShape& obs1b,
                        RenderShape& obs2a,
                        RenderShape& obs2b,
                        bool& bridgeOpen)
{
    auto apply = [](RenderShape& s, const Layout::ShapeState& st) {
        s.tx = st.tx; s.ty = st.ty;
        s.rotation = st.rotation;
        s.sx = st.sx; s.sy = st.sy;
    };
    apply(ball,  layout.ball);
    apply(hole,  layout.hole);
    apply(obs1a, layout.obs1a);
    apply(obs1b, layout.obs1b);
    apply(obs2a, layout.obs2a);
    apply(obs2b, layout.obs2b);
    bridgeOpen = layout.bridgeOpen;
}

#endif
