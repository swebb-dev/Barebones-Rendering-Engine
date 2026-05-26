#include "../inc/mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8
#define MAX_LINE_LENGTH 256

typedef struct {
    float (*data)[3];
    int count;;
    int capacity;
} Vec3Array;

static void vec3array_init(Vec3Array* arr) {
    arr->count = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->data = (float(*)[3])malloc(arr->capacity * sizeof(*arr->data));
}

static void vec3array_push(Vec3Array* arr, float x, float y, float z) {
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (float(*)[3])realloc(arr->data, arr->capacity * sizeof(*arr->data));
    }
    arr->data[arr->count][0] = x;
    arr->data[arr->count][1] = y;
    arr->data[arr->count][2] = z;
    arr->count++;
}

static void vec3array_free(Vec3Array* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

typedef struct {
    float* data;
    int count;;
    int capacity;
} FloatArray;

static void floatarray_init(FloatArray* arr) {
    arr->count = 0;
    arr->capacity = INITIAL_CAPACITY * 6; // Initial capacity
    arr->data = (float*)malloc(arr->capacity * sizeof(float));
}

static void floatarray_push(FloatArray* arr, float v0, float v1, float v2, float v3, float v4, float v5) {
    if (arr->count + 6 > arr->capacity) {
        arr->capacity *= 2;
        arr->data = (float*)realloc(arr->data, arr->capacity * sizeof(float));
    }
    arr->data[arr->count++] = v0;
    arr->data[arr->count++] = v1;
    arr->data[arr->count++] = v2;
    arr->data[arr->count++] = v3;
    arr->data[arr->count++] = v4;
    arr->data[arr->count++] = v5;
}

static void floatarray_free(FloatArray* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

Mesh mesh_init(const char* path) {
    Vec3Array positions = {NULL, 0, 0};
    Vec3Array normals   = {NULL, 0, 0};
    FloatArray buffer   = {NULL, 0, 0};

    vec3array_init(&positions);
    vec3array_init(&normals);
    floatarray_init(&buffer);

    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        Mesh empty = {0, 0, 0};
        return empty;
    }

    char line[MAX_LINE_LENGTH]; // obj file line length limit
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "vn ", 3) == 0) { // Normal vector
            float x, y, z;
            sscanf(line + 3, "%f %f %f", &x, &y, &z);
            vec3array_push(&normals, x, y, z);
        } else if (strncmp(line, "v ", 2) == 0) { // Vertex position
            float x, y, z;
            sscanf(line + 2, "%f %f %f", &x, &y, &z);
            vec3array_push(&positions, x, y, z);
        } else if (strncmp(line, "f ", 2) == 0) { // Supports only triangles, quads and ngons will not function as expected
            int vi[3], vni[3], dummy;
            int matched = sscanf(line + 2,
                "%d/%d/%d %d/%d/%d %d/%d/%d",
                &vi[0], &dummy, &vni[0],
                &vi[1], &dummy, &vni[1],
                &vi[2], &dummy, &vni[2]);
            if (matched != 9) { // STOLEN FROM INTERNET, NO CLUE WHAT THIS DOES, SOME WEIRD FORMATTING THING
                matched = sscanf(line + 2,
                    "%d//%d %d//%d %d//%d",
                    &vi[0], &vni[0],
                    &vi[1], &vni[1],
                    &vi[2], &vni[2]);
                if (matched != 6) {
                    fprintf(stderr, "Mesh_load: skipping unrecognised face line: %s", line);
                    continue;
                }
            }

            for (int i = 0; i < 3; i++) {
                // OBJ indices are 1-based (yuck)
                int pi = vi[i]  - 1;
                int ni = vni[i] - 1;
                if (pi < 0 || pi >= positions.count ||
                    ni < 0 || ni >= normals.count) {
                    fprintf(stderr, "Mesh_load: index out of range (v=%d vn=%d)\n", pi, ni);
                    continue;
                }
 
                floatarray_push(&buffer,
                    positions.data[pi][0],
                    positions.data[pi][1],
                    positions.data[pi][2],
                    normals.data[ni][0],
                    normals.data[ni][1],
                    normals.data[ni][2]);
            }
        }
    }

    fclose(f);

    Mesh mesh;
    mesh.vertexCount = buffer.count / 6;
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, buffer.count * sizeof(float), buffer.data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    vec3array_free(&positions);
    vec3array_free(&normals);
    floatarray_free(&buffer);

    return mesh;
}

void mesh_draw(Mesh* mesh) {
    glBindVertexArray(mesh->VAO);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
}

void mesh_free(Mesh* mesh) {
    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->VBO);
    mesh->VAO         = 0;
    mesh->VBO         = 0;
    mesh->vertexCount = 0;
}