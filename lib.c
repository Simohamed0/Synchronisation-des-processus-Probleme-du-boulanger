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



void set_sem(int id, char *restrict sem_name, char *restrict msg, ...) {
    int n;
    char p[PRD_MAX_LEN + 1];
    va_list ap;

    va_start(ap, msg);
    if ((n = vsnprintf(p, PRD_MAX_LEN + 1, msg, ap)) > PRD_MAX_LEN) {
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

////////////////// client.c ///////////////////////////////////

int grouper_par_produit(char (*prd)[PRD_MAX_LEN + 1], int *qty, int n) {
    
    int i, j, l;
    for (i = 0; i < n; i++) 
    {
        for (j = i + 1; j < n; j++) 
        {
            if (strcmp(prd[i], prd[j]) == 0) 
            {    
                qty[i] += qty[j];   
                for (l = j; l < n - 1; l++) 
                {    
                    qty[l] = qty[l + 1];
                }
                n--;
                j--;
            }
        }
    }
    return n;
}



int acheter_prod(char prd[PRD_MAX_LEN + 1], int qty) {
    
    int fd, n, suivant = 0, num = 0;
    struct magazin s;
    magazin_init(&s);

    CHK(fd = open(prd, O_RDWR, 0666));

    // creation des semaphores
    char sem_file_name[SEM_MAX_LEN + 1]; 
    char sem_name[SEM_MAX_LEN + 1];
    
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    // deux sempahores un pour le fichier 
    sem_t *sem_file, *sem; 

    //ouverture des semaphores
    sem_file = sem_open(sem_file_name, 0, 0666);
    sem = sem_open(sem_name, 0, 0666);

    if ( sem_file== SEM_FAILED || sem == SEM_FAILED ) 
    {
        raler(1, "sem_open failure");
    }

    TCHK(sem_wait(sem)); 
    TCHK(sem_wait(sem_file)); 

  

    CHK(n = read(fd, &s, sizeof(s)));

    if (n == 0) 
    {
        suivant = 1; // pour que le client suivant rale
        num = -1;
    } 

    CHK(lseek(fd, 0, SEEK_SET)); 

    if (n == sizeof(s) && s.qty > 0)
    {
        
        num =s.qty;
        if (num > qty)
        {
            num = qty;
        } 
        s.qty -= num;

        suivant = 1; // 
        CHK(write(fd, &s, sizeof(s)));
    }

    CHK(close(fd));

    TCHK(sem_post(sem_file)); // deverouiller un client ou un venduer potentiel
    if (suivant) 
    {
        TCHK(sem_post(sem));
    }

    CHK(sem_close(sem_file)); // fermer la semaphore
    CHK(sem_close(sem));

    return num;
}


void shopping(char (*prd)[PRD_MAX_LEN + 1], int *qty , int n) {
    
    int i, qty_copy, num;
    for (i = 0; i < n; i++) 
    {
        qty_copy = qty[i];
        while (qty_copy> 0) 
        {
            num = acheter_prod(prd[i], qty_copy);
            if (num < 0) {
                raler(0, "magazin %s closed", prd[i]);
            }
            qty_copy -= num;
        }
    }
    
}

/////////////////// magazin.c //////////////////////////////

