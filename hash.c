#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum estados {VACIO='V', BORRADO='B', OCUPADO='O'};

#define CAPACIDAD_INICIAL 127
#define FACTOR_CARGA_MAXIMO 0.7
#define FACTOR_MIN 4
#define VALOR_DE_REDIMENSION 2

/*Defino struct tabla_hash*/
typedef struct campo_hash{
    char* clave;
    void* dato;
    char estado; //V:vacio, O:ocupado, B:borrado
}campo_hash_t;

/* Definicion del struct hash */
struct hash {
    campo_hash_t*tabla;
    size_t capacidad;
    size_t cantidad_elem;
    size_t cantidad_borrados;
    hash_destruir_dato_t funcion_destruir;
};

/* Definicion del struct hash_iter */
struct hash_iter {
  const hash_t* hash;
  size_t posicion; 
};

/* ******************************************************************
 *                 PRIMITIVAS DEL STRUCT AUXILIAR
 * *****************************************************************/

//FNV hash 
size_t hashear_clave(const char* clave, size_t tamanio_tabla) {
  size_t hash = 146959810537;
  for(size_t i = 0; i < strlen(clave); i++){
    hash *= 1099511628211;
    hash ^= clave[i];
  }
  return hash%tamanio_tabla;
}

// Crea una copia de la clave
// Recibe el puntero a la clave a copiar
// Devuelve la copia  de la clave
char* copiar_clave(const char*clave) {
  char* copia_clave = malloc(sizeof(char)*(strlen(clave)+1));
  if (!copia_clave) {
    return NULL;
  }
  strcpy(copia_clave,clave);
  return copia_clave;
}

// Crea una tabla del hash vacia
// Recibe el tamano deseado para la tabla
// Devuelve la tabla vacia
campo_hash_t *tabla_hash_crear(size_t capacidad) {
  campo_hash_t* tabla_hash = calloc(capacidad, sizeof(campo_hash_t));
  if (!tabla_hash) {
    return NULL;
  }
  for (size_t i=0; i < capacidad; i++) {
    tabla_hash[i].estado = VACIO;
    tabla_hash[i].clave = NULL;
    tabla_hash[i].dato = NULL;
  }
  return tabla_hash;
}

// Realiza una busqueda de la clave en el hash
// Recibe el hash y la clave
// Devuelve la posicion en la que se encuentra la clave,
// o el estado de la posicion actual esta vacia en caso de no encontrar la clave
size_t hash_busqueda(const campo_hash_t *tabla_hash, const char *clave, size_t capacidad) {
  size_t i = hashear_clave(clave, capacidad);
  bool pertenece = false;
  while (!pertenece && tabla_hash[i].estado != VACIO) {
    campo_hash_t campo = tabla_hash[i];
    if (campo.estado == OCUPADO && !strcmp(campo.clave,clave)) {
      pertenece = true;
    } else {
      if (i+1 == capacidad) {
        i = 0;
      } else {
        i++;
      }
    }
  }
  return i;
}

// Verifica la necesidad de redimensionar el hash
// Recibe el hash
// Devuelve true en caso de ser necesaria una redimension
bool hash_necesita_redimension(hash_t *hash) {
  return(((hash_cantidad(hash)+hash->cantidad_borrados)/hash->capacidad) >= FACTOR_CARGA_MAXIMO
        || (hash_cantidad(hash)*FACTOR_MIN <= hash->capacidad && hash->capacidad/VALOR_DE_REDIMENSION >= CAPACIDAD_INICIAL));
}

// Redimensiona el hash en una nueva capacidad
// Recibe el hash y la nueva capacidad
// Devuelve true si fue exitosa la redimension, o false en caso de fallar
bool hash_redimensionar (hash_t *hash, size_t nueva_capacidad) {
  campo_hash_t *nueva_tabla = tabla_hash_crear(nueva_capacidad);
  if (!nueva_tabla) {
    return false;
  }
  for (size_t i=0; i < hash->capacidad; i++) {
    if (hash->tabla[i].estado == OCUPADO) {
      size_t posicion_nueva = hash_busqueda(nueva_tabla,hash->tabla[i].clave,nueva_capacidad);
      nueva_tabla[posicion_nueva].clave=hash->tabla[i].clave;
      nueva_tabla[posicion_nueva].dato=hash->tabla[i].dato;
      nueva_tabla[posicion_nueva].estado=OCUPADO;
    }
  }
  free(hash->tabla);
  hash->tabla = nueva_tabla;
  hash->capacidad = nueva_capacidad;
  hash->cantidad_borrados = 0;
  return true;
}

/* ******************************************************************
 *                 PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
  hash_t* hash = malloc(sizeof(hash_t));
  if(!hash){
    return NULL;
  }
  hash->capacidad = CAPACIDAD_INICIAL;
  hash->tabla = tabla_hash_crear(hash->capacidad);
  if (!hash->tabla){
    free(hash);
    return NULL;
  }
  hash->cantidad_elem = 0;
  hash->cantidad_borrados = 0;
  hash->funcion_destruir = destruir_dato;
  return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
  if(hash_necesita_redimension(hash)){
    if (!hash_redimensionar(hash, hash->capacidad*VALOR_DE_REDIMENSION)) {
      return false;
    }
  }
  size_t i = hash_busqueda(hash->tabla,clave,hash->capacidad);
  if (hash->tabla[i].estado == VACIO) {
    char* clave_copiada = copiar_clave(clave);
    if (!clave_copiada) {
      return false;
    }
    hash->tabla[i].clave = clave_copiada;
    hash->tabla[i].estado = OCUPADO;
    hash->cantidad_elem++;
  } else {
    if (hash->funcion_destruir) {
      hash->funcion_destruir(hash->tabla[i].dato);
    }
  }
  hash->tabla[i].dato = dato;
  return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {
  size_t i = hash_busqueda(hash->tabla,clave,hash->capacidad);
  if (hash->tabla[i].estado == OCUPADO) {
    void* dato_borrado = hash->tabla[i].dato;
    free(hash->tabla[i].clave);
    hash->tabla[i].estado = BORRADO;
    hash->cantidad_elem--;
    hash->cantidad_borrados++;
    if(hash_necesita_redimension(hash)){
      if (!hash_redimensionar(hash, hash->capacidad/VALOR_DE_REDIMENSION)) {
        return NULL;
      }
    }
    return dato_borrado;
  }
  return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave) {
  size_t i = hash_busqueda(hash->tabla,clave,hash->capacidad);
  if (hash->tabla[i].estado == OCUPADO) {
    return hash->tabla[i].dato;
  }

  return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
  size_t posicion = hash_busqueda(hash->tabla,clave,hash->capacidad);
  return (hash->tabla[posicion].estado==OCUPADO);
}

size_t hash_cantidad(const hash_t *hash){
  return (hash->cantidad_elem);
}

void hash_destruir(hash_t *hash) {
  for (size_t i=0; i < hash->capacidad; i++) {
    if (hash->tabla[i].estado == OCUPADO) {
      free(hash->tabla[i].clave);
      if (hash->funcion_destruir) {
        hash->funcion_destruir(hash->tabla[i].dato);
      }
    }
  }
  free(hash->tabla);
  free(hash);
}

/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
  hash_iter_t *iter= malloc(sizeof(hash_iter_t));
  if (!iter) {
    return NULL;
  }
  iter->hash = hash;
  iter->posicion = -1;
  hash_iter_avanzar(iter);
  return iter;
}

bool hash_iter_al_final(const hash_iter_t *iter){
  return ((iter->posicion == iter->hash->capacidad) || (iter->hash->cantidad_elem==0));
}

bool hash_iter_avanzar(hash_iter_t *iter){
  if (hash_iter_al_final(iter)) {
    return false;
  }
  iter->posicion++;
  while (!hash_iter_al_final(iter)) {
    if (iter->hash->tabla[iter->posicion].estado == OCUPADO) {
      return true;
    }
    iter->posicion++;
  }
  return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
  if (!hash_iter_al_final(iter)) {
    return iter->hash->tabla[iter->posicion].clave;
  }
  return NULL;
}

void hash_iter_destruir(hash_iter_t *iter){
  free(iter);
}