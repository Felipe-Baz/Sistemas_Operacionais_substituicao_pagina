#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estruturas do programa
#define MAX 100
#define CLOCK_SIZE 50
#define N_LOOPS 50
#define START_LOOPS 10
#define END_LOOPS 90
#define N_TESTES 10

typedef struct page {
    int id;
    int r;
    int m;
    struct page *next;
} Page;

typedef struct {
  Page *start;
  Page *end;
  int length;
} ListaCircular;

typedef struct {
    char time_last_access; //tempo ultimo acesso
    int counter_mru; // contador mru
    int presente; //presente/ausente
    int modificada; // modificada
    int referenciada; // referenciada
    int moldura; //moldura da pagina
} Memoria;

void criar_lista(ListaCircular *listaCircular){
    listaCircular->start = NULL;
    listaCircular->end = NULL;
    listaCircular->length = 0;
}

void inserir_no_inicio(ListaCircular *listaCircular, int num, int m, int r){
    Page *novo = malloc(sizeof(Page));

    if(novo){
        novo->id = num;
        novo->m = m;
        novo->r = r;

        novo->next = listaCircular->start;
        listaCircular->start = novo;
        if(listaCircular->end == NULL)
            listaCircular->end = novo;
        listaCircular->end->next = listaCircular->start;
        listaCircular->length++;
    }
    else
        printf("Erro ao alocar memoria!\n");
}

void inicializa_listacircular(ListaCircular *listaCircular, Memoria *memoria_virtual){
    for (int i = 0; i < CLOCK_SIZE; i++)
    {
        /* preenche clock */
        int address = rand() % END_LOOPS + START_LOOPS;
        Memoria memory_item = memoria_virtual[address];
        inserir_no_inicio(listaCircular, address, memory_item.modificada, memory_item.referenciada);
    }
}

void rotaciona_lista(ListaCircular *listaCircular) {
    listaCircular->start = listaCircular->start->next;
    listaCircular->end = listaCircular->end->next;
}

Page* remover(ListaCircular *listaCircular, int num){
    Page *aux, *remover = NULL;

    if(listaCircular->start){
        if(listaCircular->start == listaCircular->end && listaCircular->start->id == num){
            remover = listaCircular->start;
            listaCircular->start = NULL;
            listaCircular->end = NULL;
            listaCircular->length--;
        }
        else if(listaCircular->start->id == num){
            remover = listaCircular->start;
            listaCircular->start = remover->next;
            listaCircular->end->next = listaCircular->start;
            listaCircular->length--;
        }
        else{
            aux = listaCircular->start;
            while(aux->next != listaCircular->start && aux->next->id != num)
                aux = aux->next;
            if(aux->next->id == num){
                if(listaCircular->end == aux->next){
                    remover = aux->next;
                    aux->next = remover->next;
                    listaCircular->end = aux;
                }
                else{
                    remover = aux->next;
                    aux->next = remover->next;
                }
                listaCircular->length--;
            }
        }
    }

    return remover;
}

Page* buscar(ListaCircular *listaCircular, int num){
    Page *aux = listaCircular->start;

    if(aux){
        do{
            if(aux->id == num)
                return aux;
            aux = aux->next;
        }while(aux != listaCircular->start);
    }
    return NULL;
}

void imprimir(ListaCircular listaCircular){
    Page *page = listaCircular.start;
    printf("\n\tLista tam %d: ", listaCircular.length);
    if(page){
        do{
            printf("id: %d m: %d r: %d\n", page->id, page->m, page->r);
            page = page->next;
        }while(page != listaCircular.start);
        printf("\nInicio: %d\n", page->id);
    }
    printf("\n\n");
}

// Funções helper do clock
void inserir_pagina(ListaCircular *listaCircular, int address, int escrita) {
    Page *pagina_atual = listaCircular->start;
    
    if (pagina_atual->r == 0) // Se R for 0
    {
        // • Remove a página
        remover(listaCircular, pagina_atual->id);
        // • Coloca nova página no lugar
        if (escrita)
        {
            /* Escrita */
            inserir_no_inicio(listaCircular, address, 1, 1);
        } else {
            /* Leitura */
            inserir_no_inicio(listaCircular, address, 0, 1);
        }
        // • Avança o ponteiro
        rotaciona_lista(listaCircular);
    } else {
        // • Coloca 0 em R
        pagina_atual->r = 0;
        // • Avança o ponteiro
        rotaciona_lista(listaCircular);
        // • Repete verificação
        inserir_pagina(listaCircular, address, escrita);
    }
}

void teste(int seed){
    int page_miss_counter = 0;

    ListaCircular listaCircular;
    criar_lista(&listaCircular);

    // Criar memoria virtual
    Memoria memoria_virtual[MAX];
    for (int i = 0; i < MAX; i++)
    {
        memoria_virtual[i].modificada = rand() & 1;
        memoria_virtual[i].referenciada = rand() & 1;
        memoria_virtual[i].presente = rand() & 1;
    }
    
    inicializa_listacircular(&listaCircular, memoria_virtual);

    // printf("lista circular inicial\n\n\n");
    // imprimir(listaCircular);
    // printf("\n\n");
    // Randomizar um item para pegar na memoria
    for (int i = 0; i < N_LOOPS; i++)
    {
        int address = rand() % END_LOOPS + START_LOOPS;
        Memoria memory_item = memoria_virtual[address];
        // printf("Item buscando: address: %d presente: %d M: %d R: %d\n", address, memory_item.presente, memory_item.modificada, memory_item.referenciada);
        // Verificar se foi escrita ou leitura
        int escrita = rand() & 1;
        // Buscar item na lista circular
        Page *page = buscar(&listaCircular, address);
        if (page == NULL) {
            /* page miss */

            page_miss_counter++;
            inserir_pagina(&listaCircular, address, escrita);
            // printf("lista circular atualizada\n\n\n");
            // imprimir(listaCircular);
            // printf("\n\n");
        } else {
            /* Editar pagina*/
            if (escrita)
            {
                /* Escrita */
                page->m = 1;
                page->r = 1;
            } else {
                /* Leitura */
                page->m = 0;
                page->r = 1;
            }
        }
    }
    
    // Medir quantidade de page miss
    printf("page_miss_counter: %d\n", page_miss_counter);
    // Medir O(n) gpt
    // Tempo de execução
}

int main() {
    for (int i = 0; i < N_TESTES; i++)
    {
        printf("Teste clock %d:\n", i);
        teste(i);
    }
}
