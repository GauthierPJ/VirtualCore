#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define INS_SIZE 4
#define ERROR -1 
#define SUCCESS 0
#define REGISTER_NUMBERS 16


// start of test zone
typedef void (*Operation)();
typedef struct asso
{
    char* name;
    unsigned short mask;
    Operation func;
} ASSO;

void verifier(char* name,short mask){
    printf("L'OP code est %s , la valeur du masque est \"%02hhx\"\n",name,mask);
}
// end of test zone


// Returns 0 if the current ins is not a BCC. Something else otherwise.
int is_BCC(__uint8_t bcc){
    return (bcc & 0xF0); //= 11110000(2) 
}   

void fn_cmp(long op1, long op2, __uint8_t* flag){
    // Reset flag for the next instruction
    *flag = 0x0;
    if(op1 == op2){
        *flag = *flag | 0x32;
    } else {
        *flag = *flag | 0x16;
    }
    if(op1 <= op2)
        *flag = *flag | 0x8;
    if(op1 >= op2) 
        *flag = *flag | 0x4;
    if(op1 < op2) 
        *flag = *flag | 0x2;
    if(op1 > op2) 
        *flag = *flag | 0x1;
}

void fn_and(long* dest, long op1, long op2){
    *dest = op1 & op2;
}

void fn_or(long* dest, long op1, long op2){
    *dest = op1 | op2;
}

void fn_xor(long* dest, long op1, long op2){
    *dest = op1 ^ op2;
}

void fn_add(long* dest, long op1, long op2){
    *dest = op1 + op2;
}

void fn_sub(long* dest, long op1, long op2){
    *dest = op1 - op2;
}


void fn_mov(long* dest, long* op1){
    dest = op1;
}

void fn_lsh(long* dest, long op1, long op2){
    *dest = op1 << op2;
}

void fn_rsh(long* dest, long op1, long op2){
    *dest = op1 >> op2;
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
                get_hex[++i] = c;
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

void fetch(__uint8_t** instructions,ASSO* tab_opcode ,unsigned int nb_ins){
    // The PC corresponds to the address of the next instructions, which means the 
    // first one at the beginning.
    long long int PC = instructions[0][0]; 
    // Unsigned byte for the flag
    __uint8_t flag = 0;
    __uint8_t* ptf_flag = &flag;
    // For each instruction
    printf("Traduction : \n");
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        // If the current instruction is a BCC
        if(is_BCC(instructions[i][0])){
            // Calcul new PC and not execute the instruction
            printf("instruction BCC ignorée pour l'instant\n ");
            
        } else {
            // PC += 1 and execute the instruction
            //printf("Y = %x \n", instructions[i][0]);
            for(unsigned short k = 0; k<11;k++){
                char opcode = (0xF0 & instructions[i][1]);
                printf("opcode : %hhx, mask : %hhx,res = %hhx\n",opcode,tab_opcode[k].mask,(opcode==tab_opcode[k].mask));
                if((opcode == tab_opcode[k].mask) ){
                    printf("%hhx : %s\n",i,tab_opcode[k].name);
                }
            }
        }
    }

}

void decode(){

}

void execute(){

}

void print_ins(__uint8_t** instructions, unsigned int nb_ins ){
    printf("Instructions : \n\n");
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        printf("%03d : ", i);
        for(unsigned int y = 0 ; y < INS_SIZE ; y++){
            printf("%02hhx ",instructions[i][y]);
        }
        printf("\n");
    }
    printf("\n");
}


int free_2d(__uint8_t** array, unsigned int len){

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

__uint8_t** init_ins(char* argv, unsigned int* ptr_nb_ins){
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
    //n instructions -> each instruction is n/4 bytes
    const unsigned int nb_ins = nb_bytes_to_read/INS_SIZE;
    if(nb_ins > 4294967296 ){
        return NULL;
    }
    fseek(f, 0L, SEEK_SET);
    
    //initialiaze the pointer to the content of the program
    __uint8_t* program = (__uint8_t*) malloc(nb_bytes_to_read*sizeof(__uint8_t));
    fread(program, INS_SIZE, nb_bytes_to_read, f);
    

    __uint8_t** instructions = (__uint8_t**) malloc(nb_ins*sizeof(__uint8_t*));
    for(unsigned int r = 0 ; r < nb_ins ; r++){
        instructions[r] = (__uint8_t*) malloc(INS_SIZE * sizeof(__uint8_t));
    }

    if(instructions == NULL){
        return NULL;
    } 


    for (unsigned int l = 0; l < nb_ins; l++){
      // for each insctruction
      for (unsigned int b = 0; b < INS_SIZE; b++){
        // save the 4 bytes in a cell
        instructions[l][b] = program[(l*INS_SIZE)+b];
      }
    }


    // Modify the value of the pointer
    *ptr_nb_ins = nb_ins;
    free(program);
    program = NULL;
    return instructions;
}



ASSO* initAsso(){
    ASSO* opcodes = (ASSO*) malloc(11*sizeof(ASSO));

    opcodes[0].name = "AND";
    opcodes[1].name = "OR";
    opcodes[2].name = "XOR";
    opcodes[3].name = "ADD";
    opcodes[4].name = "ADC";
    opcodes[5].name = "CMP";
    opcodes[6].name = "SUB";
    opcodes[7].name = "SBC";
    opcodes[8].name = "MOV";
    opcodes[9].name = "LSH";
    opcodes[10].name = "RSH";

    for (unsigned short i = 0; i < 11; i++){
        opcodes[i].mask = 0x00 + (i*0x10);
        opcodes[i].func = verifier;
    }
   
    return opcodes;
}

int main(int argc, char *argv[]){ 
    
    ASSO* opcodes = initAsso();

    // for (size_t i = 0; i < 11; i++)
    // {
    //     opcodes[i].func(opcodes[i].name,opcodes[i].mask);
    // }
    

    // verify that the right number of arguments were specifide, exits otherwise
    if(argc != 2){
        printf("Error : An executable file must be specified!\n");
        printf("Hint : ./core <filename>\n");
        return ERROR;
    }

    unsigned int nb_ins = 0;
    unsigned int* ptr_nb_ins = &nb_ins;

    

    
    /***********************************************************************
     *                           INIT VARIABLES                            *
     * ********************************************************************/

    __uint8_t** instructions = init_ins(argv[1], ptr_nb_ins);

    long* registers = init_registers();

    if(instructions == NULL){ //|| register == NULL){
        return ERROR;
    }

    print_ins(instructions, nb_ins);

    fetch(instructions,opcodes, nb_ins);

  
    free(registers);
    free_2d(instructions,nb_ins);
    instructions = NULL;
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