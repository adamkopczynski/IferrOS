#include <stdarg.h>
#include "stdio.h"
#include "../terminal.h"

int print_i(int32_t arg);
int print_f(double arg);
int print_string(char* str);
int get_digit_at(const int32_t, int);


int printf(const char* format, ...){

  if(!format)
    return 0;

  int i = 0,
      count = 0;

  va_list valist;
  va_start(valist, format);

  while(*(format+i) != '\0'){
    if(*(format+i) == '%'){

      switch(*(format+i+1)){
        
        //string with \x0 as terminator
        case 's':
          count += print_string(va_arg(valist, char*));
        break;

        //int32_t
        case 'd':
          count += print_i(va_arg(valist, int32_t));
        break;

        //double
        case 'f':
          count += print_f(va_arg(valist, double));
        break;

      }

      
      i += 2;
      continue;
    }
    else{
      putchar(*(format+i));
      count++;
    }

    i++;
  }
  
  va_end(valist);
  return count;
}

int print_i(int32_t arg){

  int32_t value = arg,
          count = 0;

  if(value == 0){
    putchar('0');
    count++;
  }

  if(value < 0){
    value *= -1;
    putchar('-');
    count++;
  }

  for(int i = 0; i < (int)log10(value)+1; i++){
    putchar('0' + get_digit_at(value, i+1));
    count++;
  }

  return count;
}

int print_f(double arg){
  
  int i_part, count = 0;

  if(arg < 0){
    i_part = ceil(arg);;
  }
  else{
    i_part = floor(arg);
  }

  double f_part = arg >= 0 ? arg - i_part : -1*(arg - i_part);

  count += print_i(i_part);

  putchar('.');
  count++;

  for(int i = 0; i < 5; i++){
    f_part *= 10;
    putchar('0' + (int)f_part%10);
    count++;
  }

  return count;
}

int print_string(char* str){

  int count = 0;

  if(str){
    int i = 0;

    while(*(str+i) != '\x0'){
      putchar(*(str+i));
      count++;
      i++;
    }
  }

  return count;
}

int get_digit_at(const int32_t nr, int pos){

  int len = log10(nr)+1;
  int32_t copy = nr;

  for(int i = 0; i < len - pos; i++){
    copy /= 10;
  }

  return copy%10;
}

void putchar(char c){
    terminal_putchar(c);
}