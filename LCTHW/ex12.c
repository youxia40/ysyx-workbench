#include <stdio.h>
int main(int argc, char *argv[])
{
    int i = 0;
    if(argc == 1) {
        printf("You don't have any argument. You suck.\n");
    } else if(argc > 1 && argc < 11){
        printf("Here's your arguments,they can be counted in your two hands:\n");
        for(i = 0; i < argc; i++) {
            printf("%s ", argv[i+1]);     //have done
        }
        printf("\n");
    } else {
        printf("You have too many arguments. You can't count them by your hands.\n");
    }
    return 0;
}
