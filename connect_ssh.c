#include "examples_common.h"

// ssh连接
ssh_session connect_ssh(char *ip,char *user,char *port,char *pawd){
    ssh_session session;
    // 创建一个ssh会话
    if((session=ssh_new()) == NULL){
        fprintf(stderr, "创建ssh会话失败\n");  
        exit(1); 
    }
    // 设置ssh会话相关信息
    if (ip!=NULL && user!=NULL && port!=NULL)
    {
        // 设置ip
        if (ssh_options_set(session,SSH_OPTIONS_HOST,ip) < 0)
        {
            ssh_free(session);
            fprintf(stderr, "ssh会话设置ip失败\n");  
            return NULL;
        }
        // 设置用户        
        if (ssh_options_set(session,SSH_OPTIONS_USER,user) < 0)
        {
            ssh_free(session);
            fprintf(stderr, "ssh会话设置user失败\n");  
            return NULL;
        }     
        // 设置端口   
        if (ssh_options_set(session,SSH_OPTIONS_PORT_STR,port) < 0)
        {
            ssh_free(session);
            fprintf(stderr, "ssh会话设置port失败\n");  
            return NULL;
        }    
    }
    // ssh连接远程主机
    if (ssh_connect(session))
    {
        fprintf(stderr, "连接失败 : %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return NULL;
    }
    // 新增knownhost
    if (verify_knownhost(session)<0)
    {
        ssh_disconnect(session);
        ssh_free(session);
        fprintf(stderr, "knownhost失败\n");
        return NULL;
    }
    // 验证
    int auth=0;
    if ((auth=authenticate_console(session,pawd))==SSH_AUTH_SUCCESS)
        return session;
    else if(auth==SSH_AUTH_DENIED)
        fprintf(stderr,"Authentication failed\n");
    else
        fprintf(stderr,"Error while authenticating : %s\n",ssh_get_error(session));
    ssh_disconnect(session);
    ssh_free(session);
    return NULL;
}
