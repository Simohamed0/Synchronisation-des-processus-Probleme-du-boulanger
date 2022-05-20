#include "lib.h"

noreturn void raler(int syserr, const char * fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
  
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1) {
        perror("\t");
    }

    exit(EXIT_FAILURE);
}

void snprintf_s(char *restrict str, size_t size, const char *restrict fmt,
                ...) {
    int n;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(str, size, fmt, ap);
    va_end(ap);

    if (n < 0) {
        raler(1, "vsnprintf failure");
    }
    if ((size_t)n >= size) {
        raler(1, "format string too long");
    }
}


void set_prd(char *restrict prd, const char *restrict fmt, ...) {
    int n;
    va_list ap;

    va_start(ap, fmt);
    if ((n = vsnprintf(prd, PRD_MAX_LEN + 1, fmt, ap)) > PRD_MAX_LEN) {
        raler(0, "product name too long");
    }
    if (n < 0) {
        raler(0, "vsnprintf failure");
    }
    va_end(ap);

    prd[PRD_MAX_LEN] = '\0';
}

void set_sem(int id, char *restrict sem_name, char *restrict fmt, ...) {
    int n;
    char p[PRD_MAX_LEN + 1];
    va_list ap;

    va_start(ap, fmt);
    if ((n = vsnprintf(p, PRD_MAX_LEN + 1, fmt, ap)) > PRD_MAX_LEN) {
        raler(0, "product name too long");
    }
    if (n < 0) {
        raler(0, "vsnprintf failure");
    }
    va_end(ap);

    p[PRD_MAX_LEN] = '\0';

    snprintf_s(sem_name, SEM_MAX_LEN + 1, "%s.%d", p, id);

    sem_name[SEM_MAX_LEN] = '\0';
}




size_t strlcpy(char *restrict dst, const char *restrict src, size_t siz) {
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0') {
                break;
            }
        }
    }

    if (n == 0) {
        if (siz != 0) {
            *d = '\0';
        }
        while (*s++) {
        }
    }
    return s - src - 1;
}

void magazin_init(struct magazin *s) {
    s->qty = 0;
    
}