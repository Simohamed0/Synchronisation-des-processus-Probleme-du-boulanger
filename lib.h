#pragma once

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

#define PRD_MAX_LEN 10 // max length of product name
#define SEM_DFF_LEN 3  // max length of semaphore id

#define SEM_MAX_LEN PRD_MAX_LEN + SEM_DFF_LEN


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
 * @brief fonction raler traditionelle
 *
 */
noreturn void raler(int syserr, const char *msg, ...);

/**
 * @brief safe call to snprintf.
 *
 * @param str string to write to
 * @param size size of the string
 * @param fmt formated message to print
 */
void snprintf_s(char *restrict str, size_t size, const char *restrict fmt, ...);


/**
 * @brief set a product name from command line arguments.
 *
 * @param prd product name to set
 * @param fmt suitable format string (max `PRD_MAX_LEN` characters)
 */
void set_prd(char *restrict prd, const char *restrict fmt, ...);

/**
 * @brief set a semaphore name from a product name.
 *
 * @param id suffixe id (max `SEM_DFF_LEN` characters)
 * @param sem_name semaphore name to set
 * @param fmt formated product name (max `PRD_MAX_LEN` characters)
 */
void set_sem(int id, char *restrict sem_name, char *restrict fmt, ...);
      

/**
 * @brief copies the string pointed to by src, including the terminating null
 * byte ('\0')
 *
 * @param dest destination string
 * @param src source string
 * @param n number of characters to copy
 * @return size_t - the number of characters copied, not including the
 * terminating null byte
 */
size_t strlcpy(char *restrict dest, const char *restrict src, size_t n);

/**
 * @brief initialise magazin structure.
 *
 * @param s magazin structure
 */
void magazin_init(struct magazin *s);