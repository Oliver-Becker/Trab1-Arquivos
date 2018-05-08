#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUANTIDADE_ARGUMENTOS {3, 2, 4, 3, 3, 8, 9, 2, 2}
#define ERRO_REGISTRO "Registro inexistente.\n"
#define ERRO_GERAL "Falha no processamento do arquivo.\n"
#define ARQUIVO_SAIDA "saida.bin"
#define ARQUIVO_DESFRAGMENTADO "desfragmentacao.bin"
#define TAMANHO_DATA 10
#define TAMANHO_CABECALHO 5
#define TAMANHO_REGISTRO 112
#define BYTE_OFFSET(RRN) ((4 + TAMANHO_REGISTRO) * RRN) + TAMANHO_CABECALHO
#define DATA_VAZIA "0000000000"
#define CAMPO_VAZIO ""

typedef struct {
	int codEscola;
	char dataInicio[TAMANHO_DATA + 1];
	char dataFinal[TAMANHO_DATA + 1];
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

void AlteraTopoDaPilha(char* nomeArquivo, int topoPilha) {
	if (nomeArquivo == NULL || topoPilha < -1)
		return;

	FILE* fp = fopen(nomeArquivo, "rb+");
	if (fp == NULL)
		return;

	fseek(fp, 1, SEEK_SET);		// Pula o char do status para ler o topoPilha

	fwrite(&topoPilha, sizeof(topoPilha), 1, fp);
	fclose(fp);
}

int TopoDaPilha(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return -2;

	FILE* fp = fopen(nomeArquivo, "rb");
	if (fp == NULL)
		return -2;

	fseek(fp, 1, SEEK_SET);		// Pula o char do status para ler o topoPilha

	int topoPilha;
	fread(&topoPilha, sizeof(topoPilha), 1, fp);
	fclose(fp);

	return topoPilha;
}

void AlteraStatusDoArquivo(char* nomeArquivo, char status) {
	if (nomeArquivo == NULL || (status != 0 && status != 1))
		return;

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+");
	if (fp == NULL)
		return;

	fwrite(&status, sizeof(status), 1, fp);
	fclose(fp);
}

void ConfereConsistenciaDoArquivo(char* nomeArquivo) {
	if (nomeArquivo == NULL) {
		printf(ERRO_GERAL);
		exit(-3);
	}

	FILE* fp = fopen(nomeArquivo, "rb");
	if (fp == NULL) {
		printf(ERRO_GERAL);
		exit(-3);
	}

	char status;
	fread(&status, sizeof(status), 1, fp);
	fclose(fp);
	if (status == 1) 
		return;

	printf(ERRO_GERAL);
	exit(-3);
}

// Função para criar um novo arquivo de saída, já inserindo o cabeçalho.
void CriaArquivoDeSaida(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "wb+");
	char status = 0;	// Status para indicar a consistência do arquivo de dados.
	int topoPilha = -1;	// Armazena o RRN do último registro logicamente removido.
	fwrite(&status, sizeof(status), 1, fp);
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp);

	fclose(fp);
}

void AcrescentaRegistroNoFinal(char* nomeArquivo, REGISTRO* registro) {
	if (nomeArquivo == NULL || registro == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "ab+");
	int registroRemovido = 0;

	fwrite(&registroRemovido, sizeof(int), 1, fp);
	fwrite(&(registro->codEscola), sizeof(int), 1, fp);
	fwrite(registro->dataInicio, sizeof(char), strlen(registro->dataInicio), fp);
	fwrite(registro->dataFinal, sizeof(char), strlen(registro->dataFinal), fp);
	fwrite(&registro->tamNome, sizeof(int), 1, fp);
	fwrite(registro->nomeEscola, sizeof(char), registro->tamNome, fp);
	fwrite(&registro->tamMunicipio, sizeof(int), 1, fp);
	fwrite(registro->municipio, sizeof(char), registro->tamMunicipio, fp);
	fwrite(&registro->tamEndereco, sizeof(int), 1, fp);
	fwrite(registro->endereco, sizeof(char), registro->tamEndereco, fp);

	int bytesRestantes = TAMANHO_REGISTRO;
	bytesRestantes -= 36 + registro->tamNome + registro->tamEndereco + registro->tamMunicipio;

	char* finalDoRegistro = (char*)malloc(sizeof(char) * bytesRestantes);
	fwrite(finalDoRegistro, sizeof(char), bytesRestantes, fp);

	REGISTRO* r = registro;
	printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
		r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
		r->endereco);

	free(finalDoRegistro);
	fclose(fp);
}

void SubstituiRegistro(char* nomeArquivo, REGISTRO* registro, int RRN) {
	if (nomeArquivo == NULL || registro == NULL)
		return;

	FILE* fp = fopen(nomeArquivo, "rb+");
	if (fp == NULL)
		return;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);
	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if (registroExiste < 0) {	// Caso o registro não exista.
		int topoPilha = TopoDaPilha(nomeArquivo);
		if (topoPilha != RRN) {
			fclose(fp);
			return;
		}
		fread(&topoPilha, sizeof(topoPilha), 1, fp);
		AlteraTopoDaPilha(nomeArquivo, topoPilha);
	}

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);
	registroExiste = 0;

	fwrite(&registroExiste, sizeof(registroExiste), 1, fp);
	fwrite(&registro->codEscola, sizeof(registro->codEscola), 1, fp);
	fwrite(registro->dataInicio, sizeof(char), strlen(registro->dataInicio), fp);
	fwrite(registro->dataFinal, sizeof(char), strlen(registro->dataFinal), fp);
	fwrite(&registro->tamNome, sizeof(registro->tamNome), 1, fp);
	fwrite(registro->nomeEscola, sizeof(char), registro->tamNome, fp);
	fwrite(&registro->tamMunicipio, sizeof(registro->tamMunicipio), 1, fp);
	fwrite(registro->municipio, sizeof(char), registro->tamMunicipio, fp);
	fwrite(&registro->tamEndereco, sizeof(registro->tamEndereco), 1, fp);
	fwrite(registro->endereco, sizeof(char), registro->tamEndereco, fp);

	fclose(fp);
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
		SubstituiRegistro(nomeArquivo, registro, topoPilha);
}

void LiberaRegistro(REGISTRO* registro) {
	if (registro == NULL)
		return;

	if (registro->nomeEscola != NULL && strcmp(registro->nomeEscola, CAMPO_VAZIO))
		free(registro->nomeEscola);
	if (registro->municipio != NULL && strcmp(registro->municipio, CAMPO_VAZIO))
		free(registro->municipio);
	if (registro->endereco != NULL && strcmp(registro->endereco, CAMPO_VAZIO))
		free(registro->endereco);

	free(registro);
	registro = NULL;
}

void LiberaVetorDeRegistros(VETREGISTROS* vetRegistros) {
	if (vetRegistros == NULL)
		return;

	for (int i = 0; i < vetRegistros->numElementos; ++i)
		LiberaRegistro(vetRegistros->registro[i]);

	free(vetRegistros->registro);
	free(vetRegistros);
	vetRegistros = NULL;
}

void InsereVetorDeRegistros(char* nomeArquivo, VETREGISTROS* vetRegistros) {
	if (nomeArquivo == NULL || vetRegistros == NULL)
		return;

	if (vetRegistros->numElementos == 0) {
		printf(ERRO_REGISTRO);
		exit(1);
	}

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0);

	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		InsereRegistro(nomeArquivo, vetRegistros->registro[i]);
	}

	LiberaVetorDeRegistros(vetRegistros);

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);
}

VETREGISTROS* LeituraArquivoDeEntrada(char* nomeArquivo) {
	if (nomeArquivo == NULL) {
		printf(ERRO_GERAL);
		return NULL;
	}

	FILE* fp = fopen(nomeArquivo, "r");	// Abre o arquivo de entrada no modo de leitura.
	if (fp == NULL) {			// Caso o arquivo não exista, imprime erro.
		printf(ERRO_GERAL);
		return NULL;
	}

	// Aloca espaço para o VETREGISTROS que será retornado pela função.
	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	REGISTRO* registro; 	// Variável de registro auxiliar.

	int counter = 0; 	// Contador auxiliar para saber quantos registros já foram lidos.
	char* string[6];	// Strings para armazenarem os campos lidos do arquivo de entrada.

	// Loop para ler todos os registros até que encontre o fim do arquivo.
	while (fscanf(fp, "%m[^;\n]", &string[0]) != EOF) {
		for (int i = 1; i < 6; ++i)			// Caso o registro exista, lê os 5
			fscanf(fp, "%*c%m[^;\n]", &string[i]);		// campos restantes.
		fgetc(fp);					// Consome o \n.

		// Vai aumentando o espaço do vetor de registros conforme é lido da entrada.
		vetRegistros->registro = (REGISTRO**)realloc(vetRegistros->registro, 
				sizeof(REGISTRO*) * (++vetRegistros->numElementos));

		// Aloca espaço para o registro que será anexado ao vetor.
		registro = (REGISTRO*)malloc(sizeof(REGISTRO));

		// Atribui para cada campo da struct o valor correspondente, com base na ordem de
		// leitura dos campos do arquivo de entrada.
		registro->codEscola = atoi(string[0]);
		// Caso o campo seja vazio (string null), atribui a string de zeros para o campo.
		strcpy(registro->dataInicio, (string[4] != NULL) ? string[4] : DATA_VAZIA);
		strcpy(registro->dataFinal, (string[5] != NULL) ? string[5] : DATA_VAZIA);
		registro->tamNome = (string[1] != NULL) ? strlen(string[1]) : 0;
		registro->nomeEscola = (string[1] != NULL) ? string[1] : CAMPO_VAZIO;
		registro->tamMunicipio = (string[2] != NULL) ? strlen(string[2]) : 0;
		registro->municipio = (string[2] != NULL) ? string[2] : CAMPO_VAZIO;
		registro->tamEndereco = (string[3] != NULL) ? strlen(string[3]) : 0;
		registro->endereco = (string[3] != NULL) ? string[3] : CAMPO_VAZIO;

		// Atribui o registro ao vetor.
		vetRegistros->registro[counter++] = registro;

		// Libera a memória que não será mais utilizada.
		free(string[0]);
		if (string[4] != NULL) free(string[4]);
		if (string[5] != NULL) free(string[5]);
	}

	/*REGISTRO* r;
	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->registro[i];
		printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
			r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
			r->endereco);
	}*/
	//printf("tamanho do vetor de registros: %d\n", vetRegistros->numElementos);

	fclose(fp);

	return vetRegistros;
}

VETREGISTROS* RecuperaRegistrosCodEscola(char* valor){
	
	
	int regExiste;
	int count = 0;
	int num = atoi(valor);
	printf("%d\n", num);
	
	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");

	fseek(fp, 5, SEEK_SET);

	VETREGISTROS *vTotal = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	int i = 0;
	
	while(feof(fp) == 0){

		if(++i == 11) break;

		// Vai aumentando o espaço do vetor de registros conforme é lido da entrada.
		vTotal->registro = (REGISTRO**)realloc(vTotal->registro, sizeof(REGISTRO*) * (++vTotal->numElementos));
		vTotal->registro[count] = (REGISTRO*) malloc(sizeof(REGISTRO));
		
		fread(&regExiste, sizeof(int), 1 , fp); //lê o int com o status do registro, -1 é registro removido
		
		if(regExiste == -1) //lê o proximo registro caso o atual foi removido
			continue;

		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);
		
		printf("-> %d\n", vTotal->registro[count]->codEscola);

		//verifica se o código da escola é o procurado pelo usuário
		if(num == vTotal->registro[count]->codEscola){  
			printf("entrou no if\n");
			//break;
			//lê a data de início e salva no vetRegistro[count].dataInicio
			fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
			printf("dataInicio: %s\n", vTotal->registro[count]->dataInicio);
			//lê a data final e salva no vetRegistro[count].dataFim
			fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
			printf("dataFinal: %s\n", vTotal->registro[count]->dataFinal);
	
			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
			printf("tamNome: %d\n", vTotal->registro[count]->tamNome);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			printf("tam = %d\n", tamNome);
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * (tamNome+20));
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			printf("%zd\n", ftell(fp));
			fread((vTotal->registro[count]->nomeEscola), sizeof(char), tamNome, fp); 
			printf("aki\n");
			vTotal->registro[count]->nomeEscola[tamNome] = '\0';
			printf("nomeEscola: %s\n", vTotal->registro[count]->nomeEscola);
			printf("%zd\n", ftell(fp));
			

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
		
			fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			printf("tam = %d\n", tamMunicipio);
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);
			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			printf("tamt = %d\n", tamMunicipio);
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

			count++; //incrementa a posição do vetor vTotal
		}
		else{  //consome o registro que não é o procurado

			printf("posicao 1 = %zd\n", ftell(fp));

			int tam;
			fseek(fp, 20, SEEK_CUR); //consome os registros de data inicial e data final
	
			//consome o nome da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do nome da escola
			printf("tam = %d\n", tam);
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola

			printf("posicao 2 = %zd\n", ftell(fp));
		}
	}
	
	//vTotal->numElementos = count; //salva a quantidade de registros encontrados

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	fclose(fp);

	return vTotal;
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
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
		
		//verifica se a data inicial é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->dataInicio) == 0){  

			//lê a data final e salva no vetRegistro[count].dataFim
			fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
	
			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			fread(&(vTotal->registro[count]->nomeEscola), tamNome, 1, fp); 

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
			fseek(fp, 10, SEEK_CUR); //consome os registros de data de início e data final
	
			//consome o nome da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do nome da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}

	vTotal->numElementos = count; //salva a quantidade de registros encontrado

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	fclose(fp);

	return vTotal;
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
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
		
		//verifica se a data final é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->dataFinal) == 0){  

			//leitura do nome da escola
			//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
			fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
			int tamNome =  vTotal->registro[count]->tamNome; 
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
			//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
			fread(&(vTotal->registro[count]->nomeEscola), tamNome, 1, fp); 

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
	
			//consome o nome da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do nome da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do nome da escola

			//consome o municipio da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}

	vTotal->numElementos = count; //salva a quantidade de registros encontrado

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);
	
	fclose(fp);

	return vTotal;
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
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), tamNome, 1, fp); 

		//verifica se o nome da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->nomeEscola) == 0){  

			//leitura do municipio da escola
			//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
			fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor municipio 	
			int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
			//aloca memória para o vetor municipio
			vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
			//lê o nome da escola e salva no vetRegistro[count].municipio 
			fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;
	
			//consome o municipio da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do municipio da escola

			//consome o endereço da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}

	vTotal->numElementos = count; //salva a quantidade de registros encontrado

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	fclose(fp);
	
	return vTotal;
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
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), tamNome, 1, fp); 

		//leitura do municipio da escola
		//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
		fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor municipio 	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
		//aloca memória para o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
		//lê o nome da escola e salva no vetRegistro[count].municipio 
		fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);
		
		//verifica se o municipio da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->municipio) == 0){  

			//leitura do endereço da escola
			//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
			//quantidade de memória que precisa ser alocada pra o vetor endereco		
			int tamEndereco = vTotal->registro[count]->tamEndereco;
			//aloca memória para o vetor nomeEscola
			vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
			//lê o nome da escola e salva no vetRegistro[count].endereco
			fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
		else{  //consome o registro que não é o procurado

			int tam;

			//consome o endereço da escola
			fread(&tam, sizeof(int), 1, fp); //lê o tamanho do registro do municipio da escola
			fseek(fp, tam, SEEK_CUR); //consome o registro do endereço da escola
		}
	}

	vTotal->numElementos = count; //salva a quantidade de registros encontrado

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	fclose(fp);
	
	return vTotal;
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
		//lê o código da escola e salva no vetRegistro[count].codEscola
		fread(&(vTotal->registro[count]->codEscola), sizeof(int), 1, fp);

		//lê a data de início e salva no vetRegistro[count].dataInicio
		fread(&(vTotal->registro[count]->dataInicio), 10, 1, fp); 
		
		//lê a data final e salva no vetRegistro[count].dataFim
		fread(&(vTotal->registro[count]->dataFinal), 10, 1, fp);
		
		//leitura do nome da escola
		//lê o tamanho do registro do nome da escola e salva no vetRegistro[count].tamNome
		fread(&(vTotal->registro[count]->tamNome), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor nomeEscola 
		int tamNome =  vTotal->registro[count]->tamNome; 
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->nomeEscola = (char*) malloc(sizeof(char) * tamNome);
		//lê o nome da escola e salva no vetRegistro[count].nomeEscola 
		fread(&(vTotal->registro[count]->nomeEscola), tamNome, 1, fp); 

		//leitura do municipio da escola
		//lê o tamanho do registro do municipio da escola e salva no vetRegistro[count].municipio
		fread(&(vTotal->registro[count]->tamMunicipio), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor municipio 	
		int tamMunicipio =  vTotal->registro[count]->tamMunicipio;
		//aloca memória para o vetor municipio
		vTotal->registro[count]->municipio = (char*) malloc(sizeof(char) * tamMunicipio);
		//lê o nome da escola e salva no vetRegistro[count].municipio 
		fread(&(vTotal->registro[count]->municipio), tamMunicipio, 1, fp);
		
		//leitura do endereço da escola
		//lê o tamanho do registro de endereco da escola e salva no vetRegistro[count].endereco
		fread(&(vTotal->registro[count]->tamEndereco), sizeof(int), 1, fp);
		//quantidade de memória que precisa ser alocada pra o vetor endereco		
		int tamEndereco = vTotal->registro[count]->tamEndereco;
		//aloca memória para o vetor nomeEscola
		vTotal->registro[count]->endereco = (char*) malloc(sizeof(char) * tamEndereco);
		//lê o nome da escola e salva no vetRegistro[count].endereco
		fread(&(vTotal->registro[count]->endereco), tamEndereco, 1, fp); 

		//verifica se o endereço da escola é a procurado pelo usuário
		if(strcmp(valor, vTotal->registro[count]->endereco) == 0){  

			count++; //incrementa a posição do vetor vTotal
			if(count == buffer){ //verifica se é necessário dar realloc
				buffer = buffer + 10;
				vTotal = (VETREGISTROS*) realloc(vTotal, buffer);
			}
		}
	}

	vTotal->numElementos = count; //salva a quantidade de registros encontrado

	if(count == 0) //não encontrou o registro a ser buscado pelo usuário
		printf(ERRO_REGISTRO);

	fclose(fp);

	return vTotal;
}

VETREGISTROS* RecuperaTodosRegistros() 
{
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");
	if (fp == NULL)
		return NULL;

	int registroRemovido;
	int counter = 0;
	int bytesRestantes;
	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	REGISTRO* registro;

	fseek(fp, TAMANHO_CABECALHO, SEEK_SET);

	fread(&registroRemovido, sizeof(registroRemovido), 1, fp);
	
	do {
		if (registroRemovido < 0) {	// Caso o arquivo esteja removido, pula o registro.
			fseek(fp, TAMANHO_REGISTRO, SEEK_CUR);
		} else {
			vetRegistros->registro = (REGISTRO**)realloc(vetRegistros->registro, 
					sizeof(REGISTRO*) * (++vetRegistros->numElementos));

			registro = (REGISTRO*)calloc(1, sizeof(REGISTRO));

			fread(&registro->codEscola, sizeof(int), 1, fp);
			fread(registro->dataInicio, sizeof(char), TAMANHO_DATA, fp);
			fread(registro->dataFinal, sizeof(char), TAMANHO_DATA, fp);

			fread(&registro->tamNome, sizeof(int), 1, fp);
			registro->nomeEscola = (char*)calloc(registro->tamNome + 1, sizeof(char));
			fread(registro->nomeEscola, sizeof(char), registro->tamNome, fp);

			fread(&registro->tamMunicipio, sizeof(int), 1, fp);
			registro->municipio = (char*)calloc(registro->tamMunicipio + 1, sizeof(char));
			fread(registro->municipio, sizeof(char), registro->tamMunicipio, fp);

			fread(&registro->tamEndereco, sizeof(int), 1, fp);
			registro->endereco = (char*)calloc(registro->tamEndereco + 1, sizeof(char));
			fread(registro->endereco, sizeof(char), registro->tamEndereco, fp);

			vetRegistros->registro[counter++] = registro;
			bytesRestantes = TAMANHO_REGISTRO - 36 - registro->tamNome;
			bytesRestantes -= registro->tamEndereco + registro->tamMunicipio;

			fseek(fp, bytesRestantes, SEEK_CUR);
		}
		fread(&registroRemovido, sizeof(registroRemovido), 1, fp);
	} while (!feof(fp));

	fclose(fp);

	/*REGISTRO* r;
	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		r = vetRegistros->registro[i];
		printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
			r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
			r->endereco);
	}*/

	return vetRegistros;

/*	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");// Abre o arquivo
	
	VETREGISTROS *vTotal; // vetor com todos os registros
	int buffer = 10;
	int count=0; // realloc do vetor de registros
	
 // registro do momento que está sendo lido (Registro horizontal)
	vTotal = (VETREGISTROS*) malloc(sizeof(VETREGISTROS)); // vTotal é uma struct, logo aloca-se uma struct
	vTotal->registro = (REGISTRO**) malloc(sizeof(REGISTRO*)*buffer); // ** é para aumentar verticalment
	
	char existealgo;
	fread(&(existealgo),1,1,fp);
	
		printf(ERRO_GERAL);
	
	if(existealgo == '0') // significa que não tem nada para ser lido
		return vTotal;

	fseek(fp,4,SEEK_SET); // vai pular direto pra quinta posição, pois as 5 primeiras são cabeçalho
	while(feof(fp)!=0)
	{
		int existe; // Serve para um registro individualmente (Pois o registro pode ter sido apagado)
		fread(&existe,sizeof(int),1,fp); // Saber se existe aquele registro 
		if(existe < 0) 
			fseek(fp,sizeof(REGISTRO),SEEK_CUR);
		/\*
			Como altera algo, utiliza-se endereço, Dentro do vTotal tem o vetor de registros, vai alterar dentro do vetor de registros
			Por alterar pressupoe-se ler o valor daquela linha do arquivo
			Altera o registro da linha atual (linha atual = count)
		*\/
		
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

	fclose(fp);
	
	return vTotal;
*/
}

int comparaCampoDoRegistro(REGISTRO* registro, char* nomeDoCampo, char* valor) {
	if (registro == NULL || nomeDoCampo == NULL || valor == NULL) {
		return 0;
	}

	if (!strcmp(nomeDoCampo, "codEscola"))
		return registro->codEscola == atoi(valor);
	else if (!strcmp(nomeDoCampo, "dataInicio"))
		return !strcmp(registro->dataInicio, valor);
	else if (!strcmp(nomeDoCampo, "dataFinal"))
		return !strcmp(registro->dataFinal, valor);
	else if (!strcmp(nomeDoCampo, "nomeEscola"))
		return !strcmp(registro->nomeEscola, valor);
	else if (!strcmp(nomeDoCampo, "municipio"))
		return !strcmp(registro->municipio, valor);
	else if (!strcmp(nomeDoCampo, "endereco"))
		return !strcmp(registro->endereco, valor);
	
	return 0;
}

VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {

	if (nomeDoCampo == NULL || valor == NULL)
		return NULL;

	VETREGISTROS* todosRegistros = RecuperaTodosRegistros();

	if (todosRegistros == NULL || todosRegistros->numElementos == 0)
		return NULL;

	VETREGISTROS* registrosCompativeis = (VETREGISTROS*)calloc(1, sizeof(VETREGISTROS));
	int counter = 0;

	for (int i = 0; i < todosRegistros->numElementos; ++i) {
		if (comparaCampoDoRegistro(todosRegistros->registro[i], nomeDoCampo, valor)) {
			registrosCompativeis->registro = (REGISTRO**)realloc(registrosCompativeis->registro, 
					sizeof(REGISTRO*) * (++registrosCompativeis->numElementos));

			registrosCompativeis->registro[counter++] = todosRegistros->registro[i];
		} else {
			LiberaRegistro(todosRegistros->registro[i]);
		}
	}

	free(todosRegistros->registro);
	free(todosRegistros);

	return registrosCompativeis;
/*	
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");

	ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA); //confere a consistência do arquivo
	
	fseek(fp, 4, SEEK_SET); //consome o topo da pilha

	if(strcmp(nomeDoCampo, "codEscola") == 0){
		printf("CODIGO ESCOLA\n");
		return RecuperaRegistrosCodEscola(valor);
	}	
	else if(strcmp(nomeDoCampo, "dataInicio") == 0){
		printf("DATA INICIO\n");
		return RecuperaRegistrosDataInicio(valor);
	}
	else if(strcmp(nomeDoCampo, "dataFinal") == 0){
		printf("DATA FINAL\n");
		return RecuperaRegistrosDataFinal(valor);
	}
	else if(strcmp(nomeDoCampo, "nomeEscola") == 0){
		printf("NOME DA ESCOLA\n");
		return RecuperaRegistrosNomeEscola(valor);
	}
	else if(strcmp(nomeDoCampo, "municipio") == 0){
		printf("MUNICIPIO\n");
		return RecuperaRegistrosMunicipio(valor);
	}
	else{
		printf("ENDEREÇO\n");
		return RecuperaRegistrosEndereco(valor);
	}
*/
}

int UltimaPosicaoDoArquivo(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return -1;

	FILE* fp = fopen(nomeArquivo, "rb");
	if (fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	int posicao = ftell(fp);
	fclose(fp);

	return posicao;
}

REGISTRO* RecuperaRegistroPorRRN(int RRN) 
{	
	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return NULL;

	FILE* fp = fopen(ARQUIVO_SAIDA,"rb");
	if (fp == NULL)
		return NULL;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);

	REGISTRO* registro = (REGISTRO*) calloc(1, sizeof(REGISTRO));

	int registroRemovido;
	fread(&registroRemovido, sizeof(registroRemovido), 1, fp);

	if (registroRemovido < 0) {
		fclose(fp);
		return NULL;
	}

	fread(&registro->codEscola, sizeof(int), 1, fp);
	fread(registro->dataInicio, sizeof(char), TAMANHO_DATA, fp);
	fread(registro->dataFinal, sizeof(char), TAMANHO_DATA, fp);

	fread(&registro->tamNome, sizeof(int), 1, fp);
	registro->nomeEscola = (char*)calloc(registro->tamNome + 1, sizeof(char));
	fread(registro->nomeEscola, sizeof(char), registro->tamNome, fp);

	fread(&registro->tamMunicipio, sizeof(int), 1, fp);
	registro->municipio = (char*)calloc(registro->tamMunicipio + 1, sizeof(char));
	fread(registro->municipio, sizeof(char), registro->tamMunicipio, fp);

	fread(&registro->tamEndereco, sizeof(int), 1, fp);
	registro->endereco = (char*)calloc(registro->tamEndereco + 1, sizeof(char));
	fread(registro->endereco, sizeof(char), registro->tamEndereco, fp);

	fclose(fp);
	return registro;
}

int RemocaoLogicaPorRRN(int RRN) {
	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return -1;

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+");
	if (fp == NULL)
		return 0;
	
	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //vai para o byte offset de posição RRN

	int registroExiste;

	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if(registroExiste < 0) //verifica se o registro existe
		return -1;

	int n = -1;
	int topoPilha = TopoDaPilha(ARQUIVO_SAIDA);
	AlteraTopoDaPilha(ARQUIVO_SAIDA, RRN);

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //vai para o byte offset de posição RRN
	fwrite(&n, sizeof(n), 1, fp); //marca o registro como removido
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp); //salva o topo da pilha no registro removido

	printf("Registro removido com sucesso.\n");
	
	fclose(fp);

	return 1;
}
 
void AtualizaRegistroPorRRN(REGISTRO* registro, int RRN) 
{
	if (registro == NULL)
		return;
	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return;

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");
	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);
	fclose(fp);
	if (registroExiste < 0)
		return;

	SubstituiRegistro(ARQUIVO_SAIDA, registro, BYTE_OFFSET(RRN));
}

void DesfragmentaArquivoDeDados() {
	CriaArquivoDeSaida(ARQUIVO_DESFRAGMENTADO);
	VETREGISTROS* vetRegistros = RecuperaTodosRegistros();
	InsereVetorDeRegistros(ARQUIVO_DESFRAGMENTADO, vetRegistros);
	if (remove(ARQUIVO_SAIDA)) {		// Remove o antigo arquivo de saída.
		printf(ERRO_GERAL);
		exit(-8);
	}
	if (rename(ARQUIVO_DESFRAGMENTADO, ARQUIVO_SAIDA)) {	// Muda o nome do novo arquivo para
		printf(ERRO_GERAL);					// o do antigo.
		exit(-8);
	}
	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);
}

int* RecuperaRRNLogicamenteRemovidos() {

	int topoPilha = TopoDaPilha(ARQUIVO_SAIDA);

	int counter = 0;
	int registroExiste;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");
	if (fp == NULL)
		return NULL;

	int* vet = (int*) malloc(sizeof(int));

	while(topoPilha >= 0){ //recupera os RRN enquanto a pilha não estiver vazia
		fseek(fp, BYTE_OFFSET(topoPilha), SEEK_SET);
		fread(&registroExiste, sizeof(registroExiste), 1, fp); //verifica se o registro realmente foi removido

		if(registroExiste < 0) {
			vet = (int*) realloc(vet, sizeof(int) * (++counter + 1));
			vet[0] = counter;
			vet[counter] = topoPilha;

			fread(&topoPilha, sizeof(topoPilha), 1, fp); //salva o novo topo da pilha 
		} else
			topoPilha = -1;
	}

	fclose(fp);

	if (counter > 0) {
		vet = (int*) realloc(vet, sizeof(int) * (++counter + 1));
		vet[0] = counter;
		vet[counter] = topoPilha;
	}

	return vet;
}

void ConfereEntrada(int argc, int valorEsperado) {
	if (argc != valorEsperado) {
		printf(ERRO_GERAL);
		exit(-2);
	}
}

void ImprimeRegistro(REGISTRO* registro) {
	if (registro == NULL)
		return;

	REGISTRO* r = registro;

	printf("%1$d %2$s %3$s %4$d%10$.*4$s%5$s %6$d%10$.*6$s%7$s %8$d %9$s\n", r->codEscola,
		r->dataInicio, r->dataFinal, r->tamNome, r->nomeEscola, r->tamMunicipio,
		r->municipio, r->tamEndereco, r->endereco, " ");
}

void ImpremeVetorDeRegistros(VETREGISTROS *vetRegistros) {
	
	if(vetRegistros == NULL){  //caso algum erro foi encontrado
		printf(ERRO_GERAL);
		return;
	}

	if(vetRegistros->numElementos == 0){ //caso não haja registros
		printf(ERRO_REGISTRO);
		return;
	}

	for(int i = 0; i < vetRegistros->numElementos; i++)
		ImprimeRegistro(vetRegistros->registro[i]);
}

void ImprimeVetor(int* vet) {

	if(vet == NULL){
		printf(ERRO_GERAL);
		return;
	}

	int tam = vet[0];
	
	if(tam < 1){
		printf("Pilha vazia\n");
		return;
	}

	for(int i=1; i<tam; i++)
		printf("%d ", vet[i]);
	printf("\n");
}

REGISTRO* LeRegistroDaEntrada(char* campo[]) {
	REGISTRO* registro = (REGISTRO*)malloc(sizeof(REGISTRO));

	registro->codEscola = atoi(campo[0]);
	strcpy(registro->dataInicio, strcmp(campo[1], "0") ? campo[1] : DATA_VAZIA);
	strcpy(registro->dataFinal, strcmp(campo[2], "0") ? campo[2] : DATA_VAZIA);

	registro->tamNome = strlen(campo[3]);
	registro->nomeEscola = (char*) malloc(sizeof(char) * registro->tamNome);
	strcpy(registro->nomeEscola, campo[3]);

	registro->tamMunicipio = strlen(campo[4]);
	registro->municipio = (char*) malloc(sizeof(char) * registro->tamMunicipio);
	strcpy(registro->municipio, campo[4]);

	registro->tamEndereco = strlen(campo[5]);
	registro->endereco = (char*) malloc(sizeof(char) * registro->tamEndereco);
	strcpy(registro->endereco, campo[5]);

	/*REGISTRO* r = registro;
	printf("%d %s %s %d %s %d %s %d %s\n", r->codEscola, r->dataInicio, r->dataFinal,
		r->tamNome, r->nomeEscola, r->tamMunicipio, r->municipio, r->tamEndereco,
		r->endereco);
	*/
	return registro;
}

int main(int argc, char *argv[]){

	if (argc < 2 || atoi(argv[1]) < 1 || atoi(argv[1]) > 9) {
		printf(ERRO_GERAL);
		return -1;
	}

	VETREGISTROS* vetRegistros = NULL;
	REGISTRO* registro = NULL;
	int* vetPilha = NULL;

	int quantidadeArgumentos[9] = QUANTIDADE_ARGUMENTOS;
	ConfereEntrada(argc, quantidadeArgumentos[atoi(argv[1]) - 1]);

	switch (atoi(argv[1])) {
		case 1:
			vetRegistros = LeituraArquivoDeEntrada(argv[2]);
			CriaArquivoDeSaida(ARQUIVO_SAIDA);
			InsereVetorDeRegistros(ARQUIVO_SAIDA, vetRegistros);

			printf("Arquivo carregado.\n");
			break;
		case 2:
			vetRegistros = RecuperaTodosRegistros();
			ImpremeVetorDeRegistros(vetRegistros);
			break;
		case 3:
			vetRegistros = RecuperaRegistrosPorCampo(argv[2], argv[3]);
			ImpremeVetorDeRegistros(vetRegistros);
			break;
		case 4:
			registro = RecuperaRegistroPorRRN(atoi(argv[2]));
			ImprimeRegistro(registro);
			break;
		case 5:
			RemocaoLogicaPorRRN(atoi(argv[2]));
			break;
		case 6:
			//ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);
			registro = LeRegistroDaEntrada(argv+2);
			InsereRegistro(ARQUIVO_SAIDA, registro);

			printf("Registro inserido com sucesso.\n");
			break;
		case 7:
			registro = LeRegistroDaEntrada(argv+3);
			AtualizaRegistroPorRRN(registro, atoi(argv[2]));
			break;
		case 8:
			ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);
			DesfragmentaArquivoDeDados();
			printf("Arquivo de dados compactado com sucesso.\n");
			break;
		case 9:
			vetPilha = RecuperaRRNLogicamenteRemovidos();
			ImprimeVetor(vetPilha);
			break;
		default:
			printf(ERRO_GERAL);
	}

	return 0;
}
