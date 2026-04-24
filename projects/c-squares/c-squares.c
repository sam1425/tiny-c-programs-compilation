
//c script written by Christos Angelopoulos, Jan 2023
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include<signal.h>

// found in https://www.geeksforgeeks.org/c-program-not-suspend-ctrlz-pressed/
void sighandler(int sig_num) 
{ 
 printf("\033[?25h"); //show cursor
 struct winsize w;
 ioctl(0, TIOCGWINSZ, &w);
 for (int x=0;x>w.ws_row;x++)
 {
  printf("\n");
 }
//    system("clear");
 exit(0);
} 
int compareTwoString(char *, char *);

void check_flags(int argc, char **argv,char **flag_short,char **flag_long,char **value_s)
{
 int v;
  for (v = 0; v < argc; v++)
 {
  int compare1 = compareTwoString(argv[v], *flag_short);
  int compare2 = compareTwoString(argv[v], *flag_long);
  if ((compare1 == 0)||(compare2 == 0))
  {
   *value_s=argv[v+1];
  }
 }
}
int get_int_code(char *val, int code)
{
 int x,def;
 def=code;
 char vals[10][2]={"0","1","2","3","4","5","6","7","8","9"};
 for(x=0;x<10;x++)
 {
  int compare = compareTwoString(val,vals[x]);
    if (compare == 0)
  {
   def=x;
   break;
  }
 }
   return(def);
}//get_int_code

void print_help()
{
 printf("C-SQUARES\n\nThis script written in the C language will render random coloured\nrectangulars in the terminal, while the font, speed, density and\nnumber of shapes are fully costumizable.\nEvery time a rectangular is complete,\na new one starts to take shape.\nThis script was inspired by this bash script:\n\nhttps://github.com/pipeseroni/pipes.sh\n\nIt also is a variation of another project of mine:\n\nhttps://gitlab.com/christosangel/c-pipes\n\n\nUSAGE\n\nWhile in the c-squares/ directory,first you need to\ncompile the c script:\n\n\tgcc c-squares.c -Wall -o c-squares\n\nThen you can run the executable:\n\n\t./c-squares\n\nYou may want to add the flags that you prefer in order to have \nan outcome of your liking.\n\nFLAGS\n\nThe user can either use the short or the long flag version\n(i.e. c-squares -h and c-squares --help are the same).\n\n-h,--help\tShows this help text.\n\n-l,--lines\tDefines the number of lines rectangulars drawn\n\t\ton the terminal window at any moment.\n\t\tAcceptable values 1-9 (default: 3).\n\n-s,--speed\tDefines the speed of the drawing.\n\t\tAcceptable values 0-9 (default: 3).\n\n-d,--dense\tDefines the density of the rectangulars  drawn before the screen\n\t\tis empty again.\n\t\tAcceptable values 1-9 (default: 5).\n\t\t1 Screen refreshed after only a few rectangulars.\n\t\t9 Screen packed with shapes.\n\n-f,--font\tDefines the font of the lines.\n\t\tAcceptable values 0-9 (default: 1).\n\n\t\t0 Chars used:─│┘┐└┌\t1 Chars used:─│╯╮╰╭\n\n\t\t2 Chars used:━┃┛┓┗┏\t3 Chars used:═║╝╗╚╔\n\n\t\t4 Chars used:┈┊┐┘└┌\t5 Chars used:┄┆┘┐└┌\n\n\t\t6 Chars used:┅┇┛┓┗┏\t7 Chars used:┉┋┛┓┗┏\n\n\t\t8 Chars used:↑↓↗↖↘↙←→\t9 Chars used:█\n\n-t,--type\tDefines the type of rectangulars rendered on the screen.\n\t\tAcceptable values 0-3 (default: 0).\n\t\t0: Perfect squares (almost).\n\t\t1: Width bigger than height.\n\t\t2: Height bigger than width.\n\t\t3: Random width/height ratio.\n\n-m,--monochrome\tRenders single colored lines.\n\t\tIf this flag is ommitted, each line gets a random color.\n\t\tAcceptable values 0-7.\n\t\t0 black\t1 red\n\t\t2 green\t3 yellow\n\t\t4 blue\t5 magenta\n\t\t6 cyan\t7 white\n\n");
 exit(0);
 }//print_help

void refresh_screen(char *(*MATRIX)[67][270],int (*COLOR)[67][270])
{
 int x,y;
 for(x=0;x<67;x++)
 {
  for(y=0;y<270;y++)
  {
   *(*(*MATRIX+x)+y)=" ";
   *(*(*COLOR+x)+y)=32;
  }
 }
}
void new_coordinates(int (*cursor_x)[10],int (*cursor_y)[10],int (*direction)[10],int (*s)[10],int *row,int *col,int (*color)[10],int *i,int *mono,int (*sq_height)[10],int (*sq_width)[10],int *type)
{
 struct winsize w;
 ioctl(0, TIOCGWINSZ, &w);
 int a=w.ws_row,b=w.ws_col;
 if (*type==0)
 {
  (*(*sq_height+*i))=rand() % ((a/4) -2)+2;
  (*(*sq_width+*i))=(*(*sq_height+*i)) * 5 / 2.2 ;
 }
 else if (*type==1)
 {
  (*(*sq_height+*i))=rand() % ((a/4) -2)+2;
  (*(*sq_width+*i))=(*(*sq_height+*i)) * 4;
 }
 else if (*type==2)
 {
  (*(*sq_height+*i))=rand() % ((a/4) -2)+2;
  (*(*sq_width+*i))=(*(*sq_height+*i)) ;
 }
 else if (*type==3)
 {
  (*(*sq_width+*i))=rand() % ((b/6) - 2 )+2;
  (*(*sq_height+*i))=rand() % ((a/4) -2)+2;
 }
 (*(*direction+*i))=0-(*i * 3 );
 (*(*s+*i))=0;
 *(*cursor_x+*i)=rand() % ((a-1) - ((*(*sq_height+*i)) +1) ) + ((*(*sq_height+*i))+1) ;
 *(*cursor_y+*i)=rand() % (b -2 - *(*sq_width+*i));
 if(*mono==11){*(*color+*i)=rand() % 7 + 31;}else{*(*color+*i)=30 + *mono;}
 *row=w.ws_row;*col=w.ws_col;
}

int main(int argc, char **argv) {
 // Handling signals
 signal(SIGTSTP, sighandler);
 signal(SIGSTOP, sighandler);
 signal(SIGKILL, sighandler);
 signal(SIGINT, sighandler);
 printf("\033[?25l"); //hide cursor
 srand(time(0));

//declare variables
 char *flag_h="-h";
 char *flag_help="--help";

 char *flag_s="-s";
 char *flag_speed="--speed";
 char *speed_s="";
 int speed=3;

 char *flag_t="-t";
 char *flag_type="--type";
 char *type_s="";
 int type=0;

 char *flag_l="-l";
 char *flag_lines="--lines";
 char *lines_s="";
 int lines=3;

 char *flag_d="-d";
 char *flag_dense="--dense";
 char *dense_s="";
 int dense=5;

 char *flag_f="-f";
 char *flag_font="--font";
 char *font_s="";
 int font=1;


 char *flag_m="-m";
 char *flag_mono="--monochrome";
 char *mono_s="";
 int mono=11;
 int color[10];

 int a,b,d,i,ii,v;
 int s[10];
 int sq_height[10];
 int sq_width[10];
 int row=67,col=270; //max values
 int direction[10],new_direction[10],new_cursor_x[10],new_cursor_y[10];
 char *element[10];
 char *north[10][2]={{"│","└"},{"│","╰"},{"┃","┗"},{"║","╚"},{"┊","└"},{"┆","└"},{"┇","┗"},{"┋","┗"},{"↑","↖"},{"█","█"}};
 char *east[10][4]={{"─","┌"},{"─","╭"},{"━","┏"},{"═","╔"},{"┈","┌"},{"┄","┌"},{"┅","┏"},{"┉","┏"},{"→","↗"},{"█","█","█","█"}};
 char *south[10][2]={{"│","┐"},{"│","╮"},{"┃","┓"},{"║","╗"},{"┊","┐"},{"┆","┐"},{"┇","┓"},{"┋","┓"},{"↓","↘"},{"█","█"}};
 char *west[10][2]={{"─","┘"},{"─","╯"},{"━","┛"},{"═","╝"},{"┈","┘"},{"┄","┘"},{"┅","┛"},{"┉","┛"},{"←","↙"},{"█","█"}};
 int loop=1;
 int cursor_x[10];
 int cursor_y[10];
 char *MATRIX[row][col];
 int MATRIX_COLOR[67][270];
 refresh_screen(&MATRIX,&MATRIX_COLOR);
 for (v = 0; v < argc; v++)
 {
  int compare1 = compareTwoString(argv[v], flag_h);
  int compare2 = compareTwoString(argv[v], flag_help);
  if ((compare1 == 0)||(compare2 == 0)){print_help();}
 }
 check_flags(argc,argv,&flag_s,&flag_speed,&speed_s);
 check_flags(argc,argv,&flag_t,&flag_type,&type_s);
 check_flags(argc,argv,&flag_l,&flag_lines,&lines_s);
 check_flags(argc,argv,&flag_d,&flag_dense,&dense_s);
 check_flags(argc,argv,&flag_f,&flag_font,&font_s);
 check_flags(argc,argv,&flag_m,&flag_mono,&mono_s);
 speed=get_int_code(speed_s,speed);
 type=get_int_code(type_s,type);
 lines=get_int_code(lines_s,lines);
 dense=get_int_code(dense_s,dense);
 font=get_int_code(font_s,font);
 mono=get_int_code(mono_s,mono);
 if((mono>7)||(mono<0)){mono=11;}
 for(ii=0;ii<lines;ii++)
 {
  new_coordinates(&cursor_x,&cursor_y,&direction,&s,&row,&col,&color,&ii,&mono,&sq_height,&sq_width,&type);
 }

 // LOOP***************************************************************
 while(loop==1)
 {
  while (d<1+50*dense)
  {
  for(i=0;i<lines;i++)
  {
   if (direction[i]>-1)
   {
      //DRAW LINE***********************************************************

       new_direction[i]=direction[i];
       s[i]++;

      switch (new_direction[i])
      {
       case 0: if (s[i]==1){element[i]=north[font][1];}else{element[i]=north[font][0];};
               new_cursor_x[i]=cursor_x[i]-1;new_cursor_y[i]=cursor_y[i];break;
       case 1: if (s[i]==1){element[i]=east[font][1];}else{element[i]=east[font][0];};
               new_cursor_x[i]=cursor_x[i];new_cursor_y[i]=cursor_y[i]+1;break;
       case 2: if (s[i]==1){element[i]=south[font][1];}else{element[i]=south[font][0];};
               new_cursor_x[i]=cursor_x[i]+1;new_cursor_y[i]=cursor_y[i];break;
       case 3: if (s[i]==1){element[i]=west[font][1];}else{element[i]=west[font][0];};
               new_cursor_x[i]=cursor_x[i];new_cursor_y[i]=cursor_y[i]-1;break;
      }
      //usleep(10000);
      MATRIX[cursor_x[i]][cursor_y[i]]=element[i];
      if(mono==11){MATRIX_COLOR[cursor_x[i]][cursor_y[i]]=color[i];}
      else{MATRIX_COLOR[cursor_x[i]][cursor_y[i]]=30 + mono;}
      cursor_x[i]=new_cursor_x[i];
      cursor_y[i]=new_cursor_y[i];
      direction[i]=new_direction[i];
      struct winsize w;
      ioctl(0, TIOCGWINSZ, &w);
      if ((row != w.ws_row )||( col != w.ws_col))
      {
       refresh_screen(&MATRIX,&MATRIX_COLOR);
       for(ii=0;ii<lines;ii++)
       {
       new_coordinates(&cursor_x,&cursor_y,&direction,&s,&row,&col,&color,&ii,&mono,&sq_height,&sq_width,&type);
       }
      }

      if (((direction[i]==0)||(direction[i]==2))&&(s[i]>sq_height[i]))
      {
       d++;
       direction[i]++;
       s[i]=0;
      }
      if (((direction[i]==1)||(direction[i]==3))&&(s[i]>sq_width[i]))
      {
       d++;
       direction[i]++;
       s[i]=0;
      }
      if (direction[i]>3)
       {
        new_coordinates(&cursor_x,&cursor_y,&direction,&s,&row,&col,&color,&i,&mono,&sq_height,&sq_width,&type);
       }
    }else {direction[i]++;}
     //usleep(10000);
   }//line loop
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
    if ((row != w.ws_row )||( col != w.ws_col))
    {
     refresh_screen(&MATRIX,&MATRIX_COLOR);
     for(ii=0;ii<lines;ii++)
     {
     new_coordinates(&cursor_x,&cursor_y,&direction,&s,&row,&col,&color,&ii,&mono,&sq_height,&sq_width,&type);
     }
    }
//  system("clear");
  fputs("\x1b[H", stdout);
  for(a=0;a+1<w.ws_row;a++)
  {
   for(b=0;b<w.ws_col;b++)
   {
    printf("\x1b[%dm%s\x1b[0m",MATRIX_COLOR[a][b],MATRIX[a][b]);
   }
  }
  printf("\n");
  usleep(30000 + speed*10000);
 }//dense
 refresh_screen(&MATRIX,&MATRIX_COLOR);
  for(ii=0;ii<lines;ii++)
  {
   new_coordinates(&cursor_x,&cursor_y,&direction,&s,&row,&col,&color,&ii,&mono,&sq_height,&sq_width,&type);
  }
 d=0;
 s[i]=0;

 }//loop
}

// Comparing both the strings using pointers
//code as found in https://www.scaler.com/topics/c/string-comparison-in-c/
int compareTwoString(char *a, char *b)
{
    int flag = 0;
    while (*a != '\0' && *b != '\0') // while loop
    {
        if (*a != *b)
        {
            flag = 1;
        }
        a++;
        b++;
    }
 if(*a!='\0'||*b!='\0')
       return 1;
    if (flag == 0)
        return 0;
    else
        return 1;
}
