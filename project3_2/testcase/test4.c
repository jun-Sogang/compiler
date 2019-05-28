/* error 2
   duplicated function, variable */

int a;
int a;
void fun(void) {
	;
}
int fun(int a) {
	a = 1;
}
void main(void) {
	a = 2;
}
