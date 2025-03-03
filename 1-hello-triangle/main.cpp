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

    const float vertices[4*7] = {
        -0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,
        0.5, -0.5, 0.0,  0.0, 1.0, 0.0, 1.0,
        -0.5, 0.5, 0.0,   0.0, 0.0, 1.0, 1.0,
        0.5, 0.5, 0.0,  1.0, 1.0, 0.0, 1.0,
    };

    const uint16_t elements[6] = {0, 1, 2, 3, 1, 2};
    
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    GLuint vertexBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * 7 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    GLuint elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint16_t), elements, GL_STATIC_DRAW);

    GLuint positionLoc = 0;
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, 7 * sizeof(float), nullptr);

    GLuint colorLoc = 1;
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, false, 7 * sizeof(float), (void *) (3*sizeof(float)));

    GLint time_loc = glGetUniformLocation(program, "time");
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vertexArray);

        glUniform1f(time_loc, time);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}
