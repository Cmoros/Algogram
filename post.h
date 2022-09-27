#include <stdbool.h>
#include <stddef.h>

typedef struct post post_t;

/* Recibe el contenido del post, el id y el nick del autor
 * Pre: 
 * Post: Crea el nuevo post en el struct post_t con su id, contenido y autor.
 */
post_t* post_crear(char* texto_post, size_t nuevo_id, char* autor);

/* 
 * Pre: Se creó el post.
 * Post: Devuelve el id del post.
 */
size_t post_obtener_id(post_t* post);

/* 
 * Pre: Se creó el post.
 * Post: Devuelve el autor del post.
 */
char* post_obtener_autor(post_t* post);

/* 
 * Pre: Se creó el post.
 * Post: Devuelve el contenido del post.
 */
char* post_obtener_contenido(post_t* post);

/* 
 * Pre: Se creó el post.
 * Post: Devuelve la cantidad de likes del post.
 */
size_t post_obtener_cantidad_likes(post_t* post);

/* Guarda en el post el like del usuario.
 * Pre: Se creó el post.
 * Post: Se guardó el nick del usuario que likeo el post.
 */
void post_likear(post_t* post, char* nick);

/* Recorre todos los likes del post y los imprime en orden alfabetico.
 * Pre: Se creó el post.
 * Post: Se imprimio todos los likes del post.
 */
void post_imprimir_likes(post_t* post);

/* 
 * Pre: Se creó el post.
 * Post: Se destruyó el post.
 */
void post_destruir(post_t* post);