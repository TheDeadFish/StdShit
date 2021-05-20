#include "stdshit.h"
const char progName[] = "hello";

int main(int argc, char* argv[])
{
	contError(NULL, "contError %d, %s\n", 1234, "poop");
	fatalError("fatalError %d, %s\n", 5678, "piss");
}
