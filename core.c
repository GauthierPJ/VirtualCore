#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define INS_SIZE 4

void fetch(){

}

void decode(){

}

void execute(){

}

void print_ins(char** instructions, const unsigned int nb_ins ){
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        printf("%03d : ", i);
        for(unsigned char y = 0 ; y < INS_SIZE ; y++){
            printf("%02hhx ",instructions[i][y]);
        }
        printf("\n");
    }
}

void free_2d(char** array, unsigned int len){
    for(unsigned int i = 0 ; i < len ; i++){
        free(array[i]);
    }
    free(array);
}

int main(int argc, char *argv[]){ 
    
    // verify that the right number of arguments were specifide, exits otherwise
    if(argc != 2){
        printf("Error : An executable file must be specified!\n");
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

    // get size of the file and replaces the reader to the begining
    fseek(f, 0L, SEEK_END);
    long int nb_bytes_to_read = ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    // initialiaze the pointer to the content of the program
    char * program = malloc(nb_bytes_to_read);
    // get the data from the binary
    fread(program, INS_SIZE, nb_bytes_to_read, f);

    //n instructions -> each instruction is n/4 bytes
    const unsigned int nb_ins = nb_bytes_to_read/INS_SIZE;
    char ** instrutions = (char**) malloc(nb_ins*sizeof(char*));
    
    for(unsigned char i = 0; i < nb_ins ; i++){
        instrutions[i] = (char*) malloc(INS_SIZE * sizeof(char));
    }

    for (unsigned int l = 0; l < nb_ins; l++){
      // for each insctruction
      for (unsigned char b = 0; b < INS_SIZE; b++){
        // save the 4 bytes in a cell
        instrutions[l][b] = program[(l*4)+b];
      }
    }

    // // print table ** TEMPORARY **
    // for (unsigned int l = 0; l < nb_ins; l++){
    //   // for each insctruction
    //   for (unsigned char b = 0; b < INS_SIZE; b++){
    //     // save the 4 bytes in a cell
    //     printf("%hhx ",instrutions[l][b]);
    //   }
    //   printf("\n");
    // }
    
    print_ins(instrutions, nb_ins);

    free_2d(instrutions, nb_ins);

    //char** instructions = read_bytes_from_file();

    
    // printf("Number of bytes to read : %ld \n",nb_bytes_to_read);
    // // read each line
    //     for(int i=0; i < nb_bytes_to_read;i++){
    //         printf("%hhx : ",program[i]);
        
    //     }
    
    free(program);
    return 0;
    
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
    Taille types C : https://fr.wikipedia.org/wiki/Types_de_donn%C3%A9e_du_langage_C


*/