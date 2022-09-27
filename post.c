#include "post.h"
#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct post{
  char* contenido;              // String contenido del post
  size_t id;                    // Id del post
  abb_t* likes;                 // abb de NULL con claves->nick de usuarios - strcmp
  char* nick_autor;             // Nick del usuario autor del post
};

post_t* post_crear(char* texto_post, size_t nuevo_id, char* autor){
  post_t* post = malloc(sizeof(post_t));
  if (!post) return NULL;
  post->nick_autor = malloc(sizeof(char) * strlen(autor) +1 );
  post->contenido = malloc(sizeof(char) * strlen(texto_post) +1 );
  strcpy(post->contenido, texto_post);
  strcpy(post->nick_autor, autor);
  post->id = nuevo_id;
  post->likes = abb_crear(strcmp, NULL);
  return post;
}

size_t post_obtener_id(post_t* post) {
  return post->id;
}

char* post_obtener_autor(post_t* post) {
  return post->nick_autor;
}

char* post_obtener_contenido(post_t* post) {
  return post->contenido;
}

size_t post_obtener_cantidad_likes(post_t* post) {
  return abb_cantidad(post->likes);
}

void post_likear(post_t* post, char* nick) {
  abb_guardar(post->likes, nick, NULL);
}

void post_imprimir_likes(post_t* post) {
  abb_iter_t* iter = abb_iter_in_crear(post->likes);
  while (!abb_iter_in_al_final(iter)) {
    const char* usuario = abb_iter_in_ver_actual(iter);
    printf("\t%s\n",usuario);
    abb_iter_in_avanzar(iter);
  }
  abb_iter_in_destruir(iter);
}

void post_destruir(post_t* post) {
  free(post->contenido);
  free(post->nick_autor);
  abb_destruir(post->likes);
  free(post);
}