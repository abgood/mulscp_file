/*
Copyright 2009 Aris Adamantiadis

This file is part of the SSH Library

You are free to copy this file, modify it in any way, consider it being public
domain. This does not apply to the rest of the library though, but it is
allowed to cut-and-paste working code from this file to any license of
program.
The goal is to show the API in action. It's not a reference on how terminal
clients must be made or how a client should react.
*/
#ifndef EXAMPLES_COMMON_H_
#define EXAMPLES_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libssh/libssh.h>
#include <mysql/mysql.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

typedef struct iplist
{
    int is_ssh;
    char *ip;
    char *user;
    char *pawd;
    char *port;
    char *path;
    ssh_session session;
    ssh_scp scp;
    FILE *file;
}*ipinfo;

// 枚举
enum {READ,WRITE};

// 数据库查询
MYSQL_RES *query_mysql(char *str);

// 发包
int libssh_scp(ipinfo iplist);

// ssh连接创建
ssh_session connect_ssh(char *ip,char *user,char *port,char *pawd);

// 验证
int authenticate_console(ssh_session session,char *password);

// knownhosts
int verify_knownhost(ssh_session session);

#endif /* EXAMPLES_COMMON_H_ */
