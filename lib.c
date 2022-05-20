#include "lib.h"



// note : j'ai ajouter rm -r -f /dev/shm/sem.* après chaque test dans test2.sh pour supprimer les semaphores après chaque test







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

void set_sem(int type,  char *sem_name, char *prod){
    
    char *suffixe;
    switch(type)
    {
        case 0:
            suffixe = "file";
            break;
        case 1:
            suffixe = "nofi";
            break;
    }
    strcpy(sem_name, prod);
    strcat(sem_name, suffixe);

}

void magazin_init(struct magazin *s) {
    
    s->qtn = 0;
}
////////////////// client.c ///////////////////////////////////

int grouper_par_produit(char (*prd)[PRD_MAX_LEN + 1], int *qtn, int n) {
    
    int i, j, l;
    for (i = 0; i < n; i++) 
    {
        for (j = i + 1; j < n; j++) 
        {
            if (strcmp(prd[i], prd[j]) == 0) 
            {    
                qtn[i] += qtn[j];   
                for (l = j; l < n - 1; l++) 
                {    
                    qtn[l] = qtn[l + 1];
                }
                n--;
                j--;
            }
        }
    }
    return n;
}



int acheter_prod(char prd[PRD_MAX_LEN + 1], int qtn) {
    
    int fd, n, suivant = 0, num = 0;
    struct magazin s;
    magazin_init(&s);

    CHK(fd = open(prd, O_RDWR, 0666));

    // creation des semaphores

    char sem_file_name[SEM_MAX_LEN + 1]; 
    char sem_name[SEM_MAX_LEN + 1];
    
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    // deux sempahores 
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

    if (n == sizeof(s) && s.qtn > 0)
    {
        num =s.qtn;
        if (num > qtn)
        {
            num = qtn;
        } 
        s.qtn -= num;

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


void shopping(char (*prd)[PRD_MAX_LEN + 1], int *qtn , int n) {
    
    int i, qty_copy, num;
    for (i = 0; i < n; i++) 
    {
        qty_copy = qtn[i];
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


void fermer_magazin(char prd[PRD_MAX_LEN + 1]) {
    
    int fd;

    //  creation des semaphores
    char sem_file_name[SEM_MAX_LEN + 1], sem_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    sem_t *sem_file, *sem; 
    sem_file = sem_open(sem_file_name, O_CREAT, 0666, 1);
    sem = sem_open(sem_name, O_CREAT, 0666, 0);

    // attendre un producteur ou un client 
    TCHK(sem_wait(sem_file)); 

    CHK(fd = open(prd, O_RDWR | O_TRUNC, 0666)); 
    CHK(close(fd));                             
    CHK(unlink(prd));                            

    // deverouiller un client ou un vendeur
    TCHK(sem_post(sem_file)); 
    TCHK(sem_post(sem)); 

    //  fermer et effacer les semaphores
    CHK(sem_close(sem_file)); 
    CHK(sem_close(sem));
    CHK(sem_unlink(sem_file_name)); 
    CHK(sem_unlink(sem_name));

}


void ajouter_produit(char prd[PRD_MAX_LEN + 1], int qtn) {
    
    int fd, n;
    struct magazin s;
    magazin_init(&s);

    // creation des semphores
    char sem_file_name[SEM_MAX_LEN + 1], sem_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    sem_t *sem_file, *sem; 
    sem_file = sem_open(sem_file_name, O_CREAT, 0666, 1);
    sem = sem_open(sem_name, O_CREAT,0666, 0);

    if(sem_file == SEM_FAILED || sem_file == SEM_FAILED) 
    {
        raler(1, "sem_open failure");
    }

    TCHK(sem_wait(sem_file)); // attendre un vendeur ou producer or consumer

    CHK(fd = open(prd, O_RDWR | O_CREAT, 0666));


    if ((n = read(fd, &s, sizeof(s))) == 0) 
    {
        s.qtn = 0;
    } 
    if (n == -1) 
    {
        raler(1, "read");
    } 


    s.qtn += qtn;
    CHK(lseek(fd, 0, SEEK_SET)); 
    CHK(write(fd, &s, sizeof(s)));

    CHK(close(fd));

    TCHK(sem_post(sem_file)); // deverouiller un client ou un vendeur potentiel
    TCHK(sem_post(sem)); 

    CHK(sem_close(sem_file)); // fermer la semaphore
    CHK(sem_close(sem));

}
