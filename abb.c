#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pila.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo nodo_t;

struct nodo {
    nodo_t* der;
    nodo_t* izq;
    char* clave;
    void* dato;
};

struct abb {
    nodo_t* raiz;
    size_t cantidad;
    abb_comparar_clave_t funcion_comparar;
    abb_destruir_dato_t funcion_destruir;
};

struct abb_iter {
    pila_t* pila;
    const abb_t* arbol;
};

/* ******************************************************************
 *                 PRIMITIVAS AUXILIARES
 * *****************************************************************/

// Recibe una clave
// Crea en memoria dinamica una copia de la clave
// Devuelve la copia
char* copiar_clave2(const char*clave) {
    char* copia_clave = malloc(sizeof(char)*(strlen(clave)+1));
    if (!copia_clave) {
    return NULL;
    }
    strcpy(copia_clave,clave);
    return copia_clave;
}

// Recibe una clave
// Crea un nodo que será una hoja del arbol
// Devuelve el nodo creado
nodo_t* creacion_hoja(const char* clave, bool* agregado) {
    nodo_t *nodo_creado = malloc(sizeof(nodo_t));
    if (nodo_creado == NULL) {
        return NULL;
    }
    char* copia_clave = copiar_clave2(clave);
    if (!copia_clave) {
        free(nodo_creado);
        return NULL;
    }
    nodo_creado->der = NULL;
    nodo_creado->izq = NULL;
    nodo_creado->clave = copia_clave;
    nodo_creado->dato = NULL;
    *agregado = true;
    return nodo_creado;
}

// Funcion auxiliar resursiva, recibe nodo, clave, funcion comparar y flag agregar y agregado
// agregar indica la necesidad de tener (o no) que crear una hoja de no encontrar la clave
// agregado indica que se agregó una hoja al arbol, para su posterior aumento de elementos
// Busca el nodo con la clave asociada y lo devuelve, de no encontrarla, devuelve NULL.
// Si agregar=true, de no encontrarlo, crea la hoja, sino igual devuelve el nodo y luego actualiza el dato
nodo_t* abb_busqueda(nodo_t* nodo, const char* clave, abb_comparar_clave_t cmp,bool agregar, bool* agregado) {
    nodo_t* nuevo_nodo = NULL;
    if (!nodo) return NULL;
    if (cmp(clave,nodo->clave) > 0) {
        if (!nodo->der && agregar) {
            nodo->der = creacion_hoja(clave, agregado);
            if (!nodo->der) return NULL;
            nuevo_nodo = nodo->der;
        } else {
            nuevo_nodo = abb_busqueda(nodo->der,clave,cmp,agregar,agregado);
        }
    } else if (cmp(clave,nodo->clave) < 0) {
        if (!nodo->izq && agregar) {
            nodo->izq = creacion_hoja(clave, agregado);
            if (!nodo->izq) return NULL;
            nuevo_nodo = nodo->izq;
        } else {
            nuevo_nodo = abb_busqueda(nodo->izq,clave,cmp,agregar,agregado);
        }
    } else {
        nuevo_nodo = nodo;
    }
    return nuevo_nodo;
}

// Funcion auxiliar recursiva, recibe nodo y la funcion destruir
// Destruye todos los nodos, de existir funcion destruir, libera el dato de cada nodo.
void abb_liberar(nodo_t* nodo,abb_destruir_dato_t funcion_destruir) {
    if (!nodo) {
        return;
    }
    abb_liberar(nodo->izq,funcion_destruir);
    abb_liberar(nodo->der,funcion_destruir);
    if (funcion_destruir) {
        funcion_destruir(nodo->dato);
    }
    free(nodo->clave);
    free(nodo);
}

// Funcion auxiliar recursiva, recibe el nodo hijo izquierdo del nodo a retirar
// Busca el nodo de mayor clave que esté a la derecha del nodo a retirar, que sirva para reemplazarlo
// Actualiza los punteros de los nodos involucrados
// Devuelve el nodo
nodo_t* abb_busqueda_reemplazo(nodo_t* nodo) {
    if (!nodo->der) {
        return nodo;
    }
    nodo_t* reemplazo;
    if (!nodo->der->der) {
        reemplazo = nodo->der;
        nodo->der=nodo->der->izq;
    } else {
        reemplazo = abb_busqueda_reemplazo(nodo->der);
    }
    return reemplazo;
}

nodo_t* abb_reemplazo(nodo_t* nuevo_nodo) {
    nodo_t* nodo_reemplazo = abb_busqueda_reemplazo(nuevo_nodo->izq);
    if (nodo_reemplazo!=nuevo_nodo->izq) {
        nodo_reemplazo->izq = nuevo_nodo->izq;
    }
    nodo_reemplazo->der = nuevo_nodo->der;
    return nodo_reemplazo;
}

// Funcion auxiliar recursiva, recibe nodo, clave, funcion comparar y flag conectar
// Flag conectar sirve para saber si hay que conectar el nodo actual con los hijos del nodo a borrar
// Cambia de estado al realizar la conexion + la supresion del nodo, y de ser necesario el nodo reemplazante
// Devuelve el nodo en cuestion que se va a liberar, y el dato que se va a devolver
nodo_t* abb_suprimir(nodo_t* nodo,const char* clave, abb_comparar_clave_t cmp) {
    if (!nodo) {
        return NULL;
    }
    nodo_t* nuevo_nodo;
    if (cmp(nodo->clave,clave) == 0) {
            return nodo;
    } else if (cmp(clave,nodo->clave) > 0) {
        nuevo_nodo = abb_suprimir(nodo->der,clave,cmp);
        if (nuevo_nodo && nuevo_nodo==nodo->der) {
            if (!nuevo_nodo->der || !nuevo_nodo->izq) {
                nodo->der = nuevo_nodo->der != NULL? nuevo_nodo->der: nuevo_nodo->izq;
            } else {
                nodo_t* nodo_reemplazo = abb_reemplazo(nuevo_nodo);
                nodo->der = nodo_reemplazo;
            }
        }
    } else {
        nuevo_nodo = abb_suprimir(nodo->izq,clave,cmp);
        if (nuevo_nodo && nuevo_nodo==nodo->izq) {
            if (!nuevo_nodo->der || !nuevo_nodo->izq) {
                nodo->izq = nuevo_nodo->der != NULL? nuevo_nodo->der: nuevo_nodo->izq;
            } else {
                nodo_t* nodo_reemplazo = abb_reemplazo(nuevo_nodo);
                nodo->izq = nodo_reemplazo;
            }
        }
    }
    return nuevo_nodo;
}

/* ******************************************************************
 *             PRIMITIVAS DEL ARBOL BINARIO DE BUSQUEDA
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t* arbol = malloc(sizeof(abb_t));
    if (!arbol) return NULL;
    arbol->raiz = NULL;
    arbol->cantidad = 0;
    arbol->funcion_comparar = cmp;
    arbol->funcion_destruir = destruir_dato;
    return arbol;
}

// El flag agregado es para determinar si un nodo fue agregado o si solo fue actualizado
// Esto para determinar si se debe sumar en 1 la cantidad de elementos
// Esto se pudo haber hecho tambien preguntando antes abb_pertenece(arbol,clave)
// pero como cuesta log(n), siendo n la cantidad de elementos.
// Entonces nos pareció mejor usar una flag, aunque complique un poco el código.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    nodo_t* nueva_hoja = NULL;
    bool agregado = false;
    if (arbol->raiz) {
        nueva_hoja = abb_busqueda(arbol->raiz, clave, arbol->funcion_comparar, true, &agregado);
        if (!nueva_hoja) return false;
        if (agregado) {
            arbol->cantidad++;
        } else {
            if (arbol->funcion_destruir) {
                arbol->funcion_destruir(nueva_hoja->dato);
            }
        }
    } else {
        nueva_hoja = creacion_hoja(clave, &agregado);
        if (nueva_hoja == NULL) return false;
        arbol->raiz = nueva_hoja;
        arbol->cantidad++;
    }
    nueva_hoja->dato = dato;
    return true;
}

void *abb_borrar(abb_t *arbol, const char *clave) {
    if (!arbol->raiz) {
        return NULL;
    }
    nodo_t *nodo_borrar = NULL;
    nodo_borrar = abb_suprimir(arbol->raiz,clave,arbol->funcion_comparar);
    if (!nodo_borrar) return NULL;
    if (nodo_borrar == arbol->raiz) {
        if ((!nodo_borrar->der || !nodo_borrar->izq)) {
            arbol->raiz = nodo_borrar->der != NULL? nodo_borrar->der : nodo_borrar->izq;
        } else {
            nodo_t* nodo_reemplazo = abb_reemplazo(nodo_borrar);
            arbol->raiz=nodo_reemplazo;
        }
    }
    if (!nodo_borrar) return NULL;
    void* dato = nodo_borrar->dato;
    free(nodo_borrar->clave);
    free(nodo_borrar);
    arbol->cantidad--;
    return dato;
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
    nodo_t* nodo_buscado = abb_busqueda(arbol->raiz,clave,arbol->funcion_comparar,false,NULL);
    if (!nodo_buscado) return NULL;
    return nodo_buscado->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    nodo_t* nuevo_nodo = abb_busqueda(arbol->raiz,clave,arbol->funcion_comparar,false,NULL);
    return nuevo_nodo != NULL;
}

size_t abb_cantidad(const abb_t *arbol) {
    return arbol->cantidad;
}

void abb_destruir(abb_t *arbol) {
    abb_liberar(arbol->raiz,arbol->funcion_destruir);
    free(arbol);
}

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Recibe el nodo actual, la funcion visitar y el extra
// Le aplica visitar a los elementos In-Order y se detiene al devolver false.
// iter_visitar devuelve false si la funcion visitar devuelve false, el resto de los casos devuelve true.
bool iter_visitar(nodo_t* nodo, bool visitar(const char *clave, void *dato, void *extra), void* extra) {
    if (!nodo) {
        return true;
    }
    if (!iter_visitar(nodo->izq,visitar,extra)) {
        return false;
    }
    if (!visitar(nodo->clave,nodo->dato,extra)) {
        return false;
    }
    if (!iter_visitar(nodo->der,visitar,extra)) {
        return false;
    }
    return true;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    iter_visitar(arbol->raiz,visitar,extra);
}

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

bool abb_iter_in_apilar_izquierda(nodo_t* nodo, abb_iter_t* iter) {
    while (nodo) {
        if (!pila_apilar(iter->pila,nodo)) {
            abb_iter_in_destruir(iter);
            return false;
        }
        nodo = nodo->izq;
    }
    return true;
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if (!iter) {
        return NULL;
    }
    pila_t* pila = pila_crear();
    if (!pila) {
        free(iter);
        return NULL;
    }
    iter->pila = pila;
    iter->arbol = arbol;
    nodo_t* nodo = arbol->raiz;
    if (!abb_iter_in_apilar_izquierda(nodo,iter)) return NULL;
    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
    if (abb_iter_in_al_final(iter)) {
        return false;
    }
    nodo_t* nodo_desapilado=pila_desapilar(iter->pila);
    if (nodo_desapilado->der) {
        nodo_t* nodo = nodo_desapilado->der;
        if (!abb_iter_in_apilar_izquierda(nodo,iter)) return false;
    }
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
    nodo_t* nodo = pila_ver_tope(iter->pila);
    if (!nodo) {
        return NULL;
    }
    return (nodo->clave);
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
    return (pila_esta_vacia(iter->pila));
}

void abb_iter_in_destruir(abb_iter_t* iter) {
    while (!pila_esta_vacia(iter->pila)) {
        pila_desapilar(iter->pila);
    }
    pila_destruir(iter->pila);
    free(iter);
}