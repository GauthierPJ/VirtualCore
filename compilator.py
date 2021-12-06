#!/usr/bin/python3

from sys import argv
from re import match


###############################################################################
#                                  VERIFYING                                 #
##############################################################################

def is_register(op) : 
    return match("^R[0-9]$", op) or match("^R1[0-5]$", op)


def verif_standard(operand_list) : 
    try : 
       dest = operand_list[0] 
       op1  = operand_list[1] 
       op2  = operand_list[2] 
       return  (len(operand_list) == 3 ) and is_register(dest) and is_register(op1) and (is_register(op2) or (op2.isdigit() and int(op2) < 256 and int(op2) >= 0))
    except IndexError:
        return False
    

def verif_branchement(operand_list) : 
    try:
        offset = operand_list[0]
        #Remove the "-" for the isdigit function*
        # 134217727
        return (len(operand_list) == 1 ) and (offset.replace("-","").isdigit() and int(offset.replace("-","")) < 134217727)
    except IndexError:
        return False
    

def verif_cmp(operand_list) : 
    try: 
       op1 = operand_list[0] 
       op2 = operand_list[1] 
       return (len(operand_list) == 2 ) and is_register(op1) and (is_register(op2) or (op2.isdigit() and int(op2) < 256 and int(op2) >= 0))
    except IndexError :
        return False
    
# Content : List of String (instructions asm)
def verify_content(content, test=False) : 
    for ins in content : 
        # Get op code, which is the first instuction's part
        opcode = ins.split(" ")[0]
        # If the opcode exists 
        if opcode in dic_opcode :
            # We verify he hasn't the good format with the rest of the instruction strings
            if(not dic_opcode[opcode]["format"](ins.split(" ")[1:])) :
                print(f"Error : the instruction '{ins}' is not correct. ")
                if(not test) :
                    return (None,False)
        else :
            print(f"Error : the instruction '{ins}' is not correct. ")
            if(not test) :
                return (None,False)
    return (content,True)
    
    
###############################################################################
#                                  PRINT                                     #
##############################################################################
    
def print_ins_b(ins) : 
    print("\n#---------------------------------------#")
    print("Always 0       [31-25] : " + ' '.join(ins[0:7]))
    print("Immediate value ? [24] : " + ' '.join(ins[7]))
    print("OPCODE :       [23-20] : " + ' '.join(ins[8:12]))
    print("OP 1           [19-16] : " + ' '.join(ins[12:16]))
    print("OP 2           [15-12] : " + ' '.join(ins[16:20]))
    print("Dest            [11-8] : " + ' '.join(ins[20:24]))
    print("Immediate value  [7-0] : " + ' '.join(ins[24:32]))    
    
    
def print_ins_b_branchement(ins) : 
    print("\n#---------------------------------------#")
    print("BCC            [31-28] : " + ' '.join(ins[0:4]))
    print("Signe             [27] : " + ' '.join(ins[4]))
    print("Program Counter [26-0] : " + ' '.join(ins[5:32]))
    
    
    

    
###############################################################################
#                                  FILES                                     #
##############################################################################    

def write_bin_to_file(bits_string, f) :     
    
    i = 0
    buffer = bytearray()  
    
    #De base = 00000000 00000000 00001010 00001000
    
    # Généré = 80 00 00 0A
    # 10000000 00000000 00000000 00001010
    
    while(i < 32) :
        buffer.append(int(bits_string[i:i+8], 2))
        i += 8
    f.write(buffer)
    
        
        
# Filename : String
def read_file(filename) : 
    with open(filename,"r") as f : 
        content = f.readlines()
        # Remove the side whitespaces and the newline char
        for i in range(len(content)) : 
            content[i] = content[i].replace("\n", "").strip()
        # Remove empty elements
        content[:] = (value for value in content if value != '')
        return content        
        
###############################################################################
#                                  TRANSLATION                               #
##############################################################################  
            
        
def translate_ins_standard(op_list) :
    #Init 
    res = ["0" for i in range(32)]
    opcode = op_list[0]
    dest   = op_list[1]
    op1    = op_list[2]
    op2    = op_list[3]
    
    #If op2 is a register
    if(is_register(op2)) : 
        res[16:20] = dic_register[op2]
    else : 
        res[24:32] = '{0:08b}'.format(int(op2))
        res[7] = "1"
    res[20:24] = dic_register[dest]
    res[12:16] = dic_register[op1]
    res[8:12] = dic_opcode[opcode]["code"]
    
   
    return ''.join(res)    

def translate_ins_branchement(op_list) :
    #Init 
    res = ["0" for i in range(32)]
    opcode = op_list[0]
    offset = op_list[1]
    if(offset[0] == "-") : 
        res[4] = "1"
    res[5:32] = '{0:027b}'.format(int(offset.replace("-","")))
    res[0:4] = dic_opcode[opcode]["code"]
    return ''.join(res)    


def translate_ins_cmp(op_list) :
    #Init 
    res = ["0" for i in range(32)]
    opcode = op_list[0]
    op1    = op_list[1]
    op2    = op_list[2]
    
    #If op2 is a register
    if(is_register(op2)) : 
        res[16:20] = dic_register[op2]
    else : 
        res[24:32] = '{0:08b}'.format(int(op2))
        res[7] = "1"
    
    if(opcode == "MOV") : # IF MOV, store op1 in dest register
        res[20:24] = dic_register[op1]
    else : # IF CMP, store in op1 register
        res[12:16] = dic_register[op1]
        
    res[8:12] = dic_opcode[opcode]["code"]
   
    return ''.join(res)  
    
    
###############################################################################
#                                  DICS                                      #
##############################################################################  

dic_register = {
    
    "R0" : '{0:04b}'.format(0),
    "R1" : '{0:04b}'.format(1),
    "R2" : '{0:04b}'.format(2),
    "R3" : '{0:04b}'.format(3),
    "R4" : '{0:04b}'.format(4),
    "R5" : '{0:04b}'.format(5),
    "R6" : '{0:04b}'.format(6),
    "R7" : '{0:04b}'.format(7),
    "R8" : '{0:04b}'.format(8),
    "R9" : '{0:04b}'.format(9),
    "R10" : '{0:04b}'.format(10),
    "R11" : '{0:04b}'.format(11),
    "R12" : '{0:04b}'.format(12),
    "R13" : '{0:04b}'.format(13),
    "R14" : '{0:04b}'.format(14),
    "R15" : '{0:04b}'.format(15)
}   

dic_opcode = {
    "AND":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "0000"
        
    },
    "ORR":{
        "format" : verif_standard,
        "execute": translate_ins_standard,
        "code"   : "0001"
    },
    "EOR":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "0010"
    },
    "ADD":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "0011"
    },
    "ADC":{
        "format": verif_standard, # A CHANGER : carry
        "execute": translate_ins_standard,
        "code" : "0100"
    },
    "CMP":{
        "format": verif_cmp,
        "execute": translate_ins_cmp,
        "code" : "0101"
    },
    "SUB":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "0110"
    },
    "SBC":{
        "format": verif_standard, # A CHANGER : carry
        "execute": translate_ins_standard,
        "code" : "0111"
    },
    "MOV":{
        "format": verif_cmp, 
        "execute": translate_ins_cmp,
        "code" : "1000"
    },
    "LSH":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "1001"
    },
    "RSH":{
        "format": verif_standard,
        "execute": translate_ins_standard,
        "code" : "1010"
    },
    "B":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1000"
    },
    "BEQ":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1001"
    },
    "BNE":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1010"
    },
    "BLE":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1011"
    },
    "BGE":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1100"
    },
    "BL":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1101"
    },
    "BG":{
        "format": verif_branchement,
        "execute": translate_ins_branchement,
        "code" : "1110"
    }
}               
    
###############################################################################
#                                  MAIN                                      #
##############################################################################  

def main() : 
    if(len(argv) != 2) : 
        print("Error : You must specify at least .s file to read!.")
        print("Hint : ./compilator.py <filename>")
        exit(-1)
    filename = argv[1]
    if(filename.split(".")[1] != "s") : 
        print("Error : You must specify a .s file!")
        print("Hint : ./compilator.py <filename>")
        exit(-1)
    # Tests if the file containts some errors
    instructions_list,no_error =  verify_content(read_file(filename))
    if no_error : 
        print("OK1")
        binary_file = open("code.bin", "wb+")
        for ins in instructions_list : 
            opcode = ins.split(" ")[0] 
            ins_b = dic_opcode[opcode]["execute"](ins.split(" ")[:])
            write_bin_to_file(ins_b,binary_file)
            #print_ins_b_branchement(ins_b)
            #print_ins_b(ins_b)
        print("OK2")
        binary_file.close()
    else : 
        print("KO")   


    

if __name__ == '__main__': main()
