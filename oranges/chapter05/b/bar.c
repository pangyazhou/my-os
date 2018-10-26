// bar.c
//编译连接
// gcc -c -o bar.o bar.c
//

void myprint(char* msg, int len);

int choose(int a, int b)
{
	if(a >= b)
	{
		myprint("the 1st one\n", 13);
	}
	else
	{
		myprint("the 2st one\n", 13);
	}
	return 0;
}