#ifndef UTILS_H
# define UTILS_H

# include <stdint.h>
# include "klog.h"
# include "mem.h"

/* Time delay which a human can feel... */
# define HUMAN_TIME 1000000

/* Shut up compiler! */
# define DO_NOTHING_WITH(X) (void)(X)

/* Masks */
# define MASK32     0x00000000
# define DATAMASK32 0xFFFFFFF0

/* Status */
# define ERROR32    0xFFFFFFFF

/* Some usefull functions... */
uint32_t strlen(const char *str);
char *itoa(int val, int base);
char *strcat(const char *str1, const char *str2);
int strcmp(const char *str1, const char *str2);

#endif /* !UTILS_H */
