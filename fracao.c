#include "fracao.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct fracao fracao;

int gcd(int a, int b) {
	return (b ? gcd(b, a % b) : a);
}

fracao fracao_(int numerador_, int denominador_) {
	fracao res;
	if (denominador_ < 0) numerador_ *= -1, denominador_ *= -1;
	
	int g = gcd(abs(numerador_), denominador_);
	res.numerador = numerador_ / g;
	res.denominador = denominador_ / g; return res; }

fracao inverter_(fracao a) {
	return fracao_(a.denominador, a.numerador);
}

void inverter(fracao* a) {
	*a = inverter_(*a);
}

fracao negar_(fracao a) {
	return fracao_(-a.numerador, a.denominador);
}

void negar(fracao* a) {
	*a = negar_(*a);
}

fracao somar_(fracao a, fracao b) {
    return fracao_(a.numerador * b.denominador + b.numerador * a.denominador, a.denominador * b.denominador);
}

void somar(fracao* a, fracao b) {
	*a = somar_(*a, b);	
}

fracao subtrair_(fracao a, fracao b) {
	return somar_(a, negar_(b));
}

void subtrair(fracao* a, fracao b) {
	*a = subtrair_(*a, b);
}

fracao multiplicar_(fracao a, fracao b) {
	return fracao_(a.numerador * b.numerador, a.denominador * b.denominador);
}

void multiplicar(fracao* a, fracao b) {
	*a = multiplicar_(*a, b);
}

fracao dividir_(fracao a, fracao b) {
	return multiplicar_(a, inverter_(b));
}

void dividir(fracao* a, fracao b) {
	*a = dividir_(*a, b);
}

fracao exponenciar_(fracao a, int n) {
	fracao res = fracao_(1, 1);
	while (n) {
		if (n & 1) res = multiplicar_(res, a);
		n >>= 1;
		a = multiplicar_(a, a);
	}
	return res;
}

void exponenciar(fracao* a, int n) {
	*a = exponenciar_(*a, n);	
}

int maior(fracao a, fracao b) {
    return a.numerador * b.denominador > b.numerador * a.denominador;
}

int menor(fracao a, fracao b) {
    return a.numerador * b.denominador < b.numerador * a.denominador;
}

int igual(fracao a, fracao b) {
    return a.numerador == b.numerador && a.denominador == b.denominador;
}

void printar(fracao a, char fim) {
	if (a.denominador != 1) {
		printf("%d/%d%c", a.numerador, a.denominador, fim);
	} else {
		printf("%d%c", a.numerador, fim);
	}
}

void fprintar(FILE* f, fracao a) {
    fwrite(&a.numerador, sizeof(int), 1, f);
    fwrite(&a.denominador, sizeof(int), 1, f);
}

void receber(fracao* a) {
	int numerador_, denominador_;
	scanf("%d", &numerador_);	

	char c = getchar();	
	if (c == '/') {
		scanf("%d", &denominador_);	
	} else {
		ungetc(c, stdin);
		denominador_ = 1;
	}
	*a = fracao_(numerador_, denominador_);
}

void freceber(FILE* f, fracao* a) {
    fread(&a->numerador, sizeof(int), 1, f);
    fread(&a->denominador, sizeof(int), 1, f);
}
