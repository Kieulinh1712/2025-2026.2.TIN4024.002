/*
THÔNG TIN NHÓM 1
1. Đào Hữu Khánh 
2. Trần Thị Như Sang
3. Nguyễn Duy Phong
4. Lê Trọng Kha 
5. Lê Quang Duy 
*/

#include  <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}