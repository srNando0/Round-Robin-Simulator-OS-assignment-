#include <stdlib.h>
#include "fila.h"



// No da fila
struct str_FilaNo {
	void * elemento;				// elemento armazenado
	struct str_FilaNo * proximo;	// ponteiro para o proximo no
};

// Fila usando lista encadeada
struct str_Fila {
	struct str_FilaNo * cabeca;		// no cabeca da lista
	struct str_FilaNo * rabo;		// no rabo da lista
	int numero;						// numero de elementos da fila
};



// Construtor de Fila
Fila * filaConstrutor() {
	// cria a fila
	Fila * fila = (Fila *) malloc(sizeof(Fila));
	
	// inicia a fila vazia
	fila->cabeca = NULL;
	fila->rabo = NULL;
	fila->numero = 0;
	
	// retorna a fila criada
	return fila;
}

// Desconstrutor de Fila
void filaDesconstrutor(Fila * fila) {
	// remove todos os elementos da fila
	while(0 < fila->numero) { filaPop(fila); }
	
	// desaloca memoria
	free(fila);
}


// Metodo push de fila
void filaPush(Fila * fila, void * elemento) {
	// cria no
	FilaNo * no = (FilaNo *) malloc(sizeof(FilaNo));
	
	// inicia o no
	no->elemento = elemento;	// elemento do no
	no->proximo = NULL;			// final da fila
	
	if(fila->numero == 0) {
		// caso da fila vazia
		fila->cabeca = no;
		fila->rabo = no;
	} else {
		// caso da fila não vazia
		fila->rabo->proximo = no;	// rabo antigo ---> rabo novo
		fila->rabo = no;			// rabo da fila = rabo novo
	}
	
	// incrementa o numero de elementos
	fila->numero++;
}



// Metodo pop de fila
void * filaPop(Fila * fila) {
	// caso de fila vazia
	if(fila->numero == 0) { return NULL; }	// retorna nada
	
	// caso de fila nao vazia
	FilaNo * cabeca = fila->cabeca;	// cabeca da lista
	void * elemento = cabeca->elemento;			// primeiro elemento da fila
	
	// fazendo um bypass no primeiro no da lista
	fila->cabeca = cabeca->proximo;
	
	// desalocando memoria
	free(cabeca);
	
	// decrementa o numero de elementos
	fila->numero--;
	
	// retorna o elemento
	return elemento;
}



// Retorna o primeiro elemento da fila
void * filaPrimeiro(Fila * fila) {
	return fila->cabeca->elemento;
}

int filaVazia(Fila * fila) {
	if(fila->numero == 0) {
		return true;
	} else {
		return false;
	}
}