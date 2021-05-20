#include "stdshit.h"
const char progName[] = "hello";

int main(int argc, char* argv[])
{
	xstr str = xstrfmt("%d, %X, %f, %s\n", 10, 10, 10.3, "hello you fat shit");
	contError(NULL, "hello: %s, %s", argv[0], str.data);
}
