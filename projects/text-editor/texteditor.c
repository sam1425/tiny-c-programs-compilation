//terminal stuff
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

//mine
#include "texteditor.h"
void draw_rope_visual(RopeNode *node, int *current_line,
                      size_t vis_start, size_t vis_end,
                      size_t *idx) {
    if (!node) return;
    if (node->fragment) {
        for (char *c = node->fragment; *c; c++, (*idx)++) {
            if (*c == '\n') {
                // end highlight before newline if inside selection
                if (*idx >= vis_start && *idx < vis_end)
                    printf("\033[0m");
                (*current_line)++;
                printf("\n%3d ", *current_line);
            } else {
                if (*idx == vis_start)
                    printf("\033[7m"); // enter reverse video (highlight)
                putchar(*c);
                if (*idx + 1 == vis_end)
                    printf("\033[0m"); // exit highlight
            }
        }
    } else {
        draw_rope_visual(node->left,  current_line, vis_start, vis_end, idx);
        draw_rope_visual(node->right, current_line, vis_start, vis_end, idx);
    }
}
void draw_rope(RopeNode *node, int *current_line) {
    if (!node) return;
    if (node->fragment) {
        for (char *c = node->fragment; *c; c++) {
            if (*c == '\n') {
                (*current_line)++;
                printf("\n%3d ", *current_line);
            } else {
                putchar(*c);
            }
        }
    } else {
        draw_rope(node->left, current_line);
        draw_rope(node->right, current_line);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("2 arguments less, argv:%s", argv[1]);
    }
    
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10000000;

    struct termios raw, original;
    tcgetattr(STDIN_FILENO, &original);
    raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    struct winsize width;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &width);
    printf("Cols: %d  Rows:%d \n",width.ws_col, width.ws_row);
    Editor ed = {
        .state = NORMAL,
        .rope = { .root = NULL, .total_len = 0 },
        .start_cols = 5,
        .start_rows = 1,
        .cursorx    = 5,
        .cursory    = 1,
        .currentcursor = 0,
        .active= true,
        .lastchar = '\0',
	    .cursors = {
		    "\033[2 q", // block
		    "\033[4 q", // underline
		    "\033[6 q"  // vertical bar
	    }
    };
    ed.max_rows = width.ws_row;
    ed.max_cols = width.ws_col;
    printf(ed.cursors[ed.currentcursor]);

    char input_buffer[BUFFER_SIZE];
    char lastchar = '\0';
    lastchar = lastchar;
    bool first_draw = true;
    while(ed.active) {
        ssize_t num_read = read(STDIN_FILENO, input_buffer, BUFFER_SIZE);
        if (num_read > 0 ){
            for(int i = 0; i < num_read; i++) {
                mode_functions[ed.state](&ed, input_buffer[i]);
            }
            first_draw = true;
        }
        
        if (first_draw){
            if (ed.cursorx < ed.start_cols) ed.cursorx = ed.start_cols; 
            if (ed.cursory < ed.start_rows) ed.cursory = ed.start_rows; 
            if (ed.cursorx > ed.max_cols) ed.cursorx = ed.max_cols;
            if (ed.cursory > ed.max_rows -1) ed.cursory = ed.max_rows -1;

            int line_count = 1;
            printf("\033[H\033[J");
            printf("\033[1;1H");
            printf("%3d ", line_count);
            fflush(stdout);
            if (ed.state == VISUAL) {
                size_t vs = ed.visual_start < ed.visual_end ? ed.visual_start : ed.visual_end;
                size_t ve = ed.visual_start < ed.visual_end ? ed.visual_end + 1 : ed.visual_start + 1;
                size_t idx = 0;
                draw_rope_visual(ed.rope.root, &line_count, vs, ve, &idx);
            } else {
                draw_rope(ed.rope.root, &line_count);
            }
            //bar
            printf("\033[%ld;1H", ed.max_rows); 
            printf("\033[7m -- %s -- %03ld:%03ld -- Len: %zu \033[0m", 
                ed.state == NORMAL ? "NORMAL" : ed.state == INSERT ? "INSERT" : "VISUAL",
                ed.cursory, ed.cursorx, ed.buffer_len);
            printf("\033[%ld;%ldH", ed.cursory, ed.cursorx);
            first_draw = false;
        }


        fflush(stdout); nanosleep(&ts, NULL);
        }
        printf(ed.cursors[ed.currentcursor]);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        return 0;
    }
