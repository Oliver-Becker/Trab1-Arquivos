/*
	Trabalho realizado para a disciplina SCC0215 - Organização de Arquivos,
	ministrada pela professora Cristina Dutra de Aguiar Ciferri.

	Clara Rosa Silveira 	-  9021070
	Óliver Savastano Becker - 10284890
	Rafael Medeiros Vieira	- 10284239
						ICMC | USP | 1º semestre 2018
*/
#include <registro.h>
#include <arquivo.h>

#define QUANTIDADE_ARGUMENTOS {3, 2, 4, 3, 3, 8, 9, 2, 2}
#define ERRO_GERAL "Falha no processamento do arquivo.\n"
#define IMPRIME_ERRO_GERAL printf(ERRO_GERAL);
#define ERRO_REGISTRO "Registro inexistente.\n"
#define IMPRIME_ERRO_REGISTRO printf(ERRO_REGISTRO);
#define ARQUIVO_SAIDA "saida.bin"
#define ARQUIVO_DESFRAGMENTADO "desfragmentacao.bin"

typedef struct {
	REGISTRO** registro;
	int numElementos;
} VETREGISTROS;

int SubstituiRegistro(char* nomeArquivo, REGISTRO* registro, int RRN) {
	
	if (nomeArquivo == NULL || registro == NULL)
		return 0; //verifica se o nome do arquivo e o regitro são válidos

	FILE* fp = fopen(nomeArquivo, "rb+"); //abre o arquivo
	if (fp == NULL) //verifica se o arquivo é válido
		return 0;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //modifica o ponteiro para o byte offset do registro
	int registroExiste;					   //a ser substituido
	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if (registroExiste < 0) {	// verifica se o registro não existe
		int topoPilha = TopoDaPilha(nomeArquivo);
		if (topoPilha != RRN) { //verifica se o topo da pilha é igual ao RRN dado pelo usuário
			fclose(fp); //fecaha o arquivo caso não seja
			return 0;
		}
		fread(&topoPilha, sizeof(topoPilha), 1, fp);
		AlteraTopoDaPilha(nomeArquivo, topoPilha); 
	}

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //modifica o ponteiro para o byte offset do registro
	registroExiste = 0;					   //a ser substituido

	//ecreve os dados do registro, substituindo os dados antigos
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

	return 1;
}

int InsereRegistro(char* nomeArquivo, REGISTRO* registro) {
	
	if (nomeArquivo == NULL || registro == NULL) //verifica se o nome do arquivo e o registro são válidos
		return 0;

	FILE* fp = fopen(nomeArquivo, "rb+"); //abre o arquivo
	if (fp == NULL) //verifica se o arquivo é válido
		return 0;

	fseek(fp, 1, SEEK_SET); //pula o status do arquivo

	int topoPilha;
	fread(&topoPilha, sizeof(topoPilha), 1, fp); //lê o topo da pilha

	fclose(fp);

	//decide de registro vai substituir algum registro removido
	//ou se deve ser inserido no final
	return (topoPilha == -1) ? AcrescentaRegistroNoFinal(nomeArquivo, registro) : 
		SubstituiRegistro(nomeArquivo, registro, topoPilha);
}

void LiberaVetorDeRegistros(VETREGISTROS* vetRegistros) {
	
	if (vetRegistros == NULL) //verifica se o vetor é válido
		return;

	for (int i = 0; i < vetRegistros->numElementos; ++i) //libera a memória do vetor de 
		LiberaRegistro(vetRegistros->registro[i]);		 //registros alocado

	free(vetRegistros->registro);
	free(vetRegistros);

	vetRegistros = NULL;
}

int InsereVetorDeRegistros(char* nomeArquivo, VETREGISTROS* vetRegistros) {
	
	if (nomeArquivo == NULL || vetRegistros == NULL)
		return 0; //verifica se o nome do arquivo e o vetRegsitros são válidos 

	if (vetRegistros->numElementos == 0) //verifica se há elementos no vetRegistros
		return -1;

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0); //altera o status do arquivo para 0

	for (int i = 0; i < vetRegistros->numElementos; ++i) //insere os regitros no arquivo desejado
		InsereRegistro(nomeArquivo, vetRegistros->registro[i]);

	LiberaVetorDeRegistros(vetRegistros); //libera o vetor vetRegistros

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1); //altera o status do arquivo para 1
	return 1;
}

VETREGISTROS* LeituraArquivoDeEntrada(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return NULL;

	FILE* fp = fopen(nomeArquivo, "r");	// Abre o arquivo de entrada no modo de leitura.
	if (fp == NULL)				// Caso o arquivo não exista, imprime erro.
		return NULL;

	// Aloca espaço para o VETREGISTROS que será retornado pela função.
	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	REGISTRO* registro; 	// Variável de registro auxiliar.

	int counter = 0; 	// Contador auxiliar para saber quantos registros já foram lidos.
	char* campo[6];		// Strings para armazenarem os campos lidos do arquivo de entrada.

	// Loop para ler todos os registros até que encontre o fim do arquivo.
	while (fscanf(fp, "%m[^;\n]", &campo[0]) != EOF) {
		for (int i = 1; i < 6; ++i)			// Caso o registro exista, lê os 5
			fscanf(fp, "%*c%m[^;\n]", &campo[i]);		// campos restantes.
		fgetc(fp);					// Consome o \n.

		// Vai aumentando o espaço do vetor de registros conforme é lido da entrada.
		vetRegistros->registro = (REGISTRO**)realloc(vetRegistros->registro, 
				sizeof(REGISTRO*) * (++vetRegistros->numElementos));

		// Aloca espaço para o registro que será anexado ao vetor.
		registro = (REGISTRO*)malloc(sizeof(REGISTRO));

		// Atribui para cada campo da struct o valor correspondente, com base na ordem de
		// leitura dos campos do arquivo de entrada.
		registro->codEscola = atoi(campo[0]);
		// Caso o campo seja vazio (campo null), atribui a campo de zeros para o campo.
		strcpy(registro->dataInicio, (campo[4] != NULL) ? campo[4] : DATA_VAZIA);
		strcpy(registro->dataFinal, (campo[5] != NULL) ? campo[5] : DATA_VAZIA);
		registro->tamNome = (campo[1] != NULL) ? strlen(campo[1]) : 0;
		registro->nomeEscola = (campo[1] != NULL) ? campo[1] : CAMPO_VAZIO;
		registro->tamMunicipio = (campo[2] != NULL) ? strlen(campo[2]) : 0;
		registro->municipio = (campo[2] != NULL) ? campo[2] : CAMPO_VAZIO;
		registro->tamEndereco = (campo[3] != NULL) ? strlen(campo[3]) : 0;
		registro->endereco = (campo[3] != NULL) ? campo[3] : CAMPO_VAZIO;

		// Atribui o registro ao vetor.
		vetRegistros->registro[counter++] = registro;

		// Libera a memória que não será mais utilizada.
		free(campo[0]);
		if (campo[4] != NULL) free(campo[4]);
		if (campo[5] != NULL) free(campo[5]);
	}

	fclose(fp); // Fecha o arquivo aberto.

	return vetRegistros; // Retorna o vetor com todos os registros lidos.
}

VETREGISTROS* RecuperaTodosRegistros() {

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb"); // abre o arquivo
	if (fp == NULL) // Se o arquivo não existir, retorna NULL  
		return NULL;
 
	int registroExiste;
	int counter = 0;
	int bytesRestantes;
	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	// A struct registros é alocada, com Calloc para numElementos ser inicializado com 0
	REGISTRO* registro;
 
	fseek(fp, TAMANHO_CABECALHO, SEEK_SET); // pula os bytes do cabeçalho
 
	fread(&registroExiste, sizeof(registroExiste), 1, fp); // checa se o arquvivo está com registro removido
 
	do {
		if (registroExiste < 0) {	// Caso o arquivo esteja removido, pula o registro.      
			fseek(fp, TAMANHO_REGISTRO, SEEK_CUR);
		} else { // senão, será lido o registro e passado para o vetRegistros
			vetRegistros->registro = (REGISTRO**)realloc(vetRegistros->registro,
			// aumenta o tamanho do vetor de registros para acrescentar o atual 
					sizeof(REGISTRO*) * (++vetRegistros->numElementos));
 
			registro = (REGISTRO*)calloc(1, sizeof(REGISTRO)); // aloca-se um registro auxiliar para receber os dados
 
			
			// Nos 3 primeiros campos -> codEscola, dataInicio, dataFinal, 
			// tem-se um tamanho fixo que será lido, dessa forma apenas recebe os 
			// valores diretamente.
			// Para os próximos campos, referentes a strings, como: 
			// municipio/endereço/nomeEscola tem-se o tamanho predeterminado por 
			// uma variável recebida previamente.
			// Assim, aloca-se no momento, o tamanho exato da string utilizada.
			
 
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
 
			fseek(fp, bytesRestantes, SEEK_CUR); // consome os bytes que sobraram do registro (lixos)
		}
		fread(&registroExiste, sizeof(registroExiste), 1, fp); // descobre se o possível próximo registro existe 
	} while (!feof(fp)); // se chegou no final do arquivo sai
 
	fclose(fp); // fecha arquivo
	return vetRegistros;
}

VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {

	if (nomeDoCampo == NULL || valor == NULL) 
		return NULL; //verifica se o nome do campo e o valor são válidos

	VETREGISTROS* todosRegistros = RecuperaTodosRegistros(); //armazena todos os registros em um vetor

	if (todosRegistros == NULL || todosRegistros->numElementos == 0)
		return todosRegistros; //verifica se há regitros armazenados e se o vetor é válido

	VETREGISTROS* registrosCompativeis = (VETREGISTROS*)calloc(1, sizeof(VETREGISTROS));
	int counter = 0;

	for (int i = 0; i < todosRegistros->numElementos; ++i) { //procura por registros compatíveis
		if (ComparaCampoDoRegistro(todosRegistros->registro[i], nomeDoCampo, valor)) { //comparando o campo
			registrosCompativeis->registro = (REGISTRO**)realloc(registrosCompativeis->registro, 
					sizeof(REGISTRO*) * (++registrosCompativeis->numElementos)); //e alocando memória caso seja encontrado

			registrosCompativeis->registro[counter++] = todosRegistros->registro[i];
		} else {
			LiberaRegistro(todosRegistros->registro[i]); //libera o registro caso não seja compatível com a busca
		}
	}

	free(todosRegistros->registro);
	free(todosRegistros);

	return registrosCompativeis; //retorna os registros encontrados
}

int UltimaPosicaoDoArquivo(char* nomeArquivo) {
	
	//retona qual é a uultima posição do arquivo
	if (nomeArquivo == NULL) //verifica se o nomo do arquivo é vaálido
		return -1;

	FILE* fp = fopen(nomeArquivo, "rb"); //abreo arquivo em modo leitura
	if (fp == NULL) //verifica se o arquivo é válido
		return -1;

	fseek(fp, 0, SEEK_END); //muda o ponteiro para o ultimo byte offset do arquivo
	int posicao = ftell(fp); //salva esta informção na variável posição
	fclose(fp);

	return posicao;
}

REGISTRO* RecuperaRegistroPorRRN(int RRN) {

	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return NULL; //verifica se o RRN dado pelo usuário é válido

	FILE* fp = fopen(ARQUIVO_SAIDA,"rb"); //abre o arquivo no modo leituta
	if (fp == NULL) //verifica se o arquivo é válido
		return NULL; 

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //muda a o ponteiro para o byte offset do registro
										   //a ser recuperado
	REGISTRO* registro = (REGISTRO*) calloc(1, sizeof(REGISTRO));

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp); 

	if (registroExiste < 0) { //verifica se o registro não foi removido
		fclose(fp);			  // e retorna erro caso tenha sido
		return NULL;
	}

	//faz a devida leitura do registro e aloca memória para armazenar a informações
	//na varável registro
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

	return registro; //retorna a variável registro com as informações encontradas
}

int RemocaoLogicaPorRRN(int RRN) {

	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return -1; //verifica se o RRN dado pelo usuário é válido

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb+"); //abre o arquivo
	if (fp == NULL) //retorna erro caso o arquivo não seja válido
		return 0;
	
	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //vai para o byte offset de posição RRN

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if(registroExiste < 0) //verifica se o registro existe
		return -1;

	int n = -1;
	int topoPilha = TopoDaPilha(ARQUIVO_SAIDA); //verifica o topo da pilha atual
	AlteraTopoDaPilha(ARQUIVO_SAIDA, RRN); //e atualiza para o novo RRN do registro a ser removido

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //vai para o byte offset de posição RRN
	fwrite(&n, sizeof(n), 1, fp); //marca o registro como removido
	fwrite(&topoPilha, sizeof(topoPilha), 1, fp); //salva o topo da pilha no registro removido

	fclose(fp);

	return 1;
}
 
int AtualizaRegistroPorRRN(REGISTRO* registro, int RRN) {

	if (registro == NULL) //verifica se o regitro é válido
		return 0;
	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return -1; //verifica se o RRN dado pelo usuário é válido

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb"); //abre o arquivo no modo de leitura
	if (fp == NULL)
		return 0;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET); //move o ponteiro para o registro de RRN dado pelo usuário

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp); //verifica se o regitro não foi removido
	fclose(fp);
	if (registroExiste < 0) //retorna erro caso o registro já tenha sido removido
		return -1;

	return SubstituiRegistro(ARQUIVO_SAIDA, registro, RRN); //substitui o regitro caso esteja tudo certo
}

int DesfragmentaArquivoDeDados() {

	CriaArquivoDeSaida(ARQUIVO_DESFRAGMENTADO); //cria um novo arquivo de saíd
	VETREGISTROS* vetRegistros = RecuperaTodosRegistros(); //recupera todos os registros existentes e armazena
	InsereVetorDeRegistros(ARQUIVO_DESFRAGMENTADO, vetRegistros); //em um vetor para ser inserido no novo arquivo
	if (remove(ARQUIVO_SAIDA))		// Remove o antigo arquivo de saída.
		return 0;
	if (rename(ARQUIVO_DESFRAGMENTADO, ARQUIVO_SAIDA))	// Muda o nome do novo arquivo para 'saida.bin'
		return 0;

	return 1;
}

int* RecuperaRRNLogicamenteRemovidos() {

	int topoPilha = TopoDaPilha(ARQUIVO_SAIDA); //salva o topo da pilha para verficar

	int counter = 0;							//se é diferente de -1
	int registroExiste;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb"); //abre o arquivo em modo de leitura	
	if (fp == NULL)  //retorna erro caso o arquivo não seja válido
		return NULL;

	int* vet = (int*) calloc(1, sizeof(int));

	while(topoPilha >= 0){ //recupera os RRN enquanto a pilha não estiver vazia
		fseek(fp, BYTE_OFFSET(topoPilha), SEEK_SET);
		//verifica se o registro realmente foi removido
		fread(&registroExiste, sizeof(registroExiste), 1, fp);

		if(registroExiste < 0) { //verifica se o registro já foi removido
			vet = (int*) realloc(vet, sizeof(int) * (++counter + 1)); //realoca mais uma posição do vetor
			vet[0] = counter; //usa a posição 0 do vetor como marcador da quantidade de registros
			vet[counter] = topoPilha;	//encontrados

			fread(&topoPilha, sizeof(topoPilha), 1, fp); //salva o novo topo da pilha 
		} else {
			topoPilha = -1; 
			free(vet); //desaloca o vetor caso não haja registros removidos
			return NULL;
		}
	}

	fclose(fp);

	if (counter > 0) { //se houver registros removidos, é necessario alocar mais uma posição do vetor
		vet = (int*) realloc(vet, sizeof(int) * (++counter + 1));
		vet[0] = counter;
		vet[counter] = topoPilha; //e salvar o valor final do topo da pilha
	}

	return vet; //retorna o vetor com os resultados encontrados
}

int ImpremeVetorDeRegistros(VETREGISTROS *vetRegistros) {
	
	if(vetRegistros == NULL)		//caso algum erro foi encontrado
		return 0;

	if(vetRegistros->numElementos == 0)	//caso não haja registros
		return -1;

	for(int i = 0; i < vetRegistros->numElementos; i++) 
		ImprimeRegistro(vetRegistros->registro[i]); //imprime os registros armazenados

	return 1;
}

int ImprimeVetor(int* vet) { //imprimime o vetor com os RRNs encontrados

	if(vet == NULL)
		return 0;

	int tam = vet[0];
	
	if(tam < 1)
		return -2;

	for(int i=1; i<tam; i++)
		printf("%d ", vet[i]);
	printf("\n");

	return 1;
}

int ImprimeErro(int erro) { //verifica qual é o retorno da função e imprime seu respectivo erro
	switch(erro) {
		case -2:
			printf("Pilha vazia.\n");
			return 3;
		case -1:
			IMPRIME_ERRO_REGISTRO
			return 2;
		case 0:
			IMPRIME_ERRO_GERAL
			return 1;
		default:
			return 0;
	}
}

int Funcionalidade1(char* arquivoEntrada) { //função de leitura dos registros
	VETREGISTROS* vetRegistros = LeituraArquivoDeEntrada(arquivoEntrada);
	CriaArquivoDeSaida(ARQUIVO_SAIDA);
	int retornoFuncao = InsereVetorDeRegistros(ARQUIVO_SAIDA, vetRegistros);

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

	if (retornoFuncao > 0) {
		printf("Arquivo carregado.\n");
		return 0;
	}

	printf("Falha no carregamento do arquivo.\n");
	return 1;
}

int Funcionalidade2() { //função que recupera todos os registros
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		VETREGISTROS* vetRegistros = RecuperaTodosRegistros();

		retornoFuncao = ImpremeVetorDeRegistros(vetRegistros);
	}

	return ImprimeErro(retornoFuncao); 
}

int Funcionalidade3(char* nomeDoCampo, char* valor) { //função que recupera registro por campo
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		VETREGISTROS* vetRegistros = RecuperaRegistrosPorCampo(nomeDoCampo, valor);

		retornoFuncao = ImpremeVetorDeRegistros(vetRegistros);
	}

	return ImprimeErro(retornoFuncao); 
}

int Funcionalidade4(char* RRN) { //funçao que recupera registro por RRN
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		REGISTRO* registro = RecuperaRegistroPorRRN(atoi(RRN));

		retornoFuncao = ImprimeRegistro(registro);
	}

	return ImprimeErro(retornoFuncao); 
}

int Funcionalidade5(char* RRN) { //função de remoção lógica dos registros
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0);
		retornoFuncao = RemocaoLogicaPorRRN(atoi(RRN));
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

		if (retornoFuncao > 0)
			printf("Registro removido com sucesso.\n");
	}

	return ImprimeErro(retornoFuncao); 
}

int Funcionalidade6(char* valoresCampo[]) {//função de inserção lógica dos regustros
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		REGISTRO* registro = InsereCamposEmRegistro(valoresCampo);

		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0);
		retornoFuncao = InsereRegistro(ARQUIVO_SAIDA, registro);
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

		if (retornoFuncao > 0)
			printf("Registro inserido com sucesso.\n");
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade7(char* RRN, char* valoresCampo[]) { //função de atualização de regitro por RRN
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		REGISTRO* registro = InsereCamposEmRegistro(valoresCampo);

		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0);
		retornoFuncao = AtualizaRegistroPorRRN(registro, atoi(RRN));
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

		if (retornoFuncao > 0)
			printf("Registro alterado com sucesso.\n");
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade8() { //função de desfragmentação do arquivo
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		retornoFuncao = DesfragmentaArquivoDeDados();
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

		if (retornoFuncao > 0)
			printf("Arquivo de dados compactado com sucesso.\n");
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade9() { //função de recuperação dos RRNs logicamente removidos
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		int* vetPilha = RecuperaRRNLogicamenteRemovidos();

		retornoFuncao = ImprimeVetor(vetPilha);
	}

	return ImprimeErro(retornoFuncao);
}

void ConfereEntrada(int argc, int valorEsperado) {
	if (argc != valorEsperado) { //verifica se a quantidade de argumentos é válida
		printf(ERRO_GERAL);
		exit(-2);
	}
}

int main(int argc, char *argv[]){

	if (argc < 2 || atoi(argv[1]) < 1 || atoi(argv[1]) > 9) {
		printf(ERRO_GERAL); //verifica se os argumentos da entrada são válidos
		return -1;
	}

	// Vetor contendo quantos argumentos cada funcionalidade deve ter para ser executada
	// corretamente.
	int quantidadeArgumentos[9] = QUANTIDADE_ARGUMENTOS;
	// Chama a função para conferir a quantidade de argumentos com base em qual é a funcionalidade
	// a ser executada.
	ConfereEntrada(argc, quantidadeArgumentos[atoi(argv[1]) - 1]);

	switch (atoi(argv[1])) {
		case 1:
			return Funcionalidade1(argv[2]);
		case 2:
			return Funcionalidade2();
		case 3:
			return Funcionalidade3(argv[2], argv[3]);
		case 4:
			return Funcionalidade4(argv[2]);
		case 5:
			return Funcionalidade5(argv[2]);
		case 6:
			return Funcionalidade6(argv+2);
		case 7:
			return Funcionalidade7(argv[2], argv+3);
		case 8:
			return Funcionalidade8();
		case 9:
			return Funcionalidade9();
		default:
			printf(ERRO_GERAL);
	}

	return -1;
}
