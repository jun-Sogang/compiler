/* error13
   wrong */
void func(void) {
	int a;
}
int funb(int c) {
	c = 1;
	return 1;
}
int main(void) {
	func(); funb(1);
}
