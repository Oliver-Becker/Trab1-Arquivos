#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERRO_GERAL "Falha no carregamento do arquivo.\n"
#define ERRO_REGISTRO "Registro inexistente.\n"
#define ARQUIVO_SAIDA "saida.bin"
#define TAMANHO_CABECALHO 5
#define TAMANHO_REGISTRO 112
#define BYTE_OFFSET(RRN) ((4 + TAMANHO_REGISTRO) * RRN) + TAMANHO_CABECALHO

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

void AlteraStatusDoArquivo(char* nomeArquivo, char status) {
	if (nomeArquivo == NULL || (status != 0 && status != 1))
		return;

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+");
	fwrite(&status, sizeof(status), 1, fp);
	fclose(fp);
}

void ConfereConsistenciaDoArquivo(char* nomeArquivo) {
	if (nomeArquivo == NULL) {
		printf(ERRO_GERAL);
		exit(1);
	}

	FILE* fp = fopen(nomeArquivo, "rb");
	if (fp == NULL) {
		printf(ERRO_GERAL);
		exit(1);
	}

	char status;
	fread(&status, sizeof(status), 1, fp);
	if (status == 1) 
		return;

	printf(ERRO_GERAL);
	exit(1);
}

void CriaArquivoDeSaida(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "wb");
	char status = 0;
	int topoPilha = -1;
	fwrite(&status, sizeof(status), 1, fp);
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp);

	topoPilha = 324;
	printf("topoPilha antes: %d\n", topoPilha);
	fseek(fp, 1, SEEK_SET);
	fread(&topoPilha, 4, 1, fp);
	printf("topo DEPOISS: %d\n", topoPilha);
}

void AcrescentaRegistroNoFinal(char* nomeArquivo, REGISTRO* registro) {
	if (nomeArquivo == NULL || registro == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "ab");
	printf("inserindo registro %d da escola %s\n", registro->codEscola, registro->nomeEscola);
	printf("ftell no começo%ld\n", ftell(fp));

	fseek(fp, 0, SEEK_END);

	int registroRemovido = 0;
	printf("%ld ", ftell(fp));
	fwrite(&registroRemovido, sizeof(registroRemovido), 1, fp);
	printf("%ld ", ftell(fp));
	fwrite(&registro->codEscola, sizeof(registro->codEscola), 1, fp);
	printf("%ld ", ftell(fp));
	fwrite(registro->dataInicio, sizeof(char), strlen(registro->dataInicio), fp);
	printf("%ld ", ftell(fp));
	fwrite(registro->dataFinal, sizeof(char), strlen(registro->dataFinal), fp);
	printf("%ld ", ftell(fp));
	fwrite(&registro->tamNome, sizeof(registro->tamNome), 1, fp);
	printf("%ld ", ftell(fp));
	fwrite(registro->nomeEscola, sizeof(char), registro->tamNome, fp);
	printf("%ld ", ftell(fp));
	fwrite(&registro->tamEndereco, sizeof(registro->tamEndereco), 1, fp);
	printf("%ld ", ftell(fp));
	fwrite(registro->endereco, sizeof(char), registro->tamEndereco, fp);
	printf("%ld ", ftell(fp));
	fwrite(&registro->tamMunicipio, sizeof(registro->tamMunicipio), 1, fp);
	printf("%ld ", ftell(fp));
	fwrite(registro->municipio, sizeof(char), registro->tamMunicipio, fp);
	printf("%ld ", ftell(fp));

	int bytesRestantes = TAMANHO_REGISTRO;
	bytesRestantes -= 36 + registro->tamNome + registro->tamEndereco + registro->tamMunicipio;

	char* finalDoRegistro = (char*)malloc(sizeof(char) * bytesRestantes);
	fwrite(finalDoRegistro, sizeof(char), bytesRestantes, fp);
	printf("ftell FINALLL: %ld\n", ftell(fp));
}

void SubstituiRegistro(char* nomeArquivo, REGISTRO* registro, int byteOffset) {
}

void InsereRegistro(char* nomeArquivo, REGISTRO* registro) {
	if (nomeArquivo == NULL || registro == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "rb+");

	fseek(fp, 1, SEEK_SET);

	int topoPilha;
	fread(&topoPilha, sizeof(topoPilha), 1, fp);

	fclose(fp);

	(topoPilha == -1) ? AcrescentaRegistroNoFinal(nomeArquivo, registro) : 
		SubstituiRegistro(nomeArquivo, registro, BYTE_OFFSET(topoPilha));
}

void InsereVetorDeRegistros(char* nomeArquivo, VETREGISTROS* vetRegistros) {
	if (nomeArquivo == NULL || vetRegistros == NULL)
		return;

	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		InsereRegistro(nomeArquivo, vetRegistros->registro[i]);
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

VETREGISTROS* RecuperaRegistrosCodEscola(char* valor){
	
	int regExiste;
	int buffer = 10;
	int count = 0;
	int num = atoi(valor);

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//verifica se o código da escola é o procurado pelo usuário
		if(num == vTotal->registro[count]->codEscola){  

			//lê a data de início e salva no vetRegistro[count].dataInicio
			fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
			//lê a data final e salva no vetRegistro[count].dataFim
			fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
	
			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
			fseek(fp, 10, SEEK_CUR); //consome o registro de data final
	
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

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaRegistrosDataInicio(char* valor){
	
	int regExiste;
	int buffer = 10;
	int count = 0;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
		
		//verifica se a data inicial é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->dataInicio) == 0){  

			//lê a data final e salva no vetRegistro[count].dataFim
			fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
	
			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

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

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaRegistrosDataFinal(char* valor){	
	
	int regExiste;
	int buffer = 10;
	int count = 0;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
		
		//verifica se a data final é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->dataFinal) == 0){  

			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

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

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaRegistrosNomeEscola(char* valor){

	int regExiste;
	int buffer = 10;
	int count = 0;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

		//verifica se o nome da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->nomeEscola) == 0){  

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

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

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaRegistrosMunicipio(char* valor){	

	int regExiste;
	int buffer = 10;
	int count = 0;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

		//leitura do municipio da escola
		//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
		fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor municipio 	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
		//aloca memória para o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
		//lê o nome da escola e salva no vetRegistro[count].municipio 
		fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);
		
		//verifica se o municipio da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->municipio) == 0){  

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

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

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaRegistrosEndereco(char* valor){

	int regExiste;
	int buffer = 10;
	int count = 0;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	VETREGISTROS *vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)*buffer);
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer);

	while(feof(fp) != 0){

		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), 1, sizeof(int), fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 1, 10, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 1, 10, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), 1, tamNome, fp); 

		//leitura do municipio da escola
		//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
		fread(&(vTotal->registro[count]->tamMunicipio), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor municipio 	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
		//aloca memória para o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
		//lê o nome da escola e salva no vetRegistro[count].municipio 
		fread(&(vTotal->registro[count]->municipio), 1, tamMunicipio, fp);
		
		//leitura do endereço da escola
		//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
		fread(&(vTotal->registro[count]->tamEndereco), 1, sizeof(int), fp);
		//quantidade de memória que precisa ser alocada pra o vetor endereco		
		int tamEndereco = vTotal->registro[count]->tamEndereco;
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
		//lê o nome da escola e salva no vetRegistro[count].endereco
		fread(&(vTotal->registro[count]->endereco), 1, tamEndereco, fp); 

		//verifica se o endereço da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->endereco) == 0){  

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
	}

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	return vTotal;

	fclose(fp);
}

VETREGISTROS* RecuperaTodosRegistros() 
{
	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");// Abre o arquivo
	
	VETREGISTROS *vTotal; // vetor com todos os registros
	int buffer = 10;
	int count=0; // realloc do vetor de registros
	
 // registro do momento que está sendo lido (Registro horizontal)
	vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)); // vTotal é uma struct, logo aloca-se uma struct
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer); // ** é para aumentar verticalment
	
	char existealgo;
	fread(&(existealgo),1,1,fp);
	if(existealgo == '0') // significa que não tem nada para ser lido
	{
		printf(ERRO_GERAL);
		return vTotal;
	}
	fseek(fp,4,SEEK_SET); // vai pular direto pra quinta posição, pois as 5 primeiras são cabeçalho
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
		
		//Vetor de registro na posição count da struct vai receber o valor da data de inicio
		fread(&(vTotal->registro[count]->dataInicio),1,10,fp);
		fread(&(vTotal->registro[count]->dataFinal),1,10,fp);// Data Final
		
		fread(&(vTotal->registro[count]->tamNome),1,4,fp); // Aqui tem o tamanho do nome da escola que vai ser lida (Alocada)
		int Tamanho_Nome = vTotal->registro[count]->tamNome;
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char)*Tamanho_Nome); // Criei um espaço do tamanho do nome para usar no fread
		fread(&(vTotal->registro[count]->nomeEscola),1,Tamanho_Nome,fp); // Leio o nome da Escola 

		fread(&(vTotal->registro[count]->tamMunicipio),1,4,fp); //Aqui tem o tamanho do nome do município que vai ser lida (Alocada)
		int Tamanho_Municipio = vTotal->registro[count]->tamMunicipio ;
		// Aloquei para o nome do município o tamanho descoberto na palavra anterior
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char)*Tamanho_Municipio); 
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
	return vTotal;
	fclose(fp);
}


VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {
	
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");

	//ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA); //confere a consistência do arquivo
	
	fseek(fp, 4, SEEK_SET); //consome o topo da pilha

	if(strcmp(nomeDoCampo, "codEscola") == 0)	
		return RecuperaRegistrosCodEscola(valor);
	else if(strcmp(nomeDoCampo, "dataInicio") == 0)
		return RecuperaRegistrosDataInicio(valor);
	else if(strcmp(nomeDoCampo, "dataFinal") == 0)
		return RecuperaRegistrosDataFinal(valor);
	else if(strcmp(nomeDoCampo, "nomeEscola") == 0)
		return RecuperaRegistrosNomeEscola(valor);
	else if(strcmp(nomeDoCampo, "municipio") == 0)
		return RecuperaRegistrosMunicipio(valor);
	else
		return RecuperaRegistrosEndereco(valor);
}

VETREGISTROS* RecuperaRegistroPorRRN(int RRN) 
{	
	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");

	VETREGISTROS *vTotal;
	vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)); // vTotal é uma struct, logo aloca-se uma struct
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)); // ** é para aumentar verticalment


	char existealgo;
	fread(&(existealgo),1,1,fp);
	if(existealgo == '0')
	{
		printf(ERRO_GERAL);
		return  vTotal;
	}

	fseek(fp,4,SEEK_SET); // pula o cabeçalho
	fseek(fp,RRN,SEEK_CUR);

	int existe ;
	fread(&existe,sizeof(int),1,fp); // Saber se existe aquele registro 

	if(existe < 0) 
		return vTotal;

	fread(vTotal->registro[0]->dataInicio,1,10,fp);
	fread(&(vTotal->registro[0]->dataFinal),1,10,fp);// Data Final

	fread(&(vTotal->registro[0]->tamNome),1,4,fp);
	int Tamanho_Nome = vTotal->registro[0]->tamNome;
	vTotal->registro[0]->nomeEscola = (char*) malloc(sizeof(char)*Tamanho_Nome); // Criei um espaço do tamanho do nome para usar no fread
	fread(&(vTotal->registro[0]->nomeEscola),1,Tamanho_Nome,fp);
	

	fread(&(vTotal->registro[0]->tamMunicipio),1,4,fp); //Aqui tem o tamanho do nome do município que vai ser lida (Alocada)
	int Tamanho_Municipio = vTotal->registro[0]->tamMunicipio ;// Aloquei para o nome do município o tamanho descoberto na palavra anterior
	vTotal->registro[0]->municipio = (char*) malloc(sizeof(char)*Tamanho_Municipio); 
	fread(&(vTotal->registro[0]->municipio),1,Tamanho_Municipio,fp);

	fread(&(vTotal->registro[0]->tamEndereco),1,4,fp);
	int Tamanho_Endereco = vTotal->registro[0]->tamEndereco;
	vTotal->registro[0]->endereco = (char*) malloc(sizeof(char)*Tamanho_Endereco);
	fread(&(vTotal->registro[0]->endereco),1,Tamanho_Endereco,fp);
	return vTotal;
	fclose(fp);
}

void RemocaoLogicaPorRRN(int RRN) {

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+");
	
	int topoPilha, regExiste, fimArquivo;

	fseek(fp, 0, SEEK_END); //aponta para o fim do arquivo

	fimArquivo = ftell(fp); //salva a quantidade de bits do arquivo

	fseek(fp, 0, SEEK_SET); //volta para o início do arquivo

	ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);
	
	fread(&topoPilha, sizeof(int), 1, fp); //lê o número do topo da pilha

	if(RRN > fimArquivo){ //verifica se o numero do RRN é válido
		printf(ERRO_REGISTRO);
		return;
	}

	fseek(fp, RRN, SEEK_CUR); //vai para o byte offset de posição RRN

	fread(&regExiste, sizeof(int), 1, fp); 

	if(regExiste == -1){ //verifica se o registro existe
		printf(ERRO_REGISTRO);
		return;
	}

	int n = -1;

	fwrite(&n, sizeof(n), 1, fp); //marca o registro como removido

	fwrite(&topoPilha, sizeof(topoPilha), 1, fp); //salva o topo da pilha no registro removido

	fseek(fp, 1, SEEK_SET); //retorna ao cabeçalho com o endereço da pilha

	fwrite(&RRN, sizeof(RRN), 1, fp); //salva o RRN removido no cabeçalho

	printf("Registro removido com sucesso.\n");
	
	fclose(fp);
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
			CriaArquivoDeSaida(ARQUIVO_SAIDA);
			InsereVetorDeRegistros(ARQUIVO_SAIDA, vetRegistros);
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
