#include <errno.h>

#include "wsv_wrapper.h"
#include "wsv_error_log.h"

void*		wsv_malloc(size_t size)
{
	void*	retval;

	retval = malloc(size);
	if (retval == 0)
		wsv_error_log("wsv_malloc(): ");
	return (retval);
}

int			wsv_open(const char *pathname, int flags)
{
	int		retval;

	retval = open(pathname, flags);
	if (retval < 0)
		wsv_error_log("wsv_open(): ");
	return (retval);
}

int			wsv_open_m(const char *pathname, int flags, mode_t mode)
{
	int		retval;

	retval = open(pathname, flags, mode);
	if (retval < 0)
		wsv_error_log("wsv_open_m(): ");
	return (retval);
}

int			wsv_close(int fd)
{
	int		retval;

	retval = close(fd);
	if (retval < 0)
		wsv_error_log("wsv_close(): ");
	return (retval);
}

int			wsv_socket(int domain, int type, int protocol)
{
	int		retval;

	retval = socket(domain, type, protocol);
	if (retval < 0)
		wsv_error_log("wsv_socket(): ");
	return (retval);
}

int			wsv_setsockopt(int sockfd,
						   int level,
						   int optname,
						   const void *optval,
						   socklen_t optlen)
{
	int		retval;

	retval = setsockopt(sockfd, level, optname, optval, optlen);
	if (retval < 0)
		wsv_error_log("wsv_setsockopt(): ");
	return (retval);
}

int			wsv_bind(int sockfd,
					 const struct sockaddr *addr,
					 socklen_t addrlen)
{
	int		retval;

	retval = bind(sockfd, addr, addrlen);
	if (retval < 0)
		wsv_error_log("wsv_bind(): ");
	return (retval);
}

int			wsv_listen(int sockfd, int backlog)
{
	int		retval;

	retval = listen(sockfd, backlog);
	if (retval < 0)
		wsv_error_log("wsv_listen(): ");
	return (retval);
}

int			wsv_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int		retval;

	retval = accept(sockfd, addr, addrlen);
	if (retval < 0)
		wsv_error_log("wsv_accept(): ");
	return (retval);
}

ssize_t		wsv_send(int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t	retval;

	retval = send(sockfd, buf, len, flags);
	if (retval < 0)
		wsv_error_log("wsv_send(): ");
	return (retval);
}

ssize_t		wsv_recv(int sockfd, void *buf, size_t len, int flags)
{
	ssize_t	retval;

	retval = recv(sockfd, buf, len, flags);
	if (retval < 0)
		wsv_error_log("wsv_recv(): ");
	return (retval);
}

int			wsv_select(int nfds, fd_set *readfds, fd_set *writefds,
					   fd_set *exceptfds, struct timeval *timeout)
{
	int		retval;

	retval = select(nfds, readfds, writefds, exceptfds, timeout);
	if (retval < 0)
		wsv_error_log("wsv_select(): ");
	return (retval);
}

ssize_t		wsv_read(int fd, void *buf, size_t count)
{
	ssize_t	retval;

	retval = read(fd, buf, count);
	if (retval < 0)
		wsv_error_log("wsv_read(): ");
	return (retval);
}

ssize_t		wsv_write(int fd, const void *buf, size_t count)
{
	ssize_t	retval;

	retval = write(fd, buf, count);
	if (retval < 0)
		wsv_error_log("wsv_write(): ");
	return (retval);
}

int			wsv_stat(const char *pathname, struct stat *statbuf)
{
	int		retval;

	retval = stat(pathname, statbuf);
	if (retval < 0)
		wsv_error_log("wsv_stat(): ");
	return (retval);
}


int			wsv_fstat(int fd, struct stat *statbuf)
{
	int		retval;

	retval = fstat(fd, statbuf);
	if (retval < 0)
		wsv_error_log("wsv_fstat(): ");
	return (retval);
}

int			wsv_fcntl_int(int fildes, int cmd, int arg)
{
	int		retval;

	retval = fcntl(fildes, cmd, arg);
	if (retval < 0)
		wsv_error_log("wsv_fstat(): ");
	return (retval);
}

int			wsv_chdir(const char *path)
{
	int		retval;

	retval = chdir(path);
	if (retval < 0)
		wsv_error_log("wsv_chdir(): ");
	return (retval);
}

sig_t		wsv_signal(int signum, sig_t handler)
{
	sig_t	retval;

	retval = signal(signum, handler);
	if (retval == SIG_ERR)
		wsv_error_log("wsv_signal(): ");
	return (retval);
}

DIR*		wsv_opendir(const char *name)
{
	DIR*	retval;

	retval = opendir(name);
	if (retval == 0)
		wsv_error_log("wsv_opendir(): ");
	return (retval);
}

int			wsv_closedir(DIR *dirp)
{
	int		retval;

	retval = closedir(dirp);
	if (retval < 0)
		wsv_error_log("wsv_closedir(): ");
	return (retval);
}

dir_t*		wsv_readdir(DIR *dirp)
{
	dir_t*	retval;

	retval = readdir(dirp);
	if (retval == 0 && errno != 0)
		wsv_error_log("wsv_readdir(): ");
	return (retval);
}

int			wsv_pipe(int pipefd[2])
{
	int		retval;

	retval = pipe(pipefd);
	if (retval < 0)
		wsv_error_log("wsv_pipe(): ");
	return (retval);
}

int			wsv_unlink(const char *pathname)
{
	int		retval;

	retval = unlink(pathname);
	if (retval < 0)
		wsv_error_log("wsv_unlink(): ");
	return (retval);
}

pid_t		wsv_fork(void)
{
	pid_t	retval;

	retval = fork();
	if (retval < -1)
		wsv_error_log("wsv_fork(): ");
	return (retval);
}

int			wsv_dup2(int oldfd, int newfd)
{
	int		retval;

	retval = dup2(oldfd, newfd);
	if (retval < 0)
		wsv_error_log("wsv_dup2(): ");
	return (retval);
}

int			wsv_execve(const char *filename,
					   char *const argv[],
					   char *const envp[])
{
	int		retval;

	retval = execve(filename, argv, envp);
	if (retval < 0)
		wsv_error_log("wsv_execve(): ");
	return (retval);
}

off_t		wsv_lseek(int fd, off_t offset, int whence)
{
	off_t	retval;

	retval = lseek(fd, offset, whence);
	if (retval == ((off_t)-1))
		wsv_error_log("wsv_lseek(): ");
	return (retval);
}

char*		wsv_getcwd(char *buf, size_t size)
{
	char*	retval;

	retval = getcwd(buf, size);
	if (retval == 0)
		wsv_error_log("wsv_getcwd(): ");
	return (retval);
}

pid_t		wsv_waitpid(pid_t pid, int *wstatus, int options)
{
	pid_t	retval;

	retval = waitpid(pid, wstatus, options);
	if (retval < 0)
		wsv_error_log("wsv_waitpid(): ");
	return (retval);
}
