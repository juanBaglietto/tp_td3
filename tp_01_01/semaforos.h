/*
 * semaforos.h
 *
 *  Created on: 13/09/2016
 *      Author: juan
 */

#ifndef SEMAFOROS_H_
#define SEMAFOROS_H_

#include "main.h"

int crea_sem ( key_t clave, int valor_inicial );
int abre_sem (key_t clave);
void sem_P ( int semid );
void sem_V ( int semid );

union semun {
int val;
struct semid_ds *buf;
ushort *array;
};
key_t key;


#endif /* SEMAFOROS_H_ */
