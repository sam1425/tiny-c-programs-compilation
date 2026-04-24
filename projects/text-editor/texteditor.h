#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 64
#define FILE_BUFFER_SIZE 256


enum textstate { NORMAL, INSERT, VISUAL };

typedef struct RopeNode {
    struct RopeNode *left, *right;
    int weight;
    char *fragment;
} RopeNode;

typedef struct {
    RopeNode *root;
    size_t total_len;
} Rope;

typedef struct {
    enum textstate state;
    Rope rope;
    size_t buffer_len;
    size_t cursor_offset;
    size_t start_cols, start_rows;
    size_t cursorx, cursory;
    size_t visual_start;
    size_t visual_end; 
    size_t currentcursor;
    bool active;
    size_t max_rows, max_cols;
    char lastchar;
    char *cursors[3];
} Editor;

typedef void (*state_handler)(Editor *ed, char c);
void handle_normal(Editor *ed, char c);
void handle_insert(Editor *ed, char c);
void handle_visual(Editor *ed, char c);
extern state_handler mode_functions[];

RopeNode* create_node(const char *str);
RopeNode* concatenate(RopeNode *L, RopeNode *R);
void      split(RopeNode *node, int idx, RopeNode **left, RopeNode **right);
char      rope_char_at(RopeNode *node, int idx);
int       find_line_start_offset(Editor *ed);
int       find_line_end_offset(Editor *ed);
int       find_offset_of_line(Editor *ed, int target_line);
void      recalculate_cursor_visual(Editor *ed);
void      draw_rope(RopeNode *node, int *current_line);
void      draw_rope_visual(RopeNode *node, int *current_line,
                           size_t vis_start, size_t vis_end, size_t *idx);
int get_index_from_coords(Editor *ed);
