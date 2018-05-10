#ifndef _ARQUIVO_H_
#define _ARQUIVO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void AlteraTopoDaPilha(char* nomeArquivo, int topoPilha);
int TopoDaPilha(char* nomeArquivo);
void AlteraStatusDoArquivo(char* nomeArquivo, char status);
int ConfereConsistenciaDoArquivo(char* nomeArquivo);
void CriaArquivoDeSaida(char* nomeArquivo);

#endif
