/*****************************************************************/
/*          Biblioteca plantas | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#include "plantas.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

planta *planta_nova(const char *ID, const char *nome_cientifico, char **alcunhas, int n_alcunhas, int n_sementes)
{
	if(ID == NULL || nome_cientifico == NULL || n_sementes < 0 || n_alcunhas < 0) 
		return NULL;

	planta *Planta_Nova;

	Planta_Nova = (planta*) malloc(sizeof(planta));

	if (Planta_Nova == NULL)
	{
		free (Planta_Nova);
		return NULL;
	}	

	strcpy(Planta_Nova->ID, ID);

	strcpy(Planta_Nova->nome_cientifico, nome_cientifico);

	Planta_Nova->n_alcunhas = n_alcunhas;

		Planta_Nova->n_sementes = n_sementes;

	if (n_alcunhas > 0)
	{
		Planta_Nova->alcunhas = malloc(n_alcunhas * sizeof(char*));
		for (int i = 0; i < n_alcunhas; i++)
		{
			Planta_Nova->alcunhas[i] = malloc((strlen(alcunhas[i]) + 1) * sizeof(char));
			strcpy(Planta_Nova->alcunhas[i], alcunhas[i]);
		}
	}
	else 
		Planta_Nova->alcunhas = NULL;

	return Planta_Nova;
}

colecao *colecao_nova(const char *tipo_ordem)
{
	if (tipo_ordem == NULL)
		return NULL;

	colecao *Colecao_Nova;

	Colecao_Nova = (colecao*) malloc(sizeof(colecao));

	if(Colecao_Nova == NULL)
	{
		free (Colecao_Nova);
		return NULL;
	}

	Colecao_Nova->tamanho = 0;

	if(strcmp(tipo_ordem, "nome") == 0 && strcmp(tipo_ordem, "id") == 0)
		return NULL;
		
	strcpy(Colecao_Nova->tipo_ordem, tipo_ordem);

	Colecao_Nova->plantas = NULL;

	return Colecao_Nova;
}

int planta_insere(colecao *c, planta *p)
{
	if(c == NULL || p == NULL)
		return -1;

	int a, b = 0, k;
	planta *l;

	if (c->tamanho == 0)	//não existem plantas na coleção
	{
		c->plantas = malloc(sizeof(planta));
	
		if (c->plantas == NULL)
		{
			free (c->plantas);
			return -1;
		}

		c->tamanho++;

		c->plantas[0] = planta_nova(p->ID, p->nome_cientifico, p->alcunhas, p->n_alcunhas, p->n_sementes);

		planta_apaga(p);
		return 0;
	}
	else
	{
		for (int i = 0; i < c->tamanho; i++)
	   	{
			if (strcmp(c->plantas[i]->ID, p->ID) == 0)				//planta ja existe na coleção
			{
				b++;
				c->plantas[i]->n_sementes += p->n_sementes;			//atualiza nº de sementes	

					for(int j = 0; j < p->n_alcunhas; j++)			//percorre as alcunhas da planta para verificar se ja existe na plata da coleção
					{
						for (k = 0; k < c->plantas[i]->n_alcunhas; k++) 		//percorre as alcunhas da planta da coleção
						{
							a = 0;
							if (strcmp(c->plantas[i]->alcunhas[k], p->alcunhas[j]) == 0)
							{
								a = 1;
								break;
							}
						}

						if (a == 0)								//alcunha não existe
						{
							c->plantas[i]->n_alcunhas += 1;		//atualiza nº de alcunhas
							c->plantas[i]->alcunhas = (char**)realloc(c->plantas[i]->alcunhas, (c->plantas[i]->n_alcunhas) * sizeof(char *));
							c->plantas[i]->alcunhas[c->plantas[i]->n_alcunhas - 1] = (char*)malloc((strlen(p->alcunhas[j]) + 1) * sizeof(char));
							strcpy(c->plantas[i]->alcunhas[k], p->alcunhas[j]);
						}
					}
				return 1;
			}
		}
		if(b == 0)
		{
			c->tamanho++;			//planta nova
			c->plantas = (planta **)realloc(c->plantas, c->tamanho * sizeof(planta*));

			c->plantas[c->tamanho - 1] = planta_nova(p->ID, p->nome_cientifico, p->alcunhas, p->n_alcunhas, p->n_sementes);

			if(strcmp(c->tipo_ordem, "id") == 0)
			{
				int pos1 = c->tamanho - 1;  // posição da planta inserida (fim do vetor)
				int pos2 = c->tamanho - 2;  // posição da planta anterior à inserida
				while(strcmp(c->plantas[pos1]->ID, c->plantas[pos2]->ID) < 0)
				{
					l = c->plantas[pos1];
					c->plantas[pos1] = c->plantas[pos2];
					c->plantas[pos2] = l;

					pos1--;
					pos2--;

					if(pos1 == 0)  // se a planta inserida foi para o início, já não há mais nada para trocar
						break;
				}
			}

			if(strcmp(c->tipo_ordem, "nome") == 0)
			{
				int pos1 = c->tamanho - 1;  // posição da planta inserida (fim do vetor)
				int pos2 = c->tamanho - 2;  // posição da planta anterior à inserida
				while(strcmp(c->plantas[pos1]->nome_cientifico, c->plantas[pos2]->nome_cientifico) < 0)
				{
					l = c->plantas[pos1];
					c->plantas[pos1] = c->plantas[pos2];
					c->plantas[pos2] = l;

					pos1--;
					pos2--;

					if(pos1 == 0)  // se a planta inserida foi para o início, já não há mais nada para trocar
						break;
				}
			}
			planta_apaga(p);
			return 0;
		}
	}
	return -1;
}

int colecao_tamanho(colecao *c)
{
	if (c == NULL)
		return -1;
	else
		return c->tamanho;
}

colecao *colecao_importa(const char *nome_ficheiro, const char *tipo_ordem)
{
	if(nome_ficheiro == NULL || tipo_ordem == NULL)
		return NULL;

	FILE *f;
	planta *P;
	colecao *Colecao;
	char aux[500], id[10], nome[MAX_NAME];
	char **al;
	int sementes, virg, l, g, i;
	char *token;
	
	f = fopen(nome_ficheiro,"r");
	
	if (f == NULL)
		return NULL;

	Colecao = colecao_nova(tipo_ordem);

	if (Colecao == NULL)
		return NULL;
    
	while (fgets(aux, 500, f) != NULL)
	{
		g = 0;
		aux[strlen(aux) - 1] = 0;

		token = strtok(aux, ","); 
		strcpy(id, token);
  
    	for (virg = 1; token != NULL; virg++) 
		{
        	token = strtok(NULL, ",");
			if (token == NULL)
				break;

			if (virg == 1)
				strcpy(nome, token); 

			if (virg == 2)
				sementes = atoi(token); 

			if (virg > 2)
			{	
				g++;
				if(virg == 3)
					al = (char **)malloc(sizeof(char*) * g);
				if(virg > 3)
					al = (char **)realloc(al, g * sizeof(char*));

				al[g - 1] = (char *)malloc((strlen(token) + 1) * sizeof(char));
				strcpy(al[g - 1], token);
			}
		}	

		P = planta_nova(id, nome, al, g, sementes);

		if (P == NULL)
		{
			planta_apaga(P);
			for(i = 0; i < g; i++)
				free(al[i]);

			free(al);
			return NULL;
		}
			
		l = planta_insere(Colecao, P);

		if (l == -1)
		{
			planta_apaga(P);
			colecao_apaga(Colecao);
			for(i = 0; i < g; i++)
				free(al[i]);

			free(al);
			return NULL;
		}
		
		if(g > 0)
		{
			for(i = 0; i < g; i++)
				free(al[i]);

			free(al);
		}
	}
	fclose(f);
	return Colecao;
}

planta *planta_remove(colecao *c, const char *nomep)
{
	planta *P;

	if(c == NULL || nomep == NULL)
		return NULL;

	for (int i = 0; i < c->tamanho ; i++)
	{
		if(strcmp(nomep , c->plantas[i]->nome_cientifico) == 0)
		{
			P = c->plantas[i];

            for (int j = i + 1; j < c->tamanho; j++)
			{
				c->plantas[j-1] = c->plantas[j];
			}

			c->plantas = (planta**)realloc(c->plantas, (c->tamanho - 1) * sizeof(planta));
			
			c->tamanho--;
			return P;			
		}
	}
	return NULL;	
}

int planta_apaga(planta *p)
{
	if (p == NULL)
		return -1;

	for (int i = 0; i < p->n_alcunhas; i++)
		free(p->alcunhas[i]);

	free(p->alcunhas);
	free (p);

	return 0;
}

int colecao_apaga(colecao *c)
{
	if (c == NULL)
		return -1;

	for (int i = 0; i < c->tamanho; i++)
		planta_apaga(c->plantas[i]);

	free(c->plantas);
	free(c);
	
	return 0;
}

int *colecao_pesquisa_nome(colecao *c, const char *nomep, int *tam)
{
	if(c == NULL || nomep == NULL)
		return NULL;

	int *p;
	int l = 0;  
	*tam = 0;  
	
	p = (int *)calloc(c->tamanho, sizeof(int));

    for (int i = 0; i < c->tamanho; i++)
	{
		if (strcmp(nomep,c->plantas[i]->nome_cientifico) == 0)
		{
			*tam += 1;
			p[l]= i;
			l++;
			continue;		
		}

		for (int j = 0; j < c->plantas[i]->n_alcunhas; j++)
		{
			if (strcmp(nomep,c->plantas[i]->alcunhas[j]) == 0)
			{		
				*tam += 1;
				p[l]=i;
				l++;
			}
			
		}
	}

	if (*tam == 0)
	{
		free(p);
		p = NULL;
	}

	return p;
}

int colecao_reordena(colecao *c, const char *tipo_ordem)
{
	if(c == NULL || tipo_ordem == NULL)
		return -1;

	int pos = 0, imin, i;
	planta *aux;

	if((strcmp(tipo_ordem, "id") != 0) && (strcmp(tipo_ordem, "nome") != 0))
		return -1;

	if(strcmp(c->tipo_ordem, tipo_ordem) == 0)
		return 0;

	if(strcmp(c->tipo_ordem, tipo_ordem) != 0)
	{
		if (strcmp(c->tipo_ordem, "id") != 0)		
		{
			while (pos < c->tamanho - 1) 			//Ordenação por inserção fornecido em prog1
			{
				imin = pos;
				i = pos + 1;
				while (i < c->tamanho)
				{ 
					if (strcmp(c->plantas[i]->ID, c->plantas[imin]->ID) < 0) 
						imin = i;
					i++; 
				}

				if (imin != pos) 
				{ 
					aux = c->plantas[pos];
					c->plantas[pos] = c->plantas[imin];
					c->plantas[imin] = aux;
				}
				pos ++;
			}
			strcpy(c->tipo_ordem, tipo_ordem);
			return 1;
		}

		if (strcmp(c->tipo_ordem, "nome") != 0)
		{
			while (pos < c->tamanho - 1) 		//Ordenação por inserção fornecido em prog1
			{
				imin = pos;
				i = pos + 1;
				while (i < c->tamanho)
				{ 
					if (strcmp(c->plantas[i]->nome_cientifico, c->plantas[imin]->nome_cientifico) < 0) 
						imin = i;
					i++; 
				}

				if (imin != pos) 
				{ 
					aux = c->plantas[pos];
					c->plantas[pos] = c->plantas[imin];
					c->plantas[imin] = aux;
				}
				pos ++;
			}
			strcpy(c->tipo_ordem, tipo_ordem);
			return 1;
		}
	}
	return -1;
}