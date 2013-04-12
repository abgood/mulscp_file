#include "examples_common.h"

ipinfo location;

int scpbag(MYSQL_ROW rows,char **argv){
    ipinfo iplist;

    // iplist动态分配内存
    if ((iplist=malloc(sizeof(struct iplist))) == NULL)
    {
        fprintf(stderr, "iplist结构动态分配内存错误\n");
        exit(1);
    }

    // ip信息分别赋值
    iplist->is_ssh=1;
    iplist->ip=rows[1];
    iplist->user=rows[2];
    iplist->pawd=rows[3];
    iplist->port=rows[4];
    iplist->path=argv[2];

    // 发包
    if (libssh_scp(iplist) < 0)
    {
        fprintf(stderr, "传包开始失败,请检查\n");
        return -1;
    }
    return 0;           
}

int main(int argc,char **argv){
    MYSQL_RES *res;
    MYSQL_ROW rows;

    // argc长度
    if (argc!=3)
    {
        fprintf(stderr, "请使用: ./scp 源路径 目标路径\n");
        return -1;
    }

    // 设置本地ip信息
    if ((location=malloc(sizeof(struct iplist))) == NULL)
    {
        fprintf(stderr, "location结构动态分配内存错误\n");
        return -1;
    }    
    location->is_ssh=1;
    location->ip="127.0.0.1";
    location->user="root";
    location->pawd="123456";
    location->port="22";
    location->path=argv[1];

    // 查询数据库
    char *str="select * from iplist";
    if ((res=query_mysql(str)) == NULL)
    {
        fprintf(stderr, "数据库结果集返回错误\n");
        return -1;
    }

    // 获取结果集内容
    while((rows=mysql_fetch_row(res))){
        if (scpbag(rows,argv) < 0)
        {
            fprintf(stderr, "传包前的分配信息失败,请检查\n%s 传包失败\n",rows[1]);
            return -1;
        }
    }
    return 0;
}
