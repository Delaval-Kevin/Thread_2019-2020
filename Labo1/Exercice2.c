/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 1 : Exercice 2                                      */
/*Date de la dernière mise à jour : 02/02/2020             */
/***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


struct Donnees{
	char nomFich[20];
	char mot[20];
	int  nbrTab;
};

void* ThreadA(void *p);

int main()
{
	pthread_t tid;
	int *val;
	
	struct Donnees donnee;
	
	strcpy(donnee.nomFich, "Fichier1.txt");
	strcpy(donnee.mot, "sit");
	donnee.nbrTab = 0;
	
	pthread_create(&tid, NULL, ThreadA, &donnee);
	
	pthread_join(tid, (void**)&val);
	
	printf("Cpt = %d \n", *val);
	
	free(val);

}

void* ThreadA(void *p)
{
	struct Donnees *Ptr = (Donnees*)p;

	int hf, cpt, stop = 0;
	unsigned int taille;
	char test[20];
	int *ptr = (int*)malloc(sizeof(int));
	(*ptr) = 0;
	
	cpt = 0;
	
	while(stop== 0)
	{	
		hf = open(Ptr->nomFich, O_RDONLY);
		
		for(int i = 0 ; i < Ptr->nbrTab ; i++)
		{
			printf("\t");
		}
		printf("*\n");
		
		lseek(hf, cpt, SEEK_SET);
		
		taille = read(hf, test, strlen(Ptr->mot));
		close(hf);
		
		if(taille != strlen(Ptr->mot))
		{
			stop = 1;
		}
		else
		{
			cpt++;
			if(strcmp(test, Ptr->mot) == 0)
			{
				(*ptr)++;
			}
		}
	}
	
	pthread_exit(ptr);
};






