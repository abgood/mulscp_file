#include "examples_common.h"

int open_location(ipinfo iplist,int flag){
    if (iplist->is_ssh && flag==WRITE)      // scp写
    {
        // ssh连接
        if(!(iplist->session=connect_ssh(iplist->ip,iplist->user,iplist->port,iplist->pawd))){
            fprintf(stderr, "ssh连接失败\n");
            return -1;
        }
        // 新建scp会话
        if (!(iplist->scp=ssh_scp_new(iplist->session,SSH_SCP_WRITE,iplist->path)))
        {
            fprintf(stderr, "scp会话新建失败\n");
            return -1;
        }
        // 初始化scp会话
        if (ssh_scp_init(iplist->scp)==SSH_ERROR)
        {
            fprintf(stderr, "scp会话初始化失败\n");
            ssh_scp_free(iplist->scp);
            return -1;
        }
        return 0;
    }else if (iplist->is_ssh && flag==READ)     // scp读
    {
        // ssh连接
        if(!(iplist->session=connect_ssh(iplist->ip,iplist->user,iplist->port,iplist->pawd))){
            fprintf(stderr, "ssh连接失败\n");
            return -1;
        }
        // 新建scp会话
        if (!(iplist->scp=ssh_scp_new(iplist->session,SSH_SCP_READ,iplist->path)))
        {
            fprintf(stderr, "scp会话新建失败\n");
            return -1;
        }
        // 初始化scp会话
        if (ssh_scp_init(iplist->scp)==SSH_ERROR)
        {
            fprintf(stderr, "scp会话初始化失败\n");
            ssh_scp_free(iplist->scp);
            return -1;
        }
        return 0;        
    }else{      // scp文件
        if (!(iplist->file=fopen(iplist->path,flag==READ ? "r":"w")))
        {
            if (errno==EISDIR)
            {
                if (chdir(iplist->path))
                {
                    fprintf(stderr, "改变目录失败\n");
                    return -1;
                }
                return 0;
            }
            fprintf(stderr, "fopen打开path文件失败\n");
            return -1;
        }
        return 0;
    }
    return -1;
}

// 拷贝
int do_copy(ipinfo src,ipinfo dest){
    socket_t fd;
    struct stat s;
    int size;
    int mode;
    char *filename;
    int w,r;
    int total=0;
    char buffer[16384];

    // 本地主机将要传输的文件是通过fopen打开还是通过scp打开
    if (!src->is_ssh)       // fopen
    {
        fd=fileno(src->file);
        fstat(fd,&s);
        size=s.st_size;
        mode=s.st_mode & ~S_IFMT;
        filename=ssh_basename(src->path);
    }else{      // scp
        size=0;
        do
        {
            r=ssh_scp_pull_request(src->scp);
            if (r==SSH_SCP_REQUEST_NEWDIR)
            {
                ssh_scp_deny_request(src->scp,"Not in recursive mode");
                continue;
            }
            if (r==SSH_SCP_REQUEST_NEWFILE)
            {
                size=ssh_scp_request_get_size(src->scp);
                filename=strdup(ssh_scp_request_get_filename(src->scp));
                mode=ssh_scp_request_get_permissions(src->scp);
                break;
            }
            if(r==SSH_ERROR){
                fprintf(stderr,"Error: %s\n",ssh_get_error(src->session));
                return -1;
            }
        } while (r!=SSH_SCP_REQUEST_NEWFILE);
    }

    // 远程主机文件fopen还是scp
    if (dest->is_ssh)      // scp
    {
        r=ssh_scp_push_file(dest->scp,src->path,size,mode);
        if (r==SSH_ERROR)
        {
            fprintf(stderr,"error: %s\n",ssh_get_error(dest->session));
            ssh_scp_free(dest->scp);
            return -1;
        }
    }else{
        if (!dest->file)
        {
            if(!(dest->file=fopen(filename,"w"))){
                fprintf(stderr,"Cannot open %s for writing: %s\n",filename,strerror(errno));
                if (src->is_ssh)
                    ssh_scp_deny_request(src->scp,"Cannot open local file");
                return -1;
            }
        }
        if(src->is_ssh)
            ssh_scp_accept_request(src->scp);
    }

    do {
        if(src->is_ssh){
            r=ssh_scp_read(src->scp,buffer,sizeof(buffer));
            if(r==SSH_ERROR){
                fprintf(stderr,"Error reading scp: %s\n",ssh_get_error(src->session));
                return -1; 
            }   
            if(r==0)
                break;
        } else {
            r=fread(buffer,1,sizeof(buffer),src->file);
            if(r==0)
                break;
            if(r<0){
                fprintf(stderr,"Error reading file: %s\n",strerror(errno));
                return -1; 
            }   
        }   
        if(dest->is_ssh){
            w=ssh_scp_write(dest->scp,buffer,r);
            if(w == SSH_ERROR){
                fprintf(stderr,"Error writing in scp: %s\n",ssh_get_error(dest->session));
                ssh_scp_free(dest->scp);
                dest->scp=NULL;
                return -1; 
            }   
        } else {
            w=fwrite(buffer,r,1,dest->file);
            if(w<=0){
                fprintf(stderr,"Error writing in local file: %s\n",strerror(errno));
                return -1; 
            }   
        }   
        total+=r;
    } while(total < size);
    printf("wrote %d bytes\n",total);
    return 0;
}

int libssh_scp(ipinfo iplist){
    extern ipinfo location;
    // 目标主机scp会话
    if (open_location(iplist,WRITE)<0)
    {
        fprintf(stderr, "目标主机open_location失败\n");
        return -1;        
    }
    // 本地主机scp会话
    if (open_location(location,READ)<0)
    {
        fprintf(stderr, "本地主机open_location失败\n");
        return -1;        
    }
    // 拷贝
    if (do_copy(location,iplist)<0)
    {
        fprintf(stderr, "文件拷贝失败\n");
        return -1;
    }
    return 0;
}
