#include "lib.h"



int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        raler(1, "usage: %s <prd> <qtn>", argv[0]);
    }

    char prd[PRD_MAX_LEN + 1];
    
    strcpy(prd, argv[1]);    
    prd[PRD_MAX_LEN] = '\0';

    int qtn = atoi(argv[2]);
    if (qtn < 0) 
    {
        raler(0," quantite must be positive\n");
    }
    
    switch (qtn) 
    {
    case 0:
        fermer_magazin(prd);
        break;
    default:
        ajouter_produit(prd, qtn);
    }

    return 0;
}