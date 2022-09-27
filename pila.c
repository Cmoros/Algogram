#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t *pila_crear(void) {
    pila_t *pila=malloc(sizeof(pila_t));
    if (pila==NULL) {
        return NULL;
    }
    pila->datos=malloc(sizeof(pila->datos));
    if (pila->datos==NULL) {
        free(pila);
        return NULL;
    }
    pila->cantidad=0;
    pila->capacidad=1;
    return pila;
}

void pila_destruir(pila_t *pila){  	
	free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
    return pila->cantidad==0;
}

bool pila_redimensionar(pila_t *pila, const size_t nuevo_tamano) {
    void** datos_nuevo = realloc(pila->datos,nuevo_tamano*sizeof(pila->datos));
    if (datos_nuevo==NULL) {
        return false;
    }
    pila->datos=datos_nuevo;
    pila->capacidad=nuevo_tamano;
    return true;
}
bool pila_apilar(pila_t *pila, void *valor) {
    if (pila->cantidad==pila->capacidad) {
        if (!pila_redimensionar(pila,pila->capacidad*2)) {
            return false;
        }
    }
    pila->datos[pila->cantidad]=valor;
    pila->cantidad++;
    return true;
}

void *pila_ver_tope(const pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    return pila->datos[pila->cantidad-1];
}

void *pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    }
    pila->cantidad--;
    void* desapilado=pila->datos[pila->cantidad];

    /* Caso pila al cuarto de su capacidad */
    if (pila->cantidad*4<=pila->capacidad && pila->cantidad>0) {
        pila_redimensionar(pila,pila->capacidad/4);
    }
    return desapilado;
}