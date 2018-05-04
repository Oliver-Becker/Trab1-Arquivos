#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO_GERAL "Falha no carregamento do arquivo.\n"
#define ERRO_REGISTRO "Registro inexistente.\n"
#define ARQUIVO_SAIDA "saida.bin"

typedef struct {
	int codEscola;
	char dataInicio[11];
	char dataFinal[11];
	int tamNome;
	char *nomeEscola;
	int tamMunicipio;
	char *municipio;
	int tamEndereco;
	char *endereco;
} REGISTRO;

typedef struct {
	REGISTRO** vetRegistro;
	int numElementos;
} VETREGISTROS;

void FechaArquivoDeSaida(FILE* fp) {
	char status = 1;
	rewind(fp);
	fwrite(&status, sizeof(status), 1, fp);
	fclose(fp);
}

void CriaArquivoDeSaida() {
	FILE* fp = fopen(ARQUIVO_SAIDA, "wb");
	char status = 0;
	int topoPilha = -1;
	fwrite(&status, sizeof(status), 1, fp);
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp);
	FechaArquivoDeSaida(fp);
}

void InsereRegistro(REGISTRO* registro) {
}

void CriaArquivoDeSaida(VETREGISTROS* vetRegistros) {
}

VETREGISTROS* LeituraArquivoDeEntrada(char* nomeArquivo) {
	FILE* fp = fopen(nomeArquivo, "r");
	if (fp == NULL) {
		printf(ERRO_GERAL);
		return NULL;
	}

	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	REGISTRO* registro;

	int counter = 0;
	char* string[6];

	while (fscanf(fp, "%m[^;\n]", &string[0]) != EOF) {
		for (int i = 1; i < 6; ++i)
			fscanf(fp, "%*c%m[^;\n]", &string[i]);
		fgetc(fp);

		vetRegistros->vetRegistro = (REGISTRO**)realloc(vetRegistros->vetRegistro, 
				sizeof(REGISTRO*) * (++vetRegistros->numElementos));

		registro = (REGISTRO*)malloc(sizeof(REGISTRO));
/*		for (int i = 0; i < 6; ++i)
			printf(" %s |", string[i]);
		printf("\n");
*/
		registro->codEscola = atoi(string[0]);
		strcpy(registro->dataInicio, (string[4] != NULL) ? string[4] : "0000000000");
		strcpy(registro->dataFinal, (string[5] != NULL) ? string[5] : "0000000000");
		registro->tamNome = (string[1] != NULL) ? strlen(string[1]) : 0;
		registro->nomeEscola = string[1];
		registro->tamMunicipio = (string[2] != NULL) ? strlen(string[2]) : 0;
		registro->municipio = string[2];
		registro->tamEndereco = (string[3] != NULL) ? strlen(string[3]) : 0;
		registro->endereco = string[3];

		vetRegistros->vetRegistro[counter++] = registro;

		free(string[0]);
		if (string[4] != NULL) free(string[4]);
		if (string[5] != NULL) free(string[5]);
	}

	REGISTRO* r;
	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->vetRegistro[i];
		printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
			r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
			r->endereco);
	}
	//printf("tamanho do vetor de registros: %d\n", vetRegistros->numElementos);

// free do vetor de registros
/*	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->vetRegistro[i];
		if (r->nomeEscola != NULL) free(r->nomeEscola);
		if (r->municipio != NULL) free(r->municipio);
		if (r->endereco != NULL) free(r->endereco);
		free(r);
	}
	free(vetRegistros->vetRegistro);
	free(vetRegistros);
*/
	fclose(fp);

	return vetRegistros;
}

VETREGISTROS* RecuperaTodosRegistros() {
}

VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {
}

VETREGISTROS* RecuperaRegistroPorRRN(int RRN) {
}

void RemocaoLogicaPorRRN(int RRN) {
}

void InsereRegistroAdicional(char* campos[]) {
}

void AtualizaRegistroPorRRN(char* campos[], int RRN) {
}

void DesfragmentaArquivoDeDados() {
}

int* RecuperaRRNLogicamenteRemovidos() {
}

void ConfereEntrada(int argc, int valorEsperado) {
	if (argc != valorEsperado) {
		printf(ERRO_GERAL);
		exit(1);
	}
}

void ImprimeRegistros() {
}

void ImprimeVetor(int* vet) {
}

int main(int argc, char *argv[]){

	if (argc < 2) {
		printf("ERRO! Utilização do programa: %s <código-op(numero entre 1 e 9)>", argv[0]);
		printf(" <argumentos-da-operação>\n");
		return -1;
	}

	VETREGISTROS* vetRegistros = NULL;
	int* vetPilha = NULL;

	switch (atoi(argv[1])) {
		case 1:
			ConfereEntrada(argc, 3);
			vetRegistros = LeituraArquivoDeEntrada(argv[2]);
			CriaArquivoDeSaida();
			InsereVetorDeRegistros(vetRegistros);
			break;
		case 2:
			ConfereEntrada(argc, 2);
			vetRegistros = RecuperaTodosRegistros();
			ImprimeRegistros(vetRegistros);
			break;
		case 3:
			ConfereEntrada(argc, 4);
			vetRegistros = RecuperaRegistrosPorCampo(argv[2], argv[3]);
			ImprimeRegistros(vetRegistros);
			break;
		case 4:
			ConfereEntrada(argc, 3);
			vetRegistros = RecuperaRegistroPorRRN(atoi(argv[2]));
			ImprimeRegistros(vetRegistros);
			break;
		case 5:
			ConfereEntrada(argc, 3);
			RemocaoLogicaPorRRN(atoi(argv[2]));
			break;
		case 6:
			ConfereEntrada(argc, 8);
			InsereRegistroAdicional(argv+2);
			break;
		case 7:
			ConfereEntrada(argc, 9);
			AtualizaRegistroPorRRN(argv+3, atoi(argv[2]));
			break;
		case 8:
			ConfereEntrada(argc, 2);
			DesfragmentaArquivoDeDados();
			break;
		case 9:
			ConfereEntrada(argc, 2);
			vetPilha = RecuperaRRNLogicamenteRemovidos();
			ImprimeVetor(vetPilha);
			break;
		default:
			printf(ERRO_GERAL);
	}

	return 0;
}
