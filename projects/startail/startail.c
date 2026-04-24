#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <string.h>

#define TRAIL_MAX 100
#define STAR_MAX 100

typedef struct {
    int x, y;
    double t;
} TrailPoint;

typedef struct {
    int x, y;
    double t;
    char symbol;
} Star;

static struct termios orig;
TrailPoint trail[TRAIL_MAX];
int trail_len = 0;

Star stars[STAR_MAX];
int star_len = 0;

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void rgb_to_ansi(int r, int g, int b, char *buf) {
    sprintf(buf, "\033[38;2;%d;%d;%dm", r,g,b);
}

void lerp_color(int r1,int g1,int b1, int r2,int g2,int b2, double t, int *ro,int *go,int *bo) {
    *ro = (int)(r1 + (r2-r1)*t);
    *go = (int)(g1 + (g2-g1)*t);
    *bo = (int)(b1 + (b2-b1)*t);
}

void enable_raw() {
    tcgetattr(STDIN_FILENO, &orig);
    struct termios raw = orig;
    raw.c_lflag &= ~(ECHO|ICANON);
    raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf("\033[?25l"); // hide cursor
    printf("\033[?1003h\033[?1006h"); // mouse tracking
    fflush(stdout);
}

void disable_raw() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
    printf("\033[?25h");
    printf("\033[?1003l\033[?1006l");
    fflush(stdout);
}

void get_term_size(int *w, int *h) {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *w = ws.ws_col; *h = ws.ws_row;
}

void draw_head(int x, int y, const char *symbol, int r,int g,int b) {
    char color[32]; rgb_to_ansi(r,g,b,color);
    int dx[5] = {-1,1,0,0,0};
    int dy[5] = {0,0,-1,1,0};
    for(int i=0;i<5;i++) {
        printf("\033[%d;%dH%s%s\033[0m", y+dy[i], x+dx[i], color, symbol);
    }
}

void draw_tail(int w,int h, int r1,int g1,int b1, int r2,int g2,int b2) {
    double now = now_sec();
    double max_age = 0.5;
    for(int i=0;i<trail_len;i++) {
        double age = now - trail[i].t;
        if(age>max_age) continue;

        double t_lin = age/max_age;
        double t_eased = 1 - (1-t_lin)*(1-t_lin);
        double t_color = 1 - (double)i/(trail_len-1);

        int rr,gg,bb; lerp_color(r1,g1,b1,r2,g2,b2,t_color,&rr,&gg,&bb);
        char color[32]; rgb_to_ansi(rr,gg,bb,color);

        char sym = (t_eased<0.5? '+':' ');
        printf("\033[%d;%dH%s%c\033[0m", trail[i].y, trail[i].x, color, sym);
    }
}

int main() {
    enable_raw();
    atexit(disable_raw);

    int head_x=10, head_y=10;
    int head_r=255, head_g=255, head_b=0;
    int tail_r=255, tail_g=0, tail_b=0;

    char buf[64];
    double last_mouse=now_sec();

    while(1) {
        int n = read(STDIN_FILENO, buf, sizeof(buf));
        if(n>0) {
            for(int i=0;i<n;i++) if(buf[i]=='q') return 0;
            char *seq = memchr(buf,'<',n);
            if(seq) {
                int b,x,y;
                if(sscanf(seq,"<%d;%d;%dM",&b,&x,&y)==3) {
                    head_x=x; head_y=y; last_mouse=now_sec();
                    if(trail_len<TRAIL_MAX) {
                        trail[trail_len++] = (TrailPoint){x,y,now_sec()};
                    } else {
                        memmove(&trail[0],&trail[1],sizeof(trail)-sizeof(TrailPoint));
                        trail[TRAIL_MAX-1] = (TrailPoint){x,y,now_sec()};
                    }
                }
            }
        }

        // clear
        printf("\033[2J\033[H");

        int tw,th; get_term_size(&tw,&th);
        draw_tail(tw,th, head_r,head_g,head_b, tail_r,tail_g,tail_b);
        draw_head(head_x,head_y,"*", head_r,head_g,head_b);

        fflush(stdout);
        usleep(16000);
    }
}

