#ifndef GEOMETRY_H
#define GEOMETRY_H

struct MeshData
{
    float *vertexData;
    unsigned int *indexData;
    int floatCount;
    int indexCount;
};

MeshData generateSphereMesh(int subdivisionLevel, float radius);
MeshData generatePlaneMesh(int resolution, float yPosition, float extent);
void destroyMeshData(MeshData &meshData);

#endif
