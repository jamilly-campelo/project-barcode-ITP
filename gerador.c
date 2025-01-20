#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "funcs.h"

bool verificar_quantidade_numerica(char *digitos){
    if (strlen(digitos) < 8){
        printf("Identificador não possui 8 dígitos\n");
        return false;
    }

    return true;
}

bool verificar_valor_numerico(char *digitos) {
    for (int i = 0; i < 8; i++) {
        if (digitos[i] < '0' || digitos[i] > '9') {
            printf("Identificador contém valores não numéricos\n");
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("Erro: identificador do código de barras não fornecido.\n");
        exit(-1);
    } 
    char *digitos = argv[1];
    int altura = 50, espaco_lateral = 0, quantidade_pixels = 1; // parâmetros opcionais padrão

    if (!verificar_quantidade_numerica(digitos))
        exit(-1);  

    if (!verificar_valor_numerico(digitos))
        exit(-1);
    
    if (!ultimo_digito(digitos)){
        printf("Dígito verificador não é válido.\n");
        exit(-1);
    }

    // nome do arquivo padrão: "barcode_00000000.pbm"
    size_t tamanho = strlen("barcode_") + strlen(digitos) + 1;
    char *nome_do_arquivo = (char *)malloc(tamanho);

    if (!nome_do_arquivo){
        printf("Memória não alocada.\n");
        exit(-1);
    }

    strcpy(nome_do_arquivo, "barcode_");
    strcat(nome_do_arquivo, digitos);
    strcat(nome_do_arquivo, ".pbm");

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-espacamento") == 0 && i + 1 < argc) {
            espaco_lateral = atoi(argv[i+1]);
            i++;
        }

        if (strcmp(argv[i], "-pixels") == 0 && i + 1 < argc) {
            quantidade_pixels = atoi(argv[i+1]);
            i++;
        }

        if (strcmp(argv[i], "-altura") == 0 && i + 1 < argc) {
            altura = atoi(argv[i+1]);
            i++;
        }

        if (strcmp(argv[i], "-nome") == 0 && i + 1 < argc) {
            tamanho = strlen(argv[i + 1]) + strlen(".pbm") + 1;
            nome_do_arquivo = (char *)realloc(nome_do_arquivo, tamanho);
            if (!nome_do_arquivo) {
                printf("Erro ao alocar memória para o nome do arquivo.");
                exit(-1);
            }
            strcpy(nome_do_arquivo, argv[i + 1]);
            strcat(nome_do_arquivo, ".pbm");
            i++;
        }
    }

    bool digito_final;
    int tamanho_codigo, tamanho_lateral, tamanho_total;
    int tamanho_altura;
    char marcador_inicial[] = "101";
    char marcador_final[] = "101";
    char marcador_central[] = "01010";
    int conversao_inteiro;
    char parte_inicial[29];
    parte_inicial[0] = '\0';
    char parte_final[29];
    parte_final[0] = '\0';

    FILE *arquivo;
    arquivo = fopen(nome_do_arquivo, "r");
    if (arquivo) {
        char resposta;
        printf("O arquivo já existe. Deseja sobrescrevê-lo? [S/N] ");
        scanf(" %c", &resposta);
        fclose(arquivo); // Fecha o arquivo antes de reabri-lo
        if (resposta == 'N') {
            printf("Arquivo resultante já existe");
            exit(-1); // Termina a execução do programa 
        }
    }

    arquivo = fopen(nome_do_arquivo, "w");
    free(nome_do_arquivo);

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo");
        printf("Erro foi: %s\n", strerror(errno));
    }
    else{
        tamanho_codigo = (67 * quantidade_pixels);
        char linha_codigo[tamanho_codigo + (espaco_lateral * 2)];
        memset(linha_codigo, '\0', sizeof(linha_codigo));

        //Realizar a codificação
        for (int i = 0; i < 4; ++i){
            conversao_inteiro = digitos[i] - '0';
            strcat(parte_inicial, l_code[conversao_inteiro]);
        }

        for (int i = 4; i < 8; ++i){
            conversao_inteiro = digitos[i] -'0';
            strcat(parte_final, r_code[conversao_inteiro]);
        }

        //Concatenar a linha do código de barras

        strcat(linha_codigo, marcador_inicial);
        strcat(linha_codigo, parte_inicial);
        strcat(linha_codigo, marcador_central);
        strcat(linha_codigo, parte_final);
        strcat(linha_codigo, marcador_final);

        //Gerar a quantidade de pixels por área
        char linha_codigo_pixels[tamanho_codigo];
        memset(linha_codigo_pixels, '\0', sizeof(linha_codigo_pixels));
        
        int tamanho = strlen(linha_codigo);
        int contador = 0;
        for (int i = 0; i < tamanho; i++){
            for(int j = 0; j < quantidade_pixels; j++){
                linha_codigo_pixels[contador] = linha_codigo[i];
                contador++;
            }
        }
        linha_codigo_pixels[tamanho_codigo] = '\0';

        //Formar o espaçamento lateral
        tamanho_total = tamanho_codigo + (2 * espaco_lateral);
        char *codigo = (char *)malloc(tamanho_total + 1);
        memset(codigo, '\0', tamanho_total + 1);

        if (!codigo) {
        printf("Memória não alocada.\n");
        exit(-1);
    }

        if ( espaco_lateral > 0){
            char lateral_l[espaco_lateral + 1];
            char lateral_r[espaco_lateral + 1];
            memset(lateral_l, '0', espaco_lateral);
            memset(lateral_r, '0', espaco_lateral);
            lateral_l[espaco_lateral] = '\0';
            lateral_r[espaco_lateral] = '\0';

            strcat(codigo, lateral_l);
            strcat(codigo, linha_codigo_pixels);
            strcat(codigo, lateral_r);
        } else {
            strcpy(codigo, linha_codigo_pixels);
        }

        //Formar a altura
        tamanho_lateral = strlen(codigo) + 1;
        tamanho_altura = altura + (espaco_lateral * 2);

        char altura_inicial[espaco_lateral][tamanho_lateral];
        memset(altura_inicial, '0', sizeof(altura_inicial));

        for ( int i = 0; i < espaco_lateral; ++i){
            for ( int k = 0; k <= tamanho_lateral; ++k){
                altura_inicial[i][k] = '0';
            }
            altura_inicial[i][tamanho_lateral - 1] = '\0';
        }

        char altura_final[espaco_lateral][tamanho_lateral];
        memset(altura_final, '0', sizeof(altura_final));

        for ( int i = 0; i < espaco_lateral; ++i){
            for ( int k = 0; k <= tamanho_lateral; ++k){
                altura_final[i][k] = '0';
            }
            altura_final[i][tamanho_lateral - 1] = '\0';
        }


        digito_final = ultimo_digito(digitos);

        //Imprimir as linhas no arquivo PBM

        if ( digito_final){
            fprintf(arquivo, "P1\n");
            fprintf(arquivo, "%d %d\n", tamanho_total, tamanho_altura);
            for ( int i = 0; i < espaco_lateral; ++i){
                fprintf(arquivo,"%s\n", altura_inicial[i]);
            }

            for ( int i = 0; i < altura; ++i){
                fprintf(arquivo, "%s\n", codigo);
            }

            for ( int i = 0; i < espaco_lateral; ++i){
                fprintf(arquivo,"%s\n", altura_final[i]);
            }

            free(codigo);

        } else {
            printf("Dígito Verificado não é válido.\n");
        }

    }

    fclose(arquivo);

    return 0;

}
