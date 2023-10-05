#include <stdio.h>
int main(void){
float a;
float B;
float C;
printf("%s\n","ENTER NUMBER");
if(0 == scanf("%f", &a)) {
a =0;
scanf("%*s");
}
B = 0;
C = 1;
while(a>0){
B = B+C;
printf("%.2f\n", (float)(B));
C = B+C;
printf("%.2f\n", (float)(C));
a = a-2;
}
return 0;
}
