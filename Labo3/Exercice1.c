/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 3 : Exercice 1                                      */
/*Date de la dernière mise à jour : 10/02/2020             */
/***********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "Ecran.h"


typedef struct
{
	char	szDonnee[20];
	int		NbSecondes;
}DONNEE;


	/*Prototype thread*/
void* Thread(void *p);
void* fctThreadAffTime(void *p);

	/*Prototype de fonction*/
void ThreadFin(void *p);

	/*Prototype SigHandler*/
void HandlerSigAlrm(int);



int Compteur = 4;
pthread_mutex_t mutexNbrThread;
pthread_mutex_t mutexParam;
pthread_cond_t	conditionMutex;
DONNEE Param;


int main()
{
	printf("Thread %d.%u se lance\n", getpid(), pthread_self());
	int i = -1;

	/*Initialisation du mutex*/
	pthread_mutex_init(&mutexNbrThread, NULL);
	pthread_mutex_init(&mutexParam, NULL);
	
	pthread_cond_init(&conditionMutex, NULL);


	/*Armement des signaux*/
	struct sigaction A;
	A.sa_handler = HandlerSigAlrm;
	sigemptyset(&A.sa_mask);
	A.sa_flags = 0;
	if(sigaction(SIGALRM,&A,NULL))
	{
		perror("(MAGASIN) Erreur d'armement du signal SIGALRM");
		exit(1);
	}

	/*Déclaration des variables*/
	DONNEE Elm[] = { "MERCENIER" , 5 ,
					 "VILVENS" , 9 ,
					 "WAGNER" , 3 ,
					 "DE FOOZ" ,7,
					 "" , 0 };	

	
	/*Création des threads*/
	
	pthread_create(NULL, NULL, fctThreadAffTime, NULL);
	
	while( i < 3 )
	{
		i++;
		pthread_mutex_lock(&mutexParam);
		memcpy(&Param, &Elm[i], sizeof(DONNEE));
		pthread_create(NULL, NULL, Thread, &Param);


	}
	
	/*Masquage du signal SIGALRM*/
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_SETMASK, &mask,NULL);	
	
	/*Pour attendre la fin des autres threads*/
	pthread_mutex_lock(&mutexNbrThread);
	while(Compteur > 0)
	{
		pthread_cond_wait(&conditionMutex, &mutexNbrThread);
	}
					 
	exit(1);

}

void* Thread(void *p)
{	
	pthread_cleanup_push(ThreadFin,0);

	DONNEE Ptr = (*(DONNEE*)p);
	pthread_mutex_unlock(&mutexParam);
	timespec temps;
	
	printf("Thread %d.%u se lance et recoit le nom :%s\n", getpid(), pthread_self(),Ptr.szDonnee);
	
	temps.tv_sec = Ptr.NbSecondes;
	temps.tv_nsec = 0;
	
	nanosleep(&temps, NULL);

	pthread_cleanup_pop(1);
	
	pthread_exit(NULL);
};


void* fctThreadAffTime(void *p)
{
	/*Permet de lancer le SIGALRM pour la mise */
	/*à jour de l'heure toutes les secondes    */
	while (1)
	{
		alarm(1);
		pause();
	}

};


void ThreadFin(void *p)
{
	printf("Thread %d.%u se termine\n", getpid(), pthread_self());
	HandlerSigAlrm(SIGALRM);
	/*Prends le mutex pour décompter la variable Compteur*/
	pthread_mutex_lock(&mutexNbrThread);
	Compteur--;
	pthread_mutex_unlock(&mutexNbrThread);
	pthread_cond_signal(&conditionMutex);
};

void HandlerSigAlrm(int Sig)
{
	/*Récupere les données time pour afficher l'heure*/
	time_t heure;
	time(&heure);
	
	SauveCurseur();
	AffChaine(ctime(&heure),0,55,GRAS);
	RestitueCurseur();
}




