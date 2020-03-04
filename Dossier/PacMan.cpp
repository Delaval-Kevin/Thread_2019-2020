/***********************************************************/
/*Auteur : DELAVAL Kevin                                   */
/*Groupe : 2203                                            */
/*Application : Pac-Man                                    */
/*Date de la dernière mise à jour : 03/03/2020             */
/***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"

// Dimensions de la grille de jeu
#define NB_LIGNE	21
#define NB_COLONNE	17

// Macros utilisees dans le tableau tab
#define VIDE         0
#define MUR          1
#define PACMAN       -2
#define PACGOM       -3
#define SUPERPACGOM  -4
#define BONUS        -5

// Autres macros
#define LENTREE 15
#define CENTREE 8

// Structure du fantome
typedef struct
{
	int L;
	int C;
	int couleur;
	int cache;
}S_FANTOME;

// Déclaration du tableau
int tab[NB_LIGNE][NB_COLONNE]
={ {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
   {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
   {1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1},
   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
   {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
   {1,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1},
   {1,1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,1},
   {1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1},
   {1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1},
   {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
   {1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
   {1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1},
   {1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
   {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
   {1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1},
   {1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1},
   {1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1},
   {1,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1},
   {1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
   {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
   
// Déclaration des variables globales
int mode		= 1;
int score		= 0;
int nbVert		= 0;
int nbRouge		= 0;
int nbMauve		= 0;
int nbOrange	= 0;
int nbPacGom	= 0;
int C			= 0;
int L			= 0;
int dir			= 0;
int delai		= 300;
bool MAJScore	= false;
bool Reset		= false;
pthread_t tidPacMan;
pthread_t tidTimeOut;

// Déclaration des mutex
pthread_mutex_t mutexTab;
pthread_mutex_t mutexMode;
pthread_mutex_t mutexDelai;
pthread_mutex_t mutexScore;
pthread_mutex_t mutexNbPacGom;
pthread_mutex_t mutexNbFantomes;

// Déclaration des variables condition
pthread_cond_t	condTab;
pthread_cond_t	condScore;
pthread_cond_t	condNbPacGom;
pthread_cond_t	condNbFantomes;

// Déclaration de la clé et du controleur
pthread_key_t cle;
pthread_once_t controleur = PTHREAD_ONCE_INIT;


// Prototypes fonctions
void InitCle();
void Attente(int milli);
void DessineGrilleBase();
void destructeur(void *p);
void FctFinFantome(void *p);
int CreeFantome(int couleur);
int FantomeBloque(int tmpMode);
int DeplacementFantome(int dir);
void DeplacementPacMan(int lAnc, int cAnc, int lSuiv, int cSuiv);
void DeplacementFantomeCommestible(int lAnc, int cAnc, int lSuiv, int cSuiv);
void DeplacementFantomeGlouton(int lAnc, int cAnc, int lSuiv, int cSuiv, int dir);

// Prototypes threads
void* threadVies(void *p);
void* threadEvent(void *p);
void* threadScore(void *p);
void* threadBonus(void *p);
void* threadPacGom(void *p);
void* threadPacMan(void *p);
void* threadTimeOut(void *p);
void* threadFantome(void *p);
void* threadCompteurFantomes(void *p);

// Prototype SigHandler
void HandlerUp(int);
void HandlerLeft(int);
void HandlerDown(int);
void HandlerRight(int);
void HandlerAlrm(int);
void HandlerChld(int);


int main(int argc,char* argv[])
{
	//Armement des signaux
	struct sigaction Left;
	Left.sa_handler = HandlerLeft;
	sigemptyset(&Left.sa_mask);
	Left.sa_flags = 0;
	if(sigaction(SIGINT,&Left,NULL))
	{
		perror("Erreur d'armement du signal SIGINT");
		exit(1);
	}

	struct sigaction Right;
	Right.sa_handler = HandlerRight;
	sigemptyset(&Right.sa_mask);
	Right.sa_flags = 0;
	if(sigaction(SIGHUP,&Right,NULL))
	{
		perror("Erreur d'armement du signal SIGHUP");
		exit(1);
	}
	
	struct sigaction Down;
	Down.sa_handler = HandlerDown;
	sigemptyset(&Down.sa_mask);
	Down.sa_flags = 0;
	if(sigaction(SIGUSR2,&Down,NULL))
	{
		perror("Erreur d'armement du signal SIGUSR2");
		exit(1);
	}
	
	struct sigaction Up;
	Up.sa_handler = HandlerUp;
	sigemptyset(&Up.sa_mask);
	Up.sa_flags = 0;
	if(sigaction(SIGUSR1,&Up,NULL))
	{
		perror("Erreur d'armement du signal SIGUSR1");
		exit(1);
	}

	struct sigaction Alrm;
	Alrm.sa_handler = HandlerAlrm;
	sigemptyset(&Alrm.sa_mask);
	Alrm.sa_flags = 0;
	if(sigaction(SIGALRM,&Alrm,NULL))
	{
		perror("Erreur d'armement du signal SIGALRM");
		exit(1);
	}	
	
	struct sigaction Chld;
	Chld.sa_handler = HandlerChld;
	sigemptyset(&Chld.sa_mask);
	Chld.sa_flags = 0;
	if(sigaction(SIGCHLD,&Chld,NULL))
	{
		perror("Erreur d'armement du signal SIGCHLD");
		exit(1);
	}	
		
	EVENT_GRILLE_SDL event;
	char ok;
	
	pthread_t tid;
 
	srand((unsigned)time(NULL));
	
	//Masquage des signaux
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);	
  
	// Initialisation des mutex
	pthread_mutex_init(&mutexTab, NULL);
	pthread_mutex_init(&mutexMode, NULL);
	pthread_mutex_init(&mutexDelai, NULL);
	pthread_mutex_init(&mutexScore, NULL);
	pthread_mutex_init(&mutexNbPacGom, NULL);
	pthread_mutex_init(&mutexNbFantomes, NULL);
	
	// Initialisation des conditions
	pthread_cond_init(&condTab, NULL);
	pthread_cond_init(&condScore, NULL);
	pthread_cond_init(&condNbPacGom, NULL);
	pthread_cond_init(&condNbFantomes, NULL);

  	// Ouverture de la fenetre graphique
  	printf("(MAIN %d) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  	if (OuvertureFenetreGraphique() < 0)
  	{
    	printf("Erreur de OuvrirGrilleSDL\n");
    	fflush(stdout);
    	exit(1);
  	}

  	DessineGrilleBase();

	//Création des threads
	pthread_create(NULL, NULL, threadScore, NULL);
	pthread_create(NULL, NULL, threadPacGom, NULL);
	pthread_create(NULL, NULL, threadVies, NULL);
	pthread_create(NULL, NULL, threadBonus, NULL);
	pthread_create(NULL, NULL, threadCompteurFantomes, NULL);
	pthread_create(&tid, NULL, threadEvent, NULL);
	
	pthread_join(tid, NULL);

	printf("Attente de 1500 millisecondes...\n");
  	Attente(1500);
  	// -------------------------------------------------------------------------
  
  	// Fermeture de la fenetre
  	printf("(MAIN %d) Fermeture de la fenetre graphique...",pthread_self());
  	fflush(stdout);
  	
  	FermetureFenetreGraphique();
  	
  	printf("OK\n"); 
  	fflush(stdout);

  	exit(0);
}

/********************************/
/*                              */
/*           Fonctions          */			
/*                              */
/********************************/

//*********************************************************************************************
void Attente(int milli)
{
  	struct timespec del;
  	del.tv_sec = milli/1000;
  	del.tv_nsec = (milli%1000)*1000000;
  	nanosleep(&del,NULL);
}

//*********************************************************************************************
void DessineGrilleBase()
{
  	for (int l=0 ; l<NB_LIGNE ; l++)
		for (int c=0 ; c<NB_COLONNE ; c++)
    	{
      		if (tab[l][c] == VIDE) EffaceCarre(l,c);
      		if (tab[l][c] == MUR) DessineMur(l,c);
    	}
}

//*********************************************************************************************
void DeplacementPacMan(int lAnc, int cAnc, int lSuiv, int cSuiv)
{	
	//On masque les signaux pour ne pas "tricher" sur la vitesse de PacMan
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	//On vide l'ancien emplacement de PacMan
	tab[lAnc][cAnc] = VIDE;
	EffaceCarre(lAnc,cAnc);
	
	//On regarde sur quoi il va
	if(tab[lSuiv][cSuiv] == PACGOM || tab[lSuiv][cSuiv] == SUPERPACGOM)
	{
		pthread_mutex_lock(&mutexNbPacGom);
		nbPacGom--;
		pthread_mutex_unlock(&mutexNbPacGom);
		pthread_cond_signal(&condNbPacGom);	
		
		pthread_mutex_lock(&mutexScore);
		if(tab[lSuiv][cSuiv] == PACGOM)
		{
			score++;
		}
		else
		{
			int *temps = new int;
			*temps = 0;

			if(!pthread_kill(tidTimeOut, 0))
			{
				*temps = alarm(0);
				pthread_kill(tidTimeOut, SIGQUIT);
			}

			score = score + 5;
			pthread_mutex_lock(&mutexMode);
			mode = 2;
			pthread_mutex_unlock(&mutexMode);

			pthread_create(&tidTimeOut, NULL, threadTimeOut, temps);
			pthread_detach(tidTimeOut);
		}
		//Mise à jour du score
		MAJScore = true;
		pthread_mutex_unlock(&mutexScore);
		pthread_cond_signal(&condScore);	
	}
	if(tab[lSuiv][cSuiv] == BONUS)
	{
		pthread_mutex_lock(&mutexScore);

		score = score + 30;

		//Mise à jour du score
		MAJScore = true;
		pthread_mutex_unlock(&mutexScore);
		pthread_cond_signal(&condScore);		
	}
	if(tab[lSuiv][cSuiv] > 1)
	{
		pthread_mutex_lock(&mutexMode);
		if(mode == 1)
		{
			pthread_mutex_unlock(&mutexTab);
			pthread_mutex_unlock(&mutexMode);
			printf("PacMan c'est tue\n");
			pthread_exit(NULL);
		}
		else
		{
			printf("PacMan mange le fantome\n");
			pthread_mutex_unlock(&mutexMode);
			pthread_kill(tab[lSuiv][cSuiv], SIGCHLD);
		}
	}	
	
	//Si PacMan n'est pas mort il avance
	tab[lSuiv][cSuiv] = PACMAN;
	DessinePacMan(lSuiv,cSuiv,dir);

	sigdelset(&mask, SIGINT);
	sigdelset(&mask, SIGHUP);
	sigdelset(&mask, SIGUSR1);
	sigdelset(&mask, SIGUSR2);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
}

//*********************************************************************************************
int CreeFantome(int couleur)
{
	pthread_t tid;
	S_FANTOME *fantome = NULL;
	
	fantome = new S_FANTOME;
	if(fantome == NULL)
	{
		printf("Erreur lors de la création du fantome\n");
		return 0;
	}
	
	fantome->L = 9;
	fantome->C = 8;
	fantome->cache = VIDE;
	fantome->couleur = couleur;
	
	pthread_create(&tid, NULL, threadFantome, fantome);
	pthread_detach(tid);
	
	return 1;
}

//*********************************************************************************************
void InitCle()
{
	//Pour la zone spécifique
	printf("Initialisation d'une cle\n");
	pthread_key_create(&cle, destructeur);
}

void destructeur(void *p)
{
	//On libère la mémoire alouée sur laquelle pointe la zone spécifique
	printf("Liberation d'une zone specifique\n");
	free(p);
}


//*********************************************************************************************
int DeplacementFantome(int dir)
{
	int tmpMode;
	S_FANTOME *PF = NULL;
	
	PF = (S_FANTOME*)pthread_getspecific(cle);
	
	//Verification du mode
	pthread_mutex_lock(&mutexMode);
	tmpMode = mode;
	pthread_mutex_unlock(&mutexMode);
	
	//Selon la direction et le mode le fantome sera dirigé vers
	//La fonction adéquate
	if(dir == GAUCHE && tab[PF->L][(PF->C)-1] < 1)
	{
		if(tmpMode == 1)
		{
			DeplacementFantomeGlouton(PF->L, PF->C, PF->L, (PF->C)-1, dir);
			PF->C = PF->C - 1;
		}
		else
		{
			if(tab[PF->L][(PF->C)-1] != PACMAN)
			{
				DeplacementFantomeCommestible(PF->L, PF->C, PF->L, (PF->C)-1);
				PF->C = PF->C - 1;			
			}
			else
			{
				dir = FantomeBloque(tmpMode);
			}
		}
	}
	else if(dir == DROITE && tab[PF->L][(PF->C)+1] < 1)
	{
		if(tmpMode == 1)
		{
			DeplacementFantomeGlouton(PF->L, PF->C, PF->L, (PF->C)+1, dir);
			PF->C = PF->C + 1;
		}
		else
		{
			if(tab[PF->L][(PF->C)+1] != PACMAN)
			{
				DeplacementFantomeCommestible(PF->L, PF->C, PF->L, (PF->C)+1);
				PF->C = PF->C + 1;			
			}
			else
			{
				dir = FantomeBloque(tmpMode);
			}
		}		
	}
	else if(dir == HAUT && tab[(PF->L)-1][PF->C] < 1)
	{
		if(tmpMode == 1)
		{
			DeplacementFantomeGlouton(PF->L, PF->C, (PF->L)-1, PF->C, dir);
			PF->L = PF->L - 1;
		}
		else
		{
			if(tab[(PF->L)-1][PF->C] != PACMAN)
			{
				DeplacementFantomeCommestible(PF->L, PF->C, (PF->L)-1, PF->C);
				PF->L = PF->L - 1;
			}
			else
			{
				dir = FantomeBloque(tmpMode);
			}
		}
	}	
	else if(dir == BAS && tab[(PF->L)+1][PF->C] < 1)
	{
		if(tmpMode == 1)
		{
			DeplacementFantomeGlouton(PF->L, PF->C, (PF->L)+1, PF->C, dir);
			PF->L = PF->L + 1;
		}
		else
		{
			if(tab[(PF->L)+1][PF->C] != PACMAN)
			{
				DeplacementFantomeCommestible(PF->L, PF->C, (PF->L)+1, PF->C);
				PF->L = PF->L + 1;			
			}
			else
			{
				dir = FantomeBloque(tmpMode);
			}
		}
	}
	else
	{
		dir = FantomeBloque(tmpMode);
	}

	return dir;
}

//*********************************************************************************************
int FantomeBloque(int tmpMode)	//Cette fontion fera appel à la fonction qui l'a appelée (forme de récursivité)
{								//Si un chemin est possible sinon le fantome restara à sa place
	int dir;
	S_FANTOME *PF = NULL;
	
	PF = (S_FANTOME*)pthread_getspecific(cle);
	
	if(tab[PF->L][(PF->C)+1] >= 1 && tab[(PF->L)+1][PF->C] >= 1 && tab[(PF->L)-1][PF->C] >= 1 && tab[PF->L][(PF->C)-1] >= 1)
	{
		//alors je suis bloqué et je ne bouge pas
	}
	else if((tmpMode == 2 && tab[PF->L][(PF->C)+1] == PACMAN && tab[(PF->L)+1][PF->C] >= 1 && tab[(PF->L)-1][PF->C] >= 1 && tab[PF->L][(PF->C)-1] >= 1) ||
			(tmpMode == 2 && tab[PF->L][(PF->C)+1] >= 1 && tab[(PF->L)+1][PF->C] == PACMAN && tab[(PF->L)-1][PF->C] >= 1 && tab[PF->L][(PF->C)-1] >= 1) ||
			(tmpMode == 2 && tab[PF->L][(PF->C)+1] >= 1 && tab[(PF->L)+1][PF->C] >= 1 && tab[(PF->L)-1][PF->C] == PACMAN && tab[PF->L][(PF->C)-1] >= 1) ||
			(tmpMode == 2 && tab[PF->L][(PF->C)+1] >= 1 && tab[(PF->L)+1][PF->C] >= 1 && tab[(PF->L)-1][PF->C] >= 1 && tab[PF->L][(PF->C)-1] == PACMAN))
	{
		//alors je suis bloqué et je ne bouge pas car en commestible et entourer de mur
		//d'autres fantomes et de PacMan
	}
	else
	{
		int choix, ok = 0;
				
		while(ok == 0)
		{
			choix = rand()%4;
			
			//Recherche d'une direction alléatoire qui convient 
			switch(choix)
			{
				case 0:
					if(tab[(PF->L)-1][PF->C] < 1)
					{
						if(tmpMode == 2 &&  tab[(PF->L)-1][PF->C] == PACMAN)
						{
							//ok reste à 0 car le fantome est en mode
							//commestible et il doit eviter PacMan
						}
						else
						{
							ok = 1;
							dir = HAUT;
							DeplacementFantome(dir);
						}
					}
					break;
				case 1:
					if(tab[PF->L][(PF->C)+1] < 1)
					{
						if(tmpMode == 2 && tab[PF->L][(PF->C)+1] == PACMAN)
						{
							//ok reste à 0 car le fantome est en mode
							//commestible et il doit eviter PacMan						
						}
						else
						{
							ok = 1;
							dir = DROITE;
							DeplacementFantome(dir);
						}
					}
					break;
				case 2:
					if(tab[(PF->L)+1][PF->C] < 1)
					{
						if(tmpMode == 2 && tab[(PF->L)+1][PF->C] == PACMAN)
						{
							//ok reste à 0 car le fantome est en mode
							//commestible et il doit eviter PacMan								
						}
						else
						{
							ok = 1;
							dir = BAS;
							DeplacementFantome(dir);
						}
					}						
					break;
				case 3:
					if(tab[PF->L][(PF->C)-1] < 1)
					{
						if(tmpMode== 2 && tab[PF->L][(PF->C)-1] == PACMAN)
						{
							//ok reste à 0 car le fantome est en mode
							//commestible et il doit eviter PacMan						
						}
						else
						{
							ok = 1;
							dir = GAUCHE;
							DeplacementFantome(dir);
						}
					}
					break;
			}
		}	
	}
	
	return dir;
}

//*********************************************************************************************
void DeplacementFantomeGlouton(int lAnc, int cAnc, int lSuiv, int cSuiv, int dir)
{
	S_FANTOME *PF = NULL;
	
	PF = (S_FANTOME*)pthread_getspecific(cle);
	
	//remettre la case ou était le fantome en état
	tab[lAnc][cAnc] = PF->cache;
	if(PF->cache == VIDE)
		EffaceCarre(lAnc,cAnc);
	else if(PF->cache == PACGOM)
		DessinePacGom(lAnc,cAnc);
	else if(PF->cache == SUPERPACGOM)
		DessineSuperPacGom(lAnc,cAnc);
	else if(PF->cache == BONUS)
		DessineBonus(lAnc,cAnc);
	
	if(tab[lSuiv][cSuiv] == PACMAN) //Le fantome tue PacMan et vide son cache
	{
		printf("Le Fantome a tue PacMan\n");
		PF->cache = VIDE;
		pthread_cancel(tidPacMan);
	}
	else //Le fantome rempli son cache
	{
		PF->cache = tab[lSuiv][cSuiv];
	}
	
	tab[lSuiv][cSuiv] = pthread_self();

	DessineFantome(lSuiv, cSuiv, PF->couleur, dir);
}

//*********************************************************************************************
void DeplacementFantomeCommestible(int lAnc, int cAnc, int lSuiv, int cSuiv)
{
	S_FANTOME *PF = NULL;
	
	PF = (S_FANTOME*)pthread_getspecific(cle);
	
	//remettre la case ou était le fantome en état
	tab[lAnc][cAnc] = PF->cache;
	if(PF->cache == VIDE)
		EffaceCarre(lAnc,cAnc);
	else if(PF->cache == PACGOM)
		DessinePacGom(lAnc,cAnc);
	else if(PF->cache == SUPERPACGOM)
		DessineSuperPacGom(lAnc,cAnc);
	else if(PF->cache == BONUS)
		DessineBonus(lAnc,cAnc);
	
	//Le fantome rempli son cache
	PF->cache = tab[lSuiv][cSuiv];

	tab[lSuiv][cSuiv] = pthread_self();

	DessineFantomeComestible(lSuiv, cSuiv);
}

//*********************************************************************************************
void FctFinFantome(void *p)
{
	S_FANTOME *PF = NULL;
	
	//Recupère la structure dans la zone spécifique
	PF = (S_FANTOME*)pthread_getspecific(cle);
	
	//Décrémente le fantome mort
	pthread_mutex_lock(&mutexNbFantomes);
	if(PF->couleur == ROUGE)
	{
		printf("ROUGE\n");
		nbRouge--;
	}
	else if(PF->couleur == ORANGE)
	{
		printf("ORANGE\n");
		nbOrange--;
	}
	else if(PF->couleur == VERT)
	{
		printf("VERT\n");
		nbVert--;
	}
	else
	{
		printf("MAUVE\n");
		nbMauve--;
	}
	pthread_mutex_unlock(&mutexNbFantomes);
	
	//Si le fantome est tué par PacMac (non par le passage de niveau)
	//On ajoute des points au scre de PacMan
	if(Reset == false)
	{
		pthread_mutex_lock(&mutexScore);
		pthread_mutex_lock(&mutexNbPacGom);
	
		score = score + 50;
		if(PF->cache == PACGOM)
		{
			score++;
			nbPacGom--;
		}
		else if(PF->cache == SUPERPACGOM)
		{
			score = score + 5;
			nbPacGom--;
		}

		MAJScore = true;
		pthread_mutex_unlock(&mutexNbPacGom);
		pthread_mutex_unlock(&mutexScore);
		pthread_cond_signal(&condScore);
	}
}

/********************************/
/*                              */
/*            Threads           */			
/*                              */
/********************************/

//*********************************************************************************************
void* threadPacMan(void *p)
{	
	//Masquage des signaux
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGINT);
	sigdelset(&mask, SIGHUP);
	sigdelset(&mask, SIGUSR1);
	sigdelset(&mask, SIGUSR2);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	//Initialisation de PacMan de départ 
	int attente;
	C	= CENTREE;
	L	= LENTREE;
	dir	= GAUCHE;

	//On récupère le délai entre chaque mouvement
	pthread_mutex_lock(&mutexDelai);
	attente = delai;
	pthread_mutex_unlock(&mutexDelai);	
	
	
	//Pour quand il meurt, on attend que la case de réaparition
	//Et les cases à coté soient vide.
	pthread_mutex_lock(&mutexTab);
	while(tab[L][C] > 0 && tab[L][C-1] > 0 && tab[L][C+1] > 0)
	{
		pthread_mutex_unlock(&mutexTab);
		Attente(attente);
		pthread_mutex_lock(&mutexTab);
	}
		
	//On place PacMan et on attend
	tab[L][C] = PACMAN;
	DessinePacMan(L,C,dir);
	pthread_mutex_unlock(&mutexTab);
	Attente(attente);

	
	while(1)
	{
		pthread_mutex_lock(&mutexTab);
		DessinePacMan(L,C,dir);
		
		if(dir == GAUCHE)
		{
			if(L == 9 && C == 0)
			{
				DeplacementPacMan(L, C, L, 16);
				C = 16;
			}
			else if(tab[L][C-1] != 1)
			{
				DeplacementPacMan(L, C, L, C-1);
				C--;
			}	
		}
		else if(dir == DROITE)
		{
			if(L == 9 && C == 16)
			{
				DeplacementPacMan(L, C, L, 0);
				C = 0;			
			}
			else if(tab[L][C+1] != 1)
			{
				DeplacementPacMan(L, C, L, C+1);
				C++;
			}
		}
		else if(dir == HAUT && tab[L-1][C] != 1)
		{
			DeplacementPacMan(L, C, L-1, C);
			L--;
		}
		else if(dir == BAS && tab[L+1][C] != 1)
		{
			DeplacementPacMan(L, C, L+1, C);
			L++;
		}
		pthread_mutex_unlock(&mutexTab);
		
		//On masque les signaux pour ne pas "tricher" sur la vitesse de PacMan pendant l'attente
		sigfillset(&mask);
		sigprocmask(SIG_SETMASK, &mask,NULL);
		
		pthread_mutex_unlock(&mutexDelai);
		attente = delai;
		pthread_mutex_unlock(&mutexDelai);
		Attente(attente);
			
		sigdelset(&mask, SIGINT);
		sigdelset(&mask, SIGHUP);
		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGUSR2);
		sigprocmask(SIG_SETMASK, &mask,NULL);
	}
};

//*********************************************************************************************
void* threadEvent(void *p)
{
	//Permet d'envoyer le bon signal selon la touche enfoncée
	//Ou de quitter le thread avec la 'X' et donc le jeu 
	int run = 1;
	EVENT_GRILLE_SDL evenement;

	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
		
	while(run)
	{
		evenement = ReadEvent();

		if(evenement.touche == KEY_DOWN)
		{
			kill(getpid(),SIGUSR2);
		}
		if(evenement.touche == KEY_UP)
		{
			kill(getpid(),SIGUSR1);
		}
		if(evenement.touche == KEY_LEFT)
		{
			kill(getpid(),SIGINT);
		}
		if(evenement.touche == KEY_RIGHT)
		{
			kill(getpid(),SIGHUP);
		}
		if(evenement.type == CROIX)
		{
			run = 0;
		}
	}

	pthread_exit(NULL);
}

//*********************************************************************************************
void* threadPacGom(void *p)
{
	int niveau = 1;

	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	pthread_mutex_lock(&mutexTab);	
	
	while(1)
	{
		//On met les PacGom partout ou il n'y a pas de mur
		pthread_mutex_lock(&mutexNbPacGom);
		
		for(int i = 0 ; i < NB_LIGNE ; i++)
		{
			for(int j = 0 ; j < NB_COLONNE ; j++)
			{
				if(tab[i][j] == 0)
				{
					tab[i][j] = PACGOM;
					DessinePacGom(i,j);
					nbPacGom++;
				}
			}
		}
		
		//On retire les 3 PacGoms aux endroit npn désiré
		tab[15][8] = VIDE;
		EffaceCarre(15,8);

		tab[8][8] = VIDE;
		EffaceCarre(8,8);

		tab[9][8] = VIDE;
		EffaceCarre(9,8);
		nbPacGom = nbPacGom - 3;
		
		//On place les 4 SuperPacGoms
		tab[2][1] = SUPERPACGOM;
		DessineSuperPacGom(2,1);
			
		tab[2][15] = SUPERPACGOM;
		DessineSuperPacGom(2,15);
		
		tab[15][1] = SUPERPACGOM;
		DessineSuperPacGom(15,1);
		
		tab[15][15] = SUPERPACGOM;
		DessineSuperPacGom(15,15);
		
		pthread_mutex_unlock(&mutexTab);
		
		DessineChiffre(14, 22, niveau);
		
		//Attente qu'il n'y ai plus de PacGom
		while(nbPacGom > 0)
		{
			DessineChiffre(12, 22, nbPacGom/100);
			DessineChiffre(12, 23, (nbPacGom/10)%10);
			DessineChiffre(12, 24, nbPacGom%10);
			pthread_cond_wait(&condNbPacGom, &mutexNbPacGom);
		}
		pthread_mutex_unlock(&mutexNbPacGom);
		pthread_mutex_lock(&mutexTab);
		
		//Mettre le jeu en Reset car on passe au niveau supérieur
		Reset = true; // et on ne gagne pas de points en tuant les fantomes
		
		//On tue les fantomes
		for(int i = 0 ; i < NB_LIGNE ; i++)
		{
			for(int j = 0 ; j < NB_COLONNE ; j++)
			{
				if(tab[i][j] > 1)
				{
					pthread_kill(tab[i][j], SIGCHLD);
					tab[i][j] = VIDE;
				}
			}
		}
		
		//Remettre le jeu en mode normal
		Reset = false;
		
		//On enleve le threadTimeOut s'il y en avait un
		if(!pthread_kill(tidTimeOut, 0))
		{
			alarm(0);
			pthread_kill(tidTimeOut, SIGQUIT);
		}
		
		//On remet le mode sur 1
		pthread_mutex_lock(&mutexMode);
		mode = 1;
		pthread_mutex_unlock(&mutexMode);		
		
		//On replace PacMan au point de départ
		tab[L][C] = VIDE;	
		C = CENTREE;
		L = LENTREE;
		dir = GAUCHE;
		tab[L][C] = PACMAN;
		
		//On monte le niveau et accélère la vitesse 
		niveau++;
		pthread_mutex_lock(&mutexDelai);
		delai = delai / 2;
		pthread_mutex_unlock(&mutexDelai);	
		
		//On envoie un signal pour recréer les fantomes du point de départ
		pthread_cond_signal(&condNbFantomes);
	}
}

//*********************************************************************************************
void* threadScore(void *p)
{
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	while(1)
	{
		while(MAJScore == false)
		{
			DessineChiffre(16, 22, (score%10000)/1000);
			DessineChiffre(16, 23, (score%1000)/100);
			DessineChiffre(16, 24, (score%100)/10);
			DessineChiffre(16, 25, score%10);
			pthread_cond_wait(&condScore, &mutexScore);
		}
		MAJScore = false;
		pthread_mutex_unlock(&mutexScore);
	}
}

//*********************************************************************************************
void* threadBonus(void *p)
{
	int l, c;

	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	while(1)
	{
		//On attend entre 10 à 20 seconde pour l'apparition du Bonus
		Attente(((rand()%11)+10)*1000);
		
		l = 0;
		c = 0;
		//On recherche une case vide
		pthread_mutex_lock(&mutexTab);
		while(tab[l][c] != 0)
		{
			l = rand()%NB_LIGNE;
			c = rand()%NB_COLONNE;
		}
		tab[l][c] = BONUS;
		DessineBonus(l,c);
		pthread_mutex_unlock(&mutexTab);
		
		//On attend 10 secondes
		Attente(10000);
		
		//Si le Bonus est toujours la on le supprime 
		pthread_mutex_lock(&mutexTab);
		if(tab[l][c] != 0)
		{
			tab[l][c] = VIDE;
			EffaceCarre(l, c);
		}
		pthread_mutex_unlock(&mutexTab);
	}
}

//*********************************************************************************************
void* threadCompteurFantomes(void *p)
{
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	pthread_mutex_lock(&mutexNbFantomes);	// placé en dehors de la boucle, sinon deadlock
	while(1)								// a cause de la reprise de mutex au "pthread_cond_wait"
	{
		//Boucle de creation des fantomes
		while(nbRouge != 2 || nbMauve != 2 || nbOrange != 2 || nbVert != 2)
		{
			if(nbRouge != 2)
			{
				if(CreeFantome(ROUGE))
					nbRouge++;
			}
			if(nbVert != 2)
			{
				if(CreeFantome(VERT))
					nbVert++;
			}			
			if(nbOrange != 2)
			{
				if(CreeFantome(ORANGE))
					nbOrange++;
			}
			if(nbMauve != 2)
			{
				if(CreeFantome(MAUVE))
					nbMauve++;
			}		
		}
		
		//Tant que le mode est à 2 ou que tous les fantomes sont là on attend
		pthread_mutex_lock(&mutexMode);
		while(nbRouge == 2 && nbMauve == 2 && nbOrange == 2 && nbVert == 2 || mode == 2)
		{
			pthread_mutex_unlock(&mutexMode);
			pthread_cond_wait(&condNbFantomes, &mutexNbFantomes);
			pthread_mutex_lock(&mutexMode);
		}
		pthread_mutex_unlock(&mutexMode);
	}
}

//*********************************************************************************************
void* threadFantome(void *p)
{
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGCHLD);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	S_FANTOME *PF = (S_FANTOME*)p;
	int dir = HAUT;
	int attente;
	pthread_t tid = pthread_self();
	
	//Initialisation de la cle avec la fonction
	pthread_once(&controleur , InitCle);
	
	//Push la fonction de terminaison du thread
	pthread_cleanup_push(FctFinFantome,0);
	
	//On attend que la case d'entree soit vide pour pouvoir apparaitre
	pthread_mutex_lock(&mutexTab);
	while(tab[9][8] != VIDE)
	{
		pthread_mutex_unlock(&mutexTab);
		Attente(delai);
		pthread_mutex_lock(&mutexTab);
	}
	
	
	DessineFantome(PF->L, PF->C, PF->couleur, dir);
	tab[9][8] = pthread_self();
	pthread_mutex_unlock(&mutexTab);
	
	//On met la structure fantome dans la zone spécifique
	pthread_setspecific(cle, PF);
	
	while(1)
	{
		pthread_mutex_lock(&mutexTab);

		dir = DeplacementFantome(dir);

		pthread_mutex_unlock(&mutexTab);
		
		pthread_mutex_unlock(&mutexDelai);
		attente = (delai/3)*5;
		pthread_mutex_unlock(&mutexDelai);
		
		Attente(attente);
	}
	
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

//*********************************************************************************************
void* threadVies(void *p)
{
	sigset_t mask;
	sigfillset(&mask);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	int vies = 3;
	
	while(vies > 0)
	{
		DessineChiffre(18, 22, vies);
		
		pthread_create(&tidPacMan, NULL, threadPacMan, NULL);
		
		pthread_join(tidPacMan, NULL);
		
		vies--;
	}

	pthread_mutex_lock(&mutexTab);
	DessineChiffre(18, 22, vies);
	DessineGameOver(9,4);
}

//*********************************************************************************************
void* threadTimeOut(void *p)
{
	int *temps = (int*)p;
	//Masquage des signaux
	sigset_t mask;
	sigfillset(&mask);
	sigdelset(&mask, SIGALRM);
	sigdelset(&mask, SIGQUIT);
	sigprocmask(SIG_SETMASK, &mask,NULL);
	
	*temps = (rand()%8) + 8 + *temps;

	alarm(*temps);
	
	//Liberation de la mémoire pour l'allocation dynamique
	free(temps);
	pause();
	pthread_exit(NULL);
}

/********************************/
/*                              */
/*        Handlers Signaux      */			
/*                              */
/********************************/

//*********************************************************************************************
void HandlerLeft(int Sig)
{
//	printf("Réception du signal Left\n");
	
	dir = GAUCHE;
	
	struct sigaction Left;
	Left.sa_handler = HandlerLeft;
	sigemptyset(&Left.sa_mask);
	Left.sa_flags = 0;
	if(sigaction(SIGINT,&Left,NULL))
	{
		perror("(PACMAN) Erreur d'armement du signal SIGINT");
		exit(1);
	}
}

//*********************************************************************************************
void HandlerRight(int Sig)
{
//	printf("Réception du signal Right\n");
	
	dir = DROITE;
}

//*********************************************************************************************
void HandlerDown(int Sig)
{
//	printf("Réception du signal Down\n");
	
	dir = BAS;
}

//*********************************************************************************************
void HandlerUp(int Sig)
{
//	printf("Réception du signal Up\n");
	
	dir = HAUT;
}

//*********************************************************************************************
void HandlerAlrm(int Sig)
{
	//Permet de remmettre le mode à 1 et de "reveiller" threadCompteurFantomes 
	pthread_mutex_lock(&mutexMode);
	mode = 1;
	pthread_mutex_unlock(&mutexMode);
	pthread_cond_signal(&condNbFantomes);
}

//*********************************************************************************************
void HandlerChld(int Sig)
{
	pthread_exit(NULL);
}



