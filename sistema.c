#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include "heap.h"
#include "sistema.h"
#include "usuario.h"

// Recibe un string
// Devuelve el tamaño del string de haberte tenido o no un salto de linea al final
size_t verificar_enter(char* linea) {
  size_t tamano_linea;
  if (linea[strlen(linea)-1] == '\n') {
    tamano_linea = strlen(linea) - 1;
  } else {
    tamano_linea = strlen(linea);
  }
  return tamano_linea;
}

// Recibe un string
// Crea una copia del string habiendo removido el salto de linea
// Devuelve el string copia
char* deshacer_enter(char* linea) {
  size_t tamano_linea = verificar_enter(linea);
  char* nueva_linea = calloc(tamano_linea + 1,sizeof(char) );
  strncpy(nueva_linea, linea, tamano_linea);
  return nueva_linea;
}

void wrapper_usuario_destruir(void* usuario) {
  usuario_destruir(usuario);
}

void wrapper_post_destruir(void* post) {
  post_destruir(post);
}

struct sistema{
  hash_t* usuarios;             // Clave: nick de usuario, Dato: usuario_t
  char* usuario_loggeado;       // Nick de usuario
  hash_t* posts_publicados;     // Clave: id             , Dato: post_t
  size_t cantidad_posts;        // Siguiente id
};


// Recibe el sistema
// Devuelve true si hay usuario loggeado, false caso contrario
bool sistema_usuario_loggeado(sistema_t* sistema) {
  return (sistema->usuario_loggeado != NULL);
}

// Recibe el sistema y el usuario que quiere ingresar
// Devuelve por stdout un mensaje con respecto al estado del usuario:
// si el usuario no existe, o del sistema: Si hay o no un usuario loggeado
// Guarda en el sistema el usuario logeado, o sino devuelve error por pantalla
void sistema_login(sistema_t* sistema, char* user){
  if (sistema_usuario_loggeado(sistema)) {
    fprintf(stdout, "Error: Ya habia un usuario loggeado\n");
    return;
  }  
  if (!hash_pertenece(sistema->usuarios, user)) {               // Buscar en Hash es O(1)
    fprintf(stdout, "Error: usuario no existente\n");
    return;
  }
  sistema->usuario_loggeado = malloc(sizeof(char) * strlen(user) + 1);
  strcpy(sistema->usuario_loggeado, user);
  fprintf(stdout, "Hola %s\n", user);
  return;
}

// Recibe el sistema
// De haber usuario logeado, lo borra del sistema
// De no haberlo, devuelve error por pantalla
void sistema_logout(sistema_t* sistema) {
  if (sistema_usuario_loggeado(sistema)) {                      // O(1)
    free(sistema->usuario_loggeado);
    sistema->usuario_loggeado = NULL;
    fprintf(stdout, "Adios\n");
    return;
  }
  fprintf(stdout, "Error: no habia usuario loggeado\n");
  return;  
}


// Recibe sistema y el contenido del post
// De no haber usuario logeado, devuelve error por pantalla
// Caso contrario, crea el post y lo encola en el feed de todos los usuarios
void sistema_publicar_post(sistema_t* sistema, char* texto_post) {
  if(!sistema_usuario_loggeado(sistema)) {
    fprintf(stdout, "Error: no habia usuario loggeado\n");
    return;
  }
  post_t* nuevo_post = post_crear(texto_post, sistema->cantidad_posts, sistema->usuario_loggeado);
  if (!nuevo_post) {
    fprintf(stdout, "Error: no se pudo crear el nuevo post\n");
    return;
  }
  sistema->cantidad_posts++;
  int tamano = snprintf( NULL, 0, "%ld", post_obtener_id(nuevo_post) );
  char* nuevo_id = malloc( tamano + 1 );
  snprintf( nuevo_id, tamano + 1, "%ld", post_obtener_id(nuevo_post) );
  sprintf(nuevo_id, "%zu", post_obtener_id(nuevo_post));
  hash_guardar(sistema->posts_publicados, nuevo_id, nuevo_post);
  free(nuevo_id);
  usuario_t* autor = hash_obtener(sistema->usuarios, sistema->usuario_loggeado);
  hash_iter_t* iter = hash_iter_crear(sistema->usuarios);
  while (!hash_iter_al_final(iter)) {             // Se repite u veces, u:usuarios
    const char* clave = hash_iter_ver_actual(iter);
    if (strcmp(clave, sistema->usuario_loggeado) == 0) {
      hash_iter_avanzar(iter);
      continue;
    }
    usuario_t* usuario = hash_obtener(sistema->usuarios, clave);  // Buscar en hash es O(1)
    usuario_agregar_feed(nuevo_post, autor, usuario);             // Encolar en heap es O(log(p)) p: posts
    hash_iter_avanzar(iter);
  }                                                               // Total: O(u log(p))
  hash_iter_destruir(iter);
  printf("Post publicado\n");
  return;
}

// Recibe el sistema
// Desencola del feed del usuario logeado el proximo post
// De no haber usuario logeado, devuelve error por pantalla
// Imprime por pantalla toda la informacion del post desencolado
void sistema_ver_prox_post(sistema_t* sistema) {
  if (sistema_usuario_loggeado(sistema)) {
    usuario_t* usuario = hash_obtener(sistema->usuarios, sistema->usuario_loggeado);
    if(!usuario_feed_vacio(usuario)){
      usuario_imprimir_feed(usuario);                           // Desencolar del heap es O(log(p)) p: posts
      return;
    }
  }
  fprintf(stdout, "Usuario no loggeado o no hay mas posts para ver\n");
  return;
}

// Recibe el sistema y el id del post a likear
// El usuario de no haber likeado antes el post,
// se agrega su like en el abb de likes del post objetivo
// De no haber usuario logeado o no existir tal post, devuelve error por pantalla
void sistema_likear_post(sistema_t* sistema, char* id){
  if (!sistema_usuario_loggeado(sistema) || hash_cantidad(sistema->posts_publicados) == 0 || !hash_pertenece(sistema->posts_publicados, id)) {
    fprintf(stdout, "Error: Usuario no loggeado o Post inexistente\n");
    return;
  }
  post_t* post = hash_obtener(sistema->posts_publicados, id);
  usuario_t* usuario = hash_obtener(sistema->usuarios, sistema->usuario_loggeado);
  usuario_likear_post(usuario, post, id);                                 // Guardar en ABB es O(log(u)) u:usuarios
  fprintf(stdout, "Post likeado\n");
  return;
}

// Recibe el sistema y el id del post a ubicar
// Recorre e imprime el abb de likes del post
// De no existir post o no tener likes, devuelve error por pantalla
void sistema_ver_likes(sistema_t* sistema, char* id) {
  post_t* post = hash_obtener(sistema->posts_publicados, id);
  if (!post || post_obtener_cantidad_likes(post) == 0) {
    fprintf(stdout, "Error: Post inexistente o sin likes\n");
    return;
  }
  fprintf(stdout, "El post tiene %li likes:\n", post_obtener_cantidad_likes(post));
  post_imprimir_likes(post);                                             // Recorrer todo el ABB es O(u) u: usuarios
  return;
}


void sistema_orden(sistema_t* sistema, char* orden, FILE* archivo) {
  char* linea = NULL;
  size_t tam = 0;
  size_t result = 1;
  if (strcmp(orden,"login\n") == 0) {
    result = getline(&linea, &tam, archivo);
    if (result > 0) {
      char* nueva_linea = deshacer_enter(linea);
      sistema_login(sistema, nueva_linea);
      free(nueva_linea);
    }
    free(linea);
  } else if (strcmp(orden,"logout\n") == 0) {
    sistema_logout(sistema);
  } else if (strcmp(orden,"publicar\n") == 0) {
    result = getline(&linea, &tam, archivo);
    if (result > 0) {
      char* nueva_linea = deshacer_enter(linea);
      sistema_publicar_post(sistema, nueva_linea);
      free(nueva_linea);
    }
    free(linea);
  } else if (strcmp(orden,"ver_siguiente_feed\n") == 0) {
    sistema_ver_prox_post(sistema);
  } else if (strcmp(orden,"likear_post\n") == 0) {
    result = getline(&linea, &tam, archivo);
    if (result > 0) {
      char* nueva_linea = deshacer_enter(linea);
      sistema_likear_post(sistema,nueva_linea);
      free(nueva_linea);
    }
    free(linea);
  } else if (strcmp(orden,"mostrar_likes\n") == 0) {
    result = getline(&linea, &tam, archivo);
    if (result > 0) {
      char* nueva_linea = deshacer_enter(linea);
      sistema_ver_likes(sistema,nueva_linea);
      free(nueva_linea);
    }
    free(linea);
  }
  if (result < 0) sistema_destruir(sistema);
  return;
}

// Recibe el archivo de texto de entrada con los usuarios
// Recopila los usuarios con sus posiciones e inicializa usuario_t
// Devuelve un hash con clave el nick y la clave usuario_t
hash_t* sistema_crear_usuarios(FILE* archivo) {
  hash_t* hash = hash_crear(wrapper_usuario_destruir);
  if (!hash) return NULL;
  char* linea = NULL;
  size_t tam = 0;
  size_t pos = 1;
  while (getline(&linea, &tam, archivo) > 0) {
    char* nueva_linea = deshacer_enter(linea);
    usuario_t* usuario = usuario_crear(nueva_linea, pos);
    hash_guardar(hash,nueva_linea,usuario);
    pos++;
  }
  free(linea);
  return hash;
}

sistema_t* sistema_crear(FILE* archivo) {
  sistema_t* sistema = malloc(sizeof(sistema_t));
  sistema->cantidad_posts = 0;
  sistema->usuario_loggeado = NULL;
  sistema->posts_publicados = hash_crear(wrapper_post_destruir);
  sistema->usuarios = sistema_crear_usuarios(archivo);
  return sistema;
}

void sistema_destruir(sistema_t* sistema) {
  hash_destruir(sistema->usuarios);
  hash_destruir(sistema->posts_publicados);
  free(sistema->usuario_loggeado);
  free(sistema);
}