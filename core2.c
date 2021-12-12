#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define INS_SIZE 4
#define ERROR -1 
#define SUCCESS 0
#define REGISTER_NUMBERS 16



typedef void (*opcode_func)(long* registers,long val_op1,long val_op2);

typedef struct asso
{
    char* name;
    unsigned short value;
    unsigned short nb_args;
    opcode_func execute;
} ASSO;

 
typedef struct coreData
{
    __uint8_t** instructions;
    unsigned int nb_ins;
    long flags;
    long* registers;
    ASSO* opcode;
    ASSO* bcc;
    long pc;
} coreData;





/// UTILS ZONE /////////////////////////////////////////////////////////////////////////

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

int is_BCC(__uint8_t bcc){
    return (bcc & 0xF0); //= 11110000(2) 
}   

int has_imediate_value(__uint8_t* instruction){
    return (instruction[0] && 0x01);
}

////////////////////////////////////////////////////////////////////////////////////////

/// PRINT ZONE /////////////////////////////////////////////////////////////////////////

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

void print_registres(long* registres){
    for (long i = 0; i < 16; i++)
    {
        printf("R%ld : %ld\n",i,registres[i]);
    }
    
}

////////////////////////////////////////////////////////////////////////////////////////




/// ASM FUNCTION ZONE //////////////////////////////////////////////////////////////////

  // if(instruction[0] & 0x0F){
    //     printf("R%d R%d %d\n",(instruction[2] & 0x0F),(instruction[1] & 0x0F),(instruction[3]));
    // }
    // else{
    //     printf("R%d R%d R%d\n",(instruction[2] & 0x0F),(instruction[1] & 0x0F),(instruction[2]>>4));
    // }

// void fn_and(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     long op1 =  (instruction[1] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] & instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] & registres[op2];
//     }
// }

// void fn_or(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     long op1 =  (instruction[1] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] | instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] | registres[op2];
//     }
// }


// void fn_xor(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     long op1 =  (instruction[1] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] ^ instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] ^ registres[op2];
//     }
// }

// void fn_add(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     long op1 =  (instruction[1] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] + instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] + registres[op2];
//     }
// }


// /// A FAIRE
// void fn_cmp(u_int8_t* instruction,long* registres,__uint8_t* flags){
    
// }

// void fn_sub(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     long op1 =  (instruction[1] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] - instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] - registres[op2];
//     }
// }

// void fn_mov(u_int8_t* instruction,long* registres,__uint8_t* flags){
//     long dest = (instruction[2] & 0x0F);
//     if(has_imediate_value(instruction)){
//         registres[dest] = registres[op1] - instruction[3];

//     }
//     else{
//         long op2 =(instruction[2]>>4);
//         printf("valuers : dest(%ld) op1(%ld) iv(%ld)\n",(registres[dest]),registres[op1],registres[op2]);
//         registres[dest] = registres[op1] - registres[op2];
//     }
// }


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


void fn_mov(long* dest, long op1,long op2){
    *dest = op1;
}

void fn_lsh(long* dest, long op1, long op2){
    *dest = op1 << op2;
}

void fn_rsh(long* dest, long op1, long op2){
    *dest = op1 >> op2;
}

void fn_cmp(long* flag,long op1,long op2){
    *flag = 64;
    if(op1==op2)
        *flag += 32;
    if(op1!=op2)
        *flag += 16;
    if(op1<=op2)
        *flag += 8;
    if(op1>=op2)
        *flag += 4;
    if(op1<op2)
        *flag += 2;
    if(op1>op2)
        *flag += 1;
}


void three_values(__uint8_t* instruction,long* registers, ASSO opcode){
    long dest = (instruction[2] & 0x0F);
    long op1 =  (instruction[1] & 0x0F);
    if(has_imediate_value(instruction)){
        opcode.execute(&registers[dest],registers[op1],instruction[3]);

    }
    else{
        long op2 =(instruction[2]>>4);
        opcode.execute(&registers[dest],registers[op1],registers[op2]);
    }
}

void two_values(__uint8_t* instruction,long* registers,long* flags, ASSO opcode){
    long dest = (instruction[2] & 0x0F);
    long op1 =  (instruction[1] & 0x0F);
    if(has_imediate_value(instruction)){
        if (opcode.name == "CMP"){
            opcode.execute(flags,registers[op1],instruction[3]);
        }else{
            opcode.execute(&registers[dest],instruction[3],0);
        }
        
    }
    else{
        long op2 =(instruction[2]>>4);
        if (opcode.name == "CMP"){
           opcode.execute(flags,registers[op1],registers[op2]);
        }else{
            opcode.execute(&registers[dest],registers[op2],0);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////





/// CORE ZONE //////////////////////////////////////////////////////////////////////////

void execute_BCC(ASSO code,__int32_t offset,long flag,long* pc){
    if (code.nb_args&flag)
    {
        *pc += offset;
    }
    else{
        *pc+= 1;
    }
    
    
}


void decode_BCC(coreData* core){
    __uint8_t* instruction = core-> instructions[core->pc];
    ASSO* tab_bcc= core->bcc;
    ASSO code;
    __int32_t offset = ((instruction[3]))+(instruction[2]<<8)+(instruction[1]<<16)+((instruction[0]&0x07)<<24);
    if(instruction[0]&0x08){
        offset = offset*-1;
    }
    __uint8_t bcc = (0xF0 & instruction[0]);
    for(unsigned short k = 0; k<7;k++){
        
        if((bcc == tab_bcc[k].value) ){
            code = tab_bcc[k];
        }
    }
    execute_BCC(code,offset,core->flags,&core->pc);
    
}

void decode_OPCODE(coreData* core){
    ASSO* tab_opcode = core->opcode;
    __uint8_t* instruction= core->instructions[core->pc];

    ASSO code;
    for(unsigned short k = 0; k<11;k++){
        __uint8_t opcode = (0xF0 & instruction[1]);
        if((opcode == tab_opcode[k].value) ){
            code = tab_opcode[k];
        }
    }
    if (code.nb_args == 2)
    {
        two_values(instruction,core->registers,&core->flags,code);
    }
    if (code.nb_args == 3)
    {
        three_values(instruction,core->registers,code);
    }
    core->pc++;
    
    
    //code.execute(instruction,core->registers,&core->flags);
}


void decode(coreData* core){
    __uint8_t instruction = *core->instructions[core->pc];
    
    if(is_BCC(instruction)){
        decode_BCC(core);
    }
    else {
        decode_OPCODE(core);
    }
}


void fetch(coreData core){

    // For each instruction
    

    printf("Traduction : \n");
    while(core.pc < core.nb_ins){
        printf("\n\ninstruction %02ld result: \n",core.pc);
        decode(&core);
        print_registres(core.registers);

    }
}

////////////////////////////////////////////////////////////////////////////////////////



/// INIT ZONE //////////////////////////////////////////////////////////////////////////

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
            printf("%ld\n",strtol(get_hex, NULL, 0));
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


ASSO* init_asso_opcode(){
    ASSO* opcodes = (ASSO*) malloc(11 * sizeof(ASSO));


    for (unsigned short i = 0; i < 11; i++){
        opcodes[i].value = 0x00 + (i*0x10);
        opcodes[i].nb_args = 3;
    }

    opcodes[0].name = "AND";
    opcodes[0].execute = fn_and;
    
    opcodes[1].name = "OR";
    opcodes[1].execute = fn_or;
    
    opcodes[2].name = "XOR";
    opcodes[2].execute = fn_xor;

    opcodes[3].name = "ADD";
    opcodes[3].execute = fn_add;
    
    opcodes[4].name = "ADC";


    opcodes[5].name = "CMP";
    opcodes[5].execute = fn_cmp;
    opcodes[5].nb_args = 2;


    opcodes[6].name = "SUB";
    opcodes[6].execute = fn_sub;

    opcodes[7].name = "SBC";

    opcodes[8].name = "MOV";
    opcodes[8].execute = fn_mov;
    opcodes[8].nb_args = 2;


    opcodes[9].name = "LSH";
    opcodes[9].execute = fn_lsh;

    opcodes[10].name = "RSH";
    opcodes[10].execute = fn_rsh;
   
    return opcodes;
}

ASSO* init_asso_bcc(){
    ASSO* bcc = (ASSO*) malloc(7*sizeof(ASSO));
    bcc[0].name = "B";
    bcc[0].nb_args = 64;

    bcc[1].name = "BEQ";
    bcc[1].nb_args = 32;

    bcc[2].name = "BNE";
    bcc[2].nb_args = 16;

    bcc[3].name = "BLE";
    bcc[3].nb_args = 8;

    bcc[4].name = "BGE";
    bcc[4].nb_args = 4;

    bcc[5].name = "BL";
    bcc[5].nb_args = 2;

    bcc[6].name = "BG";
    bcc[6].nb_args = 1;


    for (unsigned short i = 0; i < 7; i++){
        bcc[i].value = 0x80 + (i*0x10);
    }
    return bcc;
}

long* init_registers_bis(){
    long* registers = malloc(16*sizeof(long));
    for (long i = 0; i < 16; i++)
    {
        registers[i] = i;
    }
    return registers;
}
////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]){ 
        
    // verify that the right number of arguments were specifide, exits otherwise
    if(argc != 2){
        printf("Error : An executable file must be specified!\n");
        printf("Hint : ./core <filename>\n");
        return ERROR;
    }

    coreData core;
    core.registers = init_registers_bis();
    core.nb_ins = 0;
    core.instructions = init_ins(argv[1], &core.nb_ins);
    core.opcode = init_asso_opcode();
    core.bcc = init_asso_bcc();
    core.pc = 0;



    if(core.instructions == NULL){ //|| register == NULL){
        return ERROR;
    }


   fetch(core);

  
    free(core.registers);
    free_2d(core.instructions,core.nb_ins);
    core.instructions = NULL;
    core.registers = NULL;

    return SUCCESS;
    
}