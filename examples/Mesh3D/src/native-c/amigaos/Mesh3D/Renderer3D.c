#include <libc/core.h>
#include <Mesh3D/Renderer3D.h>
#include <Am/Graphics3D/Mesh.h>
#include <Am/Graphics3D/Vectors3.h>
#include <Am/Graphics3D/Vectors2.h>
#include <Am/Graphics3D/Polygons.h>
#include <Am/Graphics3D/Vector3.h>
#include <Am/Graphics3D/Vector2.h>
#include <Am/Graphics3D/Polygon.h>
#include <Am/Imaging/Image.h>
#include <Am/Lang/Array.h>
#include <libc/core_inline_functions.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

function_result Mesh3D_Renderer3D__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }
__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Mesh3D_Renderer3D__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Mesh3D_Renderer3D__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

static void rotation_x(double angle, struct Am_Graphics3D_Matrix3 *out)
{
    double c = cos(angle);
    double s = sin(angle);
    out->m00 = 1.0; out->m01 = 0.0; out->m02 = 0.0;
    out->m10 = 0.0; out->m11 = c;   out->m12 = -s;
    out->m20 = 0.0; out->m21 = s;   out->m22 = c;
}

static void rotation_y(double angle, struct Am_Graphics3D_Matrix3 *out)
{
    double c = cos(angle);
    double s = sin(angle);
    out->m00 = c;   out->m01 = 0.0; out->m02 = s;
    out->m10 = 0.0; out->m11 = 1.0; out->m12 = 0.0;
    out->m20 = -s;  out->m21 = 0.0; out->m22 = c;
}

static void rotation_z(double angle, struct Am_Graphics3D_Matrix3 *out)
{
    double c = cos(angle);
    double s = sin(angle);
    out->m00 = c;   out->m01 = -s;  out->m02 = 0.0;
    out->m10 = s;   out->m11 = c;   out->m12 = 0.0;
    out->m20 = 0.0; out->m21 = 0.0; out->m22 = 1.0;
}

static void multiply_matrix(const struct Am_Graphics3D_Matrix3 *a,
                            const struct Am_Graphics3D_Matrix3 *b,
                            struct Am_Graphics3D_Matrix3 *out)
{
    struct Am_Graphics3D_Matrix3 r;
    r.m00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20;
    r.m01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21;
    r.m02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22;
    r.m10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20;
    r.m11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21;
    r.m12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22;
    r.m20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20;
    r.m21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21;
    r.m22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22;
    *out = r;
}

static void transform_vector(const struct Am_Graphics3D_Matrix3 *m,
                             const struct Am_Graphics3D_Vector3 *v,
                             const struct Am_Graphics3D_Vector3 *translation,
                             struct Am_Graphics3D_Vector3 *out)
{
    out->x = m->m00 * v->x + m->m01 * v->y + m->m02 * v->z + translation->x;
    out->y = m->m10 * v->x + m->m11 * v->y + m->m12 * v->z + translation->y;
    out->z = m->m20 * v->x + m->m21 * v->y + m->m22 * v->z + translation->z;
}

function_result Mesh3D_Renderer3D_drawPolygons2_0(aobject *mesh, aobject *textures, aobject *pixels, aobject *zbuffer, int bufW, int bufH)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (mesh != NULL) {
        __increase_reference_count(mesh);
    }
    if (textures != NULL) {
        __increase_reference_count(textures);
    }
    if (pixels != NULL) {
        __increase_reference_count(pixels);
    }
    if (zbuffer != NULL) {
        __increase_reference_count(zbuffer);
    }

    if (mesh == NULL || textures == NULL || pixels == NULL || zbuffer == NULL) {
        goto __exit;
    }

    double halfW = (double)bufW * 0.5;
    double halfH = (double)bufH * 0.5;
    double pd = 200.0;

    aobject *positionRotationObj =
        mesh->object_properties.class_object_properties.properties[Am_Graphics3D_Mesh_P_positionRotation].nullable_value.value.object_value;
    aobject *vertsContainer =
        mesh->object_properties.class_object_properties.properties[Am_Graphics3D_Mesh_P_vertices].nullable_value.value.object_value;
    aobject *polysContainer =
        mesh->object_properties.class_object_properties.properties[Am_Graphics3D_Mesh_P_polygons].nullable_value.value.object_value;
    aobject *tvertsContainer =
        mesh->object_properties.class_object_properties.properties[Am_Graphics3D_Mesh_P_textureVertices].nullable_value.value.object_value;

    if (positionRotationObj == NULL || vertsContainer == NULL || polysContainer == NULL || tvertsContainer == NULL) {
        goto __exit;
    }

    array_holder *positionRotationAh = get_array_holder(positionRotationObj);
    struct Am_Graphics3D_Vector3 *positionRotation = (struct Am_Graphics3D_Vector3 *)positionRotationAh->array_data;
    struct Am_Graphics3D_Vector3 meshPos = positionRotation[0];
    struct Am_Graphics3D_Vector3 meshRot = positionRotation[1];

    aobject *vertsArrayObj =
        vertsContainer->object_properties.class_object_properties.properties[Am_Graphics3D_Vectors3_P_vectors].nullable_value.value.object_value;
    aobject *polysArrayObj =
        polysContainer->object_properties.class_object_properties.properties[Am_Graphics3D_Polygons_P_polygons].nullable_value.value.object_value;
    aobject *tvertsArrayObj =
        tvertsContainer->object_properties.class_object_properties.properties[Am_Graphics3D_Vectors2_P_vectors].nullable_value.value.object_value;

    if (vertsArrayObj == NULL || polysArrayObj == NULL || tvertsArrayObj == NULL) {
        goto __exit;
    }

    array_holder *vertsAh = get_array_holder(vertsArrayObj);
    array_holder *polysAh = get_array_holder(polysArrayObj);
    array_holder *tvertsAh = get_array_holder(tvertsArrayObj);
    array_holder *texturesAh = get_array_holder(textures);
    array_holder *pixelsAh = get_array_holder(pixels);
    array_holder *zbufferAh = get_array_holder(zbuffer);

    struct Am_Graphics3D_Vector3 *vertsData = (struct Am_Graphics3D_Vector3 *)vertsAh->array_data;
    struct Am_Graphics3D_Polygon *polysData = (struct Am_Graphics3D_Polygon *)polysAh->array_data;
    struct Am_Graphics3D_Vector2 *tvertsData = (struct Am_Graphics3D_Vector2 *)tvertsAh->array_data;
    aobject **textureObjects = (aobject **)texturesAh->array_data;
    unsigned int *pixelData = (unsigned int *)pixelsAh->array_data;
    double *zbufferData = (double *)zbufferAh->array_data;

    struct Am_Graphics3D_Matrix3 rotX;
    struct Am_Graphics3D_Matrix3 rotY;
    struct Am_Graphics3D_Matrix3 rotZ;
    struct Am_Graphics3D_Matrix3 rotTmp;
    struct Am_Graphics3D_Matrix3 rotMat;

    rotation_x(meshRot.x * M_PI / 180.0, &rotX);
    rotation_y(meshRot.y * M_PI / 180.0, &rotY);
    rotation_z(meshRot.z * M_PI / 180.0, &rotZ);
    multiply_matrix(&rotY, &rotX, &rotTmp);
    multiply_matrix(&rotTmp, &rotZ, &rotMat);

    for (unsigned int pi = 0; pi < polysAh->size; ++pi) {
        struct Am_Graphics3D_Polygon poly = polysData[pi];
        int vi0 = (int)poly.vertex1Index;
        int vi1 = (int)poly.vertex2Index;
        int vi2 = (int)poly.vertex3Index;
        int ti0 = (int)poly.textureVertex1Index;
        int ti1 = (int)poly.textureVertex2Index;
        int ti2 = (int)poly.textureVertex3Index;

        struct Am_Graphics3D_Vector3 wv0;
        struct Am_Graphics3D_Vector3 wv1;
        struct Am_Graphics3D_Vector3 wv2;
        transform_vector(&rotMat, &vertsData[vi0], &meshPos, &wv0);
        transform_vector(&rotMat, &vertsData[vi1], &meshPos, &wv1);
        transform_vector(&rotMat, &vertsData[vi2], &meshPos, &wv2);

        if (wv0.z <= 0.1 || wv1.z <= 0.1 || wv2.z <= 0.1) {
            continue;
        }

        double ax = wv0.x * pd / wv0.z + halfW;
        double ay = -(wv0.y * pd / wv0.z) + halfH;
        double az = wv0.z;
        double bx = wv1.x * pd / wv1.z + halfW;
        double by = -(wv1.y * pd / wv1.z) + halfH;
        double bz = wv1.z;
        double cx = wv2.x * pd / wv2.z + halfW;
        double cy = -(wv2.y * pd / wv2.z) + halfH;
        double cz = wv2.z;

        struct Am_Graphics3D_Vector2 tv0 = tvertsData[ti0];
        struct Am_Graphics3D_Vector2 tv1 = tvertsData[ti1];
        struct Am_Graphics3D_Vector2 tv2 = tvertsData[ti2];
        double au = tv0.x;
        double av = tv0.y;
        double bu = tv1.x;
        double bv = tv1.y;
        double cu = tv2.x;
        double cv = tv2.y;

        int texIdx = (int)poly.textureIndex;
        if (texIdx < 0 || (unsigned int)texIdx >= texturesAh->size) {
            continue;
        }

        aobject *texture = textureObjects[texIdx];
        if (texture == NULL) {
            continue;
        }

        unsigned short tw = texture->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        unsigned short th = texture->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        aobject *texPixelsObj = texture->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
        if (texPixelsObj == NULL || tw == 0 || th == 0) {
            continue;
        }

        array_holder *texPixelsAh = get_array_holder(texPixelsObj);
        unsigned int *texPixels = (unsigned int *)texPixelsAh->array_data;
        double twDouble = (double)tw;
        double thDouble = (double)th;

        double minX = ax;
        if (bx < minX) { minX = bx; }
        if (cx < minX) { minX = cx; }
        double maxX = ax;
        if (bx > maxX) { maxX = bx; }
        if (cx > maxX) { maxX = cx; }
        double minY = ay;
        if (by < minY) { minY = by; }
        if (cy < minY) { minY = cy; }
        double maxY = ay;
        if (by > maxY) { maxY = by; }
        if (cy > maxY) { maxY = cy; }

        int x0 = (int)minX;
        int x1 = (int)maxX;
        int y0 = (int)minY;
        int y1 = (int)maxY;
        if (x0 < 0) { x0 = 0; }
        if (y0 < 0) { y0 = 0; }
        if (x1 >= bufW) { x1 = bufW - 1; }
        if (y1 >= bufH) { y1 = bufH - 1; }
        if (x0 > x1 || y0 > y1) {
            continue;
        }

        double denom = (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
        if (denom > -0.0001 && denom < 0.0001) {
            continue;
        }
        double invDenom = 1.0 / denom;
        double e00 = by - cy;
        double e01 = cx - bx;
        double e10 = cy - ay;
        double e11 = ax - cx;

        int py = y0;
        double fpy = (double)y0 + 0.5;
        while (py <= y1) {
            int px = x0;
            double fpx = (double)x0 + 0.5;
            while (px <= x1) {
                double w0 = (e00 * (fpx - cx) + e01 * (fpy - cy)) * invDenom;
                double w1 = (e10 * (fpx - cx) + e11 * (fpy - cy)) * invDenom;
                double w2 = 1.0 - w0 - w1;
                if (w0 >= 0.0 && w1 >= 0.0 && w2 >= 0.0) {
                    double pz = w0 * az + w1 * bz + w2 * cz;
                    int idx = py * bufW + px;
                    if (pz < zbufferData[idx]) {
                        zbufferData[idx] = pz;

                        double pu = w0 * au + w1 * bu + w2 * cu;
                        double pv = w0 * av + w1 * bv + w2 * cv;
                        if (pu < 0.0) { pu = 0.0; }
                        if (pu >= 1.0) { pu = 0.9999; }
                        if (pv < 0.0) { pv = 0.0; }
                        if (pv >= 1.0) { pv = 0.9999; }

                        int tpx = (int)(pu * twDouble);
                        int tpy = (int)(pv * thDouble);
                        pixelData[idx] = texPixels[tpy * (int)tw + tpx];
                    }
                }
                ++px;
                fpx += 1.0;
            }
            ++py;
            fpy += 1.0;
        }
    }

__exit: ;
    if (mesh != NULL) {
        __decrease_reference_count(mesh);
    }
    if (textures != NULL) {
        __decrease_reference_count(textures);
    }
    if (pixels != NULL) {
        __decrease_reference_count(pixels);
    }
    if (zbuffer != NULL) {
        __decrease_reference_count(zbuffer);
    }
    return __result;
}