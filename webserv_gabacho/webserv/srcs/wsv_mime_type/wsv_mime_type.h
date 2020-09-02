#ifndef WSV_MIME_TYPE_H
# define WSV_MIME_TYPE_H

/*
** NOTES:
** +) usually mime types are stored in a a "mime.types" file
*/

int		wsv_mime_types_initialize(void);
void	wsv_mime_type(const char* extension, const char** mime_type);

#endif
