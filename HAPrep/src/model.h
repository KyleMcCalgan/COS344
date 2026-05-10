#pragma once
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct Mesh {
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    std::string name;
    glm::vec3 centre{0.0f};  // average of all vertex positions — used as rotation pivot
};

class Model {
public:
    std::vector<Mesh> meshes;
    glm::vec3 boundsMin{  1e9f,  1e9f,  1e9f};
    glm::vec3 boundsMax{ -1e9f, -1e9f, -1e9f};

    bool load(const std::string& path);
    void draw() const;

    // Returns a uniform scale factor so the model fits inside targetSize units
    float autoScale(float targetSize = 1.0f) const {
        glm::vec3 ext = boundsMax - boundsMin;
        float maxDim = glm::max(ext.x, glm::max(ext.y, ext.z));
        return (maxDim > 0.0f) ? (targetSize / maxDim) : 1.0f;
    }

private:
    void processMesh(aiMesh* mesh);
};
