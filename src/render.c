#include "../inc/render.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static Mesh* sceneMesh = NULL;

float x_view = 0.f, y_view = -1.f, z_view = -2.f; // Camera position
float x_model = 0.f, y_model = 0.f, z_model = 0.f; // Model position

void render_setMesh(Mesh* mesh) {
    sceneMesh = mesh;
}

typedef struct {
    float m[16];
} Mat4;

static Mat4 mat4_identity() { // This is the Identity matrix
    Mat4 result = {0};

    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;

    return result;
}

static Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int k = 0; k < 4; k++) {
                result.m[col*4 + row] += a.m[k*4 + row] * b.m[col*4 + k];
            }
        }
    }
    return result;
}

static Mat4 mat4_perspective(float fovY, float aspect, float near, float far) { // THIS FUNCTION MAKES NO SENSE, I HAVE NO IDEA HOW THIS WORKS, I JUST COPIED IT FROM SOMEONE ELSE, I DON'T KNOW HOW TO WRITE THIS
    Mat4 r = {0};
    float f = 1.f / tanf(fovY * 0.5f);
    r.m[0]  = f / aspect;
    r.m[5]  = f;
    r.m[10] = (far + near) / (near - far);
    r.m[11] = -1.f;
    r.m[14] = (2.f * far * near) / (near - far);
    return r;
}

static Mat4 mat4_translate(float x, float y, float z) {
    Mat4 result = mat4_identity();

    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;

    return result;
}

static Mat4 mat4_rotateX(float angle) {
    Mat4 result = mat4_identity();
    
    result.m[5] = cosf(angle);
    result.m[6] = -sinf(angle);
    result.m[9] = sinf(angle);
    result.m[10] = cosf(angle);

    return result;
}

static Mat4 mat4_rotateY(float angle) {
    Mat4 result = mat4_identity();
    
    result.m[0] = cosf(angle);
    result.m[2] = sinf(angle);
    result.m[8] = -sinf(angle);
    result.m[10] = cosf(angle);

    return result;
}

static Mat4 mat4_rotateZ(float angle) { // Not confirmed
    Mat4 result = mat4_identity();

    result.m[0]  =  cosf(angle);
    result.m[1]  =  sinf(angle);
    result.m[4]  = -sinf(angle);
    result.m[5]  =  cosf(angle);

    return result;
}

static float vertices[] = {
    // Back face (blue)
    -0.5f,-0.5f,-0.5f, 0.2f,0.2f,0.8f,
     0.5f,-0.5f,-0.5f, 0.2f,0.2f,0.8f,
     0.5f, 0.5f,-0.5f, 0.2f,0.2f,0.8f,
     0.5f, 0.5f,-0.5f, 0.2f,0.2f,0.8f,
    -0.5f, 0.5f,-0.5f, 0.2f,0.2f,0.8f,
    -0.5f,-0.5f,-0.5f, 0.2f,0.2f,0.8f,
    // Front face (cyan)
    -0.5f,-0.5f, 0.5f, 0.2f,0.9f,0.9f,
     0.5f,-0.5f, 0.5f, 0.2f,0.9f,0.9f,
     0.5f, 0.5f, 0.5f, 0.2f,0.9f,0.9f,
     0.5f, 0.5f, 0.5f, 0.2f,0.9f,0.9f,
    -0.5f, 0.5f, 0.5f, 0.2f,0.9f,0.9f,
    -0.5f,-0.5f, 0.5f, 0.2f,0.9f,0.9f,
    // Left face (green)
    -0.5f, 0.5f, 0.5f, 0.2f,0.8f,0.2f,
    -0.5f, 0.5f,-0.5f, 0.2f,0.8f,0.2f,
    -0.5f,-0.5f,-0.5f, 0.2f,0.8f,0.2f,
    -0.5f,-0.5f,-0.5f, 0.2f,0.8f,0.2f,
    -0.5f,-0.5f, 0.5f, 0.2f,0.8f,0.2f,
    -0.5f, 0.5f, 0.5f, 0.2f,0.8f,0.2f,
    // Right face (red)
     0.5f, 0.5f, 0.5f, 0.9f,0.2f,0.2f,
     0.5f, 0.5f,-0.5f, 0.9f,0.2f,0.2f,
     0.5f,-0.5f,-0.5f, 0.9f,0.2f,0.2f,
     0.5f,-0.5f,-0.5f, 0.9f,0.2f,0.2f,
     0.5f,-0.5f, 0.5f, 0.9f,0.2f,0.2f,
     0.5f, 0.5f, 0.5f, 0.9f,0.2f,0.2f,
    // Bottom face (yellow)
    -0.5f,-0.5f,-0.5f, 0.9f,0.9f,0.2f,
     0.5f,-0.5f,-0.5f, 0.9f,0.9f,0.2f,
     0.5f,-0.5f, 0.5f, 0.9f,0.9f,0.2f,
     0.5f,-0.5f, 0.5f, 0.9f,0.9f,0.2f,
    -0.5f,-0.5f, 0.5f, 0.9f,0.9f,0.2f,
    -0.5f,-0.5f,-0.5f, 0.9f,0.9f,0.2f,
    // Top face (magenta)
    -0.5f, 0.5f,-0.5f, 0.9f,0.2f,0.9f,
     0.5f, 0.5f,-0.5f, 0.9f,0.2f,0.9f,
     0.5f, 0.5f, 0.5f, 0.9f,0.2f,0.9f,
     0.5f, 0.5f, 0.5f, 0.9f,0.2f,0.9f,
    -0.5f, 0.5f, 0.5f, 0.9f,0.2f,0.9f,
    -0.5f, 0.5f,-0.5f, 0.9f,0.2f,0.9f,
};

static const char* vertSrc = // VERTEX SHADER SOURCE (did not write this, just copied from online, don't know how to write shaders yet) (runs once per vertex, determines the position of that vertex on the screen and passes data to the fragment shader)
    "#version 330 core\n"
    "layout(location=0) in vec3 aPos;\n"
    "layout(location=1) in vec3 aColor;\n"
    "out vec3 vColor;\n"
    "uniform mat4 uMVP;\n"
    "void main() {\n"
    "    gl_Position = uMVP * vec4(aPos, 1.0);\n"
    "    vColor = aColor;\n"
    "}\n";

static const char* fragSrc = // FRAGMENT SHADER SOURCE (did not write this, just copied from online, don't know how to write shaders yet) (runs once per pixel the object takes up on the screen, determines the color of that pixel)
    "#version 330 core\n"
    "in vec3 vColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(vColor, 1.0);\n"
    "}\n";

static GLuint VAO, VBO, shaderProgram;
static float  rotationY = 0.f;
static float  rotationX = 0.f;

static GLuint compileShader(GLenum type, const char* source) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &source, NULL);
    glCompileShader(s);

    return s;
}

float fovH = 110.0f * (3.14159265f / 180.0f); // This also is a bunch of math I did not want to do :) stole from the internet
float aspect = 800.0f / 600.0f;
float fovV = 2.0f * atanf(tanf(fovH / 2.0f) / aspect);

void render_init(void) {
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Upload vertex data to the GPU

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO
}

void handleScroll(double yoffset) {
    z_view *= 1.0f + (-(float)yoffset) * 0.1f; // Adjust the zoom
}

void handleMouseMovement(double xoffset, double yoffset) {
    rotationY += -(float)xoffset * 0.005f; // Adjust sensitivity as needed
    rotationX += -(float)yoffset * 0.005f; // Adjust sensitivity as needed
}

void handleResize(int width, int height) {
    aspect = (float)width / (float)height;
    fovV = 2.0f * atanf(tanf(fovH / 2.0f) / aspect);
}

int pressCount = 0;
bool buttonPressed = false;
char keyPressed = '\0';

void handleKey(char key, bool pressed) {
    buttonPressed = pressed;
    keyPressed = key;
}

float moveSpeed = 0.1f;

void render(float deltaTime) {
    /*
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    rotation += 0.1f * deltaTime;
    
    Mat4 view = mat4_translate(0.f, 0.f, -2.f); // Move camera back

    Mat4 proj = mat4_perspective(fovV, aspect, 0.1f, 100.f);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    */

    

    if (!buttonPressed) {
        pressCount = 0;
        keyPressed = '\0';
        moveSpeed = 0.1f; // Reset speed when not pressing
    } else {
        pressCount++;

        if (pressCount % 5 == 0) {
            moveSpeed *= 1.1f; // Increase speed every 100 presses
        }

        switch (keyPressed) {
            case ' ':
                y_model += moveSpeed; // Move forward/backward
                break;
            case 'C':
                y_model -= moveSpeed; // Move backward/forward
                break;
            case 'W':
                x_model -= moveSpeed; // Move forward/backward
                break;
            case 'S':
                x_model += moveSpeed; // Move backward/forward
                break;
            case 'A':
                z_model += moveSpeed; // Move forward/backward
                break;
            case 'D':
                z_model -= moveSpeed; // Move backward/forward
                break;
            default:
                break;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // rotation += 0.1f * deltaTime; // We are going to handle rotation via user input now

    Mat4 view = mat4_translate(x_view, y_view, z_view);
    view = mat4_mul(view, mat4_rotateX(rotationX));
    view = mat4_mul(view, mat4_rotateY(rotationY));
    Mat4 model = mat4_translate(x_model, y_model, z_model);

    // Mat4 proj = mat4_perspective(fovV, aspect, 0.1f, 100.f);
    Mat4 proj = mat4_perspective(fovV, aspect, 0.01f, 1000.f); // Adjusted near plane to 0.01f for better rendering of close objects

    glUseProgram(shaderProgram);

    if (sceneMesh != NULL) {
        /* // We are handling rotation in the view matrix now.
        Mat4 model = mat4_mul(
            mat4_rotateY(rotation),
            mat4_rotateX(0.f)
        );
        */
        // Mat4 mvp = mat4_mul(proj, view);
        Mat4 mvp = mat4_mul(proj, (mat4_mul(view, model)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, mvp.m);
        mesh_draw(sceneMesh);
    }

    /*

    Mat4 model1 = mat4_mul(
        mat4_translate(0.f, 0.f, 0.f),
        mat4_mul(
            mat4_rotateY(rotation),
            mat4_rotateX(rotation * 0.4f)
        )
    );
    Mat4 mvp1 = mat4_mul(proj, mat4_mul(view, model1));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, mvp1.m);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    */

    /*
    Mat4 model2 = mat4_mul(
        mat4_translate(0.8f, 0.f, 0.f),
        mat4_mul(
            mat4_rotateY(-0.44f),
            mat4_rotateX(-rotation)
        )
    );
    Mat4 mvp2 = mat4_mul(proj, mat4_mul(view, model2));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, mvp2.m);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    */

}