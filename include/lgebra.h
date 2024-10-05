#pragma once

#include <math.h>

#ifndef LGEBRA
    #define LGEBRA static inline
#endif

#define PI 3.1415926535897932384626
#define DEG_TO_RAD(theta) (float) (((theta) * PI) / 180)

typedef struct
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct
{
    float m[9];
} mat3_t;

typedef struct
{
    float m[16];
} mat4_t;

typedef enum
{
    EMPTY,
    IDENTITY,
} mat_type_t;

typedef enum
{
    X_PLANE,
    Y_PLANE,
    Z_PLANE
} plane_t;

LGEBRA mat4_t mat4(mat_type_t type);
LGEBRA mat3_t mat3(mat_type_t type);
LGEBRA void mat4_dot(mat4_t *dst, mat4_t mat_a, mat4_t mat_b);
LGEBRA void mat4_rotate(mat4_t *mat_a, float angle, vec3_t v);
LGEBRA mat4_t mat4_scale(mat4_t *mat_a, vec3_t v);
LGEBRA void mat4_ortho(mat4_t *mat_a, float left, float right, float bottom, float top, float near, float far);
LGEBRA void mat4_perspective(mat4_t *mat_a, float fovy, float aspect, float near, float far);
LGEBRA void mat4_translate(mat4_t *mat_a, vec3_t t);

#ifdef LGEBRA_IMPLEMENTATION

LGEBRA mat3_t mat3(mat_type_t type)
{
    switch (type)
    {
    case EMPTY:
        return (mat3_t)
        {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };

    case IDENTITY:
        return (mat3_t)
        {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 1.0f
        };

    default:
        return (mat3_t)
        {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };
    }
}

LGEBRA mat4_t mat4(mat_type_t type)
{
    switch (type)
    {
    case EMPTY:
        return (mat4_t)
        {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };

    case IDENTITY:
        return (mat4_t)
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

    default:
        return (mat4_t)
        {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };
    }
}

#define MAT_AT(mat, i, j) (mat).m[(i)*4 + (j)]

LGEBRA void mat4_dot(mat4_t *dst, mat4_t mat_a, mat4_t mat_b)
{
    int inner_size = 4;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < inner_size; k++)
                dst->m[(i) * 4 + (j)] += MAT_AT(mat_a, i, k) * MAT_AT(mat_b, k, j);
        }
    }

    return;
}

LGEBRA void mat4_rotate(mat4_t *mat_a, float angle, vec3_t r)
{
    float theta = DEG_TO_RAD(angle);

    mat_a->m[0] = cosf(theta) + r.x * r.x * (1 - cosf(theta));
    mat_a->m[1] = r.x * r.y * (1 - cosf(theta)) - r.z * sinf(theta);
    mat_a->m[2] = r.x * r.z * (1 - cosf(theta)) + r.y * sinf(theta);

    mat_a->m[4] = r.y * r.x * (1 - cosf(theta)) + r.z * sinf(theta);
    mat_a->m[5] = cosf(theta) + r.y * r.y * (1 - cosf(theta));
    mat_a->m[6] = r.y * r.z * (1 - cosf(theta)) - r.x * sinf(theta);

    mat_a->m[8] = r.z * r.x * (1 - cosf(theta)) - r.y * sinf(theta);
    mat_a->m[9] = r.z * r.y * (1 - cosf(theta)) + r.x * sinf(theta);
    mat_a->m[10] = cosf(theta) + r.z * r.z * (1 - cosf(theta));

    return;
}

LGEBRA mat4_t mat4_scale(mat4_t *mat_a, vec3_t v)
{
    mat_a->m[0] *= v.x;
    mat_a->m[5] *= v.y;
    mat_a->m[10] *= v.z;

    return;
}

LGEBRA void mat4_ortho(mat4_t *mat_a, float left, float right, float bottom, float top, float near, float far)
{
    mat_a->m[0] = 2 / (right - left);
    mat_a->m[5] = 2 / (top - bottom);
    mat_a->m[10] = 2 / (far - near);

    mat_a->m[3] = -(right + left) / (right - left);
    mat_a->m[7] = -(top + bottom) / (top - bottom);
    mat_a->m[11] = -(far + near) / (far - near);

    return;
}

LGEBRA void mat4_perspective(mat4_t *mat_a, float fovy, float aspect, float near, float far)
{
    const float theta = DEG_TO_RAD(fovy);
    const float tan_half_fovy = tanf(theta * 0.5f);

    mat_a->m[0] = 1.0f / aspect * tan_half_fovy;
    mat_a->m[5] = 1.0f / tan_half_fovy;
    mat_a->m[10] = far / (far - near);
    mat_a->m[14] = 1.0f;
    mat_a->m[11] = -(far * near) / (far - near);
    mat_a->m[15] = 0.0f;

    return;
}

LGEBRA void mat4_translate(mat4_t *mat_a, vec3_t t)
{
    mat_a->m[3] = t.x;
    mat_a->m[7] = t.y;
    mat_a->m[11] = t.z;

    return;
}

#endif
