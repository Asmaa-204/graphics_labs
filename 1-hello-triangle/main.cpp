#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>
#include <string>

GLuint loadShader(const std::string& path, GLenum shaderType) {
    std::ifstream file(path);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCodeCStr = sourceCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* logStr = new char[length];
        glGetShaderInfoLog(shader, length, &length, logStr);
        std::cerr << "Error compiling shader: " << logStr << std::endl;
        delete[] logStr;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int main(){
    if(!glfwInit()) {
        std::cerr << "Error initializing glfw!\n";
        exit(1);
    }

    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(640, 480, "triangle", nullptr, nullptr);
    
    if(!window) {
        std::cerr << "Error ceeating window!\n";
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }

    GLuint vertexShader = loadShader("assets/shaders/tri.vert", GL_VERTEX_SHADER);
    GLuint fragShader = loadShader("assets/shaders/tri.frag", GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}
