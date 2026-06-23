#include <libc/core.h>
#include <Am/Ui/FolderRequester.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <libc/core_inline_functions.h>

#include <string.h>

#ifdef __APPLE__
#include <objc/runtime.h>
#include <objc/message.h>
#endif

#ifdef AM_UI_LINUX_GTK
#include <gtk/gtk.h>
#endif

// FolderRequester — NSOpenPanel pop-up. Configured to allow only a
// single directory pick (no files), returns the chosen path as an
// AmLang String, NULL on cancel. AmLang flow: AmIde calls
// `FolderRequester.requestFolder(...)` from a menu pick; we drop a
// modal panel on the user, wait for them to OK / Cancel, then return.
//
// All Cocoa calls go through the Objective-C runtime so this stays a
// plain `.c` file (matches the NSMenu bridge in Window.c).

function_result Am_Ui_FolderRequester__native_init_0(aobject *const this)         { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_FolderRequester__native_mark_children_0(aobject *const this){ (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_FolderRequester__native_release_0(aobject *const this)      { (void) this; return (function_result){ .has_return_value = false }; }

#ifdef __APPLE__

typedef long NSInteger;
#define NSModalResponseOK 1

static const char *amlang_string_to_c(aobject *s)
{
    if (s == NULL) return NULL;
    string_holder *sh = (string_holder *) (s + 1);
    if (sh == NULL) return NULL;
    return sh->string_value;
}

function_result Am_Ui_FolderRequester_requestFolder_0(aobject *title, aobject *initialPath, aobject *parent)
{
    function_result __result = { .has_return_value = true };
    __result.return_value.value.object_value = NULL;
    (void) parent;

    Class NSOpenPanel = objc_getClass("NSOpenPanel");
    Class NSString    = objc_getClass("NSString");
    Class NSURL       = objc_getClass("NSURL");
    if (NSOpenPanel == NULL || NSString == NULL) return __result;

    SEL openPanel_sel    = sel_registerName("openPanel");
    SEL setTitle_sel     = sel_registerName("setTitle:");
    SEL setCanChooseFiles_sel       = sel_registerName("setCanChooseFiles:");
    SEL setCanChooseDirectories_sel = sel_registerName("setCanChooseDirectories:");
    SEL setAllowsMultipleSelection_sel = sel_registerName("setAllowsMultipleSelection:");
    SEL setDirectoryURL_sel = sel_registerName("setDirectoryURL:");
    SEL runModal_sel     = sel_registerName("runModal");
    SEL URLs_sel         = sel_registerName("URLs");
    SEL firstObject_sel  = sel_registerName("firstObject");
    SEL path_sel         = sel_registerName("path");
    SEL UTF8String_sel   = sel_registerName("UTF8String");
    SEL stringWithUTF8String_sel = sel_registerName("stringWithUTF8String:");
    SEL fileURLWithPath_isDirectory_sel = sel_registerName("fileURLWithPath:isDirectory:");

    // [NSOpenPanel openPanel] is the convenience constructor; returns
    // an autoreleased panel ready to configure.
    id panel = ((id (*)(Class, SEL)) objc_msgSend)(NSOpenPanel, openPanel_sel);
    if (panel == NULL) return __result;

    const char *title_c = amlang_string_to_c(title);
    if (title_c != NULL) {
        id ns_title = ((id (*)(Class, SEL, const char *)) objc_msgSend)(NSString, stringWithUTF8String_sel, title_c);
        ((void (*)(id, SEL, id)) objc_msgSend)(panel, setTitle_sel, ns_title);
    }

    ((void (*)(id, SEL, signed char)) objc_msgSend)(panel, setCanChooseFiles_sel, 0);
    ((void (*)(id, SEL, signed char)) objc_msgSend)(panel, setCanChooseDirectories_sel, 1);
    ((void (*)(id, SEL, signed char)) objc_msgSend)(panel, setAllowsMultipleSelection_sel, 0);

    const char *initial_c = amlang_string_to_c(initialPath);
    if (initial_c != NULL && initial_c[0] != '\0' && NSURL != NULL) {
        id ns_path = ((id (*)(Class, SEL, const char *)) objc_msgSend)(NSString, stringWithUTF8String_sel, initial_c);
        id url = ((id (*)(Class, SEL, id, signed char)) objc_msgSend)(NSURL, fileURLWithPath_isDirectory_sel, ns_path, 1);
        if (url != NULL) {
            ((void (*)(id, SEL, id)) objc_msgSend)(panel, setDirectoryURL_sel, url);
        }
    }

    NSInteger rc = ((NSInteger (*)(id, SEL)) objc_msgSend)(panel, runModal_sel);
    if (rc != NSModalResponseOK) return __result;   // user cancelled

    id urls = ((id (*)(id, SEL)) objc_msgSend)(panel, URLs_sel);
    if (urls == NULL) return __result;
    id first = ((id (*)(id, SEL)) objc_msgSend)(urls, firstObject_sel);
    if (first == NULL) return __result;
    id ns_path = ((id (*)(id, SEL)) objc_msgSend)(first, path_sel);
    if (ns_path == NULL) return __result;
    const char *picked = ((const char *(*)(id, SEL)) objc_msgSend)(ns_path, UTF8String_sel);
    if (picked == NULL) return __result;

    aobject *amlang_picked = __create_string(picked, &Am_Lang_String);
    __result.return_value.value.object_value = amlang_picked;
    return __result;
}

#elif defined(AM_UI_LINUX_GTK)   /* GTK native folder chooser */

static const char *amlang_string_to_c(aobject *s)
{
    if (s == NULL) return NULL;
    string_holder *sh = (string_holder *) (s + 1);
    if (sh == NULL) return NULL;
    return sh->string_value;
}

function_result Am_Ui_FolderRequester_requestFolder_0(aobject *title, aobject *initialPath, aobject *parent)
{
    function_result __result = { .has_return_value = true };
    __result.return_value.value.object_value = NULL;
    (void) parent;

    if (!gtk_init_check(NULL, NULL)) return __result;   // no display / GTK down

    const char *title_c = amlang_string_to_c(title);
    GtkWidget *dlg = gtk_file_chooser_dialog_new(
        title_c ? title_c : "Select Folder",
        NULL,                                  // no parent GtkWindow handle here
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        NULL);

    const char *initial_c = amlang_string_to_c(initialPath);
    if (initial_c != NULL && initial_c[0] != '\0') {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), initial_c);
    }

    // gtk_dialog_run spins a nested GTK main loop until the user responds.
    // Safe to nest inside the Window's per-frame gtk_main_iteration pump.
    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (path != NULL) {
            __result.return_value.value.object_value = __create_string(path, &Am_Lang_String);
            g_free(path);
        }
    }
    gtk_widget_destroy(dlg);
    // Let the destroyed dialog actually disappear before we hand control back.
    while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
    return __result;
}

#else  /* !__APPLE__ && !GTK — no native folder requester */

// Returns null (treated as "cancelled" by the AmLang side).
function_result Am_Ui_FolderRequester_requestFolder_0(aobject *title, aobject *initialPath, aobject *parent)
{
    function_result __result = { .has_return_value = true };
    __result.return_value.value.object_value = NULL;
    (void) title; (void) initialPath; (void) parent;
    return __result;
}

#endif  /* __APPLE__ */
