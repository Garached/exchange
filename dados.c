#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fracao.h"
#include "dados.h"
typedef struct fracao fracao;
typedef struct movimento movimento;
typedef struct registro registro;

// le um unico registro do arquivo base;
void ler_registro(registro* a, FILE* base, int quantidade_moedas) {
    fread(a->cpf, sizeof(char), 12, base);
    fread(a->senha, sizeof(char), 7, base);
    fread(a->nome, sizeof(char), sizeof(a->nome), base);
    
    freceber(base, &a->reais);
    a->carteira = malloc(quantidade_moedas * sizeof(fracao));
	for (int i = 0; i < quantidade_moedas; i++) {
		freceber(base, &a->carteira[i]);
	}
    
    fread(&a->quantidade_movimentos, sizeof(int), 1, base);
    a->movimentos = malloc(a->quantidade_movimentos * sizeof(movimento));

    for (int i = 0; i < a->quantidade_movimentos; i++) {
        fread(a->movimentos[i].data, sizeof(char), 12, base);
        fread(a->movimentos[i].descricao, sizeof(char), sizeof(a->movimentos[i].descricao), base);
        freceber(base, &a->movimentos[i].reais);
		a->movimentos[i].carteira = malloc(quantidade_moedas * sizeof(fracao));
		for (int j = 0; j < quantidade_moedas; j++) {
			freceber(base, &a->movimentos[i].carteira[j]);
		}
    }
}

// le a base toda em binario;
registro* ler_base(int* quantidade_registros, int quantidade_moedas) {
    FILE* base = fopen("base_registros.bin", "rb");

    fread(quantidade_registros, sizeof(int), 1, base);
    registro* registros = malloc(*quantidade_registros * sizeof(registro));

    for (int i = 0; i < *quantidade_registros; i++) {
        ler_registro(&registros[i], base, quantidade_moedas);
    }

    fclose(base);
    return registros;
}

// grava um unico registro no arquivo base;
void gravar_registro(registro* a, FILE* base, int quantidade_moedas) {
    fwrite(a->cpf, sizeof(char), 12, base);
    fwrite(a->senha, sizeof(char), 7, base);
    fwrite(a->nome, sizeof(char), sizeof(a->nome), base);
    
    fprintar(base, a->reais);
	for (int i = 0; i < quantidade_moedas; i++) {
		fprintar(base, a->carteira[i]);
	}
    
    fwrite(&a->quantidade_movimentos, sizeof(int), 1, base);
    for (int i = 0; i < a->quantidade_movimentos; i++) {
        fwrite(a->movimentos[i].data, sizeof(char), 12, base);
        fwrite(a->movimentos[i].descricao, sizeof(char), sizeof(a->movimentos[i].descricao), base);
        fprintar(base, a->movimentos[i].reais);
		for (int j = 0; j < quantidade_moedas; j++) {
			fprintar(base, a->movimentos[i].carteira[i]);
		}
		free(a->movimentos[i].carteira);
    }
	free(a->movimentos);
}

// grava a base toda em binario;
void gravar_base(registro* registros, int quantidade_registros, int quantidade_moedas) {
    FILE* base = fopen("base_registros.bin", "wb");

    fwrite(&quantidade_registros, sizeof(int), 1, base);
    for (int i = 0; i < quantidade_registros; i++) {
        gravar_registro(&registros[i], base, quantidade_moedas);
    }
}

moeda* ler_moedas(int* quantidade_moedas) {
    FILE* base = fopen("base_moedas.bin", "rb");
	
    fread(quantidade_moedas, sizeof(int), 1, base);
	moeda* moedas = malloc(*quantidade_moedas * sizeof(moeda));

	for (int i = 0; i < *quantidade_moedas; i++) {
		fread(moedas[i].nome, sizeof(char), sizeof(moedas[i].nome), base);
		fread(moedas[i].apelido, sizeof(char), sizeof(moedas[i].apelido), base);
		freceber(base, &moedas[i].valor);
		freceber(base, &moedas[i].taxa_compra);
		freceber(base, &moedas[i].taxa_venda);
	}

	fclose(base);
	return moedas;
}

void gravar_moedas(moeda* moedas, int quantidade_moedas) {
    FILE* base = fopen("base_moedas.bin", "wb");

    fwrite(&quantidade_moedas, sizeof(int), 1, base);
    for (int i = 0; i < quantidade_moedas; i++) {
		fwrite(moedas[i].nome, sizeof(char), sizeof(moedas[i].nome), base);
		fwrite(moedas[i].apelido, sizeof(char), sizeof(moedas[i].apelido), base);
		fprintar(base, moedas[i].valor);
		fprintar(base, moedas[i].taxa_compra);
		fprintar(base, moedas[i].taxa_venda);
    }
    fclose(base);
	free(moedas);
}

void remove_kesimo(void** vetor, int* quantidade, int tamanho, int k) {
    char* base = (char*)(*vetor);
    memmove(base + k * tamanho, base + (k + 1) * tamanho, (*quantidade - k - 1) * tamanho);

    void* temp = realloc(*vetor, (*quantidade - 1) * tamanho);
    if (temp != NULL || *quantidade == 1) {
        *vetor = temp;
    }

	(*quantidade)--;
}

