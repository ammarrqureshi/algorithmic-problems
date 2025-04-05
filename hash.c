#include <stdio.h>

unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = (hash * 33) + c;
  }
  return hash;
}
int main() { printf("Hash of a is : %lu", hash("a")); }
