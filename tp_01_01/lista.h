/*
 * lista.h
 *
 *  Created on: 05/09/2016
 *      Author: juan
 */

#ifndef LISTA_H_
#define LISTA_H_


#include "main.h"

typedef struct {
	pid_t pid_hijo;
	int tiempo_vida;
	int demora;
	struct timeval hora_creacion;
} Info_hijo;

typedef struct _nodo {
   Info_hijo hijo;
   struct _nodo *siguiente;
} tipoNodo;

typedef tipoNodo *pNodo;
typedef tipoNodo *Lista;

void Insertar_al_final(Lista *lista,  Info_hijo v);
void Borrar_PID(Lista *l,  pid_t pid);
Info_hijo * Buscar_PID(Lista lista,pid_t pid);
int Buscar_Tiempo_faltante(Lista lista,pid_t pid);
void Decremetar_tiempo(Lista lista);

int ListaVacia(Lista l);

void BorrarLista(Lista *);
void MostrarLista(Lista l);

int Long_lista(Lista l);

#endif /* LISTA_H_ */
