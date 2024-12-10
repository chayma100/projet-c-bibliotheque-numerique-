#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct livre {
    char titre[100];
    char auteur[50];
    int stock;
    char categorie[50];
    int annee_publication;
    struct livre *suivant;
} livre;

typedef struct date {
    int jour;
    int mois;
    int annee;
} date;

typedef struct historique {
    char operation[10];
    char titre[100];
    char auteur[50];
    date date_operation;
    struct historique *suivant;
} historique;

typedef struct utilisateur {
    char nom[50];
    int nbre_pts;
    int ID;
    historique *historique_tete;  // c'est un pointeur qui pointe sur la tete de la file
    historique *historique_queue; // c'est pointeur qui pointe sur le queue de la file
} utilisateur;

// c'est une liste chainee des pointeurs utilisateurs
typedef struct liste_utilisateurs {
    utilisateur *utilisateur;
    struct liste_utilisateurs *suivant;
} liste_utilisateurs;

// ajout d'un livre
void ajouter_livre(livre** biblio) {
    char titre[100], auteur[50];
    int test = 0;
    printf("Donnez le titre du livre :\n");
    scanf("%99[^\n]", titre);
    getchar();
    printf("Donner le nom de l'auteur :\n");
    scanf("%49[^\n]", auteur);
    getchar();
    livre* aux = *biblio;
    while (aux != NULL) {
        if (strcmp(aux->titre, titre) == 0 && strcmp(aux->auteur, auteur) == 0) {
            aux->stock++;
            printf("Le livre existe deja. Le stock a ete mis a jour a : %d \n", aux->stock);
            test = 1;
            break;
        }
        aux = aux->suivant;
    }
    if (!test) {
        livre* nouveau_livre = (livre*)malloc(sizeof(livre));
        if (nouveau_livre == NULL) {
            printf("Il y a une erreur d'allocation memoire\n");
            return;
        }
        strcpy(nouveau_livre->auteur, auteur);
        strcpy(nouveau_livre->titre, titre);
        printf("Donner la categorie:\n");
        scanf("%49[^\n]", nouveau_livre->categorie);
        getchar();
        printf("Donner l'annee de publication:\n");
        scanf("%i", &nouveau_livre->annee_publication);
        getchar();
        nouveau_livre->stock = 1;
        nouveau_livre->suivant = NULL;
        if (*biblio == NULL) {
            *biblio = nouveau_livre;
        } else {
            livre* aux = *biblio;
            while (aux->suivant != NULL) {
                aux = aux->suivant;
            }
            aux->suivant = nouveau_livre;
        }
        printf("Le livre a ete ajoute dans la bibliotheque\n");
        getchar();
    }
}

// stockage des livres dans un fichier bibliotheque.txt
void stocker_biblio(livre* biblio) {
    FILE* fichier = fopen("bibliotheque.txt", "w");
    if (fichier == NULL) {
        printf("Il y a un probleme d'ouverture du fichier\n");
        return;
    }
    livre* aux = biblio;
    while (aux != NULL) {
        fprintf(fichier, "Titre: %s\n", aux->titre);
        fprintf(fichier, "Auteur: %s\n", aux->auteur);
        fprintf(fichier, "Categorie: %s\n", aux->categorie);
        fprintf(fichier, "Annee publication: %d\n", aux->annee_publication);
        fprintf(fichier, "Stock: %d\n", aux->stock);
        aux = aux->suivant;
    }
    fclose(fichier);
    printf("Les donnees ont ete bien stockees dans bibliotheque.txt\n");
}

// affichage du contenu de la bibliotheque
void afficher_biblio() {
    FILE* fichier = fopen("bibliotheque.txt", "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    char ligne[200];
    printf("Contenu de la bibliotheque:\n");
    while (fgets(ligne, sizeof(ligne), fichier)) {
        printf("%s", ligne);
    }
    fclose(fichier);
}

// calcul de la date du retour
date calculer_dateretour(date date_emp) {
    date date_retour = date_emp;
    date_retour.jour += 30;

    while (1) {
        int nbre_jours;
        if (date_retour.mois == 1 || date_retour.mois == 3 || date_retour.mois == 5 || date_retour.mois == 7 || date_retour.mois == 8 || date_retour.mois == 10 || date_retour.mois == 12) {
            nbre_jours = 31;
        } else if (date_retour.mois == 4 || date_retour.mois == 6 || date_retour.mois == 9 || date_retour.mois == 11) {
            nbre_jours = 30;
        } else if (date_retour.mois == 2) {

            if ((date_retour.annee % 4 == 0 && date_retour.annee % 100 != 0) || (date_retour.annee % 400 == 0)) {
                nbre_jours = 29;
            } else {
                nbre_jours = 28;
            }
        }

        if (date_retour.jour > nbre_jours) {
            date_retour.jour -= nbre_jours;
            date_retour.mois++;

            if (date_retour.mois > 12) {
                date_retour.mois = 1;
                date_retour.annee++;
            }
        } else {
            break;
        }
    }

    return date_retour;
}

// ajout des utilisateurs dans une liste chainee
utilisateur* ajouter_utilisateur(liste_utilisateurs** liste, int id) {
    char nom[50];
    liste_utilisateurs* aux = *liste;
    while (aux != NULL) {
        if (aux->utilisateur->ID == id) {
            printf("L'utilisateur avec l'ID %i existe deja\n", id);
            return aux->utilisateur;
        }
        aux = aux->suivant;
    }
    utilisateur* nouveau = malloc(sizeof(utilisateur));
    if (nouveau == NULL) {
        printf("Erreur d'allocation memoire\n");
        return NULL;
    }
    printf("Donnez votre nom: ");
    getchar();
    scanf("%49[^\n]", nom);
    nouveau->ID = id;
    nouveau->historique_tete = NULL;
    nouveau->historique_queue = NULL;
    nouveau->nbre_pts = 0;
    strcpy(nouveau->nom, nom);
    liste_utilisateurs* nouveau_noeud = malloc(sizeof(liste_utilisateurs));
    nouveau_noeud->utilisateur = nouveau;
    nouveau_noeud->suivant = *liste;
    *liste = nouveau_noeud;
    return nouveau;
}

// historique des utilisateurs sous forme d'un fichier historique.utilisateur.txt
void ajouter_historique_utilisateur(utilisateur* utilisateur, const char* operation, const char* titre, const char* auteur, date dateoperation) {
    historique* nv = (historique*)malloc(sizeof(historique));
    if (nv == NULL) {
        printf("Erreur d'allocation memoire\n");
    } else {
        strcpy(nv->operation, operation);
        strcpy(nv->titre, titre);
        strcpy(nv->auteur, auteur);
        nv->date_operation = dateoperation;
        nv->suivant = NULL;
        if (utilisateur->historique_queue == NULL) {
            utilisateur->historique_tete = nv;
            utilisateur->historique_queue = nv;
        } else {
            utilisateur->historique_queue->suivant = nv;
            utilisateur->historique_queue = nv;
        }
        FILE* fichier = fopen("historique_utilisateur.txt", "a");
        if (fichier == NULL) {
            printf("Erreur lors de l'ouverture du fichier\n");
        }
        fprintf(fichier, "Operation: %s, Titre: %s, Auteur: %s, Date: %d/%d/%d\n", operation, titre, auteur, dateoperation.jour, dateoperation.mois, dateoperation.annee);
        fclose(fichier);
    }
}

// affichage de l'historique
void afficher_historique_utilisateur(utilisateur* utilisateur) {
    if (utilisateur->historique_tete == NULL) {
        printf("L'historique est vide\n");
    } else {
        historique* temp = utilisateur->historique_tete;
        printf("L'historique de %s:\n", utilisateur->nom);
        while (temp != NULL) {
            printf("Operation: %s, Titre: %s, Auteur: %s, Date: %02d/%02d/%d\n", temp->operation, temp->titre, temp->auteur, temp->date_operation.jour, temp->date_operation.mois, temp->date_operation.annee);
            temp = temp->suivant;
        }
    }
}

// gerer l'operation convenable (emprunter/retourner)
void operation_livre(utilisateur* utilisateur, const char* titre, const char* auteur, date date_operation, const char* operation, livre* biblio) {
    livre* aux = NULL;
    aux = biblio;

    // Chercher le livre dans la bibliothèque
    while (aux != NULL) {
        if (strcmp(aux->titre, titre) == 0 && strcmp(aux->auteur, auteur) == 0) {
            break;
        }
        aux = aux->suivant;
    }

    if (aux == NULL) {
        printf("Le livre '%s' de '%s' n'existe pas dans la bibliotheque.\n", titre, auteur);
        return;
    }

    if (strcmp(operation, "emprunter") == 0) {
        if (aux->stock > 0) {
            aux->stock--;  // Décrémenter le stock lors de l'emprunt
            date date_retour = calculer_dateretour(date_operation);
            printf("L'utilisateur %s a emprunte le livre %s de %s à la date : %02d/%02d/%d\n", utilisateur->nom, titre, auteur, date_operation.jour, date_operation.mois, date_operation.annee);
            printf("La date de retour prevue est : %02d/%02d/%d\n", date_retour.jour, date_retour.mois, date_retour.annee);
            ajouter_historique_utilisateur(utilisateur, "emprunt", titre, auteur, date_operation);
        } else {
            printf("Desole, il n'y a plus de stock pour ce livre.\n");
        }
    } else if (strcmp(operation, "retourner") == 0) {
        aux->stock++;  // Incrémenter le stock lors du retour
        date date_retour = calculer_dateretour(date_operation);
        if (date_operation.jour > date_retour.jour ||
            (date_operation.jour == date_retour.jour && date_operation.mois > date_retour.mois) ||
            (date_operation.jour == date_retour.jour && date_operation.mois == date_retour.mois && date_operation.annee > date_retour.annee)) {
            printf("Le livre '%s' de '%s' a ete retourne en retard. Date de retour prevue : %02d/%02d/%d\n", titre, auteur, date_retour.jour, date_retour.mois, date_retour.annee);
            utilisateur->nbre_pts -= 10;
        } else {
            printf("Le livre '%s' de '%s' a ete retourne a temps.\n", titre, auteur);
            utilisateur->nbre_pts += 10;
        }
        ajouter_historique_utilisateur(utilisateur, "retour", titre, auteur, date_operation);
    }
}

// programme principal
int main() {
    liste_utilisateurs* liste = NULL;
    livre* biblio = NULL;
    int ch;
    int id;
     char nom[50];
    utilisateur* utilisateur = NULL;

    do {
        printf("\n___Bienvenue dans notre bibliotheque___\n");
        printf("Que voulez-vous faire?\n");
        printf("1/ Ajouter un livre\n");
        printf("2/ (1* Emprunter / 2* Retourner) un livre\n");
        printf("3/ Afficher votre historique\n");
        printf("4/ Afficher le contenu de la bibliotheque\n");
        printf("5/ Savoir vos points\n");
        printf("6/ Quitter le programme\n");
        printf("Veuillez entrer votre choix : ");
        scanf("%i", &ch);
        getchar();
        switch (ch) {
            case 1: {
                ajouter_livre(&biblio);
                stocker_biblio(biblio);
                break;
            }
            case 2: {
               printf("donner votre ID\n");
               scanf("%i",&id);
               getchar();
               printf("donner votre nom\n");
               scanf("%49[^\n]",nom);
               getchar();
                printf("Donnez le titre du livre: ");
                char titre[100], auteur[50];
                scanf("%99[^\n]", titre);
                getchar();
                printf("Donnez l'auteur du livre: ");
                scanf("%49[^\n]", auteur);
                getchar();
                printf("1. Emprunter\n2. Retourner\n");
                int op;
                scanf("%i", &op);
                date date_operation;
                printf("Entrez la date de l'operation (jour/mois/annee) : ");
                scanf("%d/%d/%d", &date_operation.jour, &date_operation.mois, &date_operation.annee);
                if (op == 1) {
                    operation_livre(nom, titre, auteur, date_operation, "emprunter", biblio);
                } else if (op == 2) {
                    operation_livre(nom, titre, auteur, date_operation, "retourner", biblio);
                }
                break;
            }
            case 3: {
                if (utilisateur == NULL) {
                    printf("Veuillez d'abord vous inscrire ou vous connecter\n");
                    break;
                }
                afficher_historique_utilisateur(utilisateur);
                break;
            }
            case 4: {
                afficher_biblio();
                break;
            }
            case 5: {
                if (utilisateur == NULL) {
                    printf("Veuillez d'abord vous inscrire ou vous connecter\n");
                    break;
                }
                printf("Vos points : %d\n", utilisateur->nbre_pts);
                break;
            }
            case 6: {
                printf("Merci d'avoir utilise notre systeme de bibliotheque\n");
                break;
            }
            default:
                printf("Choix invalide. Essayez encore.\n");
        }
    } while (ch != 6);

    return 0;
}
