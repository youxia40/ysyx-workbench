#include <stdio.h>
int main(int argc, char *argv[])
{
    int ages[] = {23, 43, 12, 89, 2};
    char *names[] = {
        "Alan", "Frank",
        "Mary", "John", "Lisa"
    };

    int count = sizeof(ages) / sizeof(int);
    int i = 0;

    for(i = 0; i < count; i++) {
        printf("%s has %d years alive.\nThere addresses are as follows:\n%10p %10p",*(names+i), *(ages+i),names+i,ages+i);
    }

    printf("---\n");

    int *cur_age = ages;
    char **cur_name = names;

    for(i = 0; i < count; i++) {
        printf("%s is %d years old.\nThere addresses are as follows:\n%10p %10p\n",cur_name[i], cur_age[i],cur_name+i,cur_age+i);
    }

    printf("---\n");

    for(i = 0; i < count; i++) {
        printf("%s is %d years old again.\nThere addresses are as follows:\n%10p %10p\n",*(cur_name+i), *(cur_age+i),cur_name+i,cur_age+i);
	}

    printf("---\n");

    for(cur_name = names, cur_age = ages;
            (cur_age - ages) < count;
            cur_name++, cur_age++)
    {
        printf("%s lived %d years so far.\nThere addresses are as follows:\n%10p %10p\n",*cur_name, *cur_age,cur_name,cur_age);
    }
	while(1){
			int i=0;
			printf("Thr ages' address:%10p\n",ages+i);
			printf("The names' address:%10p",names+i);
			i++;
			if(i==count)
			{
					break;
			}
	}
    return 0;
}
