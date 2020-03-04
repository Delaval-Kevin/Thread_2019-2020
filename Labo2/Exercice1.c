/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 2 : Exercice 1                                      */
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


	/*Prototype thread*/
void* Thread(void *p);

	/*Prototype SigHandler*/
void HandlerSigInt(int);


int main()
{
	printf("Thread %d.%u se lance\n", getpid(), pthread_self());

	/*Armement du signal*/
	struct sigaction A;
	A.sa_handler = HandlerSigInt;
	sigemptyset(&A.sa_mask);
	A.sa_flags = 0;
	if(sigaction(SIGINT,&A,NULL))
	{
		perror("(MAGASIN) Erreur d'armement du signal SIGINT");
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
	
	pthread_exit(NULL);

}

void* Thread(void *p)
{	
	printf("Thread %d.%u se lance\n", getpid(), pthread_self());

	pause();
	
	pthread_exit(NULL);
};

void HandlerSigInt(int Sig)
{
	printf("Thread %d.%u se termine\n", getpid(), pthread_self());
}





