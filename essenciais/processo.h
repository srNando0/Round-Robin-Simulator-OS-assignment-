#ifndef PROCESSO_H
#define PROCESSO_H

#define ESTADO_NOVO			'N'
#define ESTADO_PRONTO		'P'
#define ESTADO_EXECUCAO		'E'
#define ESTADO_BLOQUEADO	'B'
#define ESTADO_SAIDA		'S'

#define FILA_NENHUMA		'N'
#define FILA_ALTA			'A'
#define FILA_BAIXA			'B'
#define FILA_DISCO			'D'
#define FILA_FITAMAGNETICA	'F'
#define FILA_IMPRESSORA		'I'

#define TIPO_DISCO			'D'
#define TIPO_FITAMAGNETICA	'F'
#define TIPO_IMPRESSORA		'I'

#define PRIORIDADE_NENHUMA	'N'
#define PRIORIDADE_BAIXA	'B'
#define PRIORIDADE_ALTA		'A'



int pidGlobal = 1;

typedef struct str_Processo Processo;

Processo * criaProcesso(int ppid, int tempoDeChegada, int tempoDeServico);
void pushProcessoIO(Processo * processo, char tipoIO, int tempoDeServico);
int popProcessoIO(Processo * processo, char * tipoIO, int * tempoDeServico);
void destroiProcesso(Processo * processo);
int proximoPID();



#endif