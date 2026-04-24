#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>

//terminal stuff
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 64
#define FILE_BUFFER_SIZE 256

enum textstate { NORMAL, INSERT, VISUAL };

typedef struct {
    enum textstate state;
    size_t buffer_len;
    char text_content[FILE_BUFFER_SIZE];
    int start_cols, start_rows;
    int cursorx, cursory;
    int currentcursor;
    bool active;
    unsigned int max_rows, max_cols;
    char lastchar;
    char *cursors[3];
} Editor;

void handle_normal(Editor *ed, char c);
void handle_insert(Editor *ed, char c);
void handle_visual(Editor *ed, char c);

typedef void (*state_handler)(Editor *ed, char c);

state_handler mode_functions[] = {
    [NORMAL] = handle_normal,
    [INSERT] = handle_insert,
    [VISUAL] = handle_visual
};

int main(int argc, char *argv[])
{

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
        .buffer_len = 0,
        .text_content = {0},
        .start_cols = 1,
        .start_rows = 1,
        .cursorx = 1,
        .cursory = 1,
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

    char input_buffer[BUFFER_SIZE];
    printf(ed.cursors[ed.currentcursor]);
    char lastchar = '\0';

    while(ed.active) {
        ssize_t num_read = read(STDIN_FILENO, input_buffer, BUFFER_SIZE);
        if (num_read > 0 ){
            for(int i = 0; i < num_read; i++) {
                mode_functions[ed.state](&ed, input_buffer[i]);
            }
        }
        
        if (ed.cursorx < ed.start_cols) ed.cursorx = ed.start_cols; 
        if (ed.cursory < ed.start_rows) ed.cursory = ed.start_rows; 
        if (ed.cursorx > ed.max_cols) ed.cursorx = ed.max_cols;
        if (ed.cursory > ed.max_rows) ed.cursory = ed.max_rows;
        printf("\033[H\033[J");
        printf("\033[1;1H"); 
        printf("\033[%d;1H", ed.cursory); 
        printf("%s", ed.text_content);
        printf("\033[%d;%dH", ed.cursory, ed.cursorx);
        //printf("%s", ed.text_content);
        //printf("\033[%d;%dH", ed.cursory, ed.cursorx);

        //bar
        printf("\033[%d;1H", ed.max_rows); 
        printf("\033[7m -- %s -- %03d:%03d \033[0m", 
        ed.state == NORMAL ? "NORMAL" : "INSERT", 
        ed.cursory, ed.cursorx);
        
        printf("\033[%d;%dH", ed.cursory, ed.cursorx);
        fflush(stdout);
        usleep(16000);
        }
    cleanup:
        printf(ed.cursors[ed.currentcursor]);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        return 0;
    }

void handle_normal(Editor *ed,char c){
    switch (c) {
    //case 'q': goto cleanup; break;
        case 'h': ed->cursorx--; break;
        case 'l': ed->cursorx++; break;
        case 'k': ed->cursory--; break;
        case 'j': ed->cursory++; break;
        case '0': ed->cursorx = ed->start_cols; break;
        case '$': ed->cursorx = ed->max_cols; break;
    case 'g':
        if(ed->lastchar == 'g'){
            ed->cursorx = ed->start_cols;
            ed->cursory = ed->start_rows;
            //c = '\0';
        }
        break;
        case 'G': ed->cursory = ed->max_rows; ed->cursorx = ed->start_rows; break;
    case ' ':
        ed->currentcursor = (ed->currentcursor + 1) % (sizeof(ed->cursors)/sizeof(ed->cursors[0]));
        printf(ed->cursors[ed->currentcursor]);
        break;
        case 'i': ed->state = INSERT; printf(ed->cursors[2]);break;
        case 'a': ed->state = INSERT; ed->cursorx++;printf(ed->cursors[2]);break;
        case  27: ed->state = NORMAL; printf(ed->cursors[0]);break;
}
//lastchar = c;
ed->lastchar = c;
}

void handle_visual(Editor *ed,char c){
}

void handle_insert(Editor *ed, char c) {
    switch (c){
    case 27://escape
        ed->state = NORMAL;
        printf("%s", ed->cursors[0]);
        break;
    case 127://del
        if (ed->buffer_len > 0) {
            ed->buffer_len--;
            ed->text_content[ed->buffer_len] = '\0';
            ed->cursorx--;
        }
    default:
        if (c >= 32 && c <= 126 && ed->buffer_len < FILE_BUFFER_SIZE - 1) {
                ed->text_content[ed->buffer_len++] = c;
                ed->text_content[ed->buffer_len] = '\0';
                ed->cursorx++;
            }
            break;
        }
    ed->lastchar = c;
}

    // printf("\033[s");         // save position
    // printf("\033[999;999H"); // move to huge position (terminal clamps it)
    // printf("\033[6n");        // request cursor position report
    // fflush(stdout);
    // printf("\033[u");
   
        //printf("\033[?25h");

        //printf("\033[H"); clean screen
        //printf("Press 'q' to quit. Key pressed: ");
