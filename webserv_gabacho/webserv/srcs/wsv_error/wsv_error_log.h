#ifndef WSV_ERROR_LOG_H
# define WSV_ERROR_LOG_H

void		wsv_error_log_open(const char* file);
void		wsv_error_log_close(void);
void		wsv_error_log(const char* m);

#endif
