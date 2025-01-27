#include <stdio.h>
#include <ctype.h>
#include <string.h>
void print_letters(char arg[],int k);
int print_arguments(int argc, char *argv[])
{
    int i = 0;
    for(i = 0; i < argc; i++) {
		int m=strlen(argv[i]);
        print_letters(argv[i],m);
    }
	return strlen(argv[argc]);
}

void print_letters(char arg[],int k)
{
    int i = 0;
    for(i = 0; i<k; i++) {
        char ch = arg[i];

        if(isalpha(ch)||isblank(ch)) {
            printf("'%c' == %d ", ch, ch);
        }
    }

    printf("\n");
}
int main(int argc, char *argv[])
{
    print_arguments(argc, argv);
    return 0;
}
