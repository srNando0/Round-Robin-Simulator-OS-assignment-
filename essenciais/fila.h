#ifndef FILA_H
#define FILA_H



typedef struct str_FilaNo FilaNo;
typedef struct str_Fila Fila;

Fila * filaConstrutor();
void filaDesconstrutor(Fila * fila);
void filaPush(Fila * fila, void * elemento);
void * filaPop(Fila * fila);
void * filaPrimeiro(Fila * fila);
int filaVazia(Fila * fila);



#endif