/* error 6
   void type of array index */

int a(int b, int c) {
 	return b + c;
}
void bb(void) {

}

void main(void) {
 	int b;
	int c[1];
	b=bb;
	c[1] = b + a(b,1);
}
