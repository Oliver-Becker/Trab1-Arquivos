#ifndef _REGISTRO_H_
#define _REGISTRO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_DATA 10
#define TAMANHO_CABECALHO 5
#define TAMANHO_REGISTRO 112
#define BYTE_OFFSET(RRN) ((4 + TAMANHO_REGISTRO) * RRN) + TAMANHO_CABECALHO
#define DATA_VAZIA "0000000000"
#define CAMPO_VAZIO ""

typedef struct reg REGISTRO;

struct reg {
	int codEscola;
	char dataInicio[TAMANHO_DATA + 1];
	char dataFinal[TAMANHO_DATA + 1];
	int tamNome;
	char *nomeEscola;
	int tamMunicipio;
	char *municipio;
	int tamEndereco;
	char *endereco;
};

REGISTRO* InsereCamposEmRegistro(char* campo[]);
int AcrescentaRegistroNoFinal(char* nomeArquivo, REGISTRO* registro);
int ComparaCampoDoRegistro(REGISTRO* registro, char* nomeDoCampo, char* valor);
void LiberaRegistro(REGISTRO* registro);
int ImprimeRegistro(REGISTRO* registro);

#endif
