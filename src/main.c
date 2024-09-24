#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#define UTIL_IMPLEMENTATION
#include "../include/util.h"

#define LGEBRA_IMPLEMENTATION
#include "../include/lgebra.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define WINDOW_TITLE "OpenGL Template"

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "error: failed to create GLFW window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);
    glad_init();
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    
    float vertices[] =
    {
        -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int shader_program = create_shader_program("src/main.vert", "src/main.frag");

    unsigned int vbo = create_vbo(sizeof(vertices), vertices);
    unsigned int vao = create_vao();
    unsigned int ebo = create_ebo(sizeof(indices), indices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
  
    glfwSetKeyCallback(window, key_callback);
    
    float time = 0.0f;
    unsigned int uniform_time_loc = glGetUniformLocation(shader_program, "_time");
    unsigned int uniform_transformation_loc = glGetUniformLocation(shader_program, "_transform");

    mat4_t transform = mat4(IDENTITY);
    //mat4_scale(&transform, (vec3_t) { 0.5, 0.5, 0.0f });

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(NORMALIZE_COLOR(29), NORMALIZE_COLOR(29), NORMALIZE_COLOR(29), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        use_shader_program(shader_program);

        time = glfwGetTime();
        //mat4_rotate(&transform, time * 20.0f, (vec3_t) { 0.0f, 1.0f, 0.0f });        
        
        glUniform1f(uniform_time_loc, time);
        glUniformMatrix4fv(uniform_transformation_loc, 1, GL_FALSE, transform.m);
            
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(0, &vao);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
