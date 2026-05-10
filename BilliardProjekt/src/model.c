#include "../include/model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Model* load_obj(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    Model* model = calloc(1, sizeof(Model));
    char line[128];

    // Első kör: megszámoljuk mi mennyi
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) model->num_vertices++;
        else if (strncmp(line, "f ", 2) == 0) model->num_faces++;
    }

    model->vertices = malloc(sizeof(Vertex) * model->num_vertices);
    model->faces = malloc(sizeof(Face) * model->num_faces);

    rewind(file);
    int v_idx = 0, f_idx = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", &model->vertices[v_idx].x, &model->vertices[v_idx].y, &model->vertices[v_idx].z);
            v_idx++;
        } else if (strncmp(line, "f ", 2) == 0) {
            // Ez a rész feltételezi, hogy a modell csak "v" adatokat tartalmaz (vagy v//vn formátumú)
            // A dákóhoz vagy egyszerűbb tárgyakhoz ez elég
            sscanf(line, "f %d %d %d", &model->faces[f_idx].v[0], &model->faces[f_idx].v[1], &model->faces[f_idx].v[2]);
            f_idx++;
        }
    }
    fclose(file);
    return model;
}

void draw_model(Model* model) {
    if (model == NULL || model->num_faces == 0) return;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->num_faces; i++) {
        for (int j = 0; j < 3; j++) {
            int v_idx = model->faces[i].v[j] - 1; // Az OBJ 1-től indexel!
            
            // Ha nincsenek normálvektoraid, a fény kiolthatja a színt, 
            // ezért teszthez adjunk meg egy alap normált
            glNormal3f(0.0f, 1.0f, 0.0f); 
            
            glVertex3f(model->vertices[v_idx].x, 
                       model->vertices[v_idx].y, 
                       model->vertices[v_idx].z);
        }
    }
    glEnd();
}