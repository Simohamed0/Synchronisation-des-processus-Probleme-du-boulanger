#include "lib.h"



int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        raler(0, "usage: %s <prd0> <qty0> <prd1> <qty1> ...", argv[0]);
    }


    int n = argc - 1; 
    if (n % 2 != 0) {
        raler(0, "usage: %s <prd0> <qty0> <prd1> <qty1> ...", argv[0]);
    }
    n /= 2; // nombre de produits

    char prd[n][PRD_MAX_LEN + 1]; // nom des produits
    int qty[n];                   // quantitees

    for (int i = 0; i < n; i++) {
        
        strcpy(prd[i], argv[i * 2 + 1]);
        prd[i][PRD_MAX_LEN] = '\0';
        qty[i] = atoi(argv[i * 2 + 2]);
        
        if (qty[i] < 0) 
        {
            raler(0, "quantity must be positive");
        }
    }
    n = grouper_par_produit(prd, qty, n);
    shopping(prd, qty, n);
    

    return 0;
}

