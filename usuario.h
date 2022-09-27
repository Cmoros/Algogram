#include "post.h"

typedef struct usuario usuario_t;

/* Crea al usuario segun su nick y posicion en el archivo.
 * Pre: 
 * Post: Devuelve el usuario creado.
 */
usuario_t* usuario_crear(char* nick, size_t pos);

/* 
 * Pre: Se creó el usuario.
 * Post: Se agregó un post al feed del usuario.
 */
void usuario_agregar_feed(post_t* post, usuario_t* autor, usuario_t* usuario);

/* 
 * Pre: Se creó el usuario.
 * Post: Devuelve true si el feed está vacío.
 */
bool usuario_feed_vacio(usuario_t* usuario);

/* Imprime por pantalla el proximo post del feed del usuario.
 * Pre: Se creó el usuario.
 * Post: El post ya no está en el feed del usuario.
 */
void usuario_imprimir_feed(usuario_t* usuario);

/* 
 * Pre: Se creó el usuario.
 * Post: Se agregó un like en el post correspondiente de parte del usuario.
 */
void usuario_likear_post(usuario_t* usuario, post_t* post, char* id);

/* 
 * Pre: Se creó el usuario.
 * Post: Se destruyó el usuario.
 */
void usuario_destruir(usuario_t* usuario);