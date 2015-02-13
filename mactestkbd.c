#include <stdio.h>
extern const char *getMacKeyboard();
int main()
{
    fprintf(stderr, "%s\n", getMacKeyboard());
	return 0;
}
