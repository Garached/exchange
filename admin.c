#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fracao.h"
#include "dados.h"

registro* login(registro* registros, int quantidade_registros) {
    registro* usuario = NULL;
    while (1) {
        char cpf_informado[12];
        printf("[?] informe o seu cpf: ");
        scanf("%11s", cpf_informado);
		getchar();

        for (int i = 0; i < quantidade_registros && usuario == NULL; i++) {
            if (strcmp(registros[i].cpf, cpf_informado) == 0) {
                usuario = &registros[i];
            }
        }

        if (usuario != NULL) break;
        else puts("[e] cpf nao encontrado, tente novamente.");
    }

    int ok = 0;
    for (int tentativa = 1; tentativa <= 3 && !ok; tentativa++) {
        char senha_informada[6];
        printf("[?] (%d/3) informe a sua senha: ", tentativa);
		scanf("%6s", senha_informada);
		getchar();

        if (strcmp(senha_informada, usuario->senha) == 0) ok = 1;
        else if (tentativa < 3) puts("[e] senha incorreta, tente novamente.");
    }

    if (!ok) return NULL;
    return usuario;
}

int cpf_valido(char* cpf) {
	int soma = 0;
	for (int peso = 10, i = 0; i < 9; i++) {
		soma += (cpf[i] - '0') * peso--;
	}

	int primeiro = 11 - (soma % 11);
	if (primeiro >= 10) primeiro = 0;

	soma = 0;
	for (int peso = 11, i = 0; i < 10; i++) {
		soma += (cpf[i] - '0') * peso--;
	}

	int segundo = 11 - (soma % 11);
	if (segundo >= 10) segundo = 0;

	return (cpf[9] - '0') == primeiro && (cpf[10] - '0') == segundo;
}

registro* novo_usuario(registro** registros, int* quantidade_registros, int quantidade_moedas) {
    registro novo;

    int cpf_ok = 0;
    while (!cpf_ok) {
        printf("[?] informe o novo cpf: ");
        scanf("%11s", novo.cpf);
        getchar();

        cpf_ok = 1;
        for (int i = 0; i < 11 && cpf_ok; i++) {
            cpf_ok &= (novo.cpf[i] >= '0' && novo.cpf[i] <= '9');
        }

		if (!cpf_ok) {
			puts("[e] valor invalido! o cpf deve ter 11 digitos numericos!");
		} else if (!cpf_valido(novo.cpf)) {
			puts("[e] cpf nao existe!");
			cpf_ok = 0;
		}
    }

    int senha_ok = 0;
    while (!senha_ok) {
        printf("[?] informe a nova senha: ");
        scanf("%6s", novo.senha);

        senha_ok = 1;
        for (int i = 0; i < 6 && senha_ok; i++) {
            senha_ok &= (novo.senha[i] >= '0' && novo.senha[i] <= '9');
        }

        if (!senha_ok) puts("[e] valor invalido! a senha deve ter 6 digitos numericos!");
    }

    printf("[?] informe o novo nome: ");
    getchar();
    fgets(novo.nome, sizeof(novo.nome), stdin);

    novo.reais = fracao_(0, 1);
	novo.carteira = malloc(quantidade_moedas * sizeof(fracao));
	for (int i = 0; i < quantidade_moedas; i++) {
		novo.carteira[i] = fracao_(0, 1);
	}
    novo.quantidade_movimentos = 0;
    novo.movimentos = NULL;

    *registros = realloc(*registros, (*quantidade_registros + 1) * sizeof(registro));
    
    (*registros)[*quantidade_registros] = novo;
    (*quantidade_registros)++;    

    return &(*registros)[(*quantidade_registros) - 1];
}

void consultar_saldo(registro* usuario, moeda* moedas, int quantidade_moedas) {
	printf("[i] saldo atual de: %s", usuario->nome);
	printf("    .reais: "), printar(usuario->reais, '\n');
	for (int i = 0; i < quantidade_moedas; i++) {
		printf("    .%s: ", moedas[i].nome), printar(usuario->carteira[i], '\n');
	}
}

void consultar_moedas(moeda* moedas, int quantidade_moedas) {
	puts("[i] cotacao atual:");
	for (int i = 0; i < quantidade_moedas; i++) {
		printf("    [%s] cotacao: ", moedas[i].apelido), printar(moedas[i].valor, '\n');
		printf("      taxa compra: "), printar(moedas[i].taxa_compra, '\n');
		printf("       taxa venda: "), printar(moedas[i].taxa_venda, '\n');
	}
}

void consultar_extrato(registro* usuario, moeda* moedas, int quantidade_moedas) {
	printf("[i] extrato atual de: %s", usuario->nome);
	printf("[i] quantidade de movimentos: %d\n", usuario->quantidade_movimentos);

	for (int i = 0; i < usuario->quantidade_movimentos; i++) {
		printf("[i] movimento em %s:\n", usuario->movimentos[i].data);
		printf("    .descricao: %s\n", usuario->movimentos[i].descricao);
		printf("    .reais: "), printar(usuario->movimentos[i].reais, '\n');
		for (int j = 0; j < quantidade_moedas; j++) {
			printf("    .%s: ", moedas[j].nome), printar(usuario->movimentos[i].carteira[j], '\n');
		}
	}
}

int percentual_aleatorio() {
	return rand() % 11 - 5;
}

void atualizar_moedas(moeda* moedas, int quantidade_moedas) {
	printf("[i] antes:\n");
	for (int i = 0; i < quantidade_moedas; i++) {
		printf("   [%s]: ", moedas[i].apelido);
		printar(moedas[i].valor, '\n');
	}

	for (int i = 0; i < quantidade_moedas; i++) {
		multiplicar(&moedas[i].valor, fracao_(percentual_aleatorio() + 100, 100));
	}

	printf("[i] depois:\n");
	for (int i = 0; i < quantidade_moedas; i++) {
		printf("   [%s]: ", moedas[i].apelido);
		printar(moedas[i].valor, '\n');
	}
}

void limpar_terminal() {
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
	printf("\x1B[32m");
}

void retorno() {
	printf("[!] pressione qualquer tecla pra continuar... ");
	char c = getchar();
	if (c == '\n') getchar();
	limpar_terminal();
}

int main() {
	srand(time(NULL));
	int quantidade_moedas;
	moeda* moedas = ler_moedas(&quantidade_moedas);

	int quantidade_registros;
    registro* registros = ler_base(&quantidade_registros, quantidade_moedas);
	registro* usuario;

	limpar_terminal();
	puts("[+] menu administrador!");
	puts("[1] logar");
	puts("[2] novo usuario");
	puts("[3] sair\n");

	int logado = 0, sair = 0;
	while (!logado) {
		printf("[?] selecione uma das opcoes: ");	
		int selecao;
		scanf("%d", &selecao);

		logado = 1;
		switch (selecao) {
			case 1:
				usuario = login(registros, quantidade_registros);
				if (usuario == NULL) {
					puts("[e] impossivel logar! fim do programa!");
					sair = 1;
				}
				break;
			case 2:
				usuario = novo_usuario(&registros, &quantidade_registros, quantidade_moedas);
				break;
			case 3 : 
				sair = 1;
				break;
			default:
				puts("[e] selecao invalida! informe um inteiro que pertence a [1, 2]"); logado = 0; }
	}

	limpar_terminal();
	while (!sair) {
		puts("[+] menu principal!\n");
		puts("[1] cadastrar novo investidor");
		puts("[2] excluir investidor");
		puts("[3] cadastrar nova criptomoeda");
		puts("[4] excluir criptomoeda");
		puts("[5] consultar saldo de investidor");
		puts("[6] consultar extrato de investidor");
		puts("[7] atualizar cotacao");
		puts("[8] sair\n");
		printf("[?] selecione uma das opcoes: ");	
		int selecao;
		scanf("%d", &selecao);

		if (selecao >= 1 && selecao <= 8) limpar_terminal();
		switch (selecao) {
			case 1:
				retorno();
				break;
			case 2:
				retorno();
				break;
			case 3:
				retorno();
				break;
			case 4:
				retorno();
				break;
			case 5: 
				retorno();
				break;
			case 6: 
				retorno();
				break;
			case 7:
				atualizar_moedas(moedas, quantidade_moedas);
				retorno();
				break;
			case 8:
				sair = 1;
				break;
			default:
				puts("[e] selecao invalida! informe um inteiro que pertence a [1, 8]");
		}
	}

	gravar_moedas(moedas, quantidade_moedas);
	gravar_base(registros, quantidade_registros, quantidade_moedas);
    return 0;
}
