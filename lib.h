#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

#define PRD_MAX_LEN 10 // longueur maximal du nom du produit
#define SEM_MAX_LEN (PRD_MAX_LEN+4) // longueur maximal du nom du semaphore


#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            raler(1, #op); \
    } while (0)

#define TCHK(op)                \
    do {                        \
        if ((errno = (op)) > 0) \
            raler(1, #op);      \
    } while (0)

/**
 * @brief structure to handle the content of a magazin.
 *
 */
struct magazin {

    int qty; // quantity of the product
};


/**
 * @brief initialise magazin structure.
 *
 * @param s magazin structure
 */
void magazin_init(struct magazin *s);


/**
 * @brief fonction raler traditionelle
 *
 */
noreturn void raler(int syserr, const char *msg, ...);


/**
 * @brief initialiser le nom d'un semaphore.
 *
 * @param id type du semaphore
 * @param sem_name nom du semaphore 
 * @param fprod nom du produit
 */
void set_sem(int type,  char *sem_name, char *prod);


// pour client.c ////////////////////////////////////////////////////////////////////////   
/**
 * @brief grouper les produits .
 *
 * @param prd liste des noms des produits
 * @param qty liste des quantités
 * @param n nombre de produits
 * @return nombre de produits
 */
int grouper_par_produit(char (*prd)[PRD_MAX_LEN + 1], int *qty, int n);

/**
 * @brief acheter un produit du magazin.
 *
 * @param prd nom du produit
 * @param qty quantite a acheter
 * @return  nombre de prd restant
 */
int acheter_prod(char prd[PRD_MAX_LEN + 1], int qty) ;

/**     
 * @brief loop principale pour chaque produit on achete si le magazin est ouvert et que le stock le permet sinon on attend
 * @param prd nom des produit
 * @param qty quantite a acheter par produit
 * @param nb_prod nombre de produit
 */
void shopping(char (*prd)[PRD_MAX_LEN + 1], int *qty , int n);


///// vendeur.c ////////////////////////////////////////////////////////////////////////

/**
 * @brief ajouter un produit au magazin
 *
 * @param prd nom du produit
 * @param qty quantite du produit
 */
void ajouter_produit(char prd[PRD_MAX_LEN + 1], int qty);

/**
 * @brief supprimer le contenue et supprimer le magazin 
 *
 * @param prd nom du produit
 */
void fermer_magazin(char prd[PRD_MAX_LEN + 1]);