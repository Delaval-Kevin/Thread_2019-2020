/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 1 : Exercice 3                                      */
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
void* ThreadB(void *p);
void* ThreadC(void *p);
void* ThreadD(void *p);

int main()
{
	/*Déclaration des variables*/	
	
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	pthread_t tid4;
	
	int *val1;
	int *val2;
	int *val3;
	int *val4;
	
	struct Donnees donnee1;
	struct Donnees donnee2;
	struct Donnees donnee3;
	struct Donnees donnee4;
	
	/*Initialisation des structures*/
	
	strcpy(donnee1.nomFich, "Fichier1.txt");
	strcpy(donnee1.mot, "sit");
	donnee1.nbrTab = 0;
	
	strcpy(donnee2.nomFich, "Fichier2.txt");
	strcpy(donnee2.mot, "ante");
	donnee2.nbrTab = 1;
	
	strcpy(donnee3.nomFich, "Fichier3.txt");
	strcpy(donnee3.mot, "nec");
	donnee3.nbrTab = 2;
	
	strcpy(donnee4.nomFich, "Fichier4.txt");
	strcpy(donnee4.mot, "neque");
	donnee4.nbrTab = 3;
	
	/*Création des threads*/
	
	pthread_create(&tid1, NULL, ThreadA, &donnee1);
	
	pthread_create(&tid2, NULL, ThreadB, &donnee2);
	
	pthread_create(&tid3, NULL, ThreadC, &donnee3);
	
	pthread_create(&tid4, NULL, ThreadD, &donnee4);
	
	/*Récupération des valeurs de retour*/
	
	pthread_join(tid1, (void**)&val1);
	
	pthread_join(tid2, (void**)&val2);
	
	pthread_join(tid3, (void**)&val3);
	
	pthread_join(tid4, (void**)&val4);
	
	/*Affichage des résultats*/
	
	printf("Cpt1 = %d \n", *val1);
	printf("Cpt2 = %d \n", *val2);
	printf("Cpt3 = %d \n", *val3);
	printf("Cpt4 = %d \n", *val4);
	
	/*Libération de la mémoire*/
	
	free(val1);
	free(val2);
	free(val3);
	free(val4);

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


void* ThreadB(void *p)
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


void* ThreadC(void *p)
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


void* ThreadD(void *p)
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







