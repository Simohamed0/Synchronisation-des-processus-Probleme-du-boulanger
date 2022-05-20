#include "lib.h"



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
    
    switch (qty) 
    {
    case 0:
        fermer_magazin(prd);
        break;
    default:
        ajouter_produit(prd, qty);
        break;
    }

    return 0;
}