#pragma once
#include "../inc/glad/glad.h"
#include <GLFW/glfw3.h>

int Window_Init(int width, int height, char* title);

void Window_Update(void);

int Window_shouldClose(void);

void Window_Terminate(void);