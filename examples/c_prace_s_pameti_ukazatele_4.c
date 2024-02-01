// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 4. runnable code block

  #include <stdio.h>

  void vymen(int* a, int* b) {
      int docasna_hodnota = *a;  // načti hodnotu na adrese v `a`
      *a = *b;  // načti hodnotu na adrese v `b` a ulož ji na adresu v `a`
      *b = docasna_hodnota;  // ulož uloženou hodnotu na adresu v `b`
  }
  int main() {
      int x = 5;
      int y = 10;
      vymen(&x, &y);
      printf("Po prehozeni: x=%d, y=%d\n", x, y);
      return 0;
  }
  