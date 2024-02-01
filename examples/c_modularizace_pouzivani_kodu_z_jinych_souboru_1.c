// https://mrlvsb.github.io/upr-skripta/c/modularizace/pouzivani_kodu_z_jinych_souboru.html, 1. runnable code block

> #include <stdio.h>
> 
> void test() {
>   static int x = 0;
>   x += 1;
>   printf("%d\n", x);
> }
> 
> int main() {
>   test();
>   test();
>   return 0;
> }
> 