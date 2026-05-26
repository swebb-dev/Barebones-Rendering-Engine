#pragma once
#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../inc/window.h"

typedef struct {
    GLuint VAO;
    GLuint VBO;
    int vertexCount;
} Mesh;

Mesh mesh_init(const char* path);

void mesh_draw(Mesh* mesh);

void mesh_free(Mesh* mesh);