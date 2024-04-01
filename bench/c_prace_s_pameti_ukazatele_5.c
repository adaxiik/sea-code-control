// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 5. runnable code block

  #include <stdio.h>

  void vrat_dve_hodnoty(int* a, int* b) {
      *a = 5;
      *b = 6;
  }

  int main() {
      int a = 0;
      int b = 0;
      vrat_dve_hodnoty(&a, &b);

      printf("a=%d, b=%d\n", a, b);

      return 0;
  }
  