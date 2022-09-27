#include "usuario.h"
#include "hash.h"
#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct post_afinidad {
  size_t afinidad_con_autor;    // Cercania al autor del post en archivo.txt
  post_t* post;                 // Post en cuestion
} post_afinidad_t;

struct usuario{
  char* nick;                   // Nick de usuario
  size_t pos;                   // Posicion en el archivo.txt
  heap_t* feed;                 // Heap de post_afinidad_t - comparacion_afinidad
};

// Recibe 2 post_afinidad
// Calcula la cercania del autor del post con los 2 usuarios
// Devuelve 1 si el primero está mas cerca, -1 si es el segundo
// 0 si están a la misma distancia
int comparacion_afinidad(const void* afinidad1, const void* afinidad2) {
  post_afinidad_t afin1 = *((post_afinidad_t*)afinidad1);
  post_afinidad_t afin2 = *((post_afinidad_t*)afinidad2);
  int res;
  if (afin1.afinidad_con_autor < afin2.afinidad_con_autor) {
    res = 1;
  } else if (afin1.afinidad_con_autor > afin2.afinidad_con_autor) {
    res = -1;
  } else {
    if (post_obtener_id(afin1.post) > post_obtener_id(afin2.post)) {
      res = -1;
    } else {
      res = 1;
    }
  }
  return res;
}

usuario_t* usuario_crear(char* nick, size_t pos) {
  usuario_t* usuario = malloc(sizeof(usuario_t));
  usuario->pos = pos;
  usuario->nick = nick;
  usuario->feed = heap_crear(comparacion_afinidad);
  return usuario;
}

// Recibe el post, la posicion del autor y del usuario en cuestion
// Crea el post_afinidad_t que determina la afinidad entre el autor y el usuario
// Devuelve el post_afinidad_t
post_afinidad_t* usuario_crear_post_afinidad(post_t* post, size_t pos_autor, size_t pos_usuario) {
  post_afinidad_t* post_afinidad = malloc(sizeof(post_afinidad_t));
  post_afinidad->post = post;
  size_t afinidad = pos_usuario > pos_autor? pos_usuario - pos_autor : pos_autor - pos_usuario;
  post_afinidad->afinidad_con_autor = afinidad;
  return post_afinidad;
}

void usuario_agregar_feed(post_t* post, usuario_t* autor, usuario_t* usuario) {
  post_afinidad_t* post_afinidad = usuario_crear_post_afinidad(post, autor->pos, usuario->pos);
  heap_encolar(usuario->feed, post_afinidad);
}

bool usuario_feed_vacio(usuario_t* usuario) {
  return heap_esta_vacio(usuario->feed);
}

void usuario_imprimir_feed(usuario_t* usuario) {
  if (!usuario_feed_vacio(usuario)) {
    post_afinidad_t* post = heap_desencolar(usuario->feed);     // O(logp) siendo p los posts del feed del usuario
    fprintf(stdout, "Post ID %li\n%s dijo: %s\nLikes: %li\n", post_obtener_id(post->post), post_obtener_autor(post->post), post_obtener_contenido(post->post), post_obtener_cantidad_likes(post->post));
    free(post);
  }
  return;
}

void usuario_likear_post(usuario_t* usuario, post_t* post, char* id) {
  post_likear(post, usuario->nick);       // O(logu) siendo u la cantidad de usuarios que han likeado el post
}

void usuario_destruir(usuario_t* usuario) {
  free(usuario->nick);
  heap_destruir(usuario->feed, free);
  free(usuario);
}