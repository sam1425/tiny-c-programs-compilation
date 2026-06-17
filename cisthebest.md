1. Fat Pointers (The Cello / SDS Mechanism)Both Cello and Redis's sds library use fat pointers (or header-prefixed pointers). Instead of passing a pointer to a struct wrapper, you allocate memory for metadata right before the pointer you hand back to the user.  This allows your custom types to remain 100% syntactically compatible with standard C pointers and APIs.  C#include <stdlib.h>
#include <stddef.h>

typedef struct {
    size_t type_id;
    size_t ref_count;
    size_t allocation_size;
    char data[]; // Flexible array member
} MetadataHeader;

#define GET_HDR(ptr) ((MetadataHeader *)((char *)(ptr) - offsetof(MetadataHeader, data)))

void *meta_alloc(size_t type_id, size_t size) {
    MetadataHeader *hdr = malloc(sizeof(MetadataHeader) + size);
    if (hdr == nil) return nil;
    hdr->type_id = type_id;
    hdr->ref_count = 1;
    hdr->allocation_size = size;
    return hdr->data;
}

void meta_free(void *ptr) {
    if (ptr == nil) return;
    MetadataHeader *hdr = GET_HDR(ptr);
    free(hdr);
}

2. Type-Classes / Interfaces via VtablesCello emulates dynamic interfaces (like Go interfaces or Rust traits) by assigning structural IDs to fat pointers and matching them against static interface structures. You can implement a simplified version of this for generic behaviors like Hash, Eq, or Serializable.  C// Interface definition
typedef struct {
    size_t (*hash)(const void *self);
    int (*eq)(const void *self, const void *other);
} RecordInterface;

// Centralized registry mapping Type IDs to their Interface implementations
typedef struct {
    size_t type_id;
    const RecordInterface *vtable;
} TypeRegistry;

// Generic function implementation
int generic_eq(const void *a, const void *b, const TypeRegistry *registry, size_t reg_size) {
    size_t type_a = GET_HDR(a)->type_id;
    size_t type_b = GET_HDR(b)->type_id;
    if (type_a != type_b) return 0;

    for (size_t i = 0; i < reg_size; i++) {
        if (registry[i].type_id == type_a) {
            return registry[i].vtable->eq(a, b);
        }
    }
    return 0; // Interface not implemented
}

3. RAII / Scope Cleanup via AttributesTo avoid manual teardown blocks (which standard C handles poorly), you can use the GCC/Clang cleanup attribute to implement automatic resource collection.C// Context-specific automatic destruction macro
#define AUTO_RELEASE(type, var, init_val) \
    type var __attribute__((cleanup(meta_free_wrapper))) = init_val

// Helper wrapper matching the required cleanup function signature
static inline void meta_free_wrapper(void *ptr) {
    void **real_ptr = (void **)ptr;
    if (*real_ptr) {
        meta_free(*real_ptr);
    }
}

// Usage inside a block:
void processing_func(void) {
    AUTO_RELEASE(char*, str, meta_alloc(1, 100));
    // str is automatically freed when execution leaves this block scope
}
4. X-Macros (The Synchronized Data Engine)A common pattern in single-header architectures like sdb is using X-Macros to maintain absolute synchronization across components (like enums, lookup strings, and jump-tables) without manual code duplication.C// 1. Define the master structural list once
#define STATUS_CODE_LIST(X) \
    X(STATUS_OK,      0, "Operation successful") \
    X(STATUS_TIMEOUT, 1, "Connection timed out") \
    X(STATUS_NOMEM,   2, "Out of memory memory")

// 2. Expand into an enumeration definitions automatically
typedef enum {
#define AS_ENUM(name, id, description) name = id,
    STATUS_CODE_LIST(AS_ENUM)
#undef AS_ENUM
} StatusCode;

// 3. Expand into string conversion tables automatically 
const char *status_to_string(StatusCode code) {
    switch(code) {
#define AS_CASE(name, id, description) case name: return description;
        STATUS_CODE_LIST(AS_CASE)
#undef AS_CASE
        default: return "Unknown status";
    }
}
5. Intrusive Data Structuressdb and the Linux kernel rely on intrusive data structures. Instead of allocating a wrapper node that points to your data payload, you embed the list/tree linkage structures directly inside your data payload. You then use the CONTAINER_OF macro provided earlier to navigate backwards from the node link to the parent structure object.Ctypedef struct ListHead {
    struct ListHead *next;
    struct ListHead *prev;
} ListHead;

// Intrusive approach: payload encloses the structural links
typedef struct {
    int connection_fd;
    char IP[46];
    ListHead node; // Embedded link structure
} UserSession;

inline void list_add(ListHead *new_node, ListHead *head) {
    new_node->next = head->next;
    new_node->prev = head;
    head->next->prev = new_node;
    head->next = new_node;
}

