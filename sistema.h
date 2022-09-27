#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


typedef struct sistema sistema_t;

/* Crea el sistema AlgoGram a partir del archivo contenedor de los usuarios.
 * Pre: 
 * Post: Se creó el sistema.
 */
sistema_t* sistema_crear(FILE* archivo);

/* Recibe por parametro el sistema, la orden, y el medio de salida
 * Según la orden suministrada, el sistema completará un procedimiento.
 * Las ordenes validas son: login - seguido de usuario-, logout, 
 * publicar - seguido de texto, ver_siguiente_feed, likear_post - seguido de id
 * mostrar_likes - seguidode id.
 * Pre: Se creó el sistema.
 * Post: Se modificó (o no) el sistema según la orden proporcionada.
 */
void sistema_orden(sistema_t* sistema, char* orden, FILE* archivo);

/* Destruye el sistema.
 * Pre: Se creó el sistema.
 * Post: El sistema dejó de ser válido.
 */
void sistema_destruir(sistema_t* sistema);