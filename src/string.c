#include "../include/mystring.h"
#include "../include/class.h"
#include "../include/new.h"

#include <assert.h>
#include <string.h>

struct String {
    const void* class;
    char* text;
};

static void* String_Constructor(void* _self, va_list* valist)
{
    struct String* self = _self;
    const char* text = va_arg(*valist, const char*);

    self->text = malloc(strlen(text) + 1);
    assert(self->text);
    strcpy(self->text, text);
    return self;
}

static void* String_Destructor(void* _self)
{
    struct String* self = _self;
    free(self->text);
    self->text = NULL;
    return self;
}

static void* String_Clone(const void* _self)
{
    const struct String* self = _self;
    return New(String, self->text);
}

static int String_Differ(const void* _self, const void* _b)
{
    const struct String* self = _self;
    const struct String* b = _b;

    if (self == b)
        return 0;
    if (!b || b->class != String)
        return 1;
    return strncmp(self->text, b->text, strlen(self->text));
}

static const Class _String = { sizeof(String), String_Constructor, String_Destructor,
    String_Clone, String_Differ };

const void* String = &_String;
