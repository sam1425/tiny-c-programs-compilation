#include <stdio.h>

int meow(int a);

int main(int argc){
  int Meowerror = meow(argc);
  if (Meowerror != 1)
  {
    fprintf(stderr,"Meow is different than 0 closing.... error number %i\n", Meowerror);
    return Meowerror;
  }
  else 
  {
      printf("there were no errors in this program, closing\n");
  }
  return 0;

}

int meow(int a)
{
  return a;
}
