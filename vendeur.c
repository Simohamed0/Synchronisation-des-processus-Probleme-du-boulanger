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
 * @brief add a product to the stock by either adding or creating it.
 *
 * @param prd product name
 * @param qty quantity to add or create
 */
void add_to_shop(char prd[PRD_MAX_LEN + 1], int qty) {
    int fd, n;
    struct magazin s;
    magazin_init(&s);

    // named semaphore, created if not existing
    char sem_name[SEM_MAX_LEN + 1], cnd_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_name, prd);
    set_sem(1, cnd_name, prd);

    sem_t *sem, *cnd; // file protection, condition
    sem = sem_open(sem_name, O_CREAT, 0666, 1);
    cnd = sem_open(cnd_name, O_CREAT,0666, 0);

    TCHK(sem_wait(sem)); // attendre un venduer ou producer or consumer

    CHK(fd = open(prd, O_RDWR | O_CREAT, 0666));


    if ((n = read(fd, &s, sizeof(s))) == 0) {
        s.qty = 0;
    } // is the file empty?
    if (n > 0 && n != sizeof(s)) {
        raler(0, "file %s corrupted", prd);
    } // we did not read the whole structure properly
    if (n == -1) {
        raler(1, "read");
    } // error reading file

    // update the file
    s.qty += qty;
    CHK(lseek(fd, 0, SEEK_SET)); // rewind
    CHK(write(fd, &s, sizeof(s)));

    CHK(close(fd));

    TCHK(sem_post(sem)); // unlock potential new producer or consumer
    TCHK(sem_post(cnd)); // signal the condition

    CHK(sem_close(sem)); // close the semaphore
    CHK(sem_close(cnd));

    
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        raler(1, "usage: %s <prd> <qty>", argv[0]);
    }

    /* checking command line arguments */

    char prd[PRD_MAX_LEN + 1];
    
    strcpy(prd, argv[1]);    
    prd[PRD_MAX_LEN] = '\0';

    

    int qty = atoi(argv[2]);
    if (qty < 0) {
        raler(0," quantite negative ");
    }

    /* performing the action */

    switch (qty) {
    case 0:
        fermer_magazin(prd);
        break;
    default:
        add_to_shop(prd, qty);
        break;
    }

    return EXIT_SUCCESS;
}