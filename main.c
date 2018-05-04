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

void RecuperaRegistrosCodEscola(FILE *fp, char* valor){
	
	char regExiste;
	int buffer = 10;
	int count = 0;
	int num = atoi(valor);

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(regExiste == '1'){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 

		if(num == vTotal->vetRegistro[count]->codEscola){  //verifica se o código da escola é o procurado pelo usuário
				
			fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
			fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFim
	
			//leitura do nome da escola
			fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
			fseek(fp, 20, SEEK_CUR); //consome os registros de data de início e data final
	
			//consome o nome da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}
}

void RecuperaRegistrosDataInicio(FILE *fp, char* valor){
	
	char regExiste;
	int buffer = 10;
	int count = 0;

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio

		if(strcmp(valor, vTotal->vetRegistro[count]->dataInicio) == 0){  //verifica se o data inicial da escola é a procurado pelo usuário
				
			fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFim
	
			//leitura do nome da escola
			fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
			fseek(fp, 10, SEEK_CUR); //consome os registros da data final
	
			//consome o nome da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}
}

void RecuperaRegistrosDataFinal(FILE *fp, char* valor){	
	
	char regExiste;
	int buffer = 10;
	int count = 0;

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal

		if(strcmp(valor, vTotal->vetRegistro[count]->dataFinal) == 0){  //verifica se o data final da escola é a procurado pelo usuário		
	
			//leitura do nome da escola
			fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
	
			//consome o nome da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}
}

void RecuperaRegistrosNomeEscola(FILE *fp, char* valor){

	char regExiste;
	int buffer = 10;
	int count = 0;

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		if(strcmp(valor, vTotal->vetRegistro[count]->nomeEscola) == 0){  //verifica se o nome da escola é a procurado pelo usuário		
	
			//leitura do municipio da escola
			fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;

			//consome o municipio da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}
}

void RecuperaRegistrosMunicipio(FILE *fp, char* valor){	

	char regExiste;
	int buffer = 10;
	int count = 0;

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		//leitura do municipio da escola
		fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
		int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
		vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
		fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

		if(strcmp(valor, vTotal->vetRegistro[count]->municipio) == 0){  //verifica se o municipio da escola é a procurado pelo usuário		
	
			//leitura do endereço da escola
			fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;

			//consome o endereço da escola
			fread(&tam, 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}
}

void RecuperaRegistrosEndereco(FILE *fp, char* valor){

	char regExiste;
	int buffer = 10;
	int count = 0;

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->vetRegistro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->vetRegistro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->vetRegistro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->vetRegistro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->vetRegistro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->vetRegistro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->vetRegistro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->vetRegistro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		//leitura do municipio da escola
		fread(&(vTotal->vetRegistro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
		int tamMunicipio =  vTotal->vetRegistro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
		vTotal->vetRegistro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
		fread(&(vTotal->vetRegistro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

		//leitura do endereço da escola
		fread(&(vTotal->vetRegistro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
		int tamEndereco = vTotal->vetRegistro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
		vTotal->vetRegistro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->vetRegistro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

		if(strcmp(valor, vTotal->vetRegistro[count]->endereco) == 0){  //verifica se o endereço da escola é a procurado pelo usuário		

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
	}
}

VETREGISTROS* RecuperaTodosRegistros() {
}

VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {

		
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");
	int arqExiste;
	

	fread(&regExiste, 1, 1, fp);

	if(arqExiste == '0'){  //arquivo inconsistente 
		printf(ERRO_GERAL);
		return;
	}
	
	fseek(fp, 4, SEEK_SET); //consome o topo da pilha

	if(strcmp(nomeDoCampo, "codEscola") == 0)	
		RecuperaRegistrosCodEscola(fp, valor);
	else if(strcmp(nomeDoCampo, "dataInicio") == 0)
		RecuperaRegistrosDataInicio(fp, valor);
	else if(strcmp(nomeDoCampo, "dataFinal") == 0)
		RecuperaRegistrosDataFinal(fp,valor);
	else if(strcmp(nomeDoCampo, "nomeEscola") == 0)
		RecuperaRegistrosNomeEscola(fp, valor);
	else if(strcmp(nomeDoCampo, "municipio") == 0)
		RecuperaRegistrosMunicipio(fp, valor);
	else
		RecuperaRegistrosEndereco(fp, valor);
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

void ImprimeRegistros(VETREGISTROS *vetRegistros) {
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
