
#  .segment it_asm_code,memtype=0,abs=0x1100,locinc=4,wordsize=32
#  .data

.set noat
.global _IRQ_Routine
.extern _SWI_RET
.extern __it_handle

.text

 _IRQ_Routine: 
       # Marcio: set supervisor mode here!!!!

       # now push save the context
       sw $1, 0-4($29)
       sw $2, 0-8($29)
       sw $3, 0-12($29)
       sw $4, 0-16($29)
       sw $5, 0-20($29)
       sw $6, 0-24($29)
       sw $7, 0-28($29)
       sw $8, 0-32($29)
       sw $9, 0-36($29)
       sw $10, 0-40($29)
       sw $11, 0-44($29)
       sw $12, 0-48($29)
       sw $13, 0-52($29)
       sw $14, 0-56($29)
       sw $15, 0-60($29)
       sw $16, 0-64($29)
       sw $17, 0-68($29)
       sw $18, 0-72($29)
       sw $19, 0-76($29)
       sw $20, 0-80($29)
       sw $21, 0-84($29)
       sw $22, 0-88($29)
       sw $23, 0-92($29)
       sw $24, 0-96($29)
       sw $25, 0-100($29)
       sw $29, 0-104($29)
       sw $30, 0-108($29)
       sw $31,0-112($29)
       addi $29, $29, -112

       # when using PAP INTM (current version - OCT 2003) there is only one virtual IT (number 2)
       #       LDR     r0,=IRQ_NUMBER          ; Get IRQ number
       #       LDR     r0,[r0]
       addi    $4, $0, 0x2         # in XiRISC the register $4..$7 are used to parameter passing
       jal     __it_handle         # Treat IT
       add $0, $0, $0

      # put in R31 the address to return from the exception


       add $26, $0, $29

       lw $31, 0($26)
       lw $30, 4($26)
       lw $29, 8($26)
       lw $25, 12($26)
       lw $24, 16($26)
       lw $23, 20($26)
       lw $22, 24($26)
       lw $21, 28($26)
       lw $20, 32($26)
       lw $19, 36($26)
       lw $18, 40($26)
       lw $17, 44($26)
       lw $16, 48($26)
       lw $15, 52($26)
       lw $14, 56($26)
       lw $13, 60($26)
       lw $12, 64($26)
       lw $11, 68($26)
       lw $10, 72($26)
       lw $9,  76($26)
       lw $8,  80($26)
       lw $7,  84($26)
       lw $6,  88($26)
       lw $5,  92($26)
       lw $4,  96($26)
       lw $3,  100($26)
       lw $2,  104($26)
       lw $1,  108($26)

       # Marcio: change it, when EPC is avaliable in the new simulator version
       jr $27                       # Return from SVC
       add $0, $0, $0

       #addi $27, $0, 1             # Enable interrupts
