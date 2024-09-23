#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/*		Structures & Variables globales		*/

int duree_de_maintenance = 3;
double prod[3] = { 0, 0.4, 0.65 };
double V[3][3] = { {0.2, 0.73, 0.07},
                   {0.11, 0.74, 0.15},
                   {0.04, 0.61, 0.35} } ;
int cout_maintenance = 150;                             
double proba_panne = 0.15;

typedef struct Eolienne
{
    int temps_maintenance;
    struct Eolienne *suiv;
}Eolienne ;


typedef struct ParcEolien
{
	struct Eolienne* tete; 
}ParcEolien;




/*		Prototypes		*/


	/*		Fonctions utiles		*/

int zero_cent(); // retourne un entier aléatoire entre 0 et 100
void creation_liste_eolienne( ParcEolien *parc, int nombre_eolienne ); // création de la liste contenant toutes les éoliennes
void desallouer_la_memoire(ParcEolien* parc); // Pour désallouer la mémoire
int etat_du_vent( int ancien_etat );// Détermine l'etat du vent en fonction du vent du jour précédent
int* tableau_vent( int duree_test );// Création du tableau contenant les vents de tous les jours 
int* cout_par_jour( int duree_test ); // initialise un tableau de longueur du test avec que des 0, cout en milliers
double* prod_par_jour( int duree_test ); //initialise un tableau de longueur du test avec que des 0 , puissance en MW
void eolienne_tombe_en_panne( Eolienne* eolienne ); //determine si une éolienne tombe en panne et change la veleur dans la structure ATTENTION VALABLE QUE SI eolienne->temps_maintenance =0 et tableau_vent[i]=2
void parcours_parc_zero( ParcEolien *parc, int jour, double* tab_prod ); // on regarde toutes les éoliennes quand vent=0, elles ne peuvent pas tomber en panne
void parcours_parc_un( ParcEolien *parc, int jour, double* tab_prod ); // vent =1
void parcours_parc_deux( ParcEolien *parc, int jour, double* tab_prod, int* tab_cout );// vent = 2, pas de maintenance possible, possibilité de tomber en panne, donc cout en plus, prod en plus
void simuler_un_jour( ParcEolien *parc, int jour, double* tab_prod, int* tab_cout, int* tab_vent ); //Simulation d'un jour
void simulation( ParcEolien *parc, double* tab_prod, int* tab_cout, int duree_test ); // Simulation sur une durée décidée précédement, avec un nbre d'eolienne dans le parc définie avant 
void menu(void);

	/*		Fonctions pour le menu		*/

void maxprod( double *tab_prod, int duree_test ); // Affiche la production journalière maximale et le jour correspondant à ce maximum
void minprod( double *tab_prod, int duree_test ); // Affiche la production journalière minimale et le jour correspondant à ce minimum
double prod_duree( double *tab_prod, int duree_test ); // Retourne la production totale du parc durant l'étude
void prod_moyenne( double *tab_prod, int duree_test ); // Affiche la production moyenne journalière du parc
void cout_de_maintenance( int *tab_cout, int duree_test ); // Calcul le cout total des maintenances sur la durée de la simulation
void nombre_eoliennes_cassees( int *tab_cout, int duree_test );
void donnees_pour_un_jour(double *tab_prod, int *tab_cout, int jour);


/*		FONCTIONS		*/


void creation_liste_eolienne( ParcEolien *parc, int nombre_eolienne )
 {
  for( int i = 0; i<nombre_eolienne; i++ )
 {
	Eolienne *nouveau;
	nouveau = (Eolienne*) malloc( sizeof(Eolienne) );
	nouveau->temps_maintenance = 0;
	nouveau->suiv = parc->tete;
	parc->tete = nouveau;  
   }
}

void desallouer_la_memoire(ParcEolien* parc)
{
    Eolienne * courant = parc->tete;
    Eolienne * precedent;

    while (courant != NULL) 
    {
        precedent = courant;
        courant  = courant->suiv;
        free(precedent); // On libère l'espace mémoire alloué par un malloc
    }

    parc->tete = NULL;
}


int zero_cent()
{
	return rand()%101;
}

int etat_du_vent( int ancien_etat )
{

	int p;
	p = zero_cent();
	if( ancien_etat==0 )
	{
	
		if ( 0<=p && p<V[0][0]*100 )
		{
			return 0;
		}
		else if ( V[0][0]*100 <= p && p<(V[0][1]+V[0][0])*100 )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else if( ancien_etat==1 )
	{
	
		if ( 0<=p && p<V[1][0]*100 )
		{
			return 0;
		}
		else if ( V[1][0]*100 <= p && p<(V[1][1]+V[1][0])*100 )
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	if( ancien_etat==2 )
	{
	
		if ( 0<=p && p<V[2][0]*100 )
		{
			return 0;
		}
		else if ( V[2][0]*100 <= p && p<(V[2][1]+V[2][0])*100 )
		{
			return 1;
		}
		else
		{
			return 2;
		}
		return 0;
	}
}
int* tableau_vent( int duree_test )
{
	int* tableau = (int*) malloc( duree_test*sizeof(int) );
	tableau[0]=0;
	for( int i=1; i<duree_test; i++ )
	{
		tableau[i] = etat_du_vent( tableau[i-1] );
	}
	
	return tableau;
}
int* cout_par_jour( int duree_test )
{

	int* tableau = (int*) malloc( duree_test*sizeof(int) );
	for( int i = 0; i<duree_test; i++ )
	{
		tableau[i]=0;
	}
	return tableau;

}


double* prod_par_jour( int duree_test )
{

	double* tableau = (double*) malloc( duree_test*sizeof(double) );
	for( int i = 0; i<duree_test; i++ )
	{
		tableau[i]=0;
	}
	return tableau;

}

void eolienne_tombe_en_panne( Eolienne* eolienne ) // On utilise cette fonction si le vent = 2
{
	int p;
	p = zero_cent();
	if ( 0<=p && p<proba_panne*100 )
	{
		eolienne->temps_maintenance = duree_de_maintenance;
	}
	
}

void parcours_parc_zero(ParcEolien *parc, int jour, double* tab_prod)
{

	Eolienne *courant;
	courant = parc->tete;
	while(courant != NULL)
	{
	
		if ( courant->temps_maintenance != 0)
		{
			courant->temps_maintenance --;
		}
		else
		{
			tab_prod[jour] += prod[0];
		}
	courant = courant->suiv;
	}

}

void parcours_parc_un(ParcEolien *parc, int jour, double* tab_prod)
{
	Eolienne *courant;
	courant = parc->tete;
	while(courant != NULL)
	{
	
		if ( courant->temps_maintenance != 0)
		{
			courant->temps_maintenance --;
		}
		else 
		{
			tab_prod[jour] += prod[1];
		}
	courant = courant->suiv;
	}

}

void parcours_parc_deux(ParcEolien *parc, int jour, double* tab_prod, int* tab_cout)
{
	Eolienne *courant;
	courant = parc->tete;
	while(courant != NULL)
	{
	
		if ( courant->temps_maintenance == 0) // si une éolienne n'est pas en maintenance, elle peux tomber en panne
		{
			eolienne_tombe_en_panne( courant );
			if ( courant->temps_maintenance == duree_de_maintenance  )
			{
				tab_cout[jour] += cout_maintenance;
			}
		}
		if ( courant->temps_maintenance == 0)
		{
			tab_prod[jour] += prod[2];
		}
	courant = courant->suiv;
	}

}

void simuler_un_jour(ParcEolien *parc, int jour, double* tab_prod, int* tab_cout, int* tab_vent)
{

	int vent_du_jour = tab_vent[jour];
	if ( vent_du_jour == 0 )
	{
		parcours_parc_zero(parc, jour, tab_prod);
	}
	else if (vent_du_jour == 1)
	{
		parcours_parc_un(parc, jour, tab_prod);
	}
	else if (vent_du_jour == 2)
	{
		parcours_parc_deux(parc, jour, tab_prod, tab_cout);
	}
	else
	{
		printf( "Il y a un problème avec la valeur de force du vent" );
	}
}

void simulation( ParcEolien *parc, double* tab_prod, int* tab_cout, int duree_test )
{

	int* tab_vent = tableau_vent( duree_test );
	for ( int jour = 0; jour< duree_test; jour++ )
	{
		simuler_un_jour( parc, jour, tab_prod, tab_cout, tab_vent );
	}
	printf("\n\n");
}

void maxprod( double *tab_prod, int duree_test )
{

	double Max = 0;
	int iMax = 0;
	for( int i = 0; i<duree_test; i++ )	
	{
		if( tab_prod[i]>Max )
		{
			Max = tab_prod[i];
			iMax = i;
		}
	}
	printf( "Production maximale journalière : %.2lf MW (le %dème jour)\n", Max, iMax+1 );
}


void minprod( double *tab_prod, int duree_test )
{

	double Min = 1000000;
	int iMin = 1;
	for( int i = 1; i<duree_test; i++ )	
	{
		if( tab_prod[i]<Min )
		{
			Min = tab_prod[i];
			iMin = i;
		}
	}
	printf( "Production minimale journalière (sans compter le premier jour) : %.2lf MW (le %dème jour)\n", Min, iMin+1 );
}

	
double prod_duree( double *tab_prod, int duree_test )	
{

	double Production = 0;
	for( int i = 0; i<duree_test; i++)
	{
		Production = Production + tab_prod[i];
	}
	return Production;
}


void prod_moyenne( double *tab_prod, int duree_test )
{
	double Moyenne = (prod_duree(tab_prod, duree_test))/duree_test;
	printf( "La moyenne journalière de production du parc est de %.2lf MW\n", Moyenne );
}
 
 
void cout_de_maintenance( int *tab_cout, int duree_test )  
{
	int Cout = 0;
	for( int i = 0; i<duree_test; i++ )
	{
		Cout = Cout + tab_cout[i];
	}
	printf("Le coût total dû aux maintenance est de %d €\n", Cout*1000);
}
  
void nombre_eoliennes_cassees( int *tab_cout, int duree_test )  
{
	int Cout = 0;
	int nombre;
	for( int i = 0; i<duree_test; i++ )
	{
		Cout = Cout + tab_cout[i];
		
	}
	nombre=Cout/cout_maintenance;
	printf("En tout, les éoliennes se sont cassées %d fois.\n", nombre);
}

void donnees_pour_un_jour(double *tab_prod, int *tab_cout, int jour)
{
    printf("Jour %d :\n", jour ); 
	int x = jour -1;
    printf("Production : %.2lf MW\n", tab_prod[x]);
    printf("Coût de maintenance : %d €\n", tab_cout[x] * 1000);

    printf("\n");
}

void menu()
{

	bool quitter = false;
	int choix;
	ParcEolien parc;
	parc.tete = NULL;
	int nombre_eolienne, duree_test;
	int* tab_cout;
	double* tab_prod;
	
	printf("\n\n	Projet 1 ALGO PROG BARON & BRIERE	\n");
	while(!quitter)
	{
	
		printf("\n--------------------------------------------------\n0. Quitter\n1. Lancer une simulation\n\nSi vous avez lancé une simulation : \n2. Afficher la production journalière minimale et maximale\n3. Afficher la production totale sur la durée de la simulation\n4. Afficher la production journalière moyenne sur la durée de la simulation \n5. Afficher le cout total de maintenance sur la durée de la simulation \n6. Afficher le nombre de fois qu'une éolienne s'est cassée durant la simulation\n7. afficher les données pour un jour de votre choix\n-> Choix : ");
		scanf("%d", &choix);
		printf("\n");

		if(choix == 0) 
		{
		    desallouer_la_memoire(&parc);
		    quitter = true;
		}
		else if(choix == 1)
		{
		
			printf("Combien d'éoliennes voulez vous dans le parc éolien?\n");
			scanf("%d", &nombre_eolienne);
			printf("Sur combien de jours voulez vous faire la simulation?\n");
			scanf("%d",&duree_test);
			creation_liste_eolienne(&parc,nombre_eolienne);
			tab_cout = cout_par_jour(duree_test);
			tab_prod = prod_par_jour(duree_test);
			simulation(&parc, tab_prod, tab_cout, duree_test);
			printf("La simulation a été effectuée! Vous pouvez désormais observer les statistiques pour %d éoliennes dans le parc et une durée de test de %d jours.\n Si vous voulez changer une de ces valeurs, veuillez quitter le programme et le rééxecuter!\n", nombre_eolienne, duree_test);
		
		}
		else if( choix ==2)
		{
			minprod(tab_prod, duree_test);
			maxprod(tab_prod, duree_test);
		}
		else if(choix == 3)
		{
			printf( "La production totale du parc éolien durant la durée d'étude est de %.2lf MW\n", prod_duree( tab_prod, duree_test ) );
		}
		else if(choix == 4)
		{
			prod_moyenne( tab_prod, duree_test );
		}
		else if(choix == 5)
		{
			cout_de_maintenance(tab_cout, duree_test );
		}
		else if(choix == 6)
		{
			nombre_eoliennes_cassees( tab_cout, duree_test );
		}
		else if(choix==7)
		{
			int jour;
			printf("Quel jour vous intéresse?\n");
			scanf("%d", &jour);
			donnees_pour_un_jour(tab_prod, tab_cout, jour);
		}
	}

}
  
  
int main()
{	

	srand( (unsigned int)time(NULL) );
	menu();
	return 0;

}
