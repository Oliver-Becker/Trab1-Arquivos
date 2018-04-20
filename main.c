#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[]){

	if (argc < 2) {
		printf("ERRO! Utilização do programa: %s código-op(numero entre 1 e 9)", argv[0]);
		printf(" argumentos-da-operação\n");
		return -1;
	}

	return 0;
}
