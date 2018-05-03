#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO_GERAL "Falha no carregamento do arquivo.\n"
#define ERRO_REGISTRO "Registro inexistente.\n"
#define ARQUIVO_SAIDA "saida.bin"

typedef struct {
	int codEscola;
	char dataInicio[10];
	char dataFinal[10];
	int tamNome;
	char *nomeEscola;
	int tamMunicipio;
	char *municipio;
	int tamEndereco;
	char *endereco;
} REGISTRO;

void CarregaArquivo(char* nomeArquivo) {
	FILE* fp = fopen(nomeArquivo, "r");
	if (fp == NULL) {
		printf(ERRO_GERAL);
		return;
	}
//	REGISTRO registro;
	
	//while (fscanf(fp, "%d", &(registro->codEscola)) != EOF) {
	for (int i = 0; i < 6; ++i) {
		char* string = NULL;
		int n;
		//getline(&string, &n, fp);
		//getdelim(&string, &n, ';', fp);
		n = fscanf(fp, "%m[^;\n]%*c", &string);
		printf("campo%d, n=%d, strlen=%2lu: %s\n", i, n, strlen(string), string);
		free(string);
	}
	fclose(fp);
}

REGISTRO** RecuperaTodosRegistros() {
}

REGISTRO** RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {
}

REGISTRO* RecuperaRegistroPorRRN(int RRN) {
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

	REGISTRO** vetRegistro = NULL;
	REGISTRO* registro = NULL;
	int* vetPilha = NULL;

	switch (atoi(argv[1])) {
		case 1:
			ConfereEntrada(argc, 3);
			CarregaArquivo(argv[2]);
			break;
		case 2:
			ConfereEntrada(argc, 2);
			RecuperaTodosRegistros();
			ImprimeRegistros();
			break;
		case 3:
			ConfereEntrada(argc, 4);
			RecuperaRegistrosPorCampo(argv[2], argv[3]);
			ImprimeRegistros();
			break;
		case 4:
			ConfereEntrada(argc, 3);
			RecuperaRegistroPorRRN(atoi(argv[2]));
			ImprimeRegistros();
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
