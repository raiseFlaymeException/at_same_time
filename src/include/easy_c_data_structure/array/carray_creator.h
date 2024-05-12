#ifndef CARRAY_CREATOR_H
#define CARRAY_CREATOR_H

#include <stdlib.h>

///
/// @brief create the declaration of the array only for the struct part
/// (call CARRAY_CREATE_DECLARATION instead to do the two part)
///
/// @param[in] name the name of the struct (the symbol "struct name" and "name"
///     will be created and potentially "polute the namespace") (example:
///     String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION_STRUCT(name, type)                                               \
    struct name {                                                                                  \
        type  *data;                                                                               \
        size_t size;                                                                               \
        size_t cap;                                                                                \
    };                                                                                             \
    typedef struct name name;

///
/// @brief create the declaration of the array only for the struct part
/// (call CARRAY_CREATE_DECLARATION_NOTD instead to do the two part)
/// and don't create a typedef (NOTD)
///
/// @param[in] name the name of the struct (the symbol "struct name" will be
///     created and potentially "polute the namespace") (example: String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION_STRUCT_NOTD(name, type)                                          \
    struct name {                                                                                  \
        type  *data;                                                                               \
        size_t size;                                                                               \
        size_t cap;                                                                                \
    };

///
/// @brief create the declaration of the array only for the struct part
/// and don't create a typedef (NOTD) and don't put semi colon (NOSC) so
/// we can do:
/// struct <name> {
///     <type> *data;
///     ...
/// } <name of a variable to hold the struct in>;
/// (don't forget the ';' and to call CARRAY_CREATE_DECLARATION_FUNCTION)
///
/// @param[in] name the name of the struct (the symbol "struct name" will be
///     created and potentially "polute the namespace") (example: String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION_STRUCT_NOTDSC(name, type)                                        \
    struct name {                                                                                  \
        type  *data;                                                                               \
        size_t size;                                                                               \
        size_t cap;                                                                                \
    }

///
/// @brief create the declaration of the array only for the function part
/// don't forget to call CARRAY_CREATE_DECLARATION_STRUCT before or
/// call CARRAY_CREATE_DECLARATION instead to do the two part
///
/// @param[in] name the name of the struct created in the declaration of the
/// struct
///     (example: String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION_FUNCTION(name, type)                                             \
    void name##_alloc(struct name *array, size_t cap);                                             \
    void name##_free(struct name *array);                                                          \
    void name##_extend(struct name *array, size_t size);                                           \
    void name##_append(struct name *array, type value);                                            \
    void name##_append_ptr(struct name *array, const type *value);                                 \
    void name##_pop(struct name *array, size_t idx);

///
/// @brief create the declaration of the array, if you need more control see
/// (CARRAY_CREATE_DECLARATION_* functions)
///
/// @param[in] name the name of the struct created in the declaration (example:
/// String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION(name, type)                                                      \
    CARRAY_CREATE_DECLARATION_STRUCT(name, type)                                                   \
    CARRAY_CREATE_DECLARATION_FUNCTION(name, type)

///
/// @brief create the declaration of the array, if you need more control see
/// (CARRAY_CREATE_DECLARATION_* functions) and don't create a typedef (NOTD)
///
/// @param[in] name the name of the struct created in the declaration (example:
/// String)
/// @param[in] type the type of the elements in the array (example: char)
///
#define CARRAY_CREATE_DECLARATION_NOTD(name, type)                                                 \
    CARRAY_CREATE_DECLARATION_STRUCT_NOTD(name, type)                                              \
    CARRAY_CREATE_DECLARATION_FUNCTION(name, type)

///
/// @brief create the definition of the array. You need to call
/// CARRAY_CREATE_DECLARATION first
///
/// @param[in] name the name of the struct created in the declaration
/// @param[in] type the type of the struct created in the declaration
///
#define CARRAY_CREATE_DEFINITION(name, type)                                                       \
    void name##_alloc(struct name *array, size_t cap) {                                            \
        array->data = (type *)malloc(cap * sizeof(type));                                          \
        array->size = 0;                                                                           \
        array->cap  = cap;                                                                         \
    }                                                                                              \
    void name##_free(struct name *array) { free(array->data); }                                    \
    void name##_extend(struct name *array, size_t size) {                                          \
        if (size >= array->cap) {                                                                  \
            array->cap  = size;                                                                    \
            array->data = (type *)realloc(array->data, array->cap * sizeof(type));                 \
        }                                                                                          \
    }                                                                                              \
    void name##_append(struct name *array, type value) {                                           \
        if (array->size + 1 >= array->cap) { name##_extend(array, array->cap * 2); }               \
        array->data[array->size++] = value;                                                        \
    }                                                                                              \
    void name##_append_ptr(struct name *array, const type *value) {                                \
        if (array->size + 1 >= array->cap) { name##_extend(array, array->cap * 2); }               \
        array->data[array->size++] = *value;                                                       \
    }                                                                                              \
    void name##_pop(struct name *array, size_t idx) {                                              \
        array->size--;                                                                             \
        for (; idx < array->size; idx++) { array->data[idx] = array->data[idx + 1]; }              \
    }

#endif // CARRAY_CREATOR_H
