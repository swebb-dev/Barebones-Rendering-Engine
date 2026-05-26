#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../inc/window.h"
#include "../inc/render.h"
#include "../inc/mesh.h"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
char* title = "Barebones Rendering Engine";

int main(void) {
    
    if (Window_Init(WIDTH, HEIGHT, title) != 0) {
        printf("Failed to initialize window.\n");
        return -1;
    }

    render_init();
    Mesh bunny = mesh_init("assets/rex.obj");
    render_setMesh(&bunny);

    while (!Window_shouldClose()) {
        Window_Update();
    }

    mesh_free(&bunny);
    Window_Terminate();

    return 0;
}