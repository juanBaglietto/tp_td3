/*
 * semaforo.c
 *
 *  Created on: 13/09/2016
 *      Author: juan
 */
#include "semaforos.h"

#define PERMISOS 0644

/* crea_sem: abre o crea un semáforo */

int crea_sem ( key_t clave, int valor_inicial )
{
	int semid = semget(clave,1,IPC_CREAT|PERMISOS);

	if ( semid==-1 ) return -1;

	/* Da el valor inicial al semáforo */
	semctl ( semid, 0, SETVAL, valor_inicial );
	return semid;
}

int abre_sem (key_t clave)
{
	return semget(clave,1,0);
}

void sem_P ( int semid )	/* Operación P */
{
	struct sembuf op_P [] =
	{
			0, -1, 0	/* Decrementa semval o bloquea si cero */
	};

	semop ( semid, op_P, 1 );
}

void sem_V ( int semid )	/* Operación V */
{
	struct sembuf op_V [] =
	{
			0, 1, 0		/* Incrementa en 1 el semáforo */
	};
	semop ( semid, op_V, 1 );
}
