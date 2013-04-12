#include "examples_common.h"

int verify_knownhost(ssh_session session){
    int stat;
    unsigned char *hash=NULL;
    int hlen;
    char *hexa;
    char *buf="yes";
    int bufcmp;
    stat=ssh_is_server_known(session);      //检查会话knownhost
    if((hlen=ssh_get_pubkey_hash(session,&hash)) < 0)
        return -1;
    switch(stat){
        case SSH_SERVER_KNOWN_OK: break;        //The server is known and has not changed.
        case SSH_SERVER_KNOWN_CHANGED:          //The server key has changed.
            fprintf(stderr, "Host key for server changed : server's one is now :\n");
            ssh_print_hexa("Public key hash",hash, hlen);
            ssh_clean_pubkey_hash(&hash);
            fprintf(stderr,"For security reason, connection will be stopped\n");
            return -1;
        case SSH_SERVER_FOUND_OTHER:            //we had an other type recorded.
            fprintf(stderr,"The host key for this server was not found but an other type of key exists.\n");
            fprintf(stderr,"An attacker might change the default server key to confuse your client"
                           "into thinking the key does not exist\n"
                           "We advise you to rerun the client with -d or -r for more safety.\n");
            return -1;
        case SSH_SERVER_FILE_NOT_FOUND:         //The known host file does not exist.
            fprintf(stderr,"Could not find known host file. If you accept the host key here,\n");
            fprintf(stderr,"the file will be automatically created.\n");
        case SSH_SERVER_NOT_KNOWN:              //The server is unknown.        
            hexa=ssh_get_hexa(hash,hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key ?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            ssh_string_free_char(hexa);
            if ((bufcmp=strncasecmp(buf,"yes",3))!=0)
            {
                ssh_clean_pubkey_hash(&hash);
                return -1;
            }else if (bufcmp==0)
            {
                fprintf(stderr, "This new key will be written on disk for further usage. do you agree ?\n");
                if (ssh_write_knownhost(session)<0)
                {
                    ssh_clean_pubkey_hash(&hash);
                    fprintf(stderr, "error %s\n", strerror(errno));
                    return -1;
                }
            }
            break;
        case SSH_SERVER_ERROR:
            ssh_clean_pubkey_hash(&hash);
            fprintf(stderr, "%s\n", ssh_get_error(session));
            return -1;
    }
    ssh_clean_pubkey_hash(&hash);
    return 0;
}
