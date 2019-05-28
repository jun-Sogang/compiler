void func(void) {
 int a;
}

int funb(int c) {
 c = 1;
 return c;
}

int main(void) {
 func(); funb(1);
}
