#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = argc;
    while(1) {
        printf("arg %d: %s\n", i, argv[i]);
        i--;
		if(i<0)
		{
				break;
		}
    }
    char *states[] = {
        "California", "Oregon",
        "Washington", "Texas"
    };

    int num_states = 4;
    i = 0;
    while(i < num_states) {
        printf("state %d: %s\n", i, states[i]);
        i++;
    }
	i=0;
	while(i<argc)
	{
			states[i]=argv[i];
			printf("state_copy %d: %s\n",i,states[i]);
			i++;
	}
    return 0;
}
