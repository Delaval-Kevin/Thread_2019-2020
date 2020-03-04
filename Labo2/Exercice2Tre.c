/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 2 : Exercice 2 Tre                                  */
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



	/*Prototype thread*/
void* Thread(void *p);

	/*Prototype de fonction*/
void ThreadFin(void *p);

	/*Prototype SigHandler*/
void HandlerSigInt(int);
void HandlerSigUsr1(int);


int main()
{
	printf("Thread %d.%u se lance\n", getpid(), pthread_self());

	/*Masquage du signal SIGUSR1*/
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	sigprocmask(SIG_SETMASK, &mask,NULL);

	/*Armement des signaux*/
	struct sigaction A;
	A.sa_handler = HandlerSigInt;
	sigemptyset(&A.sa_mask);
	A.sa_flags = 0;
	if(sigaction(SIGINT,&A,NULL))
	{
		perror("(MAGASIN) Erreur d'armement du signal SIGINT");
		exit(1);
	}
	
	struct sigaction B;
	B.sa_handler = HandlerSigUsr1;
	sigemptyset(&B.sa_mask);
	B.sa_flags = 0;
	if(sigaction(SIGUSR1,&B,NULL))
	{
		perror("(MAGASIN) Erreur d'armement du signal SIGUSR1");
		exit(1);
	}

	/*Déclaration des variables*/	
	
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	pthread_t tid4;
	
	/*Création des threads*/
	
	pthread_create(&tid1, NULL, Thread, NULL);
	
	pthread_create(&tid2, NULL, Thread, NULL);
	
	pthread_create(&tid3, NULL, Thread, NULL);
	
	pthread_create(&tid4, NULL, Thread, NULL);
	
	pause();

	pause();

	pause();

	pause();
	
	printf("Thread %d.%u se termine\n", getpid(), pthread_self());
	
	pthread_exit(NULL);

}

void* Thread(void *p)
{	
	pthread_cleanup_push(ThreadFin,0);

	/*Masquage du signal SIGINT*/
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	printf("Thread %d.%u se lance\n", getpid(), pthread_self());

	pause();
	
	pthread_cleanup_pop(1);

	pthread_exit(NULL);
};

void HandlerSigInt(int Sig)
{
	printf("Thread %d.%u recois le SIGINT\n", getpid(), pthread_self());
	
	kill(getpid(), SIGUSR1);
}

void HandlerSigUsr1(int Sig)
{
	printf("Thread %d.%u a recu le SIGUSR1\n", getpid(), pthread_self());

}

void ThreadFin(void *p)
{
	printf("Thread %d.%u se termine\n", getpid(), pthread_self());
}




