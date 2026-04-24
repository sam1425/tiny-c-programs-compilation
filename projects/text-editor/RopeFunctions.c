#include <stdlib.h> 
#include <string.h> 
#include "texteditor.h"

int get_total_weight(RopeNode *node) {
    if (!node) return 0;
    if (node->fragment) return node->weight;
    return node->weight + get_total_weight(node->right);
}

RopeNode* create_node(const char *str) {
    RopeNode *node = calloc(1, sizeof(RopeNode));
    if (str) {
        node->fragment = strdup(str);
        node->weight = strlen(str);
    }
    return node;
}

RopeNode* concatenate(RopeNode *L, RopeNode *R) {
    if (!L) return R;
    if (!R) return L;
    RopeNode *root = create_node(NULL);
    root->left = L;
    root->right = R;
    root->weight = get_total_weight(L);
    return root;
}


void split(RopeNode *node, int idx, RopeNode **left, RopeNode **right) {
    if (!node) {
        *left = *right = NULL;
        return;
    }

    if (node->fragment) { 
        if (idx <= 0) {
            *left = NULL;
            *right = node;
        } else if (idx >= node->weight) {
            *left = node;
            *right = NULL;
        } else {
            char *s1 = strndup(node->fragment, idx);
            char *s2 = strdup(node->fragment + idx);
            *left = create_node(s1);
            *right = create_node(s2);
            free(s1); free(s2);
        }
        return;
    }

    if (idx < node->weight) {
        RopeNode *temp_right;
        split(node->left, idx, left, &temp_right);
        *right = concatenate(temp_right, node->right);
    } else {
        RopeNode *temp_left;
        split(node->right, idx - node->weight, &temp_left, right);
        *left = concatenate(node->left, temp_left);
    }
}

char rope_char_at(RopeNode *node, int idx) {
    if (!node || idx < 0) return '\0';
    if (node->fragment) {
        if (idx < node->weight) return node->fragment[idx];
        return '\0';
    }
    if (idx < node->weight)
        return rope_char_at(node->left, idx);
    else
        return rope_char_at(node->right, idx - node->weight);
}

int find_line_start_offset(Editor *ed) {
    if (ed->cursor_offset == 0) return 0;
    int pos = (int)ed->cursor_offset - 1;
    while (pos >= 0 && rope_char_at(ed->rope.root, pos) != '\n')
        pos--;
    return pos + 1;
}

int find_line_end_offset(Editor *ed) {
    size_t pos = ed->cursor_offset;
    while (pos < ed->buffer_len && rope_char_at(ed->rope.root, pos) != '\n')
        pos++;
    return pos;
}
void recalculate_cursor_visual(Editor *ed) {
    int line = 1;
    int col  = ed->start_cols;
    for (size_t i = 0; i < ed->cursor_offset; i++) {
        if (rope_char_at(ed->rope.root, i) == '\n') {
            line++;
            col = ed->start_cols;
        } else {
            col++;
        }
    }
    ed->cursory = line;
    ed->cursorx = col;
}

// Returns rope offset of the start of a given 1-based line number
int find_offset_of_line(Editor *ed, int target_line) {
    if (target_line <= 1) return 0;
    int line = 1;
    for (size_t i = 0; i < ed->buffer_len; i++) {
        if (rope_char_at(ed->rope.root, i) == '\n') {
            line++;
            if (line == target_line) return i + 1;
        }
    }
    return -1;
}
