#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/


typedef struct abb abb_t;

typedef struct abb_iter abb_iter_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *             PRIMITIVAS DEL ARBOL BINARIO DE BUSQUEDA
 * *****************************************************************/

// Crea un Arbol Binaria de Búsqueda
// Post: se creó el abb
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda en el ABB el dato con su clave asociada en el lugar que corresponda
// Pre: Se creó el ABB
// Post: Se guardó el dato y una copia de la clave, si ya existia la clave pista el dato
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Devuelve del ABB el dato a través de su clave, y libera la clave
// Pre: Se creó el ABB
// Post: Se borró la clave y el dato del ABB y se devolvio la clave. Devuelve NULL si la clave no se encuentra
void *abb_borrar(abb_t *arbol, const char *clave);

// Devuelve el valor del dato a través de su clave
// Pre: Se creó el ABB
void *abb_obtener(const abb_t *arbol, const char *clave);

// Devuelve true si la clave pertenece al ABB y false caso contrario
// Pre: Se creó el ABB
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos en el ABB
// Pre: Se creó el ABB
size_t abb_cantidad(const abb_t *arbol);

// Libera el ABB junto con sus claves (y datos si es deseado)
// Pre: Se creó el ABB
// Post: Se destruyó el ABB
void abb_destruir(abb_t *arbol);

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Recorre los elementos In-Order y les aplica la funcion visitar hasta que sea falsa
// Post: se le aplicó la funcion visitar a los elementos deseados en el ABB
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea iterador externo para el ABB.
// Post: Iterador externo fue creado.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza al siguiente elemento In-Order, false si se encuentra al final.
// Pre: Iterador externo fue creado.
// Post: Se avanzó al siguiente elemento, o devuelve false en caso de estar al final.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave del elemento en el que se encuentra el iterador, NULL en caso de estar al final.
// Pre: Iterador externo fue creado.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Devuelve true si el iterador se encuentra al final, o false caso contrario.
// Pre: Iterador externo fue creado.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador externo.
// Pre: Iterador externo fue creado.
// Post: Iterador externo se destruyó.
void abb_iter_in_destruir(abb_iter_t* iter);

#endif  // ABB_H