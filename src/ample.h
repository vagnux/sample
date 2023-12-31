#define SERVERMSG "HTTP/1.0 200 OK\nServer: Ample\nContent-type: audio/x-mpeg\n\n"
#define BUFFSIZE 1024
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
#define DIE(args) do { perror(args); exit(1); } while(0)
#define IF_DEBUG(tasks) do { if(opt_debug) { tasks } } while(0)
#define DEBUG(args) IF_DEBUG(printf("DEBUG: "); printf args;)
#define GETCWD(cwd) do {                                    \
                errno = 0;                                  \
                cwd = getcwd(NULL,0);                       \
                if(errno == EACCES) {                       \
	                DIE("---- No access to dir\n");     \
 	        } else if(errno == EINVAL && cwd == NULL) { \
			/* Auto-malloc not supported */     \
			errno = 0;                          \
			cwd = getcwd(NULL,PATH_MAX);        \
		}                                           \
		if(errno)                                   \
			/* That didn't help things */       \
			DIE("getcwd");                      \
         } while(0)

extern void playfiles(int conn);

extern int opt_debug;

struct config {
	int port;
	int order;
	char * path;
	char * user;
	char * pass;
	char * program_name;
};
