#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, height, width);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}

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

    glViewport(0, 0, 800, 600);  // glViewport(LL_CORNER, HEIGHT, WIDTH)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    glfwTerminate();
    return EXIT_SUCCESS;
}
