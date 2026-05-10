#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <GL/gl.h>

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v[3];
    int vt[3];
    int vn[3];
} Face;

typedef struct {
    Vertex* vertices;
    Vertex* texcoords;
    Face* faces;
    int num_vertices;
    int num_texcoords;
    int num_faces;
} Model;

Model* load_obj(const char* filename);
void draw_model(Model* model);
void free_model(Model* model);

#endif