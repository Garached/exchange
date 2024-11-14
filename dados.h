#ifndef dados_h
#define dados_h
#include <stdlib.h>

// guarda o movimento de uma conta, os valores que aparecem quando se consulta o seu extrato;
typedef struct movimento movimento;
struct movimento {
	char data[12];
	char descricao[100];
    fracao reais;
	fracao* carteira;
};

// guarda os valores referentes a uma conta;
typedef struct registro registro;
struct registro {
    char cpf[13];
    char senha[8];
    char nome[100];
    fracao reais;
	fracao* carteira;
	int quantidade_movimentos;
	movimento* movimentos;
};

// guarda os valores referentes a uma cotacao;
typedef struct cotacao cotacao;
struct cotacao {
    fracao bitcoin, etherium, ripple;
    fracao bitcoin_taxa_compra, etherium_taxa_compra, ripple_taxa_compra;
    fracao bitcoin_taxa_venda, etherium_taxa_venda, ripple_taxa_venda;
};

typedef struct moeda moeda;
struct moeda {
	char nome[100];
	char apelido[100];
	fracao taxa_compra, taxa_venda, valor;
};

void ler_registro(registro* a, FILE* base, int quantidade_moedas);
registro* ler_base(int* quantidade_registros, int quantidade_moedas);

void gravar_registro(registro* a, FILE* base, int quantidade_moedas);
void gravar_base(registro* registros, int quantidade_registros, int quantidade_moedas);

moeda* ler_moedas(int* quantidade_moedas);
void gravar_moedas(moeda* moedas, int quantidade_moedas);

#endif

