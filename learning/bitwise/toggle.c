#include <stdio.h>
#include <stdint.h>

uint8_t read(const uint8_t *byte, uint8_t n);
void toggle(uint8_t *byte, uint8_t n);
void print_byte(const uint8_t *byte);

int main(void){
  uint8_t toggle_bitwise = 0b00000000;;
  print_byte(&toggle_bitwise);
  toggle(&toggle_bitwise, 1);
  print_byte(&toggle_bitwise);
  toggle(&toggle_bitwise, 2);
  print_byte(&toggle_bitwise);
}
























void toggle(uint8_t *byte, uint8_t n){
  *byte = *byte ^ (1u << n);
}

uint8_t read(const uint8_t *byte, uint8_t n)
{
  return ((*byte >> n) & 1);
}

void print_byte(const uint8_t *byte){
  uint8_t bit;
  for(uint8_t i=8; i > 0 ; i--)
  {
    bit = read(byte, i);
    printf("%d", bit);
  }
  printf("\n");
}
