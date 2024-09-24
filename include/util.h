#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define INFO_LOG_BUFFER_SIZE 1024
#define NORMALIZE_COLOR(c) (float) ((c) / 255.0f)

typedef enum
{
    VERTEX_SHADER,
    FRAGMENT_SHADER,
} shader_type_t;

static int compilation_status = 0;
static char info_log[INFO_LOG_BUFFER_SIZE];
static int wireframe_mode = 0;

static long        get_stream_char_count(FILE *fp);
static const char *parse_shader(const char *shader_path);
static void        check_shader_compilation_error(unsigned int *shader, shader_type_t type);
static void        check_shader_program_compilation_error(unsigned int sp);

int          glad_init(void);
void         frame_buffer_size_callback(GLFWwindow *window, int xscale, int yscale);
void         toggle_wireframe_mode(void);
void         key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int create_vbo(unsigned int vertex_data_size, float *vertex_data);
unsigned int create_vao(void);
unsigned int create_ebo(unsigned int index_data_size, unsigned int *index_data);
unsigned int create_shader_program(const char *vshader_src_path, const char *fshader_src_path);
void         use_shader_program(unsigned int sp);
unsigned int load_texture(const char *image_path, int vflip);

#ifdef UTIL_IMPLEMENTATION

static long get_stream_char_count(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    long count = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return count;
}

static const char *parse_shader(const char *shader_path)
{
    FILE *fp = fopen(shader_path, "rb");
    if (fp == NULL)
    {
        printf("error: cannot find shader source code\n");
        exit(EXIT_FAILURE);
    }

    long size = get_stream_char_count(fp);
    char *shader_src = (char *) malloc(size + 1);

    fread(shader_src, size, 1, fp);
    shader_src[size] = '\0';

    fclose(fp);

    return shader_src;
}

static void check_shader_compilation_error(unsigned int *shader, shader_type_t type)
{
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status);
    if (compilation_status == 0)
    {
        glGetShaderInfoLog(shader, 512, NULL, info_log);

        if (type == VERTEX_SHADER)
            printf("vertex_shader::error: compilation failed\n%s\n", info_log);
        if (type == FRAGMENT_SHADER)
            printf("fragment_shader::error: compilation failed\n%s\n", info_log);

        exit(EXIT_FAILURE);
    }
}

static void check_shader_program_compilation_error(unsigned int sp)
{
    glGetShaderiv(sp, GL_COMPILE_STATUS, &compilation_status);

    if (!compilation_status)
    {
        glGetProgramInfoLog(sp, 512, NULL, info_log);
        exit(EXIT_FAILURE);
    }

    return;
}

int glad_init(void)
{
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        printf("error: failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}

void frame_buffer_size_callback(GLFWwindow *window, int xscale, int yscale)
{
    glViewport(0, 0, xscale, yscale);

    return;
}

void toggle_wireframe_mode(void)
{
    if (wireframe_mode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        wireframe_mode = !wireframe_mode;
    } else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        wireframe_mode = !wireframe_mode;
    }

    return;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        toggle_wireframe_mode();

    return;
}

unsigned int create_vbo(unsigned int vertex_data_size, float *vertex_data)
{
    unsigned int vbo = 0;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_STATIC_DRAW);

    return vbo;
}

unsigned int create_vao(void)
{
    unsigned int vao = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    return vao;
}

unsigned int create_ebo(unsigned int index_data_size, unsigned int *index_data)
{
    unsigned int ebo = 0;

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_STATIC_DRAW);

    return ebo;
}

unsigned int create_shader_program(const char *vshader_src_path, const char *fshader_src_path)
{ 
    const char *vshader_src = parse_shader(vshader_src_path);
    const char *fshader_src = parse_shader(fshader_src_path);

    // Vertex Shader
    unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vshader_src, NULL);
    glCompileShader(vshader);

    check_shader_compilation_error(vshader, VERTEX_SHADER);

    // Fragment Shader
    unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fshader_src, NULL);
    glCompileShader(fshader);

    check_shader_compilation_error(fshader, FRAGMENT_SHADER);

    // Shader Program
    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vshader);
    glAttachShader(shader_program, fshader);
    glLinkProgram(shader_program);

    check_shader_program_compilation_error(shader_program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return shader_program;
}

void use_shader_program(unsigned int sp)
{
    glUseProgram(sp);

    return;
}

unsigned int load_texture(const char *image_path, int vflip)
{
    unsigned int tex = 0;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(vflip);

    int image_width, image_height, channel;
    unsigned char *image_data = stbi_load(image_path, &image_width, &image_height, &channel, 0);

    if (image_data)
    {
        if (channel == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        if (channel == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        
        glGenerateMipmap(GL_TEXTURE_2D);
    } else
    {
        printf("error: failed to load texture\n");
        exit(EXIT_FAILURE);
    }

    return tex;
}

#endif
