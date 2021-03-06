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
    unsigned short value;
    Operation func;
} ASSO;

ASSO* init_asso_opcode(){
    ASSO* opcodes = (ASSO*) malloc(11 * sizeof(ASSO));

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
        opcodes[i].value = 0x00 + (i*0x10);
    }
   
    return opcodes;
}

ASSO* init_asso_bcc(){
    ASSO* bcc = (ASSO*) malloc(7*sizeof(ASSO));
    bcc[0].name = "B";
    bcc[1].name = "BEQ";
    bcc[2].name = "BNE";
    bcc[3].name = "BLE";
    bcc[4].name = "BGE";
    bcc[5].name = "BL";
    bcc[6].name = "BG";

    for (unsigned short i = 0; i < 7; i++){
        bcc[i].value = 0x80 + (i*0x10);
    }
    return bcc;
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







void decode_BCC(__uint8_t* instruction){

    ASSO* tab_bcc = init_asso_bcc();

    for(unsigned short k = 0; k<7;k++){
        __uint8_t bcc = (0xF0 & instruction[0]);
        __int32_t offset = ((instruction[3]))+(instruction[2]<<8)+(instruction[1]<<16)+((instruction[0]&0x07)<<24);
        if(instruction[0]&0x08){
            offset = offset*-1;
        }
        if((bcc == tab_bcc[k].value) ){
            printf("%s %d\n",tab_bcc[k].name,offset);
        }
    }
}

void decode_OPCODE(__uint8_t* instruction){

    ASSO* tab_opcode = init_asso_opcode();

    for(unsigned short k = 0; k<11;k++){
        __uint8_t opcode = (0xF0 & instruction[1]);
        if((opcode == tab_opcode[k].value) ){
            printf("%s ",tab_opcode[k].name);
        }
    }
    if(instruction[0] & 0x0F){
        printf("R%d R%d %d\n",(instruction[2] & 0x0F),(instruction[1] & 0x0F),(instruction[3]));
    }
    else{
        printf("R%d R%d R%d\n",(instruction[2] & 0x0F),(instruction[1] & 0x0F),(instruction[2]>>4));
    }
}


void decode(__uint8_t* instruction){

    if(is_BCC(instruction[0])){
        decode_BCC(instruction);
    }
    else {
        decode_OPCODE(instruction);
    }
}

void fetch(__uint8_t** instructions,unsigned int nb_ins){
    // The PC corresponds to the address of the next instructions, which means the 
    // first one at the beginning.
    long long int PC = instructions[0][0]; 
    // Unsigned byte for the flag
    __uint8_t flag = 0;
    __uint8_t* ptf_flag = &flag;
    // For each instruction
    printf("Traduction : \n");
    for(unsigned int i = 0 ; i < nb_ins ; i++){
        printf("%02d : ",i);
        decode(instructions[i]);
    }
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

int main(int argc, char *argv[]){ 
        
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

    fetch(instructions, nb_ins);

  
    free(registers);
    free_2d(instructions,nb_ins);
    instructions = NULL;
    registers = NULL;

    return SUCCESS;
    
}

/*

??tapes :
    1 : V??rifier qu'on a bien un fichier en entr??e
    2 : V??rifier le formatage correct du fichier
    3 : Extraction premi??re ligne (premi??re instruction)
    4 : Comparer chaque morceau de l'instruction au dictionaire (dico:tableau)
    5 : ??xecuter la fonction correspondante avec les donn??es extraites
        --> V??rifier les flags, op codes
        --> Sortir les valeurs des registres concern??s
        --> Assignations registres/flags...
        --> MAJ CP (numero de la prochaine instruction)
    6 : Loop sur ??tape 3
    Taille types C : https://fr.wikipedia.org/wiki/Types_de_donn%C3%A9e_du_langage_C


*/