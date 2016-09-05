/*
 * lista.c
 *
 *  Created on: 05/09/2016
 *      Author: juan
 */

/* Devuelve la longitud de una lista */
#include "lista.h"

int long_lista(Lista *lista){

	 pNodo anterior;
	int n=0;
	anterior = *lista;

	      while(anterior->siguiente==NULL){
	         anterior = anterior->siguiente;
	         ++n;
	      }

	return n;
}

void Insertar_al_final(Lista *lista, Info_hijo v) {
   pNodo nuevo, anterior;

   	   	   	   	   	   	   	   	   	   	   	   /* Crear un nodo nuevo */
   nuevo = (pNodo)malloc(sizeof(tipoNodo));
   nuevo->hijo = v;
  // nuevo->siguiente=NULL; //lo ingreso al final de la lista


   if(ListaVacia(*lista) ) {

      nuevo->siguiente = *lista;
      	  	  	  	  	  	  	  	  	  	  	 /* Ahora, el comienzo de la lista es en nuevo nodo */
      *lista = nuevo;
   } else {

      anterior = *lista;

      while(anterior->siguiente!=NULL)
         anterior = anterior->siguiente;
      	  	  	  	  	  	  	  	  	  	  	/* se incerta el nuevo nodo después del nodo anterior */
      nuevo->siguiente = anterior->siguiente;
      anterior->siguiente = nuevo;
   }
}

void Borrar_PID(Lista *lista, pid_t pid) {
   pNodo anterior, nodo;

   nodo = *lista;
   anterior = NULL;
   while(nodo && nodo->hijo.pid_hijo != pid ) {
      anterior = nodo;
      nodo = nodo->siguiente;
   }
   if(!nodo || nodo->hijo.pid_hijo != pid)
	   return;
   else { 										/* Borrar el nodo */
      if(!anterior) 							/* Primer elemento */
         *lista = nodo->siguiente;
      else 										 /* un elemento cualquiera */
         anterior->siguiente = nodo->siguiente;
      free(nodo);
   }
}

int ListaVacia(Lista lista) {
   return (lista == NULL);
}

void BorrarLista(Lista *lista) {
   pNodo nodo;

   while(*lista) {
      nodo = *lista;
      *lista = nodo->siguiente;
      free(nodo);
   }
}

void MostrarLista(Lista lista) {
   pNodo nodo = lista;

   if(ListaVacia(lista)) printf("Lista vacía\n");
   else {
      while(nodo) {
         printf("(%d) tiempo de vida (%d)\n", nodo->hijo.pid_hijo,nodo->hijo.tiempo_vida);
         nodo = nodo->siguiente;
     }

   }
}
