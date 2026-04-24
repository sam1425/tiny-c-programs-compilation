#include <stdio.h>
#include "texteditor.h"

state_handler mode_functions[] = {
    [NORMAL] = handle_normal,
    [INSERT] = handle_insert,
    [VISUAL] = handle_visual
};

int get_index_from_coords(Editor *ed) {
    return ed->cursor_offset;
}

void handle_normal(Editor *ed,char c){
    switch (c) {
    case 'q':
        ed->active = false;
        break;
    case 'h':
        if (ed->cursor_offset > 0) {
            if (rope_char_at(ed->rope.root, ed->cursor_offset - 1) != '\n') {
                ed->cursor_offset--;
                recalculate_cursor_visual(ed);
            }
        }
        
        break;
    case 'l':
        if (ed->cursor_offset < ed->buffer_len) {
            char next = rope_char_at(ed->rope.root, ed->cursor_offset);
            if (next != '\n' && next != '\0') {
                ed->cursor_offset++;
                recalculate_cursor_visual(ed);
            }
        }
        break;
        
        case 'k': {
            if (ed->cursory <= ed->start_rows) break;
            size_t col_offset = ed->cursorx - ed->start_cols;
            int prev_line_start = find_offset_of_line(ed, ed->cursory - 1);
            if (prev_line_start < 0) break;  // was: if (prev_line_start) — wrong!
            size_t prev_line_end = (size_t)prev_line_start;
            while (prev_line_end < ed->buffer_len &&
            rope_char_at(ed->rope.root, prev_line_end) != '\n')
            prev_line_end++;
            size_t prev_len = prev_line_end - (size_t)prev_line_start;
            size_t target_col = (col_offset < prev_len) ? col_offset
            : (prev_len > 0 ? prev_len - 1 : 0);
            ed->cursor_offset = (size_t)prev_line_start + target_col;
            recalculate_cursor_visual(ed);
            break;
        }
        case 'j': {
            int col_offset = ed->cursorx - ed->start_cols;
            int next_line_start = find_offset_of_line(ed, ed->cursory + 1);
            if (next_line_start < 0) break;  // line doesn't exist, stop
            size_t next_line_end = (size_t)next_line_start;
            while (next_line_end < ed->buffer_len &&
            rope_char_at(ed->rope.root, next_line_end) != '\n')
            next_line_end++;
            int next_len = (int)(next_line_end - (size_t)next_line_start);
            int target_col = (col_offset < next_len) ? col_offset
            : (next_len > 0 ? next_len - 1 : 0);
            ed->cursor_offset = (size_t)next_line_start + (size_t)target_col;
            recalculate_cursor_visual(ed);
            break;
        }
        case 'a': {
            if (ed->cursor_offset < ed->buffer_len) {
                char next = rope_char_at(ed->rope.root, ed->cursor_offset);
                if (next != '\n') ed->cursor_offset++;
            }
            ed->state = INSERT;
            recalculate_cursor_visual(ed);
            printf(ed->cursors[2]);
            break;
        }

        case '$': {
            int line_start = find_line_start_offset(ed);
            int line_end   = find_line_end_offset(ed);
            if (line_end < 0 || line_start < 0) break;

            if (line_end <= line_start) {
                ed->cursor_offset = (size_t)line_start;
            } else {
                ed->cursor_offset = (size_t)(line_end - 1);  // land on last real char
            }
            recalculate_cursor_visual(ed);
            break;
        }
        case 'A': {
            int line_end = find_line_end_offset(ed);
            ed->cursor_offset = line_end;
            ed->state = INSERT;
            recalculate_cursor_visual(ed);
            printf(ed->cursors[2]);
            break;
        }

        case 'I': {
            int line_start = find_line_start_offset(ed);
            ed->cursor_offset = line_start;
            ed->state = INSERT;
            recalculate_cursor_visual(ed);
            printf(ed->cursors[2]);
            break;
        }

    case 'x':
        if (ed->buffer_len > 0 && ed->cursor_offset < ed->buffer_len) {
            char ch = rope_char_at(ed->rope.root, ed->cursor_offset);
            if (ch != '\n') {
                RopeNode *left, *mid, *right, *temp;
                split(ed->rope.root, ed->cursor_offset, &left, &temp);
                split(temp, 1, &mid, &right);
                ed->rope.root = concatenate(left, right);
                ed->buffer_len--;
                if (ed->cursor_offset >= ed->buffer_len && ed->cursor_offset > 0)
                ed->cursor_offset--;
                recalculate_cursor_visual(ed);
            }
        }
        break;
    case 'v':
        ed->state = VISUAL;
        ed->visual_start = ed->cursor_offset;
        ed->visual_end   = ed->cursor_offset;
        break;
        case '0': {
            int line_start = find_line_start_offset(ed);
            ed->cursor_offset = line_start;
            recalculate_cursor_visual(ed);
        }
        break;
    case 'g':
        if(ed->lastchar == 'g'){
            // ed->cursorx = ed->start_cols;
            // ed->cursory = ed->start_rows;
            ed->cursorx = 0;
            ed->cursory = 0;

            // c = '\0';
        }
        // if (ed->buffer_len > 0 && ed->cursor_offset < ed->buffer_len) {
        //     char ch = rope_char_at(ed->rope.root, ed->cursor_offset);
        //     ed->cursorx = 0;
        //     ed->cursory = 0;
        //     ed->cursor_offset;
        //     if (ch != '\n') {
        //         RopeNode *left, *mid, *right, *temp;
        //         split(ed->rope.root, ed->cursor_offset, &left, &temp);
        //         split(temp, 1, &mid, &right);
        //         ed->rope.root = concatenate(left, right);
        //         ed->buffer_len--;
        //         if (ed->cursor_offset >= ed->buffer_len && ed->cursor_offset > 0)
        //         ed->cursor_offset--;
        //         recalculate_cursor_visual(ed);
        //     }
        // }

        break;
        case 'G': ed->cursory = ed->max_rows; ed->cursorx = ed->start_rows; break;
    case ' ':
        ed->currentcursor = (ed->currentcursor + 1) % (sizeof(ed->cursors)/sizeof(ed->cursors[0]));
        printf(ed->cursors[ed->currentcursor]);
        break;
        case 'i': ed->state = INSERT; printf(ed->cursors[2]);break;
        case 'd': {
            if (ed->lastchar == 'd') {
                if (ed->buffer_len == 0) {
                    c = '\0';
                    break;
                }

                int line_start = find_line_start_offset(ed);
                int line_end   = find_line_end_offset(ed);

                if (line_start < 0 || line_end < 0) {
                    c = '\0';
                    break;
                }

                size_t del_start = (size_t)line_start;
                size_t del_end   = (size_t)line_end;

                // Include trailing newline to delete the actual line structure
                if (del_end < ed->buffer_len && rope_char_at(ed->rope.root, del_end) == '\n') {
                    del_end++;
                } else if (del_start > 0 && rope_char_at(ed->rope.root, del_start - 1) == '\n') {
                    // If at EOF with no trailing newline, consume the preceding newline instead
                    del_start--;
                }

                size_t del_len = del_end - del_start;
                if (del_len > 0) {
                    RopeNode *left, *mid, *right, *temp;
                    split(ed->rope.root, (int)del_start, &left, &temp);
                    split(temp, (int)del_len, &mid, &right);
                    
                    ed->rope.root = concatenate(left, right);
                    ed->buffer_len -= del_len;

                    // Update offset and clamp to valid buffer range
                    ed->cursor_offset = del_start;
                    if (ed->cursor_offset >= ed->buffer_len) {
                        ed->cursor_offset = (ed->buffer_len > 0) ? ed->buffer_len - 1 : 0;
                    }

                    recalculate_cursor_visual(ed);
                }
                c = '\0'; // Consume 'd' so next key isn't compared against it
            }
            break;
        }
        case  27: ed->state = NORMAL; printf(ed->cursors[0]);break;
    }
    ed->lastchar = c;
}

void handle_visual(Editor *ed, char c) {
    switch (c) {
    // --- cancel ---
    case 27:  // ESC
    case 'v':
        ed->state = NORMAL;
        printf(ed->cursors[0]);
        break;

    // --- movement: same logic as normal, but extend selection ---
case 'h':
    if (ed->cursor_offset > 0) {
        if (rope_char_at(ed->rope.root, ed->cursor_offset - 1) != '\n') {
            ed->cursor_offset--;
            recalculate_cursor_visual(ed);
            ed->visual_end = ed->cursor_offset;
        }
    }
    break;

case 'l':
    if (ed->cursor_offset < ed->buffer_len) {
        char next = rope_char_at(ed->rope.root, ed->cursor_offset);
        if (next != '\n' && next != '\0') {
            ed->cursor_offset++;
            recalculate_cursor_visual(ed);
            ed->visual_end = ed->cursor_offset;
        }
    }
    break;

    case 'k': {
        int col_offset      = ed->cursorx - ed->start_cols;
        int prev_line_start = find_offset_of_line(ed, ed->cursory - 1);
        if (prev_line_start >= 0) {
            size_t prev_line_end = prev_line_start;
            while (prev_line_end < ed->buffer_len &&
            rope_char_at(ed->rope.root, prev_line_end) != '\n')
            prev_line_end++;
            int prev_len   = prev_line_end - prev_line_start;
            int target_col = (col_offset < prev_len) ? col_offset
            : (prev_len > 0 ? prev_len - 1 : 0);
            ed->cursor_offset = prev_line_start + target_col;
            recalculate_cursor_visual(ed);
            ed->visual_end = ed->cursor_offset;
        }
        break;
    }

    case 'j': {
        int col_offset      = ed->cursorx - ed->start_cols;
        int next_line_start = find_offset_of_line(ed, ed->cursory + 1);
        if (next_line_start >= 0) {
            size_t next_line_end = next_line_start;
            while (next_line_end < ed->buffer_len &&
            rope_char_at(ed->rope.root, next_line_end) != '\n')
            next_line_end++;
            int next_len   = next_line_end - next_line_start;
            int target_col = (col_offset < next_len) ? col_offset
            : (next_len > 0 ? next_len - 1 : 0);
            ed->cursor_offset = next_line_start + target_col;
            recalculate_cursor_visual(ed);
            ed->visual_end = ed->cursor_offset;
        }
        break;
    }

    case '0': {
        int line_start    = find_line_start_offset(ed);
        ed->cursor_offset = line_start;
        recalculate_cursor_visual(ed);
        ed->visual_end = ed->cursor_offset;
        break;
    }

    case '$': {
        int line_start = find_line_start_offset(ed);
        int line_end   = find_line_end_offset(ed);
        if (line_end < 0 || line_start < 0) break;
        // empty line: stay put
        if (line_end <= line_start) {
            ed->cursor_offset = (size_t)line_start;
        } else {
            ed->cursor_offset = (size_t)(line_end - 1);  // land on last real char
        }
        recalculate_cursor_visual(ed);
        break;
    }
    // --- delete selected region ---
    case 'x': {
        // normalize so vs <= ve
        size_t vs = ed->visual_start < ed->visual_end
        ? ed->visual_start : ed->visual_end;
        size_t ve = ed->visual_start < ed->visual_end
        ? ed->visual_end   : ed->visual_start;
        // ve is inclusive, so length = ve - vs + 1
        size_t del_len = ve - vs + 1;

        RopeNode *left, *mid, *right, *temp;
        split(ed->rope.root, (int)vs,       &left, &temp);
        split(temp,           (int)del_len,  &mid,  &right);
        // mid is the deleted region; just drop it
        ed->rope.root   = concatenate(left, right);
        ed->buffer_len -= del_len;

        // land cursor at start of deleted region (clamped)
        ed->cursor_offset = vs;
        if (ed->cursor_offset > ed->buffer_len && ed->buffer_len > 0)
        ed->cursor_offset = ed->buffer_len - 1;
        else if (ed->buffer_len == 0)
        ed->cursor_offset = 0;

        recalculate_cursor_visual(ed);
        ed->state = NORMAL;
        printf(ed->cursors[0]);
        break;
    }
}
ed->lastchar = c;
}


void handle_insert(Editor *ed, char c) {
    if (c == 27) {
        ed->state = NORMAL;
        printf("%s", ed->cursors[0]);
        int line_start = find_line_start_offset(ed);
        if ((int)ed->cursor_offset > line_start) {
            ed->cursor_offset--;
            recalculate_cursor_visual(ed);
        }
        return;
    }
    size_t pos = ed->cursor_offset;
    if (c == 127 || c == 8) {
        if (pos > 0) {
            RopeNode *left, *mid, *right, *temp;
            split(ed->rope.root, pos - 1, &left, &temp);
            split(temp, 1, &mid, &right);
            ed->rope.root = concatenate(left, right);
            ed->cursor_offset--;
            ed->buffer_len--;
            recalculate_cursor_visual(ed);
        }
    } else {
        char insert_char = (c == 13) ? '\n' : c;
        char str[2] = {insert_char, '\0'};
        RopeNode *new_node = create_node(str);
        RopeNode *left, *right;
        split(ed->rope.root, pos, &left, &right);
        ed->rope.root = concatenate(concatenate(left, new_node), right);
        ed->cursor_offset++;
        ed->buffer_len++;
        recalculate_cursor_visual(ed);
    }
}

// printf("\033[s");         // save position
// printf("\033[999;999H"); // move to huge position (terminal clamps it)
// printf("\033[6n");        // request cursor position report
// fflush(stdout);
// printf("\033[u");

//printf("\033[?25h");
//printf("\033[H"); clean screen
//printf("\033[%d;1H\033[7m -- %s -- %03d:%03d \033[0m", 
//ed.max_rows, 
//ed.state == NORMAL ? "NORMAL" : "INSERT", 
//ed.cursory, ed.cursorx);
//cursor
//printf("\033[H");//clear screeng
//printf("\033[%d;1H", ed.cursory); 
//printf("\033[2K");//clear line
