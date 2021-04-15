#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#define TAILLE_MESSAGE 256

typedef struct Gazon{
  char nom[TAILLE_MESSAGE];
  char prix[TAILLE_MESSAGE];
  char stock[TAILLE_MESSAGE]; // ça peut etre plus malin de mettre un int
} Gazon;

typedef struct Ticket{
    char prix_tot[TAILLE_MESSAGE];
    char nbPalette[TAILLE_MESSAGE];
} Ticket;

typedef struct Sac{
    Gazon gazon;
    Ticket ticket;
} Sac;

char *serveur(void){
    srand(time(NULL));
    int nbAlea;
    nbAlea = rand() % 2;
    switch(nbAlea){
      case 0:{
        return "sw1";
      }
      case 1:{
        return "sw2";
      }
      default:{
      }
    }
}

char *transporteur(void){
    srand(time(NULL));
    int nbAlea;
    nbAlea = rand() % 2;
    switch(nbAlea){
      case 0:{
        return "Xavier";
      }
      case 1:{
        return "France";
      }
      default:{
      }
    }
}

char *acheteur(void){
    srand(time(NULL));
    int nbAlea;
    nbAlea = rand() % 2;
    switch(nbAlea){
      case 0:{
        return "Laure";
      }
      case 1:{
        return "Philippe";
      }
      default:{
      }
    }
}

Gazon *tirageGazon(void){
  srand(time(NULL));
  int nbAlea;
  nbAlea = rand() % 2;
  Gazon gazon;
  switch(nbAlea){
    case 0:{
      char nom[TAILLE_MESSAGE] = "Classique";
      strcpy(gazon.nom, nom);
      char prix[TAILLE_MESSAGE] = "10";
      strcpy(gazon.prix, prix);
	  char stock[TAILLE_MESSAGE] = "10000"; // voir si on ne met pas plutot un int plutot que char
	  strcpy(gazon.stock, stock);
    }
    break;
    case 1:{
      char nom[TAILLE_MESSAGE] = "Rustique";
      strcpy(gazon.nom, nom);
      char prix[TAILLE_MESSAGE] = "20";
      strcpy(gazon.prix, prix);
	  char stock[TAILLE_MESSAGE] = "5000";
	  strcpy(gazon.stock, stock);
    }
    break;
    default:{
    }
  }
  Gazon *pointeurGazon = &gazon;
  return pointeurGazon;
}

int main(){

  pid_t pid_fils;
  int tube[2];
  char msgRecu[TAILLE_MESSAGE];
  Gazon *pointeurGazon  = malloc(sizeof(Gazon));
  Gazon *pointeurGazon2 = malloc(sizeof(Gazon));
  Sac *pointeurSac = malloc(sizeof(Sac));
  Sac *pointeurSac2 = malloc(sizeof(Sac));

  char ach[TAILLE_MESSAGE];
  strcpy(ach, acheteur());
  char ser[TAILLE_MESSAGE];
  strcpy(ser, serveur());
  char tra[TAILLE_MESSAGE];
  strcpy(tra, transporteur());

  printf("Bienvenue chez Gazon&Cie l'entreprise qui vend du gazon de haute qualité depuis 1897 !\n\n");
  printf("Dans cette simulation : \n L'acheteur s'appelle : %s, le serveur s'appelle : %s, accompagné de : %s le transporteur\n\n", ach, ser, tra);
  
  if(pipe(tube) != 0){
    exit(1);
  }

  pid_t pere_pid = getpid();
  switch(pid_fils = fork()){
    case -1:
    {
      exit(1);
    }
    case 0:{
      //acheteur

      //Etape2 (1s)
      sleep(1);
      read(tube[0], msgRecu, TAILLE_MESSAGE);
      printf("(Acheteur : %s) %s\n",ach, msgRecu);

      //Etape 3 (1s)
      char msgAEnvoyer2[TAILLE_MESSAGE] = " Acheteur m'a dit : Bonjour Monsieur !";
      write(tube[1], msgAEnvoyer2, TAILLE_MESSAGE);

      //Etape5 (3s)
      sleep(2);
      read(tube[0], msgRecu, TAILLE_MESSAGE);
      printf("(Acheteur : %s) %s\n",ach, msgRecu);

      //Etape6 (3s)
      pointeurGazon = tirageGazon();
      write(tube[1], pointeurGazon->nom , TAILLE_MESSAGE);

      //Etape9 (6s)
      sleep(2);
      read(tube[0], msgRecu, TAILLE_MESSAGE);
      printf("(Acheteur : %s) %s\n", ach, msgRecu);

      //Etape10 (6s)
      printf("L'acheteur prend le/la %s\n", pointeurGazon->nom);

      //Etape11 (6s)
      kill(pere_pid, SIGKILL);

    }
    break;
    default:{
      //serveur

      //Etape1 (0s)
      char msgAEnvoyer[TAILLE_MESSAGE] = " Serveur m'a dit : Bonjour Madame !";
      write(tube[1], msgAEnvoyer, TAILLE_MESSAGE);

      //Etape4 (2s)
      sleep(2);
      read(tube[0], msgRecu, TAILLE_MESSAGE);
      printf("(Serveur : %s) %s\n", ser, msgRecu);

      //Etape4bis (2s)
      char msgAEnvoyer3[TAILLE_MESSAGE] = " Serveur m'a dit : Qu'est-ce qui vous ferait plaisir ?";
      write(tube[1], msgAEnvoyer3, TAILLE_MESSAGE);

      //Etape 7 (4s)
      sleep(2);
      read(tube[0], msgRecu, TAILLE_MESSAGE);
      char ajout[TAILLE_MESSAGE] = " Acheteur m'a dit : Je voudrais un ";
      strcat(ajout, msgRecu);
      printf("(Serveur : %s) %s\n", ser, ajout);
      
      //Etape 8 (4s)
      char msgAEnvoyer4[TAILLE_MESSAGE] = " Serveur m'a dit : Tenez voici le/la ";
      strcat(msgAEnvoyer4, msgRecu);
      char ajout2[TAILLE_MESSAGE] = "\n";
      strcat(msgAEnvoyer4, ajout2);
      write(tube[1], msgAEnvoyer4, TAILLE_MESSAGE);

      //sleep(4);
    }
  }
  //printf("%d\n",getpid());
  //le pas de temps "est reset"
  pid_t nouveau_pere_pid = getpid();
  if(nouveau_pere_pid == pere_pid){
    while(1);
  }

  switch(pid_fils = fork()){
    case -1:
    {
      exit(1);
    }
    case 0:{
        //transporteur

        //Etape 2 (1s)
        sleep(1);
        read(tube[0], pointeurGazon2, sizeof(Gazon));
        printf("\n(Serveur : %s) L'acheteur m'a donné son %s\n", tra,pointeurGazon2->nom);

        //Etape 2bis (1s)
        printf("(Acheteur : %s) Serveur m'a dit : Je scan le produit. Veuillez patientier.\n", ach);

        //Etape 3 (1s)
        char msgAEnvoyer5[TAILLE_MESSAGE] = " Serveur m'a dit : Le prix a payer est ";
        strcat(msgAEnvoyer5, pointeurGazon2->prix);
        char ajout[TAILLE_MESSAGE] = " euros \n";
        strcat(msgAEnvoyer5, ajout);
        write(tube[1], msgAEnvoyer5, TAILLE_MESSAGE);

        //Etape 6 (3s)

        sleep(2);
        read(tube[0], msgRecu, TAILLE_MESSAGE);
        printf("(Serveur) %s\n", msgRecu);

        //Etape 7 (3s)

        //--Creation du ticket
        Ticket * pointeurTicket = malloc(sizeof(Ticket));
        strcpy(pointeurTicket->prix_tot, pointeurGazon2->prix);

		//--On doit faire le nombre de palettes
        char buffer [TAILLE_MESSAGE];
        sprintf(buffer, "%d\n", (surface%63)); //-------ici
        strcpy(pointeurTicket->nbPalette, buffer);

        //--Creation du Sac
        Sac sac;
        sac.gazon = *pointeurGazon2;
        sac.ticket = *pointeurTicket;

        pointeurSac = &sac;

        write(tube[1], pointeurSac, sizeof(Sac));

        //Etape 10 (5s)

        sleep(2);
        read(tube[0], msgRecu, TAILLE_MESSAGE);
        printf("(Caissiere : %s) %s\n",tra, msgRecu);

        //Etape 11 (5s)

        char msgAEnvoyer8[TAILLE_MESSAGE] = " Serveur m'a dit : Merci et à bientot !!";
        write(tube[1], msgAEnvoyer8, TAILLE_MESSAGE);

    }
    break;
    default : {
        //Acheteur

        //Etape 1 (0s)
        char texte1[TAILLE_MESSAGE] = "(Serveur : " ;
        strcat(texte1, tra);
        char texte2[TAILLE_MESSAGE] = ") Acheteur m'a dit : Voici mon produit le/la ";
        strcat(texte1,texte2);
        strcat(texte1, pointeurGazon->nom);
        char ajout[TAILLE_MESSAGE] = "\n ";
        strcat(texte1, ajout);
        printf("%s", texte1);
        write(tube[1], pointeurGazon, sizeof(Gazon));

        //Etape 4 (2s)
        sleep(2);
        read(tube[0], msgRecu, TAILLE_MESSAGE);
        printf("(Acheteur : %s) %s",ach, msgRecu);

        //Etape 5 (2s)
        char msgAEnvoyer6[TAILLE_MESSAGE] = " Acheteur m'a dit : Voici mon argent";
        write(tube[1], msgAEnvoyer6, TAILLE_MESSAGE);

        //Etape 8 (4s)
        sleep(2);
        read(tube[0], pointeurSac2, sizeof(Sac));

        printf("(Serveur) Acheteur m'a dit : j'ai recu mon sac avec mon ticket d'un montant de %s euros, un total de %s palettes du gazon %s\n", (pointeurSac2->ticket).prix_tot, (pointeurSac2->ticket).nbPalette, (pointeurSac2->gazon).nom);

        //Etape 9 (4s)
        char msgAEnvoyer7[TAILLE_MESSAGE] = " Acheteur m'a dit : Au revoir !";
        write(tube[1], msgAEnvoyer7, TAILLE_MESSAGE);

        //Etape 12 (6s)
        sleep(2);
        read(tube[0], msgRecu, TAILLE_MESSAGE);
        printf("(Acheteur : %s) %s\n", ach, msgRecu);
         printf("\nLA SIMULATION EST TERMINEE");
    }
  }
  //kill(getpid(),SIGINT);
  exit(0);
}
}
