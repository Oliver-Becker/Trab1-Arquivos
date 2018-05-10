#include <arquivo.h>

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

	FILE* fp = fopen(nomeArquivo, "rb+");
	if (fp == NULL)
		return;

	fwrite(&status, sizeof(status), 1, fp);
	fclose(fp);
}

int ConfereConsistenciaDoArquivo(char* nomeArquivo) {
	if (nomeArquivo == NULL)
		return 0;

	FILE* fp = fopen(nomeArquivo, "rb");
	if (fp == NULL)
		return 0;

	char status;
	fread(&status, sizeof(status), 1, fp);
	fclose(fp);

	return status;
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

