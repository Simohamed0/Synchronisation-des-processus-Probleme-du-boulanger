#include "lib.h"

noreturn void raler(int syserr, const char * msg, ...) {
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1) {
        perror("\t");
    }

    exit(EXIT_FAILURE);
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

    snprintf(sem_name, SEM_MAX_LEN + 1, "%s.%d", p, id);

    sem_name[SEM_MAX_LEN] = '\0';
}


void magazin_init(struct magazin *s) {
    
    s->qty = 0;
}