/*
#include <stdio.h>
int main(int argc, char** argv)
{
	char c;
	printf("char %s[] = \"", argv[1]);
	while(!feof(stdin)){
		c = getchar();
		if(c=='\n')	printf(" \\");
		putchar(c);
	}
	printf("\";\n");
	return 0;
}
*/
char bullet_model[] = "newgroup \
color 255 255 0  \
vertex -1 -0.5 0.5 \
vertex -1 0.5 0.5 \
vertex 1 -0.5 0.5 \
vertex 1 0.5 0.5 \
vertex 1 -0.5 -0.5 \
vertex 1 0.5 -0.5 \
vertex -1 -0.5 -0.5 \
vertex -1 0.5 -0.5 \
face 1 2 4 3 \
face 3 4 6 5 \
face 5 6 8 7 \
face 7 8 2 1 \
face 2 8 6 4 \
face 7 1 3 5";
