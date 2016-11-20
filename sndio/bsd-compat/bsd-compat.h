#ifndef BSD_COMPAT_H
#define BSD_COMPAT_H

#ifndef HAVE_ISSETUGID
#define issetugid _sndio_issetugid
int issetugid(void);
#endif

#ifndef HAVE_STRLCAT
#define strlcat _sndio_strlcat
size_t strlcat(char *, const char *, size_t);
#endif

#ifndef HAVE_STRLCPY
#define strlcpy _sndio_strlcpy
size_t strlcpy(char *, const char *, size_t);
#endif

#ifndef HAVE_STRTONUM
#define strtonum _sndio_strtonum
long long strtonum(const char *, long long, long long, const char **);
#endif

#endif /* !defined(BSD_COMPAT_H) */
