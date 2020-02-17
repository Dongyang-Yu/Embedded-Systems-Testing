#include <stdlib.h>
#include <stdio.h>
 
#include "cfunction.h"

#define SIZE 64

 int main() {
  
  char str[SIZE];
  for( int i = 0; i < SIZE; i++ ){
    str[i] = getchar();
    printf("str[%d] is %c", i, str[i]);
    
    }

     //fgets(str, sizeof(str), stdin);
     //printf("%s", str);

     
//     scanf("%s", str);
     validate_hostname( str, strlen(str) );

 
     return 0;                                                                  
  }
