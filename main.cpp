#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <cstring>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, height, width);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}

/**
 * array of 9 fp vals assumed to be in format: vertex0, vertex1, vertex2
 * Note these coordinates are Normalized Device Coordinates (NDC)
 * aka they're coordinates sitting on the 2x2 viewport
 */
float vertices[] = {
    -0.5f, -0.5f, 0.f,
     0.5f, -0.5f, 0.f,
     0.0f,  0.5f, 0.f,
};

int main(void) {
    glfwInit();

    // tell opengl the version we'd like to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "John's First opengl window <3", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Could not create a window </3\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD :(\n");
        return EXIT_FAILURE;
    }

    glViewport(0, 0, 900, 1600);  // glViewport(LL_CORNER, HEIGHT, WIDTH)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned VBO;  // vertex buffer object (wonder if this is like a file descriptor for the buffer?)
    glGenBuffers(1, &VBO); // create 1 new buffer and bind it to this unsigned int
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // specifically, bind a buffer array
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices from program mem to buffer

    const char *vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0"
    ;

    const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0"
    ;

    unsigned vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);    // create shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);  // attach shader source code to shader object
    glCompileShader(vertexShader);  // at runtime, compile shader

    int success;
    char infoLog[512]{};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    memset(infoLog, '\0', 512);
    unsigned fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);  // create fragment shader object
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);  // inject shader code into shader object
    glCompileShader(fragmentShader);  // at runtime, compile fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    memset(infoLog, '\0', 512);
    unsigned shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(shaderProgram);

    // this is the render loop
    while (!glfwWindowShouldClose(window)) {
        // each iteration around the render loop is called a frame

        // process user input
        processInput(window);

        // render
        /// note: this is a state *setting* function
        glClearColor(0.2f, 0.3f, 0.3f, 1.0);    // rgba color to use on clear
        /// note: this is a state *using* function
        glClear(GL_COLOR_BUFFER_BIT);   // specify we want to clear the color buffer

        // swap front and back buffers (2 buffers minimize flickering)
        glfwSwapBuffers(window);

        // check and call events
        glfwPollEvents();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glfwTerminate();
    return EXIT_SUCCESS;
}
