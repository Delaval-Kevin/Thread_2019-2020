/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Thread                                     */
/*Labo 1 : Exercice 1                                      */
/*Date de la dernière mise à jour : 02/02/2020             */
/***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>

void* ThreadA(void *p);

int main()
{
	pthread_t tid;
	int *val;
	
	pthread_create(&tid, NULL, ThreadA, NULL);
	
	pthread_join(tid, (void**)&val);
	
	printf("Cpt = %d \n", *val);
	
	free(val);

}

void* ThreadA(void *p)
{
	int *ptr = (int*)malloc(sizeof(int));
	
	(*ptr) = 5;
	
	pthread_exit(ptr);
};
