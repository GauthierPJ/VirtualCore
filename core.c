#include <stdio.h>

void fetch(){

}

void decode(){

}

void execute(){

}


int main(int argc, char *argv[]){

}


/*

Étapes :
    1 : Vérifier qu'on a bien un fichier en entrée
    2 : Vérifier le formatage correct du fichier
    3 : Extraction première ligne (première instruction)
    4 : Comparer chaque morceau de l'instruction au dictionaire (dico:tableau)
    5 : Éxecuter la fonction correspondante avec les données extraites
        --> Vérifier les flags, op codes
        --> Sortir les valeurs des registres concernés
        --> Assignations registres/flags...
        --> MAJ CP (numero de la prochaine instruction)
    6 : Loop sur étape 3

*/