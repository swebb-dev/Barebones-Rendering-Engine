#pragma once
#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../inc/window.h"
#include "../inc/mesh.h"
#include <stdbool.h>

void render_init(void);

void render_setMesh(Mesh* mesh);

void handleScroll(double yoffset);

void handleMouseMovement(double xoffset, double yoffset);

void handleResize(int width, int height);

void handleKey(char key, bool pressed);

void render(float deltaTime);