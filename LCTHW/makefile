CFLAGS = -Wall -g          
#这是一个可以一键编译所有C源文件的makefile,还可以一键删除所有的功能文件
CC = gcc
%: %.c
	$(CC) $(CFLAGS) -o $@ $<
clean:            
	rm -f $(patsubst %.c,%,$(wildcard *.c))

all:$(patsubst %.c,%,$(wildcard *.c))

chmod:
	sudo chmod 777 *.c
  
.PHONY:clean all chmod
