#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define INS_SIZE 4
#define ERROR -1 
#define SUCCESS 0
#define REGISTER_NUMBERS 16

// Returns 0 if the current ins is not a BCC. Something else otherwise.
int is_BCC(char last_ins_byte){
    return (last_ins_byte & 0xF); //(16) = 00001111(2) = 1111(2)
}   

// Returns the pointer on the 16 initialized registers
// NB : int* because a register is stored on 4 bytes
long*  init_registers(){
    long* registers = (long*) malloc(16*sizeof(long)); // Pointer on registers. Return value
    char* get_hex = (char*) malloc(11*sizeof(char)); // Get ascii hex value of each registers of the file list
    FILE* f; // Pointer on the register init state file
    char c;
    unsigned int i = 0;
    unsigned int y = 0;

    f = fopen("initial_register_state.conf","r");
    
    // check the file could be opened or exits
    if (f==NULL){
        printf("Error : Something went wrong opening the specified file\n");
        printf("Hint  : The file must be called 'initial_register_state.conf'\n");
        return NULL;
    }

    while ((c = getc(f)) != EOF){
        if(c == '='){
            while(i < 10){
                c = getc(f);
                get_hex[i] = c;
                i++;
            }
            get_hex[i++] = '\0';
            registers[y] = strtol(get_hex, NULL, 0);
            i=0;
            y++;
            if(y > 16){
                printf("Error : The number of initial registers must be 16. \n");
                return NULL;
            }
        }
    }
    free(get_hex);
    if(y < 16){
        printf("Error : The number of initial registers must be 16. \n");
        return NULL;
    }
    return registers;
}

void fetch(char** instructions, unsigned int nb_ins){
    // The PC corresponds to the address of the next instructions, which means the 
    // first one at the beginning.
    long long int PC = instructions[0][0]; 
    // For each instruction
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        // If the current instruction is a BCC
        if(is_BCC(instructions[i][3])){
            // Calcul new PC and not execute the instruction
            printf("X = %x \n", instructions[i][3]);
        } else {
            // PC += 1 and execute the instruction
            printf("Y = %1x \n", instructions[i][3]);
        }
    }

}

void decode(){

}

void execute(){

}

void print_ins(char** instructions, unsigned int nb_ins ){
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        printf("%03d : ", i);
        for(unsigned char y = 0 ; y < INS_SIZE ; y++){
            printf("%02hhx ",instructions[i][y]);
        }
        printf("\n");
    }
}

int free_2d(char** array, unsigned int len){

    if(array == NULL){
        return ERROR;
    }

    for(unsigned int i = 0 ; i < len ; i++){
        if(array[i] == NULL){
            return ERROR;
        } else {
            free(array[i]);
            array[i] = NULL;
        }
    }

    free(array);
    array = NULL;
    return SUCCESS;
}

char** init_ins(char* argv, unsigned int* ptr_nb_ins){
     // open the given file
    FILE* f;
    f = fopen(argv,"rb");
    
    // check the file could be opened or exits
    if (f==NULL){
        printf("Error : Something went wrong opening the specified file\n");
        return NULL;
    }

    // get size of the file and replaces the reader to the begining
    fseek(f, 0L, SEEK_END);
    unsigned long int nb_bytes_to_read = ftell(f);
    if(nb_bytes_to_read > 4294967295){
        return NULL;
    }
    fseek(f, 0L, SEEK_SET);
    
    // initialiaze the pointer to the content of the program
    char * program = malloc(nb_bytes_to_read);
    // get the data from the binary
    fread(program, INS_SIZE, nb_bytes_to_read, f);

    //n instructions -> each instruction is n/4 bytes
    const unsigned int nb_ins = nb_bytes_to_read/INS_SIZE;
    if(nb_ins > 65535 ){
        return NULL;
    }
    
    char ** instructions = (char**) malloc(nb_ins*sizeof(char*));
    if(instructions == NULL){
        return NULL;
    }
    
    for(unsigned char i = 0; i < nb_ins ; i++){
        instructions[i] = (char*) malloc(INS_SIZE * sizeof(char));
    }

    for (unsigned int l = 0; l < nb_ins; l++){
      // for each insctruction

      for (unsigned char b = 0; b < INS_SIZE; b++){
        // save the 4 bytes in a cell
        instructions[l][b] = program[(l*INS_SIZE)+((INS_SIZE-1)-b)];
        // switch big endian exe to little endian instructions to deal with
        // 0 -> 3 
        // 1 -> 2 
        // 2 -> 1 
        // 3 -> 0
      }
    }
    // Modify the value of the pointer
    *ptr_nb_ins = nb_ins;
    free(program);
    program = NULL;
    return instructions;
}

int main(int argc, char *argv[]){ 
    
    // verify that the right number of arguments were specifide, exits otherwise
    if(argc != 2){
        printf("Error : An executable file must be specified!\n");
        printf("Hint : ./core <filename>\n");
        return ERROR;
    }

    unsigned int nb_ins = 0;
    unsigned int* ptr_nb_ins = &nb_ins;
    

    char** instructions = init_ins(argv[1], ptr_nb_ins);

    long* registers = init_registers();

    if(instructions == NULL){
        return ERROR;
    }

    print_ins(instructions, nb_ins);

    fetch(instructions, nb_ins);

    if(free_2d(instructions, nb_ins) == ERROR){
        return ERROR;
    }    
    free(registers);
    registers = NULL;

    return SUCCESS;
    
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