/********************************************************************************************
* nom projet : mancala                                                                      *
* auteur : DJAMPOU Pedro Le Prince                                                          *
* Date_creation : 25 mars 2022                                                                              *
* description: il s'agit d'un jeu d'origine algérien. le gagnant est celui qui a le plus    *
* grand score a la fin et le score est determiner par le nombre de graine de son grenier    *
* ajouter aux graines de l'advesaire                                                        *
*******************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#define nbr_c 7//nombre de case plus le grenier
#define len_w_x 900//width du x=windows
#define len_w_y 500 //height du windows
#define len_x_c 100 //largeur des trous
#define len_y_c 200//longueur des trous

/*
creation de la struture partie qui caractérise intégralement une partie */
struct partie
{
    int plateau[2][nbr_c];   //matrice du jeu. plateau[0][nbr_c-1] represente le grenier du joueur 1 et plateau[1][nbr_c-1] represente le grenier du joueur 2
    int score[2];           //scores des joueurs
    int multiplayer;        // 0 si joueur seul et 1 si non
    int player;             //represente le joueur qui joue
    int saved;              //0 si c'est une nouvelle partie et 1 si c'est une partie enregistrée
    int num_partie;         //numero de la partie 0 pour une nouvelle
    SDL_Rect graine[2][nbr_c-1][4];    //contient toutes les graines
    SDL_Rect trou[2][nbr_c];            //contient les trous de chaque joueur trou[0][0] est le premier trou du joueur 1
    SDL_Rect exit;                      // represente le button de sorti de la partie
    SDL_Texture *motif[12];             // contient toutes les textures necessaire pour jouer
};
typedef struct partie partie;

/*creation de la structure pion qui caracterise une graine*/
struct pion
{
    int i_p;        //premiere coordonnée du pion dans la matrice graine de la structure partie
    int j_p;        //deuxieme coordonnée du pion dans la matrice graine de la structure partie
    int k;          //troixième coordonnée du pion dans la matrice graine de la structure partie
};
typedef struct pion pion;


/************************************************************
* fonction initialise prend en paramètre :                  *
* -> un pointeur sur une variable de type partie qui        *
*  represente le jeu                                        *
* -> une surface necessaire pour charger les images         *
* -> un rendu                                               *
* la fontion sert juste a initialiser les paramètres        *
* du jeu                                                    *
*************************************************************/
void initialise(partie *jeu, SDL_Surface * surface, SDL_Renderer *renderer)
{
    char filename[8];
    int i, j, k, xd, yd; //i, j, k sont juste des compteurs et x_d et y_d des paramètres utiles pour initialiser les rectangles
    //si la partie n'est pas une partie enregistrer
    if(jeu->saved!=1)
    {
        //numeros de partie par defaut est 0
        jeu->num_partie = 0;
        //initialisation des scores a 0
        jeu->score[0] = 0;
        jeu->score[1] = 0;
        //le joueur 1 est toujours le premier a jouer
        jeu->player = 1;
        //initialisaton des champs plateau, trou et graine de la struture partie
        for(i=0; i<2; i++)
            for(j=0; j<nbr_c-1; j++)
            {
                jeu->plateau[i][j] = 4;     //chaque joueur a initialement 4 graines dans chaque trou
                //init des x des trous fontion de i
                if(i==0)
                    jeu->trou[i][j].x = 100+len_x_c*j;
                if(i==1)
                    jeu->trou[i][j].x = len_x_c*(nbr_c-1)-len_x_c*j;
                jeu->trou[i][j].h = len_y_c;
                jeu->trou[i][j].w = len_x_c;
                jeu->trou[i][j].y = 50+len_y_c*(1-i);
                //init de la position des graines de chaque trou
                for(k=0; k<4; k++)
                {
                    xd=0;       //graine superieur droite
                    yd=0;
                    if(k==1)    //graine superieur gauche
                        xd=1;
                    if(k==2)    //graine rinférieur gauche
                    {
                        yd=1;
                        xd=1;
                    }
                    if(k==3)     //graine inferieur droite
                        yd=1;
                    if(i==0)
                        jeu->graine[i][j][k].x = 100+len_x_c*j + 25+25*xd;
                    if(i==1)
                        jeu->graine[i][j][k].x = len_x_c*(nbr_c-1)-len_x_c*j+ 25+25*xd;
                    jeu->graine[i][j][k].y = 50+len_y_c*(1-i)+ 50+80*yd;
                    jeu->graine[i][j][k].h = 20;
                    jeu->graine[i][j][k].w = 20;

                }
            }
        //les deux grenier sont initialement libres
        jeu->plateau[1][nbr_c-1] = 0;
        jeu->plateau[0][nbr_c-1] = 0;

        //init des trous des greniers des deux joueurs
        jeu->trou[1][nbr_c-1].x = 0;
        jeu->trou[1][nbr_c-1].y = 50;
        jeu->trou[1][nbr_c-1].w = 100;
        jeu->trou[1][nbr_c-1].h = 400;
        jeu->trou[0][nbr_c-1].x = 100*(nbr_c);
        jeu->trou[0][nbr_c-1].y = 50;
        jeu->trou[0][nbr_c-1].w= 100;
        jeu->trou[0][nbr_c-1].h = 400;

        //init du rectangle contenant le button exit
        jeu->exit.h = 50;
        jeu->exit.w = 100;
        jeu->exit.x = 800;
        jeu->exit.y = len_w_y-100;
    }
    //remplissage des textures pour le jeu
    for(i=0; i<12; i++)
    {
        itoa(i+1, filename, 10);        //convertir i+1 en chaine de caractere et la met dans filename
        strcat(filename, ".png");       // on rajoute l'extention
        surface = IMG_Load(filename);
        jeu->motif[i] = SDL_CreateTextureFromSurface(renderer, surface);

    }
}

/*
 affiche le jeu en console
*/
void affiche_console(int tab[2][nbr_c])
{
    int i;
    printf("\n");
    printf("\t");
    for(i=2; i<=nbr_c; i++)
        printf(" %d ", tab[1][nbr_c-i]);
    printf("\n%d", tab[1][nbr_c-1]);
    printf("\t%24d", tab[0][nbr_c-1]);
    printf("\n\t");
    for(i=0; i<nbr_c-1; i++)
        printf(" %d ", tab[0][i]);
}

/*********************************************
* cherche et renvoit la premiere             *
* graine trouvée dans le trou u du joueur    *
* player dans la partie jeu                  *
**********************************************/
pion search_pion(partie jeu, int player, int u)
{
    int i, j, k;
    SDL_Point point; //creation d'un point
    pion p;          // creation d'un pion p
    //cherche une graine dans u
    for(i=0; i<2; i++)
    {
        for(j=0; j<nbr_c-1; j++)
        {
            for(k=0; k<4; k++)
            {
                point.x = jeu.graine[i][j][k].x;
                point.y = jeu.graine[i][j][k].y;
                if(SDL_PointInRect(&point, &jeu.trou[player-1][u])) //la graine est dans le trou u
                {
                    p.i_p = i;
                    p.j_p = j;
                    p.k = k;
                    return p;       //on renvoit la graine trouver
                }
            }
        }
    }

}


/*************************************************************************
*refrech rafraichit l'ecran a chaque appel                               *
*************************************************************************/
void refrech(partie *jeu, SDL_Renderer *renderer, SDL_Texture *chiffre[10])
{
    int i, j, k;            //compteurs
    int ch[2];              //chiffres pour afficher le nombre de graine dans chaque trou
    SDL_Rect rect, trou, rect_ch;       //rect sert juste a affiché les motifs, trou les trous et rect_ch les chiffres
    SDL_RenderClear(renderer);          //on nettoie d'abord l'ecran


    //les rectangles des chiffres ont des dimensions fixes 20*20
    rect_ch.h = 20;
    rect_ch.w = 20;

    //affichage des chiffres, et des trous
    for(i=0; i<2; i++)
    {
        for(j=0; j<nbr_c; j++)
        {
            trou = jeu->trou[i][j];
            if(j!=nbr_c-1)
                SDL_RenderCopy(renderer, jeu->motif[4], NULL, &jeu->trou[i][j]); //affichage des trous simples
            else
                SDL_RenderCopy(renderer, jeu->motif[5], NULL, &jeu->trou[i][j]); // affichage des trous du grenier
            decompose_ch(jeu->plateau[i][j], &ch[0], &ch[1]);       //on decompose le nombre de graine dans le trou en ses chiffres
            // affichage des chiffres
            for(k=0; k<2; k++)
            {
                rect_ch.x = trou.x + 30 + 20*k;
                if(i==0 && j!=nbr_c-1)
                    rect_ch.y = trou.y + len_y_c -20;
                else if(i==0 && j==nbr_c-1)
                    rect_ch.y = trou.y + 2*len_y_c -20;
                else
                    rect_ch.y = trou.y;
                SDL_RenderCopy(renderer, chiffre[ch[k]], NULL, &rect_ch);
            }
        }
    }
    //affichage des graines dans les trous
    for(i=0; i<2; i++)
    {
        for(j=0; j<nbr_c-1; j++)
        {
            for(k=0; k<4; k++)
            {
                SDL_RenderCopy(renderer, jeu->motif[k], NULL, &jeu->graine[i][j][k]);
            }
        }
    }
    //affichage des infos sur le joueur en cours
    rect.h = 50;
    rect.w = len_w_x;
    rect.x = 0;
    rect.y = 0;
    SDL_RenderCopy(renderer, jeu->motif[8], NULL, &rect); //affichage de la banniere joueur 2
    rect.y = len_w_y-50;
    SDL_RenderCopy(renderer, jeu->motif[7], NULL, &rect); //affichage de la banniere joueur 1
    //si c'est le joueur 1
    if(jeu->player==1)
    {
        rect.x = 100;
        rect.w = 250;
        SDL_RenderCopy(renderer, jeu->motif[9], NULL, &rect); //affiche la flèche qui pointe vers le joueur 1
    }
    //si c'est le joueur 2
    if(jeu->player==2)
    {
        rect.y = 0;
        rect.x = 550;
        rect.w = 250;
        SDL_RenderCopy(renderer, jeu->motif[10], NULL, &rect);//affiche de la flèche qui pointe vers le joueur 2
    }

    SDL_RenderCopy(renderer, jeu->motif[11], NULL, &jeu->exit); // affiche button de sorti


    SDL_RenderPresent(renderer); //actualise le rendu
}

/*********************************************************
* deplace une graine p vers le trou c du joueur receiver *
*********************************************************/
void deplace_pion(int receiver, int c, pion p, partie *jeu, SDL_Renderer *renderer, SDL_Texture *chiffre[10])
{
    int x_dest, y_dest, x_ok=0, y_ok=0;     //x_dest et y_dest sont les coordonnées de destination et x_ok et y_ok indique si l'abcisse ou l'ordonne de destination est atteint
    //srand(time(0));
    SDL_Rect rect;              // sert a afficher des motif

    //on génère aléatoirement les coordonnées de destination
    rect = jeu->trou[receiver-1][c];
    x_dest = rect.x + 25+(rand()%20)*25/20;
    y_dest = rect.y + 50+(rand()%20)*80/20;

    //si c'est un grenier
    if(c==nbr_c-1)
        y_dest = rect.y + 50+(rand()%40)*270/40;

    rect = jeu->graine[p.i_p][p.j_p][p.k];
    //deplacement de la graine p vers les coordonneés de destination avec une temporisation de 10ms pour donner l'impression d'un mvt continu
    while(!(rect.x+10>x_dest && rect.x-10<x_dest && rect.y+10>y_dest && rect.y-10<y_dest))
    {
        if(rect.x>x_dest && x_ok==0)
            rect.x-=10;
        else if(rect.x<x_dest && x_ok==0)
            rect.x+=10;
        if(rect.y>y_dest && y_ok==0)
            rect.y-=10;
        else if(rect.y<y_dest && y_ok==0)
            rect.y+=10;
        if(rect.y+10>y_dest && rect.y-10<y_dest) //la graine est arrive a l'ordonnée
            y_ok=1;
        if(rect.x+10>x_dest && rect.x-10<x_dest) //la graine est arrive a l'ordonnée
            x_ok=1;
        // on modifie le rectangle contenant la graine
        jeu->graine[p.i_p][p.j_p][p.k] = rect;
        refrech(jeu, renderer, chiffre);
        SDL_Delay(10);
    }
}
/********************************************************************
* joue le trou i du joueur actuel dans jeu                          *
********************************************************************/
void jouer(int i, partie *jeu, SDL_Renderer *renderer, SDL_Texture* chiffre[10])
{
    int nbr_gr, j, receiver, u, advers, v; // nbr_gr = nombre de graine dans le trou, j v  sont des compteurs et u pour designer le trou qui recoit la graine
    pion p;                                 // creation d'une graine p
    nbr_gr = jeu->plateau[jeu->player-1][i];
    receiver = jeu->player;

    if(nbr_gr!=0 && i!=nbr_c-1) //si le nombre de graine n'est pas nul et qu'il ne sagit pas d'un grenier
    {
        u = i+1;
        jeu->plateau[jeu->player-1][i] = 0;//on vide la case jouer

        for(j=nbr_gr; j>0; j--)
        {
            //on cherche une graine dans le trou
            p = search_pion(*jeu, jeu->player, i);
            //on met un pion dans les cases suivantes en ordre
            if(u<nbr_c-1)
            {
                jeu->plateau[receiver-1][u]++;
                deplace_pion(receiver, u, p, jeu, renderer, chiffre);
                u++;
            }
            //on verifie si on a deja fini de remplir les cases d'un joueur
            else if(u>=nbr_c)
            {
                u=0;
                j++;
                if(receiver==1)
                    receiver = 2;
                else
                    receiver = 1;
            }
            //on verifie si c'est le jouer pour remplir son grenier
            else if(u==nbr_c-1 && receiver==jeu->player)
            {
                jeu->plateau[receiver-1][u]++;
                deplace_pion(receiver, u, p, jeu, renderer, chiffre);
                u++;
            }
            //ne pas remplir celui de lautre
            else if(u==nbr_c-1 && receiver!=jeu->player)
            {
                j++;
                u++;
            }

        }
        if(receiver==jeu->player && jeu->plateau[receiver-1][u-1]==1 && u!=nbr_c) //on bouf la case de l'adversaire du meme indice si on fini dans une case vide autre que le grenier
        {
            if(receiver==1)
                advers = 2;
            else
                advers = 1;
            jeu->plateau[receiver-1][nbr_c-1] += jeu->plateau[advers-1][nbr_c-u-1];
            for(v=0; v< jeu->plateau[advers-1][nbr_c-u-1]; v++)
            {
                p = search_pion(*jeu, advers, nbr_c-u-1);
                deplace_pion(receiver, nbr_c-1, p, jeu, renderer, chiffre);
            }
            jeu->plateau[advers-1][nbr_c-u-1]=0;
        }
        if(jeu->player==1 && u!=nbr_c)//on change de joueur si il n'a pas fini dans un grenier
            jeu->player = 2;
        else if(jeu->player==2 && u!=nbr_c)
            jeu->player = 1;
        refrech(jeu, renderer, chiffre);
    }
}

/************************************************
* renvoit 1 si la partie est fini et 0 sinon    *
* elle actualise aussi le score                 *
*************************************************/
int verif_etat_partie(partie *jeu)
{
    int i, j, end[2], c; //i, j, c compteurs
    int advers;
    end[0] = 1; //testeur pour voir si le joueur 1 n'as plus de graine
    end[1] = 1; //testeur pour voir si le joueur 2 n'as plus de graine
    // les scores sont d'qbord les graines contenues dans le grenier
    jeu->score[0] = jeu->plateau[0][nbr_c-1];
    jeu->score[1] = jeu->plateau[1][nbr_c-1];
    if(jeu->player==1)
        advers = 2;
    else
        advers = 1;
    // chaque joueur recupère les graines de l'advesaire
    for(i=0; i<2; i++)
    {
        for(j=0; j<nbr_c-1; j++)
        {
            if(i==jeu->player-1)
                jeu->score[i] += jeu->plateau[advers-1][j];
            else
                jeu->score[i] += jeu->plateau[jeu->player-1][j];
            if(jeu->plateau[i][j]!=0) //il a encore des graines
                end[i] = 0;
        }
    }
    //on verifi si un joueur n'a plus de graines
    if(end[0]==1 || end[1]==1)
        return 1;
    else
        return 0;
}

/*****************************************
* renvoit l'indice du trou selectionner  *
* et renvoit -1 si le clic n'est pas dans*
* un des trous du joueur                 *
******************************************/
int search_indice(SDL_Event event, partie jeu)
{
    int j;      //compteur
    SDL_Point point;
    point.x = event.button.x;
    point.y = event.button.y;
    for(j=0; j<nbr_c-1; j++)
    {
        if(SDL_PointInRect(&point, &jeu.trou[jeu.player-1][j]) && jeu.plateau[jeu.player-1][j]!=0)//si le clic a lieu dans un des trou du joueur
            return j;
    }
    return -1;
}

/********************************************
* simul un coup a la demande de la machine  *
* le coup simuler est celui du trou i du    *
* joueur actuel dans jeu                    *
*********************************************/
void simul_coup(int i, partie *jeu)
{
    int nbr_gr, j, receiver, u, advers, v; // nbr_gr = nombre de graine dans le trou, j v  sont des compteurs et u pour designer le trou qui recoit la graine
    nbr_gr = jeu->plateau[jeu->player-1][i];
    receiver = jeu->player;
    if(nbr_gr!=0 && i!=nbr_c-1)
    {
        u = i+1;
        jeu->plateau[jeu->player-1][i] = 0;//on vide la case jouer

        for(j=nbr_gr; j>0; j--)
        {
            //on met un pion dans les cases suivantes en ordre
            if(u<nbr_c-1)
            {
                jeu->plateau[receiver-1][u]++;
                u++;
            }
            //on verifie si on a deja fini de remplir les cases d'un joueur
            else if(u>=nbr_c)
            {
                u=0;
                j++;
                if(receiver==1)
                    receiver = 2;
                else
                    receiver = 1;
            }
            //on verifie si c'est le jouer pour remplir son grenier
            else if(u==nbr_c-1 && receiver==jeu->player)
            {
                jeu->plateau[receiver-1][u]++;
                u++;
            }
            //ne pas remplir celui de lautre
            else if(u==nbr_c-1 && receiver!=jeu->player)
            {
                j++;
                u++;
            }

        }
        if(receiver==jeu->player && jeu->plateau[receiver-1][u-1]==1 && u!=nbr_c) //on bouf la case de l'adversaire du meme indice si on fini dans une case vide autre que le grenier
        {
            if(receiver==1)
                advers = 2;
            else
                advers = 1;
            jeu->plateau[receiver-1][nbr_c-1] += jeu->plateau[advers-1][nbr_c-u-1];
            for(v=0; v< jeu->plateau[advers-1][nbr_c-u-1]; v++)
            {
            }
            jeu->plateau[advers-1][nbr_c-u-1]=0;
        }
        if(jeu->player==1 && u!=nbr_c)//on change de joueur si il n'a pas fini dans un grenier
            jeu->player = 2;
        else if(jeu->player==2 && u!=nbr_c)
            jeu->player = 1;
    }
}

/********************************************************
* renvoit cote du coup jouer pour quitter de prev a sec *
*********************************************************/
int get_cot(int poids, partie prev, partie sec, int firstly)
{
    int i, j, cot=0, id=1; // i, j des compteurs, cot le cote a renvoyer, id passe a zero si prev et sec ne sont pas identiques
    for(i=0; i<2; i++)
    {
        for(j=0; j<nbr_c-1; j++)
        {
            if(i==1)
                cot -= poids*(sec.plateau[i][j]-prev.plateau[i][j]); //la machine recoit une graine conte pour -poids

            if(sec.plateau[i][j]!=prev.plateau[i][j])//on a une difference
                id=0;
        }
    }
    cot += 4*poids*(sec.plateau[1][nbr_c-1]- prev.plateau[1][nbr_c-1]); //la machine recoit une graine dans son grenier conte pour +4*poids
    cot -= 5*poids*(sec.plateau[0][nbr_c-1]- prev.plateau[0][nbr_c-1]);  //le joueur recoit une graine dans son grenier conte pour -4*poids
    if(id==1 && firstly==1)//trou non jouable au premier tour
        return -100000;
    return cot;
}

/********************************************************
* renvoit la cote de la possibillité jouer en u  de la  *
* de la machine                                         *
*********************************************************/
int get_cot_event(partie jeu, int u, int poid, int firstly)
{
    partie jeu_s[2]; //pour les simulation
    jeu_s[0] = jeu;
    int cot = 0;
    int i, j, k; //compteurs
    simul_coup(u, &jeu_s[0]); //simulation
    cot += get_cot(poid, jeu, jeu_s[0], 1);
    //on simul tout les prochains coups du joueur
    for(i=0; i<nbr_c-1; i++)
    {
        jeu_s[1] = jeu_s[0];//copie l'ancienne configuration avant la simulation
        simul_coup(i, &jeu_s[1]);//simulation
        cot += get_cot(poid-1, jeu_s[0], jeu_s[1], 0);
        if(jeu_s[0].player==1 && jeu_s[1].player==1 && get_cot(poid, jeu, jeu_s[0], 1)!=-100000 && get_cot(poid-1, jeu_s[0], jeu_s[1], 0)!=0)//si l'adversaire va faire un turn free
            cot-=5000;//on évite de laisser le joueur jouer deux fois de suite
    }
    return cot;
}
/******************************************
* renvoit l'indice a jouer pour la machine*
*******************************************/
int jeu_machine(partie jeu)
{
    int i, u, cot[nbr_c-1]; // u est un compteur, i l'indice a jouer et cot contient toutes les cotes des possibilitées
    // on charge les cotes des differentes possibilitées
    for(u=0; u<nbr_c-1; u++)
    {
        cot[u] = get_cot_event(jeu, u, 2, 1);
    }
    i=0;
    // recherche de la meillieur cote
    for(u=0; u<nbr_c-1; u++)
    {
        if(cot[i]<cot[u])
            i=u;
    }
    return i;
}

/************************************************
* renvoit le nombre d'element enregistrer dans  *
* le fichier de sauvegarde                      *
*************************************************/
int verif_saving()
{
    FILE *t;
    int i; //compteur
    int ctrl ;//variable de controle
    int c=0;
    partie testeur;  //pour la lecture dans le fichier
    t = fopen("parties.part", "r");
    for(i=0; i<4; i++)
    {
        ctrl = fread(&testeur, sizeof(testeur), 1, t);
        if(ctrl!=0)//on incremente c tant qu'il a lu un element
            c++;
    }
    fclose(t);
    return c;
}

/********************************************************************
* gestion du menu continuer vers une partie enregistrer             *
*********************************************************************/
void menu_partie_save(partie *jeu, SDL_bool *quit, SDL_Renderer *renderer)
{
    FILE *t;
    SDL_Texture *motif_men_continuer[3]; //motif du menu
    SDL_Texture *texture;   //texture intermediaire
    SDL_Rect rect[4];    //contient les buttons du menu
    SDL_Surface *surface;
    SDL_Point point;
    SDL_Event event;
    SDL_bool quit_=SDL_FALSE;
    partie testeur;             //pour la lecture dans le fichier
    char filename[8];   //vas contenir successivement les noms des fichiers
    int i, nbr_part, cpt, read;              //i, cpt compteur, nbr_part= nombre de partie enregistrer

    nbr_part = verif_saving();
    //printf("%d\n", nbr_part);
    SDL_RenderClear(renderer);
    //remplissage des textures
    for(i=0; i<3; i++)
    {
        itoa(i+1, filename, 10);
        strcat(filename, "ms.png");
        surface = IMG_Load(filename);
        motif_men_continuer[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }

    //affichage du fond de menu
    surface = IMG_Load("fond_continue.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //chargement de rect[0] pour le button return
    rect[0].x = len_w_x/2-100;
    rect[0].y = len_w_y-100;
    rect[0].h = 100;
    rect[0].w = 200;

    //affichage du button return
    surface = IMG_Load("return.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect[0]);

    if(nbr_part==0)//si il y'a pas de partie enregistrer
    {
        //on affiche aucun a l'ecran
        surface = IMG_Load("aucun.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect[1].x = 100;
        rect[1].y = 100;
        rect[1].h = 200;
        rect[1].w = len_w_x-200;
        SDL_RenderCopy(renderer, texture, NULL, &rect[1]);
    }
    if(nbr_part!=0)//si il y'a des parties enregistrer
    {
        for(i=0; i<nbr_part; i++)
        {
            //on affiche toutes les parties
            rect[i+1].x = 200;
            rect[i+1].y = 120+100*i;
            rect[i+1].h = 50;
            rect[i+1].w = len_w_x-400;
            SDL_RenderCopy(renderer, motif_men_continuer[i], NULL, &rect[i+1]);
        }
    }
    SDL_RenderPresent(renderer);

    //boucle du menu
    do
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)//quitter le programme
        {
            quit_ = SDL_TRUE;
            *quit = SDL_TRUE;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN)//clic
        {
            point.x = event.button.x;
            point.y = event.button.y;
            if(nbr_part==0)//pas de parties enregistrer
            {
                //on verifi juste si il a cliquer sur le return
                if(SDL_PointInRect(&point, &rect[0]))
                {
                    quit_ = SDL_TRUE;
                    menuprinci(jeu, quit, renderer);
                }
            }
            if(nbr_part!=0)//y'a des parties
            {
                cpt=0;
                //recherche de sa selection
                for(i=0; i<nbr_part; i++)
                {
                    if(SDL_PointInRect(&point, &rect[i+1]))//selection trouver
                    {
                        t = fopen("parties.part","r");
                        //recherche de la partie dans le fichier
                        do
                        {
                            read = fread(&testeur, sizeof(testeur), 1, t);
                            if(read!=0)
                                cpt++;
                        }
                        while(cpt!=i+1);
                        fclose(t);
                        //copie de la partie dans jeu pour pouvoir la continuer
                        *jeu = testeur;
                        quit_ = SDL_TRUE;
                    }
                }
                if(SDL_PointInRect(&point, &rect[0]))//button return
                {
                    quit_ = SDL_TRUE;
                    menuprinci(jeu, quit, renderer);
                }
            }
        }
    }
    while(!quit_);


}

/**********************************************************
* gestion du menu principal                               *
***********************************************************/
void menuprinci(partie *jeu, SDL_bool *quit, SDL_Renderer *renderer)
{
    int x, y; //pour les coordonnées du clic
    SDL_Event event;
    SDL_Surface *surface;
    SDL_bool quit_=SDL_FALSE;
    SDL_RenderClear(renderer);
    initialise(jeu, surface, renderer); //initialisation de la partie jeu
    SDL_RenderCopy(renderer, jeu->motif[6], NULL, NULL); //affichage de l'image du menu
    SDL_RenderPresent(renderer);
    do
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)//fenetre fermer
        {
            *quit=SDL_TRUE;
            quit_ = SDL_TRUE;
            return -1;
        }
        if(event.type==SDL_MOUSEBUTTONDOWN)//clic
        {
            x = event.button.x;
            y = event.button.y;
            if(x<580 && x>320 && y<255 && y>215)  //choix de continuer une partie enregistrer
            {
                quit_=SDL_TRUE; //on peut quitter le menu
                menu_partie_save(jeu, quit, renderer); //on appele le menu continuer une partie enregistrer
                initialise(jeu, surface, renderer); // et reinitialise car les textures sont perdues lors de l'enregistrement

            }
            if(x<565 && x>330 && y<325 && y>290)  //choix de jouer une nouvelle partie contre l'ordinateur
            {
                quit_=SDL_TRUE; //on peut quitter
                jeu->multiplayer =  1; // active le mode jouer solo
            }
            if(x<565 && x>330 && y<400 && y>360)  //choix de jouer une nouvelle parie contre la machine
            {
                quit_=SDL_TRUE; //on peut quitter
                jeu->multiplayer =  0; // activer le mode joueur mutiple
            }
        }
        SDL_Delay(50);
    }
    while(!quit_);
}

/******************************************
* charge le tableau de texture chiffre    *
* qui sera utiliser par d'autres fonctions*
*******************************************/
void charge_ch(SDL_Texture *chiffre[10], SDL_Renderer *renderer)
{
    SDL_Surface *surface;
    int i;          //compteur
    char filename[8];
    for(i=0; i<10; i++)
    {
        itoa(i, filename, 10); //convertit i en chaine de caractere filename
        strcat(filename, "_ch.png"); //on ajoute l'extention
        surface = IMG_Load(filename);
        chiffre[i] = SDL_CreateTextureFromSurface(renderer, surface);
    }
}

/************************************
* enregistre la partie jeu dans le  *
* fichier de sauvergarde            *
*************************************/
void saving(partie jeu)
{
    FILE *t;
    jeu.saved = 1; //on modifie le champ saved avant l'enregistrement pour preciser que c'est une partie enregistrer lors de son lancement
    jeu.num_partie = verif_saving(); // on modifie aussi le champ num_partie fonction du nombre de fichier deja present
    if(verif_saving()==0)
        t = fopen("parties.part", "w+");
    else
        t = fopen("parties.part", "a");
    fwrite(&jeu, sizeof(jeu), 1, t);
    fclose(t);
    t = fopen("parties.part", "r");
    // printf("\t%d\n", fread(&jeu, sizeof(jeu), 1, t));
    fclose(t);
}

/***************************************************************
* menu quitter                                                 *
****************************************************************/
void men_quit(SDL_Renderer *renderer, SDL_bool *quit, partie jeu)
{
    SDL_Surface * surface;
    SDL_Texture *texture;
    SDL_Point point;
    SDL_Event event;
    SDL_bool quit_ = SDL_FALSE;
    SDL_Rect button_quit;   //rectangle pour le button quitter
    SDL_Rect button_retour;  //rectangle pour le button retour
    SDL_Rect button_save;      //rectangle pour le button save
    int posibl_save;

    if(verif_saving()==3)//on limite le nombre de partie enregistrable a 3
        posibl_save = 0;
    else
        posibl_save = 1;
    SDL_RenderClear(renderer);

    //chargement du fond du menu
    surface = IMG_Load("fond_exit.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //chargement du button exit
    surface = IMG_Load("exit.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    button_quit.x = 350;
    button_quit.y = 275;
    button_quit.h = 100;
    button_quit.w = 200;
    SDL_RenderCopy(renderer, texture, NULL, &button_quit);

    //chargement du button return
    surface = IMG_Load("return.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    button_retour.x = 350;
    button_retour.y = 175;
    button_retour.h = 125;
    button_retour.w = 200;
    SDL_RenderCopy(renderer, texture, NULL, &button_retour);
    if(posibl_save==1)//on peut l'enregistrer
    {
        //chargement du button exit
        surface = IMG_Load("save.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        button_save.x = 350;
        button_save.y = 100;
        button_save.h = 75;
        button_save.w = 200;
        SDL_RenderCopy(renderer, texture, NULL, &button_save);
    }
    SDL_RenderPresent(renderer);

    do
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)//quitter la SDL
            quit_ = SDL_TRUE;
        if(event.type == SDL_MOUSEBUTTONDOWN)//clic
        {
            point.x = event.button.x;
            point.y = event.button.y;
            if(SDL_PointInRect(&point, &button_quit))//clic sur exit
            {
                quit_ = SDL_TRUE;
            }
            else if(SDL_PointInRect(&point, &button_retour))//clic sur retour
            {
                quit_ = SDL_TRUE;
                *quit = SDL_FALSE;//on change la condition de fin du menu principal pour y revenir
            }
            else if(SDL_PointInRect(&point, &button_save) && posibl_save==1)//clic sur save
            {
                quit_ = SDL_TRUE;
                saving(jeu);//enregistrement de la partie
            }
        }
    }
    while(!quit_);

}

/********************************************
* decompose un nombre de 2 chiffres max     *
*********************************************/
void decompose_ch(int ch, int *ch1, int *ch2)
{
    *ch1 = ch/10;
    *ch2 = ch%10;
}

/***************************************************************************
* menu de fin de partie                                                    *
****************************************************************************/
void end_partie(SDL_Renderer *renderer, SDL_bool *quit, partie *jeu, SDL_Texture *chiffre[10])
{
    SDL_Event event;
    SDL_Surface * surface;
    SDL_Texture *texture;
    SDL_Rect replay;   //button replay
    SDL_Rect quitter;    //button quitter
    SDL_Rect rect;      //pour afficher les autres infos
    SDL_Point point;
    SDL_bool quit_ = SDL_FALSE;

    //on verifie si c'est une partie enregistrer pour la supprimer
    if(jeu->saved==1)
    {
        FILE *t;
        partie tab_partie[3]; //pour recopier le fichier
        int i, nbr_part_saved, cpt=0;//i, cpt compteur

        nbr_part_saved = verif_saving();

        //chargement du fichie dans le tableau
        t = fopen("parties.part", "r");
        for(i=0; i<nbr_part_saved; i++)
        {
            fread(&tab_partie[i], sizeof(partie), 1, t);
        }
        fclose(t);

        //on ecrase l'ancien contenu pour reécrire en sautant la partie en question
        t = fopen("parties.part", "w");
        for(i=0; i<nbr_part_saved; i++)
        {
            if(i!=jeu->num_partie) //si c'est pas la partie jouer
            {
                tab_partie[i].num_partie = cpt;
                cpt++;
                fwrite(&tab_partie[i], sizeof(partie), 1, t);
            }
        }
        fclose(t);
    }

    SDL_RenderClear(renderer);
    int ch[2], i;       //i compteur , ch pour la decompose des nombrs en ses differents chiffres

    //initialisation des rectangles replay et quitter
    replay.x = 100;
    replay.y = len_w_y-100;
    replay.h = 100;
    replay.w = 200;
    quitter = replay;
    quitter.x = len_w_x-300;

    //chargement de l'image de fond
    surface = IMG_Load("fond_end.jpg");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //chargement du complementaire a fond transparent de l'image de fond
    surface = IMG_Load("fond_end.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //affichage de score du joueur 1
    rect.h = 20;
    rect.w = 20;
    rect.y = 40;
    decompose_ch(jeu->score[0], &ch[0], &ch[1]); // on decompose son score en chiffes
    //on affiche le score du joueur 1
    for(i=0; i<2; i++)
    {
        rect.x = 80+20*i;
        SDL_RenderCopy(renderer, chiffre[ch[i]], NULL, &rect);
    }

    //affichage du score du joueur 2
    decompose_ch(jeu->score[1], &ch[0], &ch[1]); // on decompose son score en chiffes
    //on affiche le score du joueur 2
    for(i=0; i<2; i++)
    {
        rect.x = 730+20*i;
        SDL_RenderCopy(renderer, chiffre[ch[i]], NULL, &rect);
    }

    //affichage du verdit final de la partie
    rect.h = 300;
    rect.w = 500;
    rect.x = 200;
    rect.y = 100;

    if(jeu->multiplayer==1)//jeu contre le pc
    {
        if(jeu->score[0]>jeu->score[1])         //joueur a gagné
            surface = IMG_Load("you_win.png");
        if(jeu->score[0]<jeu->score[1])         //pc a gagné
            surface = IMG_Load("game_over.png");
    }
    if(jeu->multiplayer==0)//multijoueur
    {
        if(jeu->score[0]>jeu->score[1])         // joueur 1 a gagné
            surface = IMG_Load("p1_win.png");
        if(jeu->score[0]<jeu->score[1])         // joueur 2 a gagné
            surface = IMG_Load("p2_win.png");
    }
    if(jeu->score[0]==jeu->score[1])    //match null
        surface = IMG_Load("null.png");
    //affichage du  verdit
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    //affichage du button replay
    surface = IMG_Load("replay.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &replay);

    //affichage du button exit
    surface = IMG_Load("exit.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &quitter);
    SDL_RenderPresent(renderer);
    do
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT) //on quitte la SDL
        {
            quit_ = SDL_TRUE;
            *quit = SDL_TRUE; //on change aussi la condition de fin de la boucle principal
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)//clic
        {
            point.x = event.button.x;
            point.y = event.button.y;
            if(SDL_PointInRect(&point, &quitter))//clic sur quitter
                quit_ = SDL_TRUE;
            if(SDL_PointInRect(&point, &replay))//clic sur replay
            {
                quit_ = SDL_TRUE;
                jeu->saved = 0; //on remet le champ saved a 0
                menuprinci(jeu, quit, renderer);//on relance le menu principal
                *quit = SDL_FALSE;
            }
        }
    }
    while(!quit_);

}

int main(int argc, char* argv[])
{
    int i;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Texture *chiffre[10];   //tableau de texture de chiffres
    SDL_Point point;
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    partie jeu;             //creation d'une partie
    jeu.saved = 0;          // une nouvelle partie donc le champ saved vaut 0
    window = SDL_CreateWindow("MANCALA", 400, 400, len_w_x, len_w_y, SDL_WINDOW_SHOWN);       //creation de la fenetre
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);                      //creation du rendu

    //on verifie que le fichier de sauvegarde est bien present et on le cree si il n'existe pas d'ou le mode a+
    FILE *t;
    t = fopen("parties.part", "a+");
    fclose(t);

    //on charge les chiffres
    charge_ch(chiffre, renderer);

    //appel du menu principal
    menuprinci(&jeu, &quit, renderer);

    //affichage du jeu en console
    affiche_console(jeu.plateau);

    //rafraichissement de l'ecran
    refrech(&jeu, renderer, chiffre);

    while(!quit)
    {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT)//quitter le programme
            quit = SDL_TRUE;
        if(event.type==SDL_MOUSEBUTTONDOWN)//clic
        {
            point.x = event.button.x;
            point.y = event.button.y;
            if(jeu.player==1 || jeu.multiplayer==0)//joueur1 joue
            {
                i = search_indice(event, jeu); //recupere l'indice du trou selectionner
                if(i!=-1)//le trou est jouable
                    jouer(i, &jeu, renderer, chiffre);//jouer
                affiche_console(jeu.plateau);//affiche dans la console
            }
            if(SDL_PointInRect(&point, &jeu.exit))//clic sur exit
            {
                quit = SDL_TRUE;
                men_quit(renderer, &quit, jeu);//menu quitter
                refrech(&jeu, renderer, chiffre);//rafraichir l'ecran
            }
        }
        SDL_Delay(50);
        while(jeu.player==2 && jeu.multiplayer==1 && verif_etat_partie(&jeu)==0) //le pc joue
        {
            i = jeu_machine(jeu);//indice du trou choisi par la machine
            jouer(i, &jeu, renderer, chiffre);  //jouer a l'indice
            affiche_console(jeu.plateau);//affichage en console
            SDL_Delay(500);
        }
        if(verif_etat_partie(&jeu)==1) //partie finie
        {
            quit = SDL_TRUE;
            end_partie(renderer, &quit, &jeu, chiffre);//menu de fin de partie
            refrech(&jeu, renderer, chiffre);//rafraichit l'ecran
        }
        SDL_Delay(50);

    }
    return EXIT_SUCCESS;
}
