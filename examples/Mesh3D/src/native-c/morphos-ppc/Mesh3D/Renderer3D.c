#include <libc/core.h>
#include <Mesh3D/Renderer3D.h>

#define STUB_VOID(name) \
function_result name { \
    function_result __result = { .has_return_value = false }; \
    bool __returning = false; \
__exit: ; \
    return __result; \
}

STUB_VOID(Mesh3D_Renderer3D__native_init_0(aobject * const this))
STUB_VOID(Mesh3D_Renderer3D__native_release_0(aobject * const this))
STUB_VOID(Mesh3D_Renderer3D__native_mark_children_0(aobject * const this))
STUB_VOID(Mesh3D_Renderer3D_drawPolygons2_0(aobject *mesh, aobject *textures, aobject *pixels, aobject *zbuffer, int bufW, int bufH))