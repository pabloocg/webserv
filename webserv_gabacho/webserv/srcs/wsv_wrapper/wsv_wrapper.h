#ifndef WSV_WRAPPER_H
# define WSV_WRAPPER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>

typedef void (*sig_t)(int);
typedef struct dirent dir_t;

/*
** Error Logging Function Wrappers
*/
void*		wsv_malloc(size_t size);

int			wsv_open(const char *pathname, int flags);
int			wsv_open_m(const char *pathname, int flags, mode_t mode);
int			wsv_close(int fd);

int			wsv_socket(int domain, int type, int protocol);
int			wsv_setsockopt(int sockfd, int level, int optname,
						   const void *optval, socklen_t optlen);
int			wsv_bind(int sockfd, const struct sockaddr *addr,
					 socklen_t addrlen);
int			wsv_listen(int sockfd, int backlog);
int			wsv_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

ssize_t		wsv_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t		wsv_recv(int sockfd, void *buf, size_t len, int flags);

int			wsv_select(int nfds, fd_set *readfds, fd_set *writefds,
					   fd_set *exceptfds, struct timeval *timeout);

ssize_t		wsv_read(int fd, void *buf, size_t count);
ssize_t		wsv_write(int fd, const void *buf, size_t count);

int			wsv_stat(const char *pathname, struct stat *statbuf);
int			wsv_fstat(int fd, struct stat *statbuf);

int			wsv_fcntl_int(int fildes, int cmd, int arg);

int			wsv_chdir(const char *path);

sig_t		wsv_signal(int signum, sig_t handler);

DIR*		wsv_opendir(const char *name);
int			wsv_closedir(DIR *dirp);
dir_t*		wsv_readdir(DIR *dirp);

int			wsv_pipe(int pipefd[2]);

int			wsv_unlink(const char *pathname);

pid_t		wsv_fork(void);

int			wsv_dup2(int oldfd, int newfd);

int			wsv_execve(const char *filename,
					   char *const argv[],
					   char *const envp[]);

off_t		wsv_lseek(int fd, off_t offset, int whence);


char*		wsv_getcwd(char *buf, size_t size);

pid_t		wsv_waitpid(pid_t pid, int *wstatus, int options);

#endif
