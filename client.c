#include "lib.h"



/**
 * @brief grouper les produits .
 *
 * @param prd liste des noms des produits
 * @param qty liste des quantités
 * @param n nombre de produits
 * @return nombre de produits
 */
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


/**
 * @brief buy a product from the magazin.
 *
 * @param prd product name
 * @param qty quantity to buy
 * @return int - number of products bought
 */
int acheter_prod(char prd[PRD_MAX_LEN + 1], int qty) {
    
    int fd, n, next = 0, num = 0;
    struct magazin s;
    magazin_init(&s);

    CHK(fd = open(prd, O_RDWR, 0666));

    // named semaphore, created if not existing
    char sem_name[SEM_MAX_LEN + 1], cnd_name[SEM_MAX_LEN + 1];
    set_sem(0, sem_name, prd);
    set_sem(1, cnd_name, prd);

    sem_t *sem, *cnd; // file protection, condition

    // these should be created when the magazin is opened
    // if the magazin does not exist, we should raler anyway
    sem = sem_open(sem_name, 0, 0666);
    cnd = sem_open(cnd_name, 0, 0666);

    if ( sem == SEM_FAILED || cnd == SEM_FAILED ) 
    {
        raler(1, "sem_open failure");
    }

    TCHK(sem_wait(cnd)); // wait for the condition
    TCHK(sem_wait(sem)); // wait for possible producer or consumer

  

    CHK(n = read(fd, &s, sizeof(s)));

    if (n == 0) {
        next = 1; // allow the next consumer to raler
        num = -1;
    } else if (n != sizeof(s)) {
        raler(0, "file %s corrupted", prd);
    }

    CHK(lseek(fd, 0, SEEK_SET)); // rewind

    if (n == sizeof(s) && s.qty > 0) {
        num =s.qty;
        if (num > qty)
        {
            num = qty;
        } 
        s.qty -= num;

        next = 1; // allow the next consumer in
        CHK(write(fd, &s, sizeof(s)));
    }

    CHK(close(fd));

    TCHK(sem_post(sem)); // unlock potential new producer or consumer
    if (next) {
        TCHK(sem_post(cnd));
    }

    CHK(sem_close(sem)); // close the semaphore
    CHK(sem_close(cnd));

    return num;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        raler(0, "usage: %s <prd0> <qty0> <prd1> <qty1> ...", argv[0]);
    }

    /* checking command line arguments */

    int n = argc - 1; // number of couple products, qty
    if (n % 2 != 0) {
        raler(0, "usage: %s <prd0> <qty0> <prd1> <qty1> ...", argv[0]);
    }
    n /= 2; // number of products

    char prd[n][PRD_MAX_LEN + 1]; // product names
    int qty[n];                   // quantities

    for (int i = 0; i < n; i++) {
        set_prd(prd[i], "%s", argv[i * 2 + 1]);
        qty[i] = atoi(argv[i * 2 + 2]);
        if (qty[i] < 0) {
            raler(0, "quantity must be positive");
        }
    }

    n = grouper_par_produit(prd, qty, n); // group redundant products

    /* performing the action */

    // for each product
    // if the magazin is closed, quit
    // if the magazin is open, buy the desired quantity if possible
    // if the product is not available, wait until it is available
    int i, _qty, num;
    for (i = 0; i < n; i++) {
        _qty = qty[i];

        while (_qty > 0) {
            num = acheter_prod(prd[i], _qty);
            if (num < 0) {
                raler(0, "magazin %s closed", prd[i]);
            }
            _qty -= num;
        }
    }

    return EXIT_SUCCESS;
}