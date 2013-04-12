CC = gcc
CFLAGS = -Wall -O -g
TARGET = scp
LIBMYSQL=`mysql_config --libs`
LIBSSH=-lssh

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.cpp
	$(XX) $(CFLAGS) -c $< -o $@

# 把所有以.c结尾的文件放到列表里
SOURCES = $(wildcard *.c)
# 匹配sources列表里所有.c文件替换成.o文件放到列表里
OBJS = $(patsubst %.c,%.o,$(SOURCES))

$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBMYSQL) $(LIBSSH)
	chmod u+x $(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
