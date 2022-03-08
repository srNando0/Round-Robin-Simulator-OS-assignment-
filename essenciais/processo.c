#include <string.h>
#include "fila.h"
#include "processo.h"



// Struct de Processo
struct str_Processo {
	// Informacoes do PCB
	int pid,		// ID do processo
		ppid;		// PID do processo parente que o criou
	char
		prioridade,	// prioridade do processo
		estado;		// estado do processo
	
	// Informacoes de simulacao
	char fila;					// fila atual do processo
	int tempoDeChegada,			// tempo que o processo entra na fila principal
		tempoDeServico;			// tempo de servico principal
	Fila
		* filaIO,				// fila de tipos de I/O
		* filaTempoDeServico;	// fila de tempos de servico
};



// Cria um processo novo
Processo * criaProcesso(int ppid, int tempoDeChegada, int tempoDeServico) {
	// cria um processo novo
	Processo * processo = (Processo *) malloc(sizeof(Processo));
	
	// define as informacoes do PCB do processo novo
	processo->pid = proximoPID();
	processo->ppid = ppid;
	processo->prioridade = PRIORIDADE_NENHUMA;
	processo->estado = ESTADO_NOVO;
	
	// define as informacoes de simulacao do processo novo
	processo->fila = FILA_NENHUMA;
	processo->tempoDeChegada = tempoDeChegada;
	processo->tempoDeServico = tempoDeServico;
	processo->filaIO = filaConstrutor();
	processo->filaTempoDeServico = filaConstrutor();
	
	// retorna o processo novo
	return processo;
}



// Da push de um tempo de I/O na fila de I/O do processo
void pushProcessoIO(Processo * processo, char tipoIO, int tempoDeServico) {
	filaPush(processo->filaIO, (void *) tipoIO);
	filaPush(processo->filaTempoDeServico, (void *) tempoDeServico);
}



// Da pop de um tempo de I/O na fila de I/O do processo
int popProcessoIO(Processo * processo, char * tipoIO, int * tempoDeServico) {
	if (processo->filaIO->numero == 0) { return 0; }
	
	* tipoIO = (char) filaPop(processo->filaIO);
	* tempoDeServico = (int) filaPop(processo->filaTempoDeServico);
	
	return -1;
}



// Libera toda memoria alocada no Processo
void destroiProcesso(Processo * processo) {
	// libera filas
	filaDesconstrutor(processo->filaIO);
	filaDesconstrutor(processo->filaTempoDeServico);
	
	// libera o processo
    free(processo);
}



// retorna um PID novo e incrementa o contador global de PID
int proximoPID() { return pidGlobal++; }