#include "examples_common.h"

// 查询数据库,返回结果集
MYSQL_RES *query_mysql(char *str){
    MYSQL mysql,*conn;
    MYSQL_RES *res;
    mysql_init(&mysql);     //初始mysql结构
    // 连接数据库
    if ((conn=mysql_real_connect(&mysql,NULL,"root","123456","iplist",0,NULL,0)) == NULL)
    {
        fprintf(stderr, "连接数据库错误\n");
        return NULL;
    }
    // 查询数据库
    if (mysql_query(conn,str))
    {
        fprintf(stderr, "查询失败\n");
        return NULL;
    }
    // 生成结果集
    if (!(res=mysql_store_result(conn)))
    {
        fprintf(stderr, "结果集生成失败\n");
        return NULL;
    }
    mysql_close(conn);
    return res;
}
