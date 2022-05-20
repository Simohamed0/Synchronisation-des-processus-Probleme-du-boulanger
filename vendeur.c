#include "lib.h"


/**
 * @brief supprimer le contenue et supprimer le magazin
 *
 * @param prd nom du produit
 */
void fermer_magazin(char prd[PRD_MAX_LEN + 1]) {
    
    int fd;

    //  creation des semaphores
    char sem_file_name[SEM_MAX_LEN + 1], sem_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    sem_t *sem_file, *sem; // file protection, condition
    sem_file = sem_open(sem_file_name, O_CREAT, 0666, 1);
    sem = sem_open(sem_name, O_CREAT, 0666, 0);

    TCHK(sem_wait(sem_file)); // attendre un producteur ou un client 

    CHK(fd = open(prd, O_RDWR | O_TRUNC, 0666)); 
    CHK(close(fd));                             
    CHK(unlink(prd));                            

    TCHK(sem_post(sem_file)); 
    TCHK(sem_post(sem)); 

    CHK(sem_close(sem_file)); //  fermer et effacer les semaphores
    CHK(sem_close(sem));
    CHK(sem_unlink(sem_file_name)); 
    CHK(sem_unlink(sem_name));

}

/**
 * @brief ajouter un produit au magazin
 *
 * @param prd nom du produit
 * @param qty quantite du produit
 */
void ajouter_produit(char prd[PRD_MAX_LEN + 1], int qty) {
    
    int fd, n;
    struct magazin s;
    magazin_init(&s);

    // named semaphore, created if not existing
    char sem_file_name[SEM_MAX_LEN + 1], sem_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_file_name, prd);
    set_sem(1, sem_name, prd);

    sem_t *sem_file, *sem; // file protection, condition
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
        s.qty = 0;
    } 
    if (n == -1) 
    {
        raler(1, "read");
    } 


    s.qty += qty;
    CHK(lseek(fd, 0, SEEK_SET)); 
    CHK(write(fd, &s, sizeof(s)));

    CHK(close(fd));

    TCHK(sem_post(sem_file)); // deverouiller un client ou un vendeur potentiel
    TCHK(sem_post(sem)); 

    CHK(sem_close(sem_file)); // fermer la semaphore
    CHK(sem_close(sem));

}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        raler(1, "usage: %s <prd> <qty>", argv[0]);
    }


    char prd[PRD_MAX_LEN + 1];
    
    strcpy(prd, argv[1]);    
    prd[PRD_MAX_LEN] = '\0';

    

    int qty = atoi(argv[2]);
    if (qty < 0) 
    {
        raler(0," quantite must be positive\n");
    }

    switch (qty) {
    case 0:
        fermer_magazin(prd);
        break;
    default:
        ajouter_produit(prd, qty);
        break;
    }

    return EXIT_SUCCESS;
}