#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "funcs.h"

void substring(const char *origem, char *destino, int inicio, int comprimento) {
    strncpy(destino, origem + inicio, comprimento);
    destino[comprimento] = '\0';
}

int decodificador(char digito_codigo[8], int lado) { // 0 -> l_code, 1 -> r_code
    int retorno = -1;
    if (lado == 0) {
        for (int i = 0; i < 10; i++) {            
            if (strcmp(digito_codigo, l_code[i]) == 0) {
                retorno = i;
                break;
            }
        }
    } else {
        for (int i = 0; i < 10; i++) {
            if (strcmp(digito_codigo, r_code[i]) == 0) {
                retorno = i;
                break;
            }
        }
    }
    return retorno;
}

int verifica_arquivo_pbm(FILE *arquivo, char **matriz, int *largura, int *altura, int *tamanho) {
    if (arquivo == NULL) {
        printf("Erro: Arquivo não encontrado ou não pode ser aberto.\n");
        return -1; // Arquivo inválido
    }

    char p1[3];

    // Lê o cabeçalho do arquivo
    if (fscanf(arquivo, "%2s %d %d", p1, largura, altura) != 3) {
        printf("Erro: Cabeçalho do arquivo inválido.\n");
        return -1; // Erro ao ler o cabeçalho
    }

    // Verifica se o tipo do arquivo é "P1"
    if (strcmp(p1, "P1") != 0) {
        printf("Erro: Tipo de arquivo não é P1. Encontrado: %s\n", p1);
        return -1; // Tipo de arquivo inválido
    }

    *tamanho = (*largura) * (*altura);
    *matriz = (char *)malloc(*tamanho * sizeof(char));
    if (*matriz == NULL) {
        printf("Erro: Falha na alocação de memória.\n");
        return -1; // Falha na alocação de memória
    }

    // Lê os dados da matriz
    for (int i = 0; i < *tamanho; i++) {
        if (fscanf(arquivo, " %c", &(*matriz)[i]) != 1) {
            printf("Erro: Falha ao ler os dados da matriz.\n");
            free(*matriz); // Libera a memória em caso de erro
            return -1;
        }
    }

    return 1; // Sucesso
}

bool verifica_linha_nula(int indice_linha, int largura, char* matriz) {
    bool ehNula = true;
    for (int j = 0; j < largura; j++) {
        if (matriz[indice_linha * largura + j] == '1') {
            ehNula = false;
            break;
        }
    }

    return ehNula;
}

bool verifica_coluna_nula(int indice_coluna, int altura, int largura, char* matriz) {
    bool ehNula = true;
    for (int i = 0; i < altura; i++) {
        if (matriz[i * largura + indice_coluna] == '1') {
            ehNula = false;
            break;
        }
    }

    return ehNula;
}

bool verifica_espacamento(int *linhas_nulas_acima, int *linhas_nulas_abaixo, int *colunas_nulas_esquerda, int *colunas_nulas_direita, int largura, int altura, char* matriz) {
    for (int i = 0; i < altura; i++) {
        if (verifica_linha_nula(i, largura, matriz)) {
            (*linhas_nulas_acima)++;
        } else {
            break;
        }
    }

    for (int i = altura - 1; i >= 0; i--) {
        if (verifica_linha_nula(i, largura, matriz)) {
            (*linhas_nulas_abaixo)++;
        } else {
            break;
        }
    }

    for (int j = 0; j < largura; j++) {
        if (verifica_coluna_nula(j, altura, largura, matriz)) {
            (*colunas_nulas_esquerda)++;
        } else {
            break;
        }           
    }

    for (int j = largura - 1; j >= 0; j--) {
        if (verifica_coluna_nula(j, altura, largura, matriz)) {
            (*colunas_nulas_direita)++;
        } else {
            break;
        }           
    }

    if (*linhas_nulas_abaixo == *linhas_nulas_acima && *linhas_nulas_abaixo == *colunas_nulas_direita && *linhas_nulas_abaixo == *colunas_nulas_esquerda) {
        return true;
    } else {
        return false;
    }
}

int verifica_barcode(int largura, int altura, char* matriz, char** matriz_digitos) {
    int linhas_nulas_acima = 0, linhas_nulas_abaixo = 0, colunas_nulas_esquerda = 0, colunas_nulas_direita = 0;

    if (!verifica_espacamento(&linhas_nulas_acima, &linhas_nulas_abaixo, &colunas_nulas_esquerda, &colunas_nulas_direita, largura, altura, matriz)) {        
        return -1;
    }

    char linha_teste[largura + 1];
    int altura_util = altura - 2*linhas_nulas_acima;
    int largura_util = largura - 2*colunas_nulas_esquerda;
    int area_barra = largura_util/67;

    for (int j = 0; j < largura; j++) {
        linha_teste[j] = matriz[linhas_nulas_acima*largura + j];
    }
    linha_teste[largura] = '\0';
    
    for (int i = linhas_nulas_acima; i < linhas_nulas_acima + altura_util; i++) {
        char linha_iteracao[largura + 1];
        for (int j = 0; j < largura; j++) {
            linha_iteracao[j] = matriz[i*largura + j];
        }
        linha_iteracao[largura] = '\0';

        if (strcmp(linha_teste, linha_iteracao) != 0) {
            return -1;
        }
    }

    char codigo_reduzido[67];
    int i_cod_reduzido = 0;

    for (int i = colunas_nulas_esquerda; i < largura - colunas_nulas_esquerda; i += area_barra) {
        codigo_reduzido[i_cod_reduzido] = matriz[linhas_nulas_acima*largura + i];
        i_cod_reduzido++;
    }
    codigo_reduzido[67] = '\0';

    char marcador_inicial[4], marcador_central[6], marcador_final[4];

    substring(codigo_reduzido, marcador_inicial, 0, 3);
    substring(codigo_reduzido, marcador_central, 31, 5);
    substring(codigo_reduzido, marcador_final, 64, 3);

    if (strcmp(marcador_inicial, "101") != 0 || strcmp(marcador_central, "01010") != 0 || strcmp(marcador_final, "101") != 0) {
        return -1;
    }

    int contador = 0;
    for (int i = 3; i < 31; i += 7) {
        matriz_digitos[contador] = (char *)malloc(8 * sizeof(char));
        substring(codigo_reduzido, matriz_digitos[contador], i, 7);
        contador++;
    }
    for (int i = 36; i < 64; i += 7) {
        matriz_digitos[contador] = (char *)malloc(8 * sizeof(char));
        substring(codigo_reduzido, matriz_digitos[contador], i, 7);
        contador++;
    }

    char codigo_decodificado[9];
    for (int i = 0; i < 8; i++) {
        int lado = (i < 4) ? 0 : 1;
        int numero = decodificador(matriz_digitos[i], lado);
        if (numero == -1) {
            return -1;
        } else {
            codigo_decodificado[i] = numero + '0';
        }
    }

    codigo_decodificado[8] = '\0';
    
    if (!ultimo_digito(codigo_decodificado)) {
        return -1;
    }

    return 1; // Sucesso
}

int main(int argc, char *argv[]) {
    // Verifica se o caminho do arquivo foi fornecido
    if (argc < 2) {
        printf("Erro: Caminho do arquivo não fornecido.\n");
        exit(-1);
    }

    // Obtém o caminho do arquivo a partir dos argumentos
    const char *caminho_arquivo = argv[1];

    FILE *arquivo = fopen(caminho_arquivo, "r");

    int tamanho, largura, altura;
    char *matriz = NULL;

    if (verifica_arquivo_pbm(arquivo, &matriz, &largura, &altura, &tamanho) == -1) {
        printf("Arquivo inválido");
        fclose(arquivo);
        exit(-1);
    }

    char* matriz_digitos[8];
    
    if(verifica_barcode(largura, altura, matriz, matriz_digitos) == -1) {
        printf("Código de barras inválido.");
        free(matriz);
        for (int i = 0; i < 8; i++) {
            free(matriz_digitos[i]);
        }
        fclose(arquivo);
        exit(-1);
    }

    printf("Código decodificado: ");
    for (int i = 0; i < 8; i++) {
        int lado = (i < 4) ? 0 : 1;
        int numero = decodificador(matriz_digitos[i], lado);
        printf("%d", numero);
    }
    printf("\n");

    // Libera a memória e fecha o arquivo
    free(matriz);
    for (int i = 0; i < 8; i++) {
        free(matriz_digitos[i]);
    }
    fclose(arquivo);
    
    return 0;
}