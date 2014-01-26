#ifndef COMMON_H

#define COMMON_H

#define _XOPEN_SOURCE 500
#define _BSD_SOURCE
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>

#define CERRS(X,Y) { if(X) { perror(Y); } }

#define CERR(X,Y) { CERRS((X) == -1,Y) }

#define timediff(t1,t2) ((t2)->tv_sec - (t1)->tv_sec)*1000LL+((t2)->tv_usec - (t1)->tv_usec)


#define DIFF(X) { gettimeofday(&t1,NULL); X; gettimeofday(&t2,NULL); }

#endif /* end of include guard: COMMON_H */
