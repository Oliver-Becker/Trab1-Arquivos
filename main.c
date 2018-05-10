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
		return 0;

	FILE* fp = fopen(nomeArquivo, "rb+");
	if (fp == NULL)
		return 0;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);
	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if (registroExiste < 0) {	// Caso o registro não exista.
		int topoPilha = TopoDaPilha(nomeArquivo);
		if (topoPilha != RRN) {
			fclose(fp);
			return 0;
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

	return 1;
}

int InsereRegistro(char* nomeArquivo, REGISTRO* registro) {
	if (nomeArquivo == NULL || registro == NULL)
		return 0;

	FILE* fp = fopen(nomeArquivo, "rb+");
	if (fp == NULL)
		return 0;

	fseek(fp, 1, SEEK_SET);

	int topoPilha;
	fread(&topoPilha, sizeof(topoPilha), 1, fp);

	fclose(fp);

	return (topoPilha == -1) ? AcrescentaRegistroNoFinal(nomeArquivo, registro) : 
		SubstituiRegistro(nomeArquivo, registro, topoPilha);
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

int InsereVetorDeRegistros(char* nomeArquivo, VETREGISTROS* vetRegistros) {
	if (nomeArquivo == NULL || vetRegistros == NULL)
		return 0;

	if (vetRegistros->numElementos == 0)
		return -1;

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 0);

	for (int i = 0; i < vetRegistros->numElementos; ++i) {
		InsereRegistro(nomeArquivo, vetRegistros->registro[i]);
	}

	LiberaVetorDeRegistros(vetRegistros);

	AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);
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
	char* campo[6];	// Strings para armazenarem os campos lidos do arquivo de entrada.

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

	fclose(fp);

	return vetRegistros;
}

VETREGISTROS* RecuperaTodosRegistros() 
{
	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");
	if (fp == NULL)
		return NULL;

	int registroExiste;
	int counter = 0;
	int bytesRestantes;
	VETREGISTROS* vetRegistros = (VETREGISTROS*) calloc(1, sizeof(VETREGISTROS));
	REGISTRO* registro;

	fseek(fp, TAMANHO_CABECALHO, SEEK_SET);

	fread(&registroExiste, sizeof(registroExiste), 1, fp);
	
	do {
		if (registroExiste < 0) {	// Caso o arquivo esteja removido, pula o registro.
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
		fread(&registroExiste, sizeof(registroExiste), 1, fp);
	} while (!feof(fp));

	fclose(fp);
	return vetRegistros;
}

VETREGISTROS* RecuperaRegistrosPorCampo(char* nomeDoCampo, char* valor) {

	if (nomeDoCampo == NULL || valor == NULL)
		return NULL;

	VETREGISTROS* todosRegistros = RecuperaTodosRegistros();

	if (todosRegistros == NULL || todosRegistros->numElementos == 0)
		return todosRegistros;

	VETREGISTROS* registrosCompativeis = (VETREGISTROS*)calloc(1, sizeof(VETREGISTROS));
	int counter = 0;

	for (int i = 0; i < todosRegistros->numElementos; ++i) {
		if (ComparaCampoDoRegistro(todosRegistros->registro[i], nomeDoCampo, valor)) {
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

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);

	if (registroExiste < 0) {
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

	fclose(fp);

	return 1;
}
 
int AtualizaRegistroPorRRN(REGISTRO* registro, int RRN) 
{
	if (registro == NULL)
		return 0;
	if (RRN < 0 || BYTE_OFFSET(RRN) > (UltimaPosicaoDoArquivo(ARQUIVO_SAIDA) - TAMANHO_REGISTRO))
		return -1;

	FILE* fp = fopen(ARQUIVO_SAIDA, "rb");
	if (fp == NULL)
		return 0;

	fseek(fp, BYTE_OFFSET(RRN), SEEK_SET);

	int registroExiste;
	fread(&registroExiste, sizeof(registroExiste), 1, fp);
	fclose(fp);
	if (registroExiste < 0)
		return -1;

	return SubstituiRegistro(ARQUIVO_SAIDA, registro, RRN);
}

int DesfragmentaArquivoDeDados() {
	CriaArquivoDeSaida(ARQUIVO_DESFRAGMENTADO);
	VETREGISTROS* vetRegistros = RecuperaTodosRegistros();
	InsereVetorDeRegistros(ARQUIVO_DESFRAGMENTADO, vetRegistros);
	if (remove(ARQUIVO_SAIDA))		// Remove o antigo arquivo de saída.
		return 0;
	if (rename(ARQUIVO_DESFRAGMENTADO, ARQUIVO_SAIDA))	// Muda o nome do novo arquivo para
		return 0;

	return 1;
}

int* RecuperaRRNLogicamenteRemovidos() {

	int topoPilha = TopoDaPilha(ARQUIVO_SAIDA);

	int counter = 0;
	int registroExiste;

	FILE *fp = fopen(ARQUIVO_SAIDA, "rb");
	if (fp == NULL)
		return NULL;

	int* vet = (int*) calloc(1, sizeof(int));

	while(topoPilha >= 0){ //recupera os RRN enquanto a pilha não estiver vazia
		fseek(fp, BYTE_OFFSET(topoPilha), SEEK_SET);
		//verifica se o registro realmente foi removido
		fread(&registroExiste, sizeof(registroExiste), 1, fp);

		if(registroExiste < 0) {
			vet = (int*) realloc(vet, sizeof(int) * (++counter + 1));
			vet[0] = counter;
			vet[counter] = topoPilha;

			fread(&topoPilha, sizeof(topoPilha), 1, fp); //salva o novo topo da pilha 
		} else {
			topoPilha = -1;
			free(vet);
			return NULL;
		}
	}

	fclose(fp);

	if (counter > 0) {
		vet = (int*) realloc(vet, sizeof(int) * (++counter + 1));
		vet[0] = counter;
		vet[counter] = topoPilha;
	}

	return vet;
}

int ImpremeVetorDeRegistros(VETREGISTROS *vetRegistros) {
	
	if(vetRegistros == NULL)		//caso algum erro foi encontrado
		return 0;

	if(vetRegistros->numElementos == 0)	//caso não haja registros
		return -1;

	for(int i = 0; i < vetRegistros->numElementos; i++)
		ImprimeRegistro(vetRegistros->registro[i]);

	return 1;
}

int ImprimeVetor(int* vet) {

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

int ImprimeErro(int erro) {
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

int Funcionalidade1(char* arquivoEntrada) {
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

int Funcionalidade2() {
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		VETREGISTROS* vetRegistros = RecuperaTodosRegistros();

		retornoFuncao = ImpremeVetorDeRegistros(vetRegistros);
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade3(char* nomeDoCampo, char* valor) {
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		VETREGISTROS* vetRegistros = RecuperaRegistrosPorCampo(nomeDoCampo, valor);

		retornoFuncao = ImpremeVetorDeRegistros(vetRegistros);
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade4(char* RRN) {
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		REGISTRO* registro = RecuperaRegistroPorRRN(atoi(RRN));

		retornoFuncao = ImprimeRegistro(registro);
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade5(char* RRN) {
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

int Funcionalidade6(char* valoresCampo[]) {
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

int Funcionalidade7(char* RRN, char* valoresCampo[]) {
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

int Funcionalidade8() {
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		retornoFuncao = DesfragmentaArquivoDeDados();
		AlteraStatusDoArquivo(ARQUIVO_SAIDA, 1);

		if (retornoFuncao > 0)
			printf("Arquivo de dados compactado com sucesso.\n");
	}

	return ImprimeErro(retornoFuncao);
}

int Funcionalidade9() {
	int retornoFuncao = ConfereConsistenciaDoArquivo(ARQUIVO_SAIDA);

	if (retornoFuncao > 0) {
		int* vetPilha = RecuperaRRNLogicamenteRemovidos();

		retornoFuncao = ImprimeVetor(vetPilha);
	}

	return ImprimeErro(retornoFuncao);
}

void ConfereEntrada(int argc, int valorEsperado) {
	if (argc != valorEsperado) {
		printf(ERRO_GERAL);
		exit(-2);
	}
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
