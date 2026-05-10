#include "model.h"
#include <glm/glm.hpp>
#include <iostream>

bool Model::load(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs     |
        aiProcess_GenNormals
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        processMesh(scene->mMeshes[i]);
    return true;
}

void Model::processMesh(aiMesh* mesh) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 centreAccum(0.0f);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        v.nx = mesh->mNormals[i].x;
        v.ny = mesh->mNormals[i].y;
        v.nz = mesh->mNormals[i].z;
        if (mesh->mTextureCoords[0]) {
            v.u = mesh->mTextureCoords[0][i].x;
            v.v = mesh->mTextureCoords[0][i].y;
        } else {
            v.u = v.v = 0.0f;
        }
        vertices.push_back(v);
        centreAccum += glm::vec3(v.x, v.y, v.z);

        boundsMin.x = glm::min(boundsMin.x, v.x);
        boundsMin.y = glm::min(boundsMin.y, v.y);
        boundsMin.z = glm::min(boundsMin.z, v.z);
        boundsMax.x = glm::max(boundsMax.x, v.x);
        boundsMax.y = glm::max(boundsMax.y, v.y);
        boundsMax.z = glm::max(boundsMax.z, v.z);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    Mesh gpuMesh;
    gpuMesh.indexCount = (unsigned int)indices.size();
    gpuMesh.name   = mesh->mName.C_Str();
    gpuMesh.centre = centreAccum / (float)mesh->mNumVertices;
    std::cout << "  mesh[" << meshes.size() << "] \"" << gpuMesh.name
              << "\" centre=(" << gpuMesh.centre.x << ", "
              << gpuMesh.centre.y << ", " << gpuMesh.centre.z << ")\n";

    glGenVertexArrays(1, &gpuMesh.VAO);
    glGenBuffers(1, &gpuMesh.VBO);
    glGenBuffers(1, &gpuMesh.EBO);

    glBindVertexArray(gpuMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    meshes.push_back(gpuMesh);
}

void Model::draw() const {
    for (const Mesh& mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
