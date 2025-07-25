#include "FrameBuffer.h"
#include <stdexcept>

// Define global data arrays here
float data[20] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

unsigned int elements[6] = {
    0, 1, 2,
    0, 2, 3
};

void changeViewPort(GLFWwindow* window, int width, int height);


FrameBuffer::FrameBuffer(size_t width, size_t height, std::string title)
    : width(width), height(height) {
    if (!glfwInit()) {
        throw std::runtime_error("Glfw couldn't init");
    }
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_API, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        throw std::runtime_error("Window couldn't be generated!");
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, changeViewPort);

    prog = new Shader("Vert.shader", "Fragment.shader");
    array.resize(width * height, { 0, 0, 0 });
    genVertexArr();
    prog->use();
    genTex();
}

bool FrameBuffer::shouldClose() {
    return glfwWindowShouldClose(window);
}

void FrameBuffer::set_buffer() {
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, array.data());
}

void FrameBuffer::prepare() {
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void FrameBuffer::display() {
    glClear(GL_COLOR_BUFFER_BIT);
    prog->setInt("texUNI", 0);
    glBindVertexArray(ID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glfwSwapBuffers(window);
}

void FrameBuffer::genVertexArr() {
    glGenVertexArrays(1, &ID);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(ID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FrameBuffer::genTex() {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glGenTextures(1, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, array.data());
}

void changeViewPort(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}