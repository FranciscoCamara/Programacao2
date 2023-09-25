/*****************************************************************/
/*          Grafo direcionado | PROG2 | MIEEC | 2020/21          */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "grafo.h"
#include "heap.h"

void muda_prioridade (no_grafo *no, heap *h, double prioridade);

grafo *grafo_novo()
{
    grafo *g = (grafo *)malloc(sizeof(grafo));
    g->tamanho = 0;
    g->nos = NULL;

    return g;
}

no_grafo *encontra_no(grafo *g, char *cidade)
{
    if (!g || !cidade)
        return NULL;

    // pesquisa por cidade no vetor de nós
    for (int i = 0; i < g->tamanho; i++)
    {
        if (strcmp(g->nos[i]->cidade, cidade) == 0)
            return g->nos[i];
    }
    return NULL;
}

no_grafo *no_insere(grafo *g, char *cidade)
{
    if (!g || !cidade)
        return NULL;

    // verifica se o nó já existe
    no_grafo *no = encontra_no(g, cidade);
    if (no)
        return NULL;

    // cria o novo nó para o user
    no = (no_grafo *)malloc(sizeof(no_grafo));
    if (!no)
        return NULL;

    // aloca espaço para o campo cidade
    no->cidade = (char *)malloc((strlen(cidade) + 1) * sizeof(char));
    if (!no->cidade)
    {
        free(no);
        return NULL;
    }
    // inicializa todos os campos
    strcpy(no->cidade, cidade);
    no->tamanho = 0;
    no->arestas = NULL;
    no->p_acumulado = 0;
    no->anterior = NULL;
    no->dataatualizada = NULL;

    // insere o nó criado no final do vetor de nós
    g->nos = (no_grafo **)realloc(g->nos, (g->tamanho + 1) * sizeof(no_grafo *));
    if (!g->nos)
    {
        free(no->cidade);
        free(no);
        return NULL;
    }
    // incrementa o tamanho do numero de nós e insere no vetor de apontadores de nós
    g->tamanho++;
    g->nos[g->tamanho - 1] = no;

    return no;
}

// função auxiliar que retorna 1 se existir a aresta para destino ou 0,
// se a aresta não existir, -1 se der erro
int existe_aresta(no_grafo *origem, no_grafo *destino, char *codigo)
{

    if (!origem || !destino)
        return -1;

    //pesquisa em todas as arestas se existe
    for (int i = 0; i < origem->tamanho; i++)
    {

        if ((origem->arestas[i]->destino == destino) && ((strcmp(origem->arestas[i]->codigo, codigo) == 0)))
            return 1;
    }

    return 0;
}

int cria_aresta(no_grafo *origem, no_grafo *destino, char *codigo, char *companhia, data partida, data chegada, float preco, int lugares)
{
    if (!origem || !destino || !codigo || !companhia)
        return -1;

    if (preco < 0 || lugares < 0)
        return -1;

    // verifica se a ligação já existe
    if (existe_aresta(origem, destino, codigo) > 0)
        return -1;

    // cria a nova ligação
    aresta_grafo *ag = (aresta_grafo *)malloc(sizeof(aresta_grafo));
    if (!ag)
        return -1;

    ag->destino = destino;
    ag->preco = preco;
    ag->lugares = lugares;
    // aloca espaço para o código
    ag->codigo = (char *)malloc((strlen(codigo) + 1) * sizeof(char));
    if (!ag->codigo)
    {
        free(ag);
        return -1;
    }
    strcpy(ag->codigo, codigo);
    // aloca espaço para o companhia
    ag->companhia = (char *)malloc((strlen(companhia) + 1) * sizeof(char));
    if (!ag->companhia)
    {
        free(ag->codigo);
        free(ag);
        return -1;
    }
    strcpy(ag->companhia, companhia);

    // inicializa todos os campos
    ag->partida.tm_year = partida.tm_year;
    ag->partida.tm_mon = partida.tm_mon;
    ag->partida.tm_mday = partida.tm_mday;
    ag->partida.tm_hour = partida.tm_hour;
    ag->partida.tm_min = partida.tm_min;
    ag->partida.tm_sec = 0;
    ag->partida.tm_isdst = 0;

    // inicializa todos os campos
    ag->chegada.tm_year = chegada.tm_year;
    ag->chegada.tm_mon = chegada.tm_mon;
    ag->chegada.tm_mday = chegada.tm_mday;
    ag->chegada.tm_hour = chegada.tm_hour;
    ag->chegada.tm_min = chegada.tm_min;
    ag->chegada.tm_sec = 0;
    ag->chegada.tm_isdst = 0;

    // insere a nova ligação no vetor de ligações
    origem->arestas = (aresta_grafo **)realloc(origem->arestas, (origem->tamanho + 1) * sizeof(aresta_grafo *));
    if (!origem->arestas)
    {
        free(ag->codigo);
        free(ag->companhia);
        free(ag);
        return -1;
    }
    origem->tamanho++;
    origem->arestas[origem->tamanho - 1] = ag;

    return 0;
}

no_grafo *no_remove(grafo *g, char *cidade)
{
    if (!g || !cidade)
        return NULL;

    no_grafo *no;

    int i, ver = 0;

    for (i = 0; i < g->tamanho; i++)    //procura no onde esta a cidade e guarda a posicao
    {
        if (strcmp(g->nos[i]->cidade, cidade) == 0)
        {
            no = g->nos[i];
            ver = 1;
            break;
        }  
    } 

    if (ver == 0)   //nao encontro no com nome cidade
        return NULL;

    for (int k = 0; k < g->tamanho; k++)    //percorre nos do grafo para retirar as arestas que tem como destino o no a remover
    {
        for (int l = 0; l < g->nos[k]->tamanho; l++)    //precorre aresta do no
        {
            if (strcmp(g->nos[k]->arestas[l]->destino->cidade, cidade) == 0)
            {
                if(aresta_apaga (g->nos[k]->arestas[l]) == -1)
                    return NULL;    

                for(int p = l; p < g->nos[k]->tamanho - 1; p++)
                    g->nos[k]->arestas[p] = g->nos[k]->arestas[p + 1];

                g->nos[k]->arestas = realloc (g->nos[k]->arestas, (g->nos[k]->tamanho - 1) * sizeof(aresta_grafo*));
                g->nos[k]->tamanho--;
                l--;
            }
        }
    }

    for (int h = 0; h < no->tamanho; h++)
        aresta_apaga(no->arestas[h]);

    free(no->arestas);   
    no->arestas = NULL;
    no->tamanho = 0;

    for (int j = i; j < g->tamanho - 1; j++)
        g->nos[j] = g->nos[j + 1];  //remove o no do grafo 

    g->tamanho--;   //reajustando as posicoes e o tamanho
    g->nos = realloc (g->nos, g->tamanho * sizeof(no_grafo*));

    return no;
}

int aresta_apaga(aresta_grafo *aresta)
{
    if (!aresta)
        return -1;

    free (aresta->codigo);
    free (aresta->companhia);
    free (aresta);

    return 0;
}

int no_apaga(no_grafo *no)
{
    if (!no)
        return -1;

    if (no->tamanho != 0)
    {
        for (int i = 0; i < no->tamanho; i++)   //apaga as arestas todas
        {
            if (aresta_apaga(no->arestas[i]) == -1)
                return -1;
        }

        free(no->arestas);
    }
    
    free(no->cidade);
    free (no);

    return 0;
}

void grafo_apaga(grafo *g)
{
    for (int i = 0; i < g->tamanho; i++) //apaga os nos todos
        no_apaga(g->nos[i]);

    free(g->nos);
    free(g);
}

no_grafo *encontra_voo(grafo *g, char *codigo, int *aresta_pos)
{
    if (!g || !codigo)
        return NULL;

    no_grafo *no;

    for (int i = 0; i < g->tamanho; i++)    //percorre nos do grafo
    {
        for (int j = 0; j < g->nos[i]->tamanho; j++)    //precorre aresta do no
        {
            if(strcmp(codigo, g->nos[i]->arestas[j]->codigo) == 0)  //encontrou
            {
                *aresta_pos = j;
                no = g->nos[i];
                return no;
            }
        }
    }

    return NULL;    //nao encontrou
}

no_grafo **pesquisa_avancada(grafo *g, char *destino, data chegada, double preco_max, int *n)
{
    if (!g || !destino || preco_max < 0)
        return NULL;

    no_grafo **nos = NULL;
    int tamanho = 0;

    for (int i = 0; i < g->tamanho; i++)    //percorre nos do grafo
    {
        for (int j = 0; j < g->nos[i]->tamanho; j++)    //precorre aresta do no
        {
            if(strcmp(destino, g->nos[i]->arestas[j]->destino->cidade) == 0 && g->nos[i]->arestas[j]->chegada.tm_mday == chegada.tm_mday && g->nos[i]->arestas[j]->chegada.tm_mon == chegada.tm_mon && g->nos[i]->arestas[j]->chegada.tm_year == chegada.tm_year && g->nos[i]->arestas[j]->preco <= preco_max)  //encontrou um com o destino pretendido
            {
                //guarda o apontador em nos e incrementa o tamnho que vai ser passdo por referencia
                tamanho++;
                nos = realloc(nos, tamanho * sizeof(no_grafo*));
                nos[tamanho-1] = g->nos[i];
            }
        }
    }

    if (tamanho == 0)   //nao encontrou voos
        return NULL;

    *n = tamanho;
    return nos;
}

no_grafo **trajeto_mais_rapido(grafo *g, char *origem, char *destino, data partida, int *n)
{
    if (g == NULL || origem == NULL || destino == NULL) 
        return NULL;

    no_grafo **nosver = malloc(g->tamanho * sizeof(no_grafo*)), *noorigem, *no, *noaux, **nosret = malloc(g->tamanho * sizeof(no_grafo*)), **nosaux = malloc(g->tamanho * sizeof(no_grafo*));
    int tamanho = 0, p = 0;
    double tempo_voo;
    heap *h = heap_nova(g->tamanho);
    
    noorigem = encontra_no(g, origem);
    if(noorigem == NULL)
    {
        free (nosret);
        free (nosver);
        free (nosaux);
        heap_apaga (h);
        return NULL;
    }
    
    noaux = encontra_no(g, destino);
    if(noaux == NULL)
    {
        free (nosret);
        free (nosver);
        free (nosaux);
        heap_apaga (h);
        return NULL;
    }

    heap_insere(h, noorigem, 0);
    

    for (int i = 0; i < g->tamanho; i++)    //insere todos os nos na heap
    {
        if (strcmp(noorigem->cidade, g->nos[i]->cidade) != 0)
        {
            heap_insere(h, g->nos[i], 99999999);
            g->nos[i]->p_acumulado = 99999999;
        }  
    }

    while(h->tamanho != 0)
    {
        no = heap_remove(h);

        for (int j = 0; j < no->tamanho; j++)
        {   
            if (mktime(&no->arestas[j]->partida) >= mktime(&partida))   //verifica se voo e depois de partida
            {
                if(no->anterior != NULL)    //Se tiver voo anterior
                {
                    if (mktime(&no->arestas[j]->partida) >= mktime(no->anterior->dataatualizada))   //data de voo de partida depois da de voo de chegada
                    {
                        tempo_voo = difftime(mktime(&no->arestas[j]->chegada), mktime(&no->arestas[j]->partida));
                        tempo_voo += difftime(mktime(&no->arestas[j]->partida), mktime(no->anterior->dataatualizada));
                    }
                    else
                        continue;
                }
                else    //1ª viagem
                    tempo_voo = difftime(mktime(&no->arestas[j]->chegada), mktime(&no->arestas[j]->partida));
            
            
                if(tempo_voo + no->p_acumulado < no->arestas[j]->destino->p_acumulado)  //altera se tempo de viagem for menor
                {
                    no->arestas[j]->destino->p_acumulado = tempo_voo + no->p_acumulado;
                    muda_prioridade(no->arestas[j]->destino, h, no->arestas[j]->destino->p_acumulado);
                    no->dataatualizada = &no->arestas[j]->chegada;
                    no->arestas[j]->destino->anterior = no;
                }
            }
        }

        nosver[p] = no;
        p++;   
    }

    while (1)
    {  
        if(noaux->anterior != NULL) // Se tiver voo para esta cidade
        {
            tamanho++;
            nosaux[tamanho - 1] = noaux;
            noaux = noaux->anterior;
        }
        else 
        {
            if (strcmp(noaux->cidade, origem) == 0) //cidade de origem -> inicio do vetor de retorno e fim da pesquisa pelo caminho
            {                
                tamanho++;
                nosaux[tamanho - 1] = noaux;
                break;
            }
            else    // nao encontrou cidade de origem -> nao ha caminho possivel para chegar a destino a partir de partida
            {
                free (nosret);
                free (nosver);
                free (nosaux);
                heap_apaga (h);
                return NULL;
            }
        }
    }

    for (int l = 0; l < tamanho; l++)
        nosret[l] = nosaux[tamanho - l - 1];

    heap_apaga(h);
    free(nosaux);
    free(nosver);

    *n = tamanho;

    for(int t = 0; t < g->tamanho; t++) // Reinicializa todos os parametros dos nos para futuras pesquisas
    {
        g->nos[t]->anterior = NULL;
        g->nos[t]->dataatualizada = NULL;
        g->nos[t]->p_acumulado = 0;
    }
    
    return nosret;
}

void muda_prioridade (no_grafo *no, heap *h, double prioridade_nova)
{
    elemento *aux;
    for (int i = 0; i < h->tamanho; i++)
    {
        if (h->elementos[i] != NULL)
        {
            if (h->elementos[i]->no == no)
            {
                h->elementos[i]->prioridade = prioridade_nova;
                while (i != 1 && h->elementos[i]->prioridade < h->elementos[i/2]->prioridade)
                {
                    aux = h->elementos[i/2];
                    h->elementos[i/2] = h->elementos[i];
                    h->elementos[i] = aux;
                    i = i/2;
                }

                break;
            }
        }
    }
}

no_grafo **menos_transbordos(grafo *g, char *origem, char *destino, data partida, int *n)
{
    if (g == NULL || origem == NULL || destino == NULL) 
        return NULL;

    no_grafo **nosver = malloc(g->tamanho * sizeof(no_grafo*)), *noorigem, *no, *noaux, **nosret = malloc(g->tamanho * sizeof(no_grafo*)), **nosaux = malloc(g->tamanho * sizeof(no_grafo*));
    int tamanho = 0, p = 0, trans = 0;
    heap *h = heap_nova(g->tamanho);
    
    noorigem = encontra_no(g, origem);
    if(noorigem == NULL)
    {
        free (nosret);
        free (nosver);
        free (nosaux);
        heap_apaga (h);
        return NULL;
    }

    noaux = encontra_no(g, destino);
    if(noaux == NULL)
    {
        free (nosret);
        free (nosver);
        free (nosaux);
        heap_apaga (h);
        return NULL;
    } 

    heap_insere(h, noorigem, 0);

    for (int i = 0; i < g->tamanho; i++)    //insere todos os nos na heap
    {
        if (strcmp(noorigem->cidade, g->nos[i]->cidade) != 0)
        {
            heap_insere(h, g->nos[i], 99999999);
            g->nos[i]->p_acumulado = 99999999;
        }  
    }

    while(h->tamanho != 0)
    {
        no = heap_remove(h);

        for (int j = 0; j < no->tamanho; j++)
        {   
            if (mktime(&no->arestas[j]->partida) >= mktime(&partida))   //Verifica se voo é depois da partida
            {
                trans = 1;  
            
                if(trans + no->p_acumulado < no->arestas[j]->destino->p_acumulado)  //Altera se o caminho tiver menos trasbordos
                {
                    no->arestas[j]->destino->p_acumulado = trans + no->p_acumulado;
                    muda_prioridade(no->arestas[j]->destino, h, no->arestas[j]->destino->p_acumulado);
                    no->arestas[j]->destino->anterior = no;
                }
            }
        }

        nosver[p] = no;
        p++;   
    }

    while (1)
    {  
        if(noaux->anterior != NULL) // Se tiver voo para esta cidade
        {
            tamanho++;
            nosaux[tamanho - 1] = noaux;
            noaux = noaux->anterior;
        }
        else 
        {
            if (strcmp(noaux->cidade, origem) == 0) //cidade de origem -> inicio do vetor de retorno e fim da pesquisa pelo caminho
            {                
                tamanho++;
                nosaux[tamanho - 1] = noaux;
                break;
            }
            else    // nao encontrou cidade de origem -> nao ha caminho possivel para chegar a destino a partir de partida
            {
                free (nosret);
                free (nosver);
                free (nosaux);
                heap_apaga (h);
                return NULL;
            }
        }
    }

    for (int l = 0; l < tamanho; l++)
        nosret[l] = nosaux[tamanho - l - 1];

    heap_apaga(h);
    free(nosaux);
    free(nosver);

    *n = tamanho;

    for(int t = 0; t < g->tamanho; t++) // Reinicializa todos os parametros dos nos para futuras pesquisas
    {
        g->nos[t]->anterior = NULL;
        g->nos[t]->dataatualizada = NULL;
        g->nos[t]->p_acumulado = 0;
    }
    
    return nosret;
}

aresta_grafo **atualiza_lugares(char *ficheiro, grafo *g, int *n)
{
    if(!ficheiro || !g)
        return NULL;

    FILE *f = fopen(ficheiro, "r");
    if (f == NULL)
        return NULL;

    char frase[20], *codigo;
    int nlugares, arestapos, tamanho = 0;
    no_grafo *no;
    aresta_grafo *aresta;
    aresta_grafo **arestasvemov = NULL;

    while(fgets(frase, 20, f))
    {
        codigo = strtok(frase, ",");
        nlugares = atoi(strtok(NULL, ","));

        if (nlugares < 0)   //numero de lugares invalido
            return NULL;

        no = encontra_voo(g, codigo, &arestapos);

        if(no == NULL)  //nao existe voo com tal codigo
            return NULL;

        if (nlugares != 0)  //atualiza numero de lugares
            no->arestas[arestapos]->lugares = nlugares;

        else    //remove voo com 0 lugares e acrescenta-o no vetor de retorno
        {
            aresta = no->arestas[arestapos];

            for(int p = arestapos; p < no->tamanho - 1; p++)    //sobrepoe as outras arestas no vetor
                no->arestas[p] = no->arestas[p + 1];

            no->arestas = realloc (no->arestas, (no->tamanho - 1) * sizeof(aresta_grafo*)); //atualiza o tamnho do vetor das arestas do no
            no->tamanho--;

            tamanho++;
            arestasvemov = realloc(arestasvemov, tamanho * sizeof(aresta_grafo*));  //atualiza o tamnho do vetor das arestas a retornar
            arestasvemov[tamanho - 1] = aresta;
        }
    }

    fclose(f);
    *n = tamanho;
    return arestasvemov;
}

grafo *grafo_importa(const char *nome_ficheiro)
{
    if (nome_ficheiro == NULL)
        return NULL;

    FILE *f = fopen(nome_ficheiro, "r");
    if (f == NULL)
        return NULL;

    grafo *g = grafo_novo();
    if (g == NULL)
    {
        fclose(f);
        return NULL;
    }
    char str[500] = {0}, codigo[15] = {0}, origem[50] = {0}, destino[50] = {0}, companhia[4] = {0};
    int lugares = 0;
    double preco = 0;
    struct tm partida, chegada;

    char *token;
    no_grafo *no_origem, *no_destino;
    int ret;
    while (fgets(str, 500 * sizeof(char), f) != NULL)
    {
        str[strlen(str) - 1] = '\0';

        token = strtok(str, ",");
        if (!token)
            break;
        strcpy(codigo, token);

        token = strtok(NULL, ",");
        strcpy(origem, token);

        token = strtok(NULL, ",");
        strcpy(destino, token);

        token = strtok(NULL, ",");
        partida.tm_year = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_mon = atoi(token) - 1;

        token = strtok(NULL, ",");
        partida.tm_mday = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_hour = atoi(token);

        token = strtok(NULL, ",");
        partida.tm_min = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_year = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_mon = atoi(token) - 1;

        token = strtok(NULL, ",");
        chegada.tm_mday = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_hour = atoi(token);

        token = strtok(NULL, ",");
        chegada.tm_min = atoi(token);

        token = strtok(NULL, ",");
        preco = atof(token);

        token = strtok(NULL, ",");
        lugares = atoi(token);

        token = strtok(NULL, ",");
        strcpy(companhia, token);

        // procura no grafo um nó com o nome da origem
        no_origem = encontra_no(g, origem);
        // se o nó ainda não existe, cria-o e insere-o no grafo
        if (!no_origem)
        {

            no_origem = no_insere(g, origem);

            if (!no_origem)
            {
                fclose(f);
                return NULL;
            }
        }
        // procura no grafo um nó com o nome do destino
        no_destino = encontra_no(g, destino);

        // se o nó ainda não existe, cria-o e insere-o no grafo
        if (!no_destino)
        {
            no_destino = no_insere(g, destino);

            if (!no_destino)
            {
                fclose(f);
                return NULL;
            }
        }

        if (existe_aresta(no_origem, no_destino, codigo) == 0)
        {

            ret = cria_aresta(no_origem, no_destino, codigo, companhia, partida, chegada, preco, lugares);

            if (ret == -1)
            {
                fclose(f);
                return NULL;
            }
        }
    }

    fclose(f);
    return g;
}
