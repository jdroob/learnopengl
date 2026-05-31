#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdio>
#include <cstring>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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
// float vertices[] = {
//     -0.5f, -0.5f, 0.f,
//      0.5f, -0.5f, 0.f,
//      0.0f,  0.5f, 0.f,
// };

// vertices of a rectangle
// can also be thought of vertices of 2 triangles
// that share 2 vertices
// triangle 0: vertex0, vertex1, vertex3
// triangle 1: vertex1, vertex2, vertex3
float vertices[] = {
    0.5f, 0.5f, 0.0f,    // top right
    0.5f, -0.5f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f, 0.5f, 0.0f,   // top left
};

unsigned indices[] = {
    0, 1, 3,  // triangle 0
    1, 2, 3   // triangle 1
};

int main(void) {
    glfwInit();

    // tell opengl the version we'd like to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // configure window object
    GLFWwindow *window = glfwCreateWindow(1600, 900, "John's First opengl window <3", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Could not create a window </3\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // load gl via glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD :(\n");
        return EXIT_FAILURE;
    }

    glViewport(0, 0, 1600, 900);  // glViewport(LL_CORNER, WIDTH, HEIGHT)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // allocate containers using openGL interface
    // move vertex / index data into appropriate containers
    unsigned VBO, VAO, EBO;  // vertex buffer / array / element buffer objects (wonder if this is like a file descriptor for the buffer / arrays?)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // create 1 new buffer and bind it to this unsigned int
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // specifically, bind a buffer array
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices from program mem to buffer
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // copy vertices from program mem to buffer

    // write basic vertex / fragment shaders
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

    /**
     * At this point, we need to link our raw vertex data to openGL's 
     * representation of vertexes.
     * 
     * Currently, our vertex buffer data is formatted as follows:
     * 
     *  float vertices[] = {
     *       -0.5f, -0.5f, 0.f,
     *       0.5f, -0.5f, 0.f,
     *       0.0f,  0.5f, 0.f,
     *   };
     * 
     * So in memory, we have:
     * 
     * VERTEX 0:
     * bytes_0:3 -0.5f
     * bytes_4:7 -0.5f
     * bytes_8:11 0.0f
     * 
     * VERTEX 1:
     * bytes_12:15 0.5f
     * bytes_16:19 -0.5f
     * bytes_20:23 0.0f
     * 
     * VERTEX 2:
     * bytes_24:27 0.0f
     * bytes_28:31 0.5f
     * bytes_32:35 0.0f
     */
     
     /**
      * Discussion of below functions
      *     glVertexAttribPointer
      *         param 0: the vertexx attribute we want to configure
      *                  In vertex shader, we specified the location of the `position`
      *                  vertex attirbute with layout = 0.
      *                  So to pass the location of the vertexes, we need to specify `position` via
      *                  attribute 0
      * 
      *         param 1: size of the vertex attrribute. We're passing 3 vertices so we pass in 3.
      *         param 2: type of the data provided (float)
      *                  Note that vec* is same as float* in GLSL
      * 
      *         param 3: this arg is a boolean asking if we want the data to be normalized (to 0 or 1)
      *                  need to look in to this further at some point
      * 
      *         param 4: stride - the space between contiguous vertex attributes
      *                  in this case, a new vertex is found every 3 * sizeof(float) bytes
      * 
      *         param 5: the offset of where the vertex data begins in the buffer
      *                  data begins at offset 0
      *                  arg is of type void* so we cast to avoid implicit conversion warnings
      */
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);

     /**
      * What if we want to render a rectangle instead of a triangle?
      * 
      * We *can* draw a rectangle using 2 triangles
      * float vertices[] = {
      *     // first triangle
      *     0.5f, 0.5f, 0.0f,   // top right
      *     0.5f, -0.5f, 0.0f,  // bottom right
      *     -0.5f, 0.5f, 0.0f,  // top left
      *     // second triangle
      *     0.5f, -0.5f, 0.0f,  // bottom right (dupe)
      *     -0.5f, -0.5f, 0.0f, // bottom left
      *     -0.5f, 0.5f, 0.0f,  // top left (dupe)
      * };
      * 
      * In this small example, we already have 50% more vertices than we need (need 4, have 6).
      * This problem gets worse as the complexity of the mesh increases.
      * It would be far more efficient to only store the unique vertices.
      * 
      * This is where Element Buffer Objects (EBO's) come into play.
      * As the name implies, these objects are buffers. The difference is we
      * can supply 2 inputs that allow us to remove redundancies.
      *     input0: list of unique vertices
      *     input1: order in which to draw each triangle
      * 
      * e.g.
      *     float vertices[] = {
      *         0.5f, 0.5f, 0.0f,    // top right
      *         0.5f, -0.5f, 0.0f,   // bottom right
      *         -0.5f, -0.5f, 0.0f,  // bottom left
      *         -0.5f, 0.5f, 0.0f,   // top left
      *     };
      * 
      *     unsigned indices[] = {
      *         0, 1, 2,  // triangle 0
      *         1, 2, 3   // triangle 1
      *     };
      * 
      *     Note: the above approach is known as **indexed drawing**.
      */



    // this is the render loop
    while (!glfwWindowShouldClose(window)) {

        // render
        /// note: this is a state *setting* function
        glClearColor(0.2f, 0.3f, 0.3f, 1.0);    // rgba color to use on clear
        /// note: this is a state *using* function
        glClear(GL_COLOR_BUFFER_BIT);   // specify we want to clear the color buffer

        // fun fact: each iteration around the render loop is called a frame
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);

        // process user input
        processInput(window);

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
