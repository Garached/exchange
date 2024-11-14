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

void novo_movimento(registro* usuario, char* descricao, moeda* moedas, int quantidade_moedas) {
	usuario->movimentos = realloc(usuario->movimentos, (usuario->quantidade_movimentos + 1) * sizeof(movimento));
	movimento novo;

    time_t t;
    struct tm *tm_info;
    char buffer[11];

    time(&t);
    tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", tm_info);

	strcpy(novo.data, buffer);
	strcpy(novo.descricao, descricao);
	novo.reais = usuario->reais;
	novo.carteira = malloc(quantidade_moedas * sizeof(fracao));
	for (int i = 0; i < quantidade_moedas; i++) {
		novo.carteira[i] = usuario->carteira[i];
	}
	usuario->movimentos[usuario->quantidade_movimentos] = novo;
	usuario->quantidade_movimentos++;
}

void depositar_reais(registro* usuario, moeda* moedas, int quantidade_moedas) {
	puts("[+] depositar reais!\n");
	consultar_saldo(usuario, moedas, quantidade_moedas);

	int ok = 0;
	fracao depositado;
	while (!ok) {
		printf("[?] informe o valor a ser depositado: ");
		receber(&depositado);
		if (depositado.numerador > 0) {
			ok = 1;
		} else {
			puts("[e] o valor deve ser > 0!");
		}
	}
	printf("[i] antes : "); printar(usuario->reais, '\n');
	printf("[i] depois: "); printar(somar_(usuario->reais, depositado), '\n');

	somar(&usuario->reais, depositado);
	char descricao[100];
	snprintf(descricao, sizeof(descricao), "deposito de %d/%d reais", depositado.numerador, depositado.denominador);
	novo_movimento(usuario, descricao, moedas, quantidade_moedas);
}

void sacar_reais(registro* usuario, moeda* moedas, int quantidade_moedas) {
	puts("[+] sacar reais!\n");
	consultar_saldo(usuario, moedas, quantidade_moedas);

	int ok = 0;
	fracao sacado;
	while (!ok) {
		printf("[?] informe o valor a ser sacado: ");
		receber(&sacado);
		ok |= (sacado.numerador > 0);
		if (sacado.numerador < 0) {
			puts("[e] o valor deve ser > 0!");
		} else if (menor(usuario->reais, sacado)) {
			printf("[e] o valor nao pode exceder o seu saldo de ");
			printar(usuario->reais, ' ');
			printf("reais!\n");
		} else {
			ok = 1;
		}
	}
	printf("[i] antes : "); printar(usuario->reais, '\n');
	printf("[i] depois: "); printar(subtrair_(usuario->reais, sacado), '\n');

	subtrair(&(usuario->reais), sacado);
	char descricao[100];
	snprintf(descricao, sizeof(descricao), "saque de %d/%d reais", sacado.numerador, sacado.denominador);
	novo_movimento(usuario, descricao, moedas, quantidade_moedas);
}

void comprar_criptomoedas(registro* usuario, moeda* moedas, int quantidade_moedas) {
	puts("[+] comprar criptomoedas!\n");
	consultar_saldo(usuario, moedas, quantidade_moedas);
	consultar_moedas(moedas, quantidade_moedas);

	fracao quantidade_informada, preco, taxa;
	fracao* moeda_usuario;
	int ok_moeda = 0;
	char selecao[3];
	while (!ok_moeda) {
		printf("[?] informe a moeda a ser comprada: ");
		scanf("%3s", selecao); getchar();

		for (int i = 0; i < quantidade_moedas; i++) {
			if (!strcmp(selecao, moedas[i].apelido)) {
				ok_moeda = 1;
				preco = moedas[i].valor;
				taxa = moedas[i].taxa_compra;
				moeda_usuario = &usuario->carteira[i];
			}
		}
		if (!ok_moeda) puts("[e] moeda invalida!");
	}

	// total = quantidade * preco * (1 + taxa_compra);
	// portanto: quantidade_maxima = balanco_reais / (preco * (1 + taxa_compra));
	fracao quantidade_maxima = dividir_(usuario->reais, multiplicar_(preco, somar_(fracao_(1, 1), taxa)));
	printf("[i] quantidade maxima a ser comprada: "); printar(quantidade_maxima, '\n');

	int ok_quantidade = 0;	
	while (!ok_quantidade) {
		printf("[?] informe a quantidade a ser comprada de %s: ", selecao);
		receber(&quantidade_informada);

		ok_quantidade = 1;
		if (maior(quantidade_informada, quantidade_maxima)) {
			ok_quantidade = 0;
			puts("[e] quantidade invalida!");
		}
	}
	
	subtrair(&usuario->reais, multiplicar_(preco, multiplicar_(quantidade_informada, taxa)));
	somar(moeda_usuario, quantidade_informada);

	char descricao[14];
	snprintf(descricao, 14, "compra de %s", selecao);

	novo_movimento(usuario, descricao, moedas, quantidade_moedas);
}

void vender_criptomoedas(registro* usuario, moeda* moedas, int quantidade_moedas) {
	puts("[+] vender criptomoedas!\n");
	consultar_saldo(usuario, moedas, quantidade_moedas);
	consultar_moedas(moedas, quantidade_moedas);

	fracao quantidade_informada, preco, taxa;
	fracao* moeda_usuario;
	int ok_moeda = 0;
	char selecao[3];
	while (!ok_moeda) {
		printf("[?] informe a moeda a ser vendida: ");
		scanf("%3s", selecao); getchar();

		for (int i = 0; i < quantidade_moedas; i++) {
			if (!strcmp(selecao, moedas[i].apelido)) {
				ok_moeda = 1;
				preco = moedas[i].valor;
				taxa = moedas[i].taxa_venda;
				moeda_usuario = &usuario->carteira[i];
			}
		}
		if (!ok_moeda) puts("[e] moeda invalida!");
	}

	fracao quantidade_maxima = *moeda_usuario;
	printf("[i] quantidade maxima a ser vendida: "); printar(quantidade_maxima, '\n');

	int ok_quantidade = 0;	
	while (!ok_quantidade) {
		printf("[?] informe a quantidade a ser vendida de %s: ", selecao);
		receber(&quantidade_informada);

		ok_quantidade = 1;
		if (maior(quantidade_informada, quantidade_maxima)) {
			ok_quantidade = 0;
			puts("[e] quantidade invalida!");
		}
	}

	// total_ganho = quantidade * preco * (1 - taxa_venda);
	subtrair(moeda_usuario, quantidade_informada);
	somar(&usuario->reais, multiplicar_(subtrair_(fracao_(1, 1), taxa), multiplicar_(quantidade_informada, preco)));

	char descricao[14];
	snprintf(descricao, 14, "compra de %s", selecao);

	novo_movimento(usuario, descricao, moedas, quantidade_moedas);
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
	puts("[+] menu inicial!");
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
		puts("[1] consultar saldo");
		puts("[2] consultar extrato");
		puts("[3] depositar reais");
		puts("[4] sacar reais");
		puts("[5] comprar criptomoedas");
		puts("[6] vender criptomoedas");
		puts("[7] atualizar cotacao");
		puts("[8] sair\n");
		printf("[?] selecione uma das opcoes: ");	
		int selecao;
		scanf("%d", &selecao);

		if (selecao >= 1 && selecao <= 8) limpar_terminal();
		switch (selecao) {
			case 1:
				consultar_saldo(usuario, moedas, quantidade_moedas);
				retorno();
				break;
			case 2:
				consultar_extrato(usuario, moedas, quantidade_moedas);
				retorno();
				break;
			case 3:
				depositar_reais(usuario, moedas, quantidade_moedas);
				retorno();
				break;
			case 4:
				sacar_reais(usuario, moedas, quantidade_moedas);
				retorno();
				break;
			case 5: 
				comprar_criptomoedas(usuario, moedas, quantidade_moedas);
				retorno();
				break;
			case 6: 
				vender_criptomoedas(usuario, moedas, quantidade_moedas);
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
