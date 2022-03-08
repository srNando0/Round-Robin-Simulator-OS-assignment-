/*
	Declaracao de bibliotecas para uso
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "essenciais\fila.c"
#include "essenciais\processo.c"



/*
	Constantes
*/
#define MAX_PROCESSOS					1024
#define FATIA_DE_TEMPO_ALTA_PRIORIDADE	2
#define FATIA_DE_TEMPO_BAIXA_PRIORIDADE	4
#define TEMPO_FITA_MAGNETICA			2
#define TEMPO_DISCO						4
#define TEMPO_IMPRESSORA				1



/*
	Variaveis globais
*/
// Filas globais do simulador
Fila * filaAlta, * filaBaixa, * filaDisco, * filaFitaMagnetica, * filaImpressora;

// Fila de processos lidos
Fila * filaProcessosLidos;

// Variaveis de simulacao
int roundRobin;
Processo * processoCPU;



/*
	Prototipos
*/
void filasGlobaisConstrutor();
void filasGlobaisDesconstrutor();

void leProcessos();
void escreveProcessos();
void simula();
void itera();
int existeProcessoVivo();
//void relogioGlobalConstrutor();
/*char interrompedor();
void receber_input_e_criar_processo();
void executar_cpu();
bool executar_fila(Fila * fila, int prioridade);
void executar();*/


/*
	Funcoes auxiliares
*/
// Instancia as filas globais
void filasGlobaisConstrutor() {
	filaAlta			= filaConstrutor();
	filaBaixa			= filaConstrutor();
	filaDisco			= filaConstrutor();
	filaFitaMagnetica	= filaConstrutor();
	filaImpressora		= filaConstrutor();
	
	filaProcessosLidos	= filaConstrutor();
}



// Libera memoria das instancias globais de Fila
void filasGlobaisDesconstrutor() {
	filaDesconstrutor(filaAlta);
	filaDesconstrutor(filaBaixa);
	filaDesconstrutor(filaDisco);
	filaDesconstrutor(filaFitaMagnetica);
	filaDesconstrutor(filaImpressora);
	
	filaDesconstrutor(filaProcessosLidos);
}



// Instancia o relogio global
/*void relogioGlobalConstrutor() {
	relogio = relogioConstrutor();
}*/



int main() {
	filasGlobaisConstrutor();
	leProcessos();
	escreveProcessos();
	
	simula();
	
	filasGlobaisDesconstrutor();
	
	return 0;
}



/*
(numero de processos)
(ppid) (tempo de chegada) (tempo de servico principal) (numero de tempos de io)
(tipo de io) (tempo de servico)
...
(tipo de io) (tempo de servico)
.
.
.
(ppid) (tempo de chegada) (tempo de servico principal) (numero de tempos de io)
(tipo de io) (tempo de servico)
...
(tipo de io) (tempo de servico)
*/
// Le os processos descritos no arquivo de entrada
void leProcessos() {
	/*
		Inicializacao
	*/
	// numero de processos a serem lidos
	int numeroProcessos;
	
	// le o numero de processos do arquivo de entrada
	scanf("%d", &numeroProcessos);
	
	// variaveis para verificar se os tempos de chegada estao ordenados corretamente no arquivo de entrada
	int tempoDeChegadaAntigo = 0, pidAntigo = 0;
	
	
	
	/*
		Leitura
	*/
	// cria cada processo do arquivo de entrada
	int pid; // variavel auxiliar na verificacao de erro
	for(pid = 1; pid <= numeroProcessos; pid++) {
		/*
			Variaveis do processo novo
		*/
		// variavel do processo novo
		Processo * processo;
		
		// informacoes de simulacao e informacoes do PBC
		int ppid,			// PID do processo parente que o criou
			tempoDeChegada,	// tempo onde o processo é criado
			tempoDeServico,	// quantidade do primeiro tempo que o processo vai demorar na CPU
			numeroIO;		// quantidade de tempos de I/O intercalados com CPU
		
		// le o processo do arquivo de entrada
		scanf("%d %d %d %d", &ppid, &tempoDeChegada, &tempoDeServico, &numeroIO);
		
		
		
		/*
			Verificacao de erro
		*/
		// verifica se os processos descritos no arquivo de entrada estao ordenados
		if(tempoDeChegada < tempoDeChegadaAntigo) {
			printf(
				"Erro no tempo de chegada do processo %d! "
				"O processo %d vem antes, mas seu tempo de chegada eh %d.",
				pid, pidAntigo, tempoDeChegadaAntigo);
			exit(-1);
		}
		
		// verifica se o ppid esta coerente
		if(pid < ppid) {
			printf(
				"Erro no ppid do processo %d! O processo %d nao existe ainda.",
				pid, ppid);
			exit(-1);
		}
		
		// atualiza as variaveis de verificacao de ordenacao
		tempoDeChegadaAntigo = tempoDeChegada;
		pidAntigo = pid;
		
		
		
		/*
			Criacao do processo novo
		*/
		// cria o processo novo
		processo = criaProcesso(ppid, tempoDeChegada, tempoDeServico);
		
		// insere o processo na fila de processos lidos
		filaPush(filaProcessosLidos, (void *) processo);
		
		
		
		/*
			Leitura de tempos de I/O
		*/
		// Testa se vai ter tempo de I/O
		if (0 < numeroIO) {	// se tiver, insere os tempos de I/O na fila de I/O do processo
			int io;
			for (io = 1; io <= numeroIO; io++) {
				char tipoIO;		// tipo de I/O
				int tempoDeServicoIO;	// tempo de servico
				
				// le as informacoes
				scanf(" %c %d", &tipoIO, &tempoDeServicoIO);
				
				// testa char
				if(tipoIO != 'D' && tipoIO != 'F' && tipoIO != 'I') {
					printf(
						"Erro no tipo do I/O %d do processo %d! O tipo %c nao existe.\n"
						"Os tipos de I/O sao:\n"
						"disco (D)\n"
						"fita magnetica (F)\n"
						"impressora (I)\n",
					io, pid, tipoIO);
					exit(-1);
				}
				
				// insere o tempo de I/O
				pushProcessoIO(processo, tipoIO, tempoDeServicoIO);
			}
		}
	}
}



// escreve os processos da fila e libera memoria (apenas para teste)
void escreveProcessos() {
	FilaNo * filaNo, * filaNoIO, * filaNoTempoDeServico;
	Processo * processo;
	
	filaNo = filaProcessosLidos->cabeca;
	while(filaNo != NULL) {
		processo = (Processo *) filaNo->elemento;
		printf(
			"pid: %d, ppid: %d, prioridade: %c, estado: %c\n"
			"tempo de chegada: %d, tempo de servico principal: %d.\n",
			processo->pid, processo->ppid, processo->prioridade, processo->estado,
			processo->tempoDeChegada, processo->tempoDeServico);
		
		char tipoIO;
		int tempoDeServico;
		
		filaNoIO 			 = processo->filaIO->cabeca;
		filaNoTempoDeServico = processo->filaTempoDeServico->cabeca;
		while(filaNoIO != NULL) {
			tipoIO 				 = (char) filaNoIO->elemento;
			tempoDeServico = (int) filaNoTempoDeServico->elemento;
			
			printf("tipo de I/O: %c, tempo de servico: %d\n", tipoIO, tempoDeServico);
			
			filaNoIO 			 = filaNoIO->proximo;
			filaNoTempoDeServico = filaNoTempoDeServico->proximo;
		}
		
		printf("\n//------//\n\n");
		
		filaNo = filaNo->proximo;
	}
}



void simula() {
	/*
		Inicio
	*/
	// variaveis de simulacao
	int	deltaTempoFitaMagnetica = 0,
		deltaTempoImpressora    = 0,
		deltaTempoDisco 		= 0;
	
	// Printa os PIDs de cada processo criado
	int pid;
	for(pid = 1; pid <= filaProcessosLidos->numero; pid++) {
		printf("  %d   ", pid);
	}
	printf("\n");
	
	// variaveis auxiliadoras de simulacao
	int tempoAtual = 0;				// quantum atual da simulacao
	
	/*
		Simulacao
	*/
	roundRobin = 0;
	processoCPU = NULL;
	while(existeProcessoVivo()) {
		/*
			Checando se há algum processo em uma fila de I/O para ser trazido para
			uma fila de CPU, ou se há algum processo cujo tempo de entrada é igual
			ao tempo atual.
		*/			
		// Primeiro, vamos checar se há algum processo cujo tempo de entrada é igual
		// ao tempo atual. Nesse caso, vamos adicioná-lo à lista de alta prioridade.
		FilaNo * filaNo = filaProcessosLidos->cabeca;
		Processo * processo;		
		// itera por todos os processos a partir dos nos
		while (filaNo != NULL) {
			processo = (Processo *) filaNo->elemento;
			if(processo->tempoDeChegada == tempoAtual) {
				processo->prioridade = PRIORIDADE_ALTA;
				processo->fila       = FILA_ALTA;
				processo->estado     = ESTADO_PRONTO;
				filaPush(filaAlta, (void *) processo);
			}
			filaNo   = filaNo->proximo;
		}
		
		// Vamos checar a fila de Disco
		if(deltaTempoDisco == TEMPO_DISCO) {
			deltaTempoDisco = 0;
			Processo * processoDisco = (Processo *) filaPop(filaDisco);
			if(processoDisco != NULL) {
				processoDisco->fila  	  = FILA_BAIXA;
				processoDisco->estado     = ESTADO_PRONTO;
				processoDisco->prioridade = PRIORIDADE_BAIXA;
				
				filaPush(filaBaixa, (void *) processoDisco);
			} 			
		}
		// Agora vamos checar a fila de Fita Magnética
		if(deltaTempoFitaMagnetica == TEMPO_FITA_MAGNETICA) {
			deltaTempoFitaMagnetica = 0;
			Processo * processoFita = (Processo *) filaPop(filaFitaMagnetica);
			if(processoFita != NULL) { 
				processoFita->fila		 = FILA_ALTA;
				processoFita->estado	 = ESTADO_PRONTO;
				processoFita->prioridade = PRIORIDADE_ALTA;
				filaPush(filaAlta, (void *) processoFita); 
			} 			
		}		
		// Agora vamos checar a fila de Impressora
		if(deltaTempoImpressora == TEMPO_IMPRESSORA) {
			deltaTempoImpressora = 0;
			Processo * processoImpressora = (Processo *) filaPop(filaImpressora);
			if(processoImpressora != NULL) { 
				processoImpressora->fila       = FILA_ALTA;
				processoImpressora->estado     = ESTADO_PRONTO;
				processoImpressora->prioridade = PRIORIDADE_ALTA;
				filaPush(filaAlta, (void *) processoImpressora); 
			} 			
		}	
		
		

		/*
			Executando uma iteração de uma unidade de tempo
			Printando o resultado da iteração atual na tela	
		*/
      		itera();

		/*
			Printando o resultado da iteração atual na tela.
		*/
		char fila, estado, prioridade;
		// pega o primeiro no da fila dos processos lidos
		filaNo = filaProcessosLidos->cabeca;
		// itera por todos os processos a partir dos nos
		while (filaNo != NULL) {
			processo = filaNo->elemento;
			
			fila =     processo->fila;
			estado = processo->estado;
			prioridade = processo->prioridade;
			
			switch(estado) {
				case ESTADO_EXECUCAO:
					printf("|%c%c%c| ", estado, fila, prioridade);
					break;
				case ESTADO_BLOQUEADO:
					printf(":%c%c%c: ", estado, fila, prioridade);
					break;
				default:
					printf(" %c%c%c  ", estado, fila, prioridade);
			}
			
			filaNo = filaNo->proximo;
		}
		
		printf("\n");

		// Incrementando o tempo
		tempoAtual++;
		if(!filaVazia(filaDisco)) 		  {deltaTempoDisco++;}
		if(!filaVazia(filaImpressora)) 	  {deltaTempoImpressora++;}
		if(!filaVazia(filaFitaMagnetica)) {deltaTempoFitaMagnetica++;}
	}
}



void itera() {
	/*
		Scheduling
	*/
	if(roundRobin == 0) {
		/*
			Devolve processo para a fila
		*/
		if(processoCPU != NULL) {
			// se um CPU-burst for terminado
			if(processoCPU->tempoDeServico == 0) {
				// se nao tem I/O-burst
				if(filaVazia(processoCPU->filaIO)) {
					processoCPU->fila       = FILA_NENHUMA;
					processoCPU->estado     = ESTADO_SAIDA;
					processoCPU->prioridade = PRIORIDADE_NENHUMA;
				} else {
					// se tem, coloca o processo na lista de I/O
					char tipoIO;
					int tempoDeServico;
					
					popProcessoIO(processoCPU, &tipoIO, &tempoDeServico);
					
					processoCPU->tempoDeServico = tempoDeServico;
					processoCPU->estado = ESTADO_BLOQUEADO;
					switch(tipoIO) {
						case TIPO_DISCO:
							processoCPU->fila = FILA_DISCO;
							filaPush(filaDisco, (void *) processoCPU);
							break;
						case TIPO_FITAMAGNETICA:
							processoCPU->fila = FILA_FITAMAGNETICA;
							filaPush(filaFitaMagnetica, (void *) processoCPU);
							break;
						case TIPO_IMPRESSORA:
							processoCPU->fila = FILA_IMPRESSORA;
							filaPush(filaImpressora, (void *) processoCPU);
							break;
						default:
							printf("Erro! Tipo %c de I/O nao existe.\n", tipoIO);
							exit(-1);
					}
				}
			} else {
				// se o CPU-burst nao terminou
				processoCPU->estado = ESTADO_PRONTO;
				processoCPU->fila = FILA_BAIXA;
				processoCPU->prioridade = PRIORIDADE_BAIXA;
				filaPush(filaBaixa, (void *) processoCPU);
			}
			
			processoCPU = NULL;
		}
		
		
		
		/*
			Pega um novo processo
		*/
		// se a fila de alta prioridade nao estiver vazia
		if(!filaVazia(filaAlta)) {
			roundRobin = FATIA_DE_TEMPO_ALTA_PRIORIDADE;
			processoCPU = (Processo *) filaPop(filaAlta);
			
			processoCPU->fila = FILA_NENHUMA;
			processoCPU->estado = ESTADO_EXECUCAO;
		} else if(!filaVazia(filaBaixa)) {
			roundRobin = FATIA_DE_TEMPO_BAIXA_PRIORIDADE;
			processoCPU = (Processo *) filaPop(filaBaixa);
			
			processoCPU->fila = FILA_NENHUMA;
			processoCPU->estado = ESTADO_EXECUCAO;
		}
	}
	
	
	
	/*
		Execucao
	*/
	// se tiver processo executando
	if(processoCPU != NULL) {
		roundRobin--;
		processoCPU->tempoDeServico--;
		
		if(processoCPU->tempoDeServico == 0) { roundRobin = 0; }
	}
}



int existeProcessoVivo() {
	FilaNo * filaNo = filaProcessosLidos->cabeca;
	Processo * processo;
	
	while (filaNo != NULL) {
		processo = filaNo->elemento;
		if(processo->estado != ESTADO_SAIDA) { return true; }
		
		// atualiza para o proximo processo da lista
		filaNo = filaNo->proximo;
	}
	
	return false;
}