#ifndef CSMA_H
#define CSMA_H

#define CSMA_NB 4    //the number of csma number
#define CSMA_BE 5    // backoff  exponent

#define BM(n) (1 << (n))

void csma_init(void);

int manual_csma(void);

#endif

