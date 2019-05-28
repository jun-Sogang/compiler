/* error 6
   void type of array index */

int a(int b, int c) {
	return b + c;
}

void main(void) {
	int b;
	int c[1];
	b=1;
	c[1] = 1;
	if (1)
		a(b, c);
}
