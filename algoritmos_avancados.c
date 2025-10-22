#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>

// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Sistema completo de detetive com árvore binária, árvore de busca e tabela hash

// ===== ESTRUTURAS =====

// 🌱 Nível Novato: Estrutura para navegação na mansão
typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// 🔍 Nível Aventureiro: Estrutura para pistas
typedef struct Pista {
    char texto[100];
    struct Pista* esquerda;
    struct Pista* direita;
} Pista;

// 🧠 Nível Mestre: Estrutura para suspeitos
typedef struct NoListaPista {
    char pista[100];
    struct NoListaPista* proximo;
} NoListaPista;

typedef struct Suspeito {
    char nome[50];
    NoListaPista* pistas;
    int contador;
} Suspeito;

typedef struct NoHash {
    Suspeito suspeito;
    struct NoHash* proximo;
} NoHash;

// ===== FUNÇÕES PARA ÁRVORE BINÁRIA (SALAS) =====

Sala* criarSala(char* nome) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

void conectarSalas(Sala* pai, Sala* esquerda, Sala* direita) {
    if (pai) {
        pai->esquerda = esquerda;
        pai->direita = direita;
    }
}

void explorarSalas(Sala* salaAtual) {
    if (!salaAtual) return;
    
    printf("\n[LOCAL] Voce esta na: %s\n", salaAtual->nome);
    printf("Opcoes: (e) esquerda, (d) direita, (s) sair\n");
    
    char opcao;
    printf("Escolha: ");
    scanf(" %c", &opcao);
    
    switch(opcao) {
        case 'e':
            if (salaAtual->esquerda) {
                explorarSalas(salaAtual->esquerda);
            } else {
                printf("Nao ha saida a esquerda!\n");
                explorarSalas(salaAtual);
            }
            break;
        case 'd':
            if (salaAtual->direita) {
                explorarSalas(salaAtual->direita);
            } else {
                printf("Nao ha saida a direita!\n");
                explorarSalas(salaAtual);
            }
            break;
        case 's':
            printf("Saindo da exploração...\n");
            return;
        default:
            printf("Opcao invalida!\n");
            explorarSalas(salaAtual);
    }
}

// ===== FUNÇÕES PARA ÁRVORE DE BUSCA (PISTAS) =====

Pista* inserirPista(Pista* raiz, char* texto) {
    if (!raiz) {
        Pista* novaPista = (Pista*)malloc(sizeof(Pista));
        strcpy(novaPista->texto, texto);
        novaPista->esquerda = NULL;
        novaPista->direita = NULL;
        return novaPista;
    }
    
    int comparacao = strcmp(texto, raiz->texto);
    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, texto);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, texto);
    }
    
    return raiz;
}

void listarPistas(Pista* raiz) {
    if (raiz) {
        listarPistas(raiz->esquerda);
        printf("[PISTA] %s\n", raiz->texto);
        listarPistas(raiz->direita);
    }
}

// ===== FUNÇÕES PARA TABELA HASH (SUSPEITOS) =====

#define TAMANHO_HASH 10

NoHash* tabelaHash[TAMANHO_HASH];

void inicializarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

int calcularHash(char* nome) {
    int soma = 0;
    for (int i = 0; nome[i] != '\0'; i++) {
        soma += nome[i];
    }
    return soma % TAMANHO_HASH;
}

NoHash* buscarSuspeito(char* nome) {
    int indice = calcularHash(nome);
    NoHash* atual = tabelaHash[indice];
    
    while (atual) {
        if (strcmp(atual->suspeito.nome, nome) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

void inserirHash(char* pista, char* nomeSuspeito) {
    int indice = calcularHash(nomeSuspeito);
    NoHash* no = buscarSuspeito(nomeSuspeito);
    
    if (!no) {
        // Criar novo suspeito
        no = (NoHash*)malloc(sizeof(NoHash));
        strcpy(no->suspeito.nome, nomeSuspeito);
        no->suspeito.pistas = NULL;
        no->suspeito.contador = 0;
        no->proximo = tabelaHash[indice];
        tabelaHash[indice] = no;
    }
    
    // Adicionar pista à lista do suspeito
    NoListaPista* novaPista = (NoListaPista*)malloc(sizeof(NoListaPista));
    strcpy(novaPista->pista, pista);
    novaPista->proximo = no->suspeito.pistas;
    no->suspeito.pistas = novaPista;
    no->suspeito.contador++;
}

void listarAssociacoes() {
    printf("\n[RELATORIO] ASSOCIACOES DE PISTAS COM SUSPEITOS:\n");
    printf("==========================================\n");
    
    for (int i = 0; i < TAMANHO_HASH; i++) {
        NoHash* atual = tabelaHash[i];
        while (atual) {
            printf("\n[SUSPEITO] %s (Pistas: %d)\n", atual->suspeito.nome, atual->suspeito.contador);
            NoListaPista* pistaAtual = atual->suspeito.pistas;
            while (pistaAtual) {
                printf("   [PISTA] %s\n", pistaAtual->pista);
                pistaAtual = pistaAtual->proximo;
            }
            atual = atual->proximo;
        }
    }
}

void encontrarSuspeitoMaisProvavel() {
    char* nomeMaisProvavel = NULL;
    int maxContador = 0;
    
    for (int i = 0; i < TAMANHO_HASH; i++) {
        NoHash* atual = tabelaHash[i];
        while (atual) {
            if (atual->suspeito.contador > maxContador) {
                maxContador = atual->suspeito.contador;
                nomeMaisProvavel = atual->suspeito.nome;
            }
            atual = atual->proximo;
        }
    }
    
    if (nomeMaisProvavel) {
        printf("\n[RESULTADO] SUSPEITO MAIS PROVAVEL: %s (%d pistas associadas)\n", nomeMaisProvavel, maxContador);
    } else {
        printf("\n[ERRO] Nenhum suspeito encontrado.\n");
    }
}

// ===== FUNÇÃO PRINCIPAL =====

int main() {
    // Configurar codificação UTF-8 para Windows
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Portuguese_Brazil.utf8");
    
    printf("DETECTIVE QUEST - SISTEMA COMPLETO\n");
    printf("=====================================\n");
    
    // Inicializar tabela hash
    inicializarHash();
    
    // Nivel Novato: Criar mapa da mansao
    printf("\n[1] CRIANDO MAPA DA MANSAO...\n");
    
    Sala* hallEntrada = criarSala("Hall de Entrada");
    Sala* biblioteca = criarSala("Biblioteca");
    Sala* cozinha = criarSala("Cozinha");
    Sala* sotao = criarSala("Sotao");
    Sala* quarto = criarSala("Quarto Principal");
    Sala* jardim = criarSala("Jardim");
    
    conectarSalas(hallEntrada, biblioteca, cozinha);
    conectarSalas(biblioteca, sotao, quarto);
    conectarSalas(cozinha, jardim, NULL);
    
    // Nivel Aventureiro: Arvore de pistas
    Pista* arvorePistas = NULL;
    
    // Adicionar pistas baseadas nas salas
    arvorePistas = inserirPista(arvorePistas, "Livro de quimica aberto na pagina sobre venenos");
    arvorePistas = inserirPista(arvorePistas, "Faca com residuos suspeitos na cozinha");
    arvorePistas = inserirPista(arvorePistas, "Carta de amor rasgada no quarto");
    arvorePistas = inserirPista(arvorePistas, "Pegadas de barro no jardim");
    arvorePistas = inserirPista(arvorePistas, "Relogio parado as 3:47 no hall");
    
    // Nivel Mestre: Associar pistas com suspeitos
    inserirHash("Livro de quimica aberto na pagina sobre venenos", "Dr. Smith");
    inserirHash("Faca com residuos suspeitos na cozinha", "Chef Maria");
    inserirHash("Carta de amor rasgada no quarto", "Joao Silva");
    inserirHash("Pegadas de barro no jardim", "Joao Silva");
    inserirHash("Relogio parado as 3:47 no hall", "Dr. Smith");
    inserirHash("Livro de quimica aberto na pagina sobre venenos", "Dr. Smith");
    
    // Menu principal
    int opcao;
    do {
        printf("\n[MENU] OPCOES DISPONIVEIS:\n");
        printf("1. Explorar mansao (Arvore Binaria)\n");
        printf("2. Ver pistas coletadas (Arvore de Busca)\n");
        printf("3. Ver associacoes pistas-suspeitos (Tabela Hash)\n");
        printf("4. Encontrar suspeito mais provavel\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        
        switch(opcao) {
            case 1:
                printf("\n[1] EXPLORANDO A MANSAO...\n");
                explorarSalas(hallEntrada);
                break;
            case 2:
                printf("\n[2] PISTAS COLETADAS (em ordem alfabetica):\n");
                printf("==========================================\n");
                listarPistas(arvorePistas);
                break;
            case 3:
                listarAssociacoes();
                break;
            case 4:
                encontrarSuspeitoMaisProvavel();
                break;
            case 0:
                printf("\n[SAINDO] Encerrando Detective Quest...\n");
                break;
            default:
                printf("\n[ERRO] Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}

