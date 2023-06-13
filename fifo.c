#include <stdio.h>
#include <stdlib.h>

// Estruturas do programa
#define MAX 100
#define FILA_SIZE 100
#define N_LOOPS 50
#define START_LOOPS 10
#define END_LOOPS 90
#define N_TESTES 10

struct Node
{
    int num;
    struct Node *prox;
};
typedef struct Node node;

typedef struct {
    char time_last_access; //tempo ultimo acesso
    int counter_mru; // contador mru
    int presente; //presente/ausente
    int modificada; // modificada
    int referenciada; // referenciada
    int moldura; //moldura da pagina
} Memoria;

int tam;

void inicia(node *FILA)
{
    FILA->prox = NULL;
    tam = 0;
}

int vazia(node *FILA)
{
    if (FILA->prox == NULL)
        return 1;
    else
        return 0;
}

node *aloca(int address)
{
    node *novo = (node *)malloc(sizeof(node));
    if (!novo)
    {
        printf("Sem memoria disponivel!\n");
        exit(1);
    }
    else
    {
        novo->num = address;
        return novo;
    }
}

void insere(node *FILA, int address)
{
    node *novo = aloca(address);
    novo->prox = NULL;

    if (vazia(FILA))
        FILA->prox = novo;
    else
    {
        node *tmp = FILA->prox;

        while (tmp->prox != NULL)
            tmp = tmp->prox;

        tmp->prox = novo;
    }
    tam++;
}

node *retira(node *FILA)
{
    if (FILA->prox == NULL)
    {
        printf("Fila ja esta vazia\n");
        return NULL;
    }
    else
    {
        node *tmp = FILA->prox;
        FILA->prox = tmp->prox;
        tam--;
        return tmp;
    }
}

void exibe(node *FILA)
{
    if (vazia(FILA))
    {
        printf("Fila vazia!\n\n");
        return;
    }

    node *tmp;
    tmp = FILA->prox;
    printf("Fila :");
    while (tmp != NULL)
    {
        printf("%d ", tmp->num);
        tmp = tmp->prox;
    }
    printf("\n\n");
}

void libera(node *FILA)
{
    if (!vazia(FILA))
    {
        node *proxNode,
            *atual;

        atual = FILA->prox;
        while (atual != NULL)
        {
            proxNode = atual->prox;
            free(atual);
            atual = proxNode;
        }
    }
}

void inicializa_fila(node *FILA, Memoria *memoria_virtual) {
    for (int i = 0; i < FILA_SIZE; i++)
    {
        int address = rand() % END_LOOPS + START_LOOPS;

        if (i == 0)
        {
            FILA->num = address;
        } else {
            /* preenche fila */
            Memoria memory_item = memoria_virtual[address];
            insere(FILA, address);
        }
    }
}

void insere_pagina(node *FILA, int address) {
    retira(FILA);
    insere(FILA, address);
}

int teste(int seed)
{
    int page_miss_counter = 0;

    node *FILA = (node *)malloc(sizeof(node));
    inicia(FILA);

    // Criar memoria virtual
    Memoria memoria_virtual[MAX];
    for (int i = 0; i < MAX; i++)
    {
        memoria_virtual[i].modificada = rand() & 1;
        memoria_virtual[i].referenciada = rand() & 1;
        memoria_virtual[i].presente = rand() & 1;
    }

    inicializa_fila(FILA, memoria_virtual);
    exibe(FILA);

    for (int i = 0; i < N_LOOPS; i++)
    {
        int address = rand() % END_LOOPS + START_LOOPS;
        Memoria memory_item = memoria_virtual[address];
        int escrita = rand() & 1;
        
        if (FILA->num != address) {
            /* page miss */

            page_miss_counter++;
            insere_pagina(FILA, address);
        }
    }
    
    // Medir quantidade de page miss
    printf("page_miss_counter: %d\n", page_miss_counter);
    // Medir O(n) gpt
    // Tempo de execução
    
    return 0;
}

int main() {
    for (int i = 0; i < N_TESTES; i++)
    {
        printf("Teste Fila -> FIFO %d:\n", i);
        teste(i);
    }
}
