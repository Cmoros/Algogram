#include "heap.h"
#include <stdlib.h>
#include <string.h>

struct heap {
    void **datos;
    size_t tam;
    size_t cant;
    cmp_func_t cmp;
};

void downheap(void* arr[],size_t pos_padre,size_t tam,cmp_func_t cmp) {
    size_t pos_h_izq = (2*pos_padre)+1;
    if (pos_h_izq>=tam) return;
    size_t pos_h_der = (2*pos_padre)+2;
    size_t maximo;
    if (pos_h_der<tam) {
        maximo = cmp(arr[pos_h_izq],arr[pos_h_der])>0?pos_h_izq:pos_h_der;
    } else {
        maximo = pos_h_izq;
    }
    if (cmp(arr[maximo],arr[pos_padre])>0) {
        void* aux = arr[maximo];
        arr[maximo] = arr[pos_padre];
        arr[pos_padre] = aux;
        downheap(arr,maximo,tam,cmp);
    }
    return;
}

void upheap(void* arr[],size_t pos_hijo,cmp_func_t cmp) {
    if (pos_hijo==0) return;
    size_t pos_padre = (pos_hijo-1)/2;
    if (cmp(arr[pos_hijo],arr[pos_padre])>0) {
        void* aux = arr[pos_hijo];
        arr[pos_hijo] = arr[pos_padre];
        arr[pos_padre] = aux;
        upheap(arr,pos_padre,cmp);
    }
    return;
}

void heapify(void* arr[],size_t tam,cmp_func_t cmp) {
    for (size_t i=1;i<=tam;i++) {
        downheap(arr,tam-i,tam,cmp);
    }
    return;
}

void heap_sort(void* arr[],size_t tam,cmp_func_t cmp) {
    heapify(arr,tam,cmp);
    for (size_t i=1;i<tam;i++) {
        void* aux = arr[0];
        arr[0] = arr[tam-i];
        arr[tam-i] = aux;
        downheap(arr,0,tam-i,cmp);
    }
    return;
}

bool heap_redimensionar(heap_t *heap, const size_t nuevo_tamano) {
    void** datos_nuevo = realloc(heap->datos,nuevo_tamano*sizeof(heap->datos));
    if (!datos_nuevo) return false;
    heap->datos=datos_nuevo;
    heap->tam=nuevo_tamano;
    return true;
}

heap_t *heap_crear(cmp_func_t cmp) {
    heap_t* heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    heap->datos = malloc(sizeof(heap->datos));
    if (!heap->datos) {
        free(heap);
        return NULL;
    }
    heap->tam=1;
    heap->cant=0;
    heap->cmp = cmp;
    return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp) {
    heap_t* heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;
    heap->datos = malloc(sizeof(void*)*2*n);
    if (!heap->datos) {
        free(heap);
        return NULL;
    }
    for (size_t i=0; i<n;i++) {
        heap->datos[i] = arreglo[i];
    }
    heap->cmp = cmp;
    heap->cant = n;
    heapify(heap->datos,heap->cant,heap->cmp);
    heap->tam = 2*n;
    return heap;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)) {
    if (destruir_elemento) {
        for (size_t i=0; i<heap->cant;i++) {
            destruir_elemento(heap->datos[i]);
        }
    }
    free(heap->datos);
    free(heap);
}


size_t heap_cantidad(const heap_t *heap) {
    return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap) {
    return heap->cant == 0;
}

bool heap_encolar(heap_t *heap, void *elem) {
    if (heap->cant == heap->tam) {
        if (!heap_redimensionar(heap,heap->tam*2)) return false;
    }
    heap->datos[heap->cant] = elem;
    upheap(heap->datos,heap->cant,heap->cmp);
    heap->cant++;
    return true;
}


void *heap_ver_max(const heap_t *heap) {
    if (heap->cant==0) return NULL;
    return heap->datos[0];
}

void *heap_desencolar(heap_t *heap) {
    if (heap->cant==0) return NULL;
    void* aux = heap->datos[0];
    heap->datos[0] = heap->datos[heap->cant-1];
    heap->datos[heap->cant-1] = aux;
    void* dato = heap->datos[heap->cant-1];
    heap->cant--;
    downheap(heap->datos,0,heap->cant,heap->cmp);
    if (heap->cant*4 <= heap->tam && heap->cant>0) {
        heap_redimensionar(heap,heap->tam/4);
    }
    return dato;
}