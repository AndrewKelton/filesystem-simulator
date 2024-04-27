#include<stdio.h>
#include<unistd.h> 
#include<stdlib.h>

int main() {
    char *buf;
    buf=(char *)malloc(100*sizeof(char));
    getcwd(buf,100);
    printf("\n %s \n",buf);
}