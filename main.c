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
	REGISTRO** registro;
	int numElementos;
} VETREGISTROS;

void AlteraStatusDoArquivo(char status) {
	if (status != 0 && status != 1)
		return;
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+");
	fwrite(&status, sizeof(status), 1, fp);
	fclose(fp);
}

int ConfereConsistenciaDoArquivo(char* nomeArquivo) {
	FILE* fp = fopen(nomeArquivo, "rb");
	char status;
	if (fp == NULL)
		return 0;

	fread(&status, sizeof(status), 1, fp);
	return status;
}

void CriaArquivoDeSaida() {
	FILE* fp = fopen(ARQUIVO_SAIDA, "wb");
	char status = 0;
	int topoPilha = -1;
	fwrite(&status, sizeof(status), 1, fp);
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp);
}

void InsereRegistro(REGISTRO* registro) {
	if (registro == NULL)
		return;

	ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	FILE* fp = fopen(ARQUIVO_SAIDA, "ab+");

	int registroRemovido = 0;
	fwrite(&registroRemovido, sizeof(registroRemovido), 1, fp);
}

void InsereVetorDeRegistros(VETREGISTROS* vetRegistros) {
	if (vetRegistros == NULL)
		return;

	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		InsereRegistro(vetRegistros->registro[i]);
	}
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

		vetRegistros->registro = (REGISTRO**)realloc(vetRegistros->registro, 
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

		vetRegistros->registro[counter++] = registro;

		free(string[0]);
		if (string[4] != NULL) free(string[4]);
		if (string[5] != NULL) free(string[5]);
	}

	REGISTRO* r;
	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->registro[i];
		printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
			r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
			r->endereco);
	}
	//printf("tamanho do vetor de registros: %d\n", vetRegistros->numElementos);

// free do vetor de registros
/*	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->registro[i];
		if (r->nomeEscola != NULL) free(r->nomeEscola);
		if (r->municipio != NULL) free(r->municipio);
		if (r->endereco != NULL) free(r->endereco);
		free(r);
	}
	free(vetRegistros->registro);
	free(vetRegistros);
*/
	fclose(fp);

	return vetRegistros;
}

void RecuperaRegistrosCodEscola(FILE *fp, char* valor){
	
	char regExiste;
	int buffer = 10;
	int count = 0;
	int num = atoi(valor);

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(regExiste == '1'){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 

		if(num == vTotal->registro[count]->codEscola){  //verifica se o código da escola é o procurado pelo usuário
				
			fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
			fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFim
	
			//leitura do nome da escola
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

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
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio

		if(strcmp(valor, vTotal->registro[count]->dataInicio) == 0){  //verifica se o data inicial da escola é a procurado pelo usuário
				
			fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFim
	
			//leitura do nome da escola
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

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
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal

		if(strcmp(valor, vTotal->registro[count]->dataFinal) == 0){  //verifica se o data final da escola é a procurado pelo usuário		
	
			//leitura do nome da escola
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

			//leitura do municipio da escola
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

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
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		if(strcmp(valor, vTotal->registro[count]->nomeEscola) == 0){  //verifica se o nome da escola é a procurado pelo usuário		
	
			//leitura do municipio da escola
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

			//leitura do endereço da escola
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

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
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		//leitura do municipio da escola
		fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
		fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

		if(strcmp(valor, vTotal->registro[count]->municipio) == 0){  //verifica se o municipio da escola é a procurado pelo usuário		
	
			//leitura do endereço da escola
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

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
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(char), 1 , fp); //lê o byte com o status do registro, 0 é registro válido
		
		if(strcmp(&regExiste, "1") == 1){
			printf(ERRO_REGISTRO);
			continue;
		}
	
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp); //lê o código da escola e salva no vetRegistro[count].codEscola 
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); //lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp); //lê a data final e salva no vetRegistro[count].dataFinal
		
		//leitura do nome da escola
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp); //lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		int tamNome =  vTotal->registro[count]->tamNome; //quantidade de memória que precisa ser alocada pra o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); //lê o nome da escola e salva no vetRegistro[count].nomeEscola

		//leitura do municipio da escola
		fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp); //lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio; //quantidade de memória que precisa ser alocada pra o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio); //aloca memória para o vetor municipio
		fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp); //lê o nome da escola e salva no vetRegistro[count].municipio

		//leitura do endereço da escola
		fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp); //lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco		
		int tamEndereco = vTotal->registro[count]->tamEndereco; //quantidade de memória que precisa ser alocada pra o vetor endereco
		vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco); //aloca memória para o vetor nomeEscola
		fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); //lê o nome da escola e salva no vetRegistro[count].endereco

		if(strcmp(valor, vTotal->registro[count]->endereco) == 0){  //verifica se o endereço da escola é a procurado pelo usuário		

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
	}
}

VETREGISTROS* RecuperaTodosRegistros() 
{
	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");// Abre o arquivo
	char existealgo;
	fread(&(existealgo),1,1,fp);
	if(existealgo == '0') // significa que não tem nada para ser lido
	{
		printf(ERRO_GERAL);
		return NULL;
	}
	fseek(fp,4,SEEK_SET); // vai pular direto pra quinta posição, pois as 5 primeiras são cabeçalho
	VETREGISTROS *vTotal; // vetor com todos os registros
	REGISTRO *at;
	int buffer = 10;
	int count=0; // realloc do vetor de registros
	at = (REGISTRO*) malloc(sizeof(REGISTRO)*buffer); // registro do momento que está sendo lido (Registro horizontal)

	//vTotal->registro[0] = at; // vetRegistro (pertence ao vTotal)s

	vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)); // vTotal é uma struct, logo aloca-se uma struct
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer); // ** é para aumentar verticalment
					

	while(feof(fp)!=0)
	{
		int existe; // Serve para um registro individualmente (Pois o registro pode ter sido apagado)
		fread(&existe,sizeof(int),1,fp); // Saber se existe aquele registro 
		if(existe < 0) 
			fseek(fp,sizeof(REGISTRO),SEEK_CUR);
		/*
			Como altera algo, utiliza-se endereço, Dentro do vTotal tem o vetor de registros, vai alterar dentro do vetor de registros
			Por alterar pressupoe-se ler o valor daquela linha do arquivo
			Altera o registro da linha atual (linha atual = count)
		*/
		fread(&(vTotal->registro[count]->dataInicio),1,10,fp); //Vetor de registro na posição count da struct vai receber o valor da data de inicio
		fread(&(vTotal->registro[count]->dataFinal),1,10,fp);// Data Final
		
		fread(&(vTotal->registro[count]->tamNome),1,4,fp); // Aqui tem o tamanho do nome da escola que vai ser lida (Alocada)
		int Tamanho_Nome = vTotal->registro[count]->tamNome;
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char)*Tamanho_Nome); // Criei um espaço do tamanho do nome para usar no fread
		fread(&(vTotal->registro[count]->nomeEscola),1,Tamanho_Nome,fp); // Leio o nome da Escola 

		fread(&(vTotal->registro[count]->tamMunicipio),1,4,fp); //Aqui tem o tamanho do nome do município que vai ser lida (Alocada)
		int Tamanho_Municipio = vTotal->registro[count]->tamMunicipio ;
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char)*Tamanho_Municipio); // Aloquei para o nome do município o tamanho descoberto na palavra anterior
		fread(&(vTotal->registro[count]->municipio),1,Tamanho_Municipio,fp);

		fread(&(vTotal->registro[count]->tamEndereco),1,4,fp);
		int Tamanho_Endereco = vTotal->registro[count]->tamEndereco;
		vTotal->registro[count]->endereco = (char*) malloc(sizeof(char)*Tamanho_Endereco);
		fread(&(vTotal->registro[count]->endereco),1,Tamanho_Endereco,fp);

		if(count == buffer)
		{
			buffer+=10;
			vTotal->registro = (REGISTRO**)realloc(vTotal->registro,buffer);
			vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);
		}
		count++;

	}
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
