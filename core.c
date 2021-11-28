#include <stdio.h>
#include <stdlib.h>
void fetch(){

}

void decode(){

}

void execute(){

}


int main(int argc, char *argv[]){
    
    // verify that the right number of arguments were specifide, exits otherwise
    if(argc < 2){
        printf("Error : An executable file must be specified!\n");
        printf("Hint : ./core <filename>\n");
        return 0;
    }
    if(argc > 2){
        printf("Error : Only one file should be specified!\n");
        printf("Hint : ./core <filename>\n");
        return 0;
    }

    // open the given file
    FILE* f;
    f = fopen(argv[1],"rb");
    
    // check the file could be opened or exits
    if (f==NULL){
        printf("Error : Something went wrong opening the specified file\n");
        return 0;
    }
    

    const size_t line_size = 300;
    char* line = malloc(line_size);
    fread(line, line_size, 200, f);
    printf("%s",line);
    free(line);
    
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