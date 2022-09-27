#include "sistema.h"
#include <stdbool.h>

#define RUTA 1
#define ARGUMENTOS_OBJETIVOS 2

// Valida los argumentos del main
// Devuelve true en caso de ser validos y false en caso contrario
bool valid_argument(int argc,char **argv) {
  if (argc != ARGUMENTOS_OBJETIVOS) {
    return false;
  }
  return true;
}

// Inicializa el archivo y verifica la ruta del archivo 
FILE* valid_src(const char* ruta_archivo) {
  FILE *archivo;
  if ((archivo = fopen(ruta_archivo,"r")) == NULL) {
    fprintf(stderr,"Error: archivo fuente inaccesible");
  }
  return archivo;
}

int main(int argc, char *argv[]) {
  if (!valid_argument(argc, argv)) {
    fprintf(stderr,"Error: Cantidad erronea de parametros");
    return 1;
  }
  const char* ruta_archivo = argv[RUTA];
  FILE* archivo = valid_src(ruta_archivo);
  sistema_t* sistema = sistema_crear(archivo);
  char* linea = NULL;
  size_t tam = 0;
  FILE* entrada = stdin;
  while ((getline(&linea, &tam, entrada)) > 0) {
    sistema_orden(sistema, linea, entrada);
  }
  free(linea);
  fclose(archivo);
  sistema_destruir(sistema);
  return 0;
}
