/*****************************************************************/
/*         Tabela de Dispersao | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tabdispersao.h"
#include "grafo.h"

tabela_dispersao *tabela_nova(int capacidade, hash_func *hfunc, sond_func *sfunc)
{
    if (capacidade < 0 || hfunc == NULL || sfunc == NULL)
        return NULL;

    tabela_dispersao *td = malloc (sizeof(tabela_dispersao));   //aloca espaco para a tabela
    if(td == NULL)
        return NULL;

    td->nos = malloc (capacidade * sizeof(no_grafo*));  //aloca espaco para os apontadores para os nos
    if (td->nos == NULL)
    {
        free (td);
        return NULL;
    }

    td->estado_celulas = calloc (capacidade, sizeof(int));  //aloca espaco para o vetor estado das celulas e inicializa-os como 0 (vazio)
    if (td->estado_celulas == NULL)
    {
        free(td->nos);
        free(td);
        return NULL;
    }

    //inicializa os restantes elementos da tabela
    td->tamanho = 0;
    td->capacidade = capacidade;
    td->hfunc = hfunc;
    td->sfunc = sfunc;

    return td;
}

int tabela_adiciona(tabela_dispersao *td, no_grafo *entrada)
{
    if (td == NULL || entrada == NULL)
        return -1;

    int index, indexsond, tent;

    if (td->capacidade == td->tamanho)  //ha espaco livre
        return -1;

    if (tabela_existe(td, entrada->cidade) != -1)   //ainda nao existe esse no
        return -1;  
    
    index = td->hfunc(entrada->cidade, td->capacidade);
    
    if (td->estado_celulas[index] == 0 || td->estado_celulas[index] == -1)
    {
        td->nos[index] = entrada;
        td->estado_celulas[index] = 1;
        td->tamanho++;
        return index;
    }
    else
    {
        tent = 0;
        while(1)    //enquanto nao encontrar um espaço livre nao para (ja foi verificado que havia espaço livre)
        {
            indexsond = td->sfunc(index, tent, td->capacidade);

            if (td->estado_celulas[indexsond] == 0 || td->estado_celulas[indexsond] == -1)
            {
                td->nos[indexsond] = entrada;
                td->estado_celulas[indexsond] = 1;
                td->tamanho++;
                return indexsond;
            }
            tent++;
        }
    }
    return -1;  
}

int tabela_remove(tabela_dispersao *td, no_grafo *saida)
{
    if (td == NULL || saida == NULL)
        return -1;

    if (td->tamanho == 0)   //tabela nao esta vazia
        return -1;

    int i = tabela_existe(td, saida->cidade);

    if(i == -1)
        return -1;
    else
    {
        td->nos[i] = NULL;
        td->estado_celulas[i] = -1; 
        td->tamanho--;
        return 0;
    }   
}

void tabela_apaga(tabela_dispersao *td)
{
    free(td->estado_celulas);
    free(td->nos);
    free(td);
}

int tabela_existe(tabela_dispersao *td, const char *cidade)
{
    if(cidade == NULL || td == NULL)
        return -1;

    if (td->tamanho == 0)
        return -1;

    int index = td->hfunc(cidade, td->capacidade), indexsond;
    indexsond = index;

    if (td->estado_celulas[index] == 1)
    {
        if (strcmp(td->nos[index]->cidade, cidade) == 0)
            return index;
    }
    else
    {
        for (int tent = 0; (td->estado_celulas[indexsond] == 1 || td->estado_celulas[indexsond] == -1) && tent < td->tamanho; tent++)
        {
            indexsond = td->sfunc(index, tent, td->capacidade);

            if(td->estado_celulas[indexsond] == 1)
            {
                if (strcmp(td->nos[indexsond]->cidade, cidade) == 0)
                    return indexsond;
            }
        }
    }

    return -1;
}

tabela_dispersao *tabela_carrega(grafo *g, int tamanho)
{
    if (g == NULL || tamanho < 0)
        return NULL;

    if (g->tamanho > tamanho)
        return NULL;

    tabela_dispersao *td;

    if ((td = tabela_nova(tamanho, hash_krm, sond_rh)) == NULL)
        return NULL;

    for (int i = 0; i < g->tamanho; i++)
        tabela_adiciona(td, g->nos[i]);
    
    return td;
}

/*================================================================================*/
unsigned long sond_rh(int pos, int tentativas, int tamanho)
{

    return (pos + tentativas * tentativas) % tamanho;
}

unsigned long hash_krm(const char *chave, int tamanho)
{
    int c, t = strlen(chave);
    unsigned long hash = 7;

    for (c = 0; c < t; c++)
    {
        hash += (int)chave[c];
    }

    return hash % tamanho;

    return 0;
}
