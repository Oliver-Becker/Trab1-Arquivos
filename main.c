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

typedef struct {
	REGISTRO** vetRegistro;
	int numElementos;
} VETREGISTROS;

void CarregaArquivo(char* nomeArquivo) {
	FILE* fp = fopen(nomeArquivo, "r");
	if (fp == NULL) {
		printf(ERRO_GERAL);
		return;
	}
	REGISTRO registro;
	
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

VETREGISTROS* RecuperaTodosRegistros() 
{
	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");// Abre o arquivo
	char existealgo;
	fread(&(existealgo),1,1,fp);
	if(existealgo == '0') // significa que não tem nada para ser lido
	{
		printf(ERRO_GERAL);
		return ;
	}
	fseek(fp,4,SEEK_SET); // vai pular direto pra quinta posição, pois as 5 primeiras são cabeçalho
	VETREGISTROS *vTotal; // vetor com todos os registros
	REGISTRO *at;
	int buffer = 10;
	int count=0; // realloc do vetor de registros
	at = (REGISTRO*) malloc(sizeof(REGISTRO)*buffereg); // registro do momento que está sendo lido (Registro horizontal)

	//vTotal->vetRegistro[0] = at; // vetRegistro (pertence ao vTotal)s

	vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)); // vTotal é uma struct, logo aloca-se uma struct
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer); // ** é para aumentar verticalment
					

	while(feof(fp)!=0)
	{
		int existe; // Serve para um registro individualmente (Pois o registro pode ter sido apagado)
		fread(&existe,sizeof(int),1,fp); // Saber se existe aquele registro 
		if(existe < 0) 
			fseek(fp,sizeof(REGISTRO),SEEK_CURR);
		/*
			Como altera algo, utiliza-se endereço, Dentro do vTotal tem o vetor de registros, vai alterar dentro do vetor de registros
			Por alterar pressupoe-se ler o valor daquela linha do arquivo
			Altera o registro da linha atual (linha atual = count)
		*/
		fread(&(vTotal->vetRegistro[count]->dataInicio),1,10,fp); //Vetor de registro na posição count da struct vai receber o valor da data de inicio
		fread(&(vTotal->vetRegistro[count]->dataFinal),1,10,fp);// Data Final
		
		fread(&(vTotal->vetRegistro[count]->tamNome),1,4,fp); // Aqui tem o tamanho do nome da escola que vai ser lida (Alocada)
		int Tamanho_Nome = vTotal->vetRegistro[count]->tamNome;
		vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char)*Tamanho_Nome); // Criei um espaço do tamanho do nome para usar no fread
		fread(&(vTotal->vetRegistro[count]->nomeEscola),1,Tamanho_Nome,fp); // Leio o nome da Escola 

		fread(&(vTotal->vetRegistro[count]->tamMunicipio,1,4,fp)); //Aqui tem o tamanho do nome do município que vai ser lida (Alocada)
		int Tamanho_Municipio = vTotal->vetRegistro[count]->tamMunicipio ;
		vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char)*Tamanho_Municipio); // Aloquei para o nome do município o tamanho descoberto na palavra anterior
		fread(&(vTotal->vetRegistro[count]->município),1,Tamanho_Municipio,fp);

		fread(&(vTotal->vetRegistro->tamEndereco),1,4,fp);
		int Tamanho_Endereco = vTotal->vetRegistro->tamEndereco;
		vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char)*Tamanho_Endereco);
		fread(&(vTotal->vetRegistro[count]->endereco),1,Tamanho_Endereco,fp);

		if(count == buffer)
		{
			buffer+=10;
			vTotal-> vetRegistro = (REGISTRO**)realloc(vTotal->vetRegistro,buffer);
			vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);
		}
		count++;

	}

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
			CarregaArquivo(argv[2]);
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
