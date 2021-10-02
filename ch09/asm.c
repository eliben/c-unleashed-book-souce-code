#if 0
/*******************************************************************************
 * File           :  asm.c
 * Description    :  Implements simulation of a computer
 *                   
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Instruction Set */

#define LOADA	   0X00
#define LOADB	   0X01
#define LOADAI    0X02
#define LOADBI    0X03
#define NOP	      0X04
#define STOREA    0X05
#define STOREB    0X06
#define ADDA	   0X07
#define ADDB	   0X08
#define COMPAI    0X09
#define COMPBI    0X0A
#define BLT	      0X0B
#define BGT 	   0X0C
#define JUMP	   0X0D
#define PRINTA    0X0E
#define BREAK     0x0F

#define MAX_LINE 1024

#define FALSE     0
#define TRUE      (!FALSE)

#define ADDRESS_LENGTH   2
#define IMMEDIATE_LENGTH 1

#define ADDRESS_SPACE    65536

typedef enum {NONE, IMMEDIATE, ADDRESS} OPERAND_TYPE;

typedef int BOOL;

typedef struct
{
   char*         InstructionName;
   OPERAND_TYPE  Operand;
   unsigned char MachineCode;
} INSTRUCTION;

static INSTRUCTION InstructionTable[] = {"LOADA",  ADDRESS,   LOADA, 
                                         "LOADB",  ADDRESS,   LOADB,	
                                         "LOADAI", IMMEDIATE, LOADAI,
                                         "LOADBI", IMMEDIATE, LOADBI,
                                         "NOP",    NONE,      NOP,	   
                                         "STOREA", ADDRESS,   STOREA,
                                         "STOREB", ADDRESS,   STOREB,
                                         "ADDA",   IMMEDIATE, ADDA,	
                                         "ADDB",   IMMEDIATE, ADDB,	
                                         "COMPAI", IMMEDIATE, COMPAI,
                                         "COMPBI", IMMEDIATE, COMPBI,
                                         "BLT",    ADDRESS,   BLT,  
                                         "BGT",    ADDRESS,   BGT, 	
                                         "JUMP",   ADDRESS,   JUMP,	
                                         "PRINTA", NONE,      PRINTA,
                                         "BREAK",  NONE,      BREAK };

/*******************************************************************************
 * Typedefs       :  Symbol Table
 * Description    :  Symbol table entries and symbol table (can be max of 
 *                   1024)
 ******************************************************************************/

typedef struct
{
	char* Symbol;
	int	MemLocation;
	int   Line;
} SYMTAB_ENTRY;

typedef struct
{
   SYMTAB_ENTRY Entry[1024];
   int          Count;
} SYM_T;


/*******************************************************************************
 * Function       :  InsertSymbol
 * Description    :  Inserts the Token into the symbol Table
 ******************************************************************************/

typedef struct
{
   unsigned int   RegA;
   unsigned int   RegB;
   unsigned int   PC;
            int   SR;
} REGISTERS;


/*******************************************************************************
 * Function       :  FindInstruction
 * Description    :  finds the instruction entry with the string Token
 ******************************************************************************/

INSTRUCTION* FindInstruction (char* Token)
{
   int            i = 0;
   INSTRUCTION*   Entry = NULL;

   for (i = 0; i < sizeof InstructionTable / sizeof InstructionTable[0]; i++)
   {
      if (0 == strcmp (InstructionTable[i].InstructionName, Token))
      {
         Entry = &InstructionTable[i];
      }
   }
   return Entry;
}

/*******************************************************************************
 * Function       :  InsertSymbol
 * Description    :  Inserts the Token into the symbol Table
 ******************************************************************************/

void InsertSymbol (char* Token, int MemLocation, SYM_T* SymTable)
{
   SYMTAB_ENTRY*  Entry;

   Entry = &SymTable->Entry[SymTable->Count];

   /* perform equivalent of strdup */
   Entry->Symbol = malloc (strlen(Token) + 1);
   if (NULL != Entry->Symbol)
   {
      strcpy (Entry->Symbol, Token);
   }

   SymTable->Count++;

   Entry->MemLocation = MemLocation;
}


/*******************************************************************************
 * Function       :  FindSymbol
 * Description    :  Searches symbol table for matching token
 ******************************************************************************/
SYMTAB_ENTRY* FindSymbol (SYM_T* SymbolTable, char* Symbol)
{
   int i = 0;
   
   SYMTAB_ENTRY*  Entry = NULL;

   for (i=0; i< SymbolTable->Count; i++)
   {
      if (0 == strcmp (SymbolTable->Entry[i].Symbol, Symbol))
      {
         Entry = &SymbolTable->Entry[i];
         break;
      }
   }
   return Entry;
}

/*******************************************************************************
 * Function       :  ReadAssembly
 * Description    :  Reads assembly listing and inserts it into memory
 ******************************************************************************/

void ReadAssembly (char* Memory, int MemLocation, char* FileName)
{
   FILE*    Fp             = NULL;
   char	   Line[MAX_LINE] = {0};
   int	   LineNo	      = 0;
   char*    Token          = NULL;
   char     Seps[]         = " ,\t\n";
   SYM_T    SymbolTable    = {0};

   BOOL     IsOk           = TRUE;

   INSTRUCTION*   Instruction = NULL;
   SYMTAB_ENTRY*  Symbol      = NULL;


   Fp = fopen ( FileName, "r" );

	if ( NULL == Fp )
	{
		fprintf ( stderr, "cannot open file %s\n", FileName );
		IsOk = FALSE;
	}

	/* go through each line */

   while (IsOk && NULL != fgets (Line, MAX_LINE, Fp ))
   {
      LineNo++;
      
      if ( '*' == Line[0] || '#' == Line[0] || '\n' == Line[0] )
      {
         /* discard comments */
         continue;
      }
      /* parse the line */
      Token = strtok (Line, Seps);
      
      if (NULL == Token)
      {
         continue;
      }
      
      /* check to see if this a label, if so then add to the symbol table */
      if (':' == Token[0])
      {
         Token++;
         InsertSymbol (Token, MemLocation, &SymbolTable);
         
      }
      else
      {
         /* assemble the token into memory */
         Instruction = FindInstruction (Token);

         if (NULL == Instruction)
         {
            printf ("Error at line %d; Do not recognise instruction %s\n",
                    LineNo, Token);
            IsOk = FALSE;
         }
         else
         {
            Memory [MemLocation] = Instruction->MachineCode;
            MemLocation++;
         
            if (IMMEDIATE == Instruction->Operand || 
               ADDRESS == Instruction->Operand)
            {
               /* Get the operand */
               Token = strtok (NULL, Seps);	
               if (NULL == Token)
               {
                  printf("Error\n");
                  break;
               }
            }
            
            /* We need to put one extra byte into memory */
            if ( IMMEDIATE == Instruction->Operand)
            {
               Memory [MemLocation] = (unsigned char) strtol (Token, NULL, 16);
               MemLocation++;
            }
            
            /* We need to put two extra bytes into memory */
            if ( ADDRESS == Instruction->Operand)
            {
               Symbol = FindSymbol (&SymbolTable, Token);

               /* make sure Symbol has already been defined */

               if (NULL == Symbol)
               {
                  printf ("Error at Line %d. Label %s not defined\n",
                           LineNo, Token);

                  IsOk = FALSE;
               }
               else
               {
                  /* the machine is big endian, so store the high byte first */
                  Memory [MemLocation] = (Symbol->MemLocation >> 8) & 0xFF;
                  MemLocation++;
                  Memory [MemLocation] = Symbol->MemLocation &0xFF;
                  MemLocation++;
               }
            }
         }
      }
   }

   if (NULL != Fp)
      fclose (Fp);
}

/*******************************************************************************
 * Function       :  LoadAddress
 * Description    :  Translates 2 bytes into one address
 ******************************************************************************/

unsigned int LoadAddress (unsigned char* Location)
{
   unsigned int Address;

   Address = Location[0] << 8;
   Address += Location[1];

   return Address;
}


/*******************************************************************************
 * Function       :  Execute
 * Description    :  Loads the next instruction and executes it.
 ******************************************************************************/

int Execute (unsigned char *Memory, REGISTERS* Reg)
{
   unsigned int   Address      = 0;
   unsigned int   Instruction  = 0;
   unsigned int   RegA         = Reg->RegA;
   unsigned int   RegB         = Reg->RegB;
           
   unsigned int   PC           = Reg->PC;
            int   SR           = Reg->SR;

   unsigned char* Operand     = NULL;

   int      Stop = FALSE;

  
   Instruction = Memory[PC];
   PC++;
   Operand     = &Memory[PC];

	/* Decode the instruction instruction */
   switch ( Instruction )
   {
   case LOADA:
      Address = LoadAddress(Operand);

      RegA = Memory[Address];
      PC += ADDRESS_LENGTH;
      break;

   case LOADB:
      Address = LoadAddress(Operand);

      RegB = Memory[Address];
      PC += ADDRESS_LENGTH;
      break;
      
   case LOADAI:
      RegA = *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case LOADBI:
      RegB = *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case NOP:
      break;
      
   case STOREA:
      Address = LoadAddress(Operand);
      Memory[Address] =  RegA;
      PC += ADDRESS_LENGTH;
      break;

   case STOREB:
      Address = LoadAddress(Operand);
      Memory[Address] =  RegB;
      PC += ADDRESS_LENGTH;
      break;

   case ADDA:
      RegA += *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case ADDB:
      RegB += *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case COMPAI:
      SR =  RegA - *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case COMPBI:
      SR =  RegB - *Operand;
      PC += IMMEDIATE_LENGTH;
      break;

   case BLT:   
      PC += ADDRESS_LENGTH;
      /* only branch if comparison less than is true */
      if (SR < 0)
      {
         Address = LoadAddress(Operand);
         PC = Address;
      }    
      break;

   case BGT:
      PC += ADDRESS_LENGTH;
      /* only branch if comparison greater than is true */
      if (SR > 0)
      {
         Address = LoadAddress(Operand);
         PC = Address;
      }
      break;

   case JUMP:
      Address = LoadAddress(Operand);
      PC = Address;
      break;

   case PRINTA:
      printf("%x\n", RegA);
      break;

   case BREAK:
      Stop = TRUE;
      break;

   default:
      /* didn't recognise the Instruction just decoded */
      Stop = TRUE;
   }

   Reg->RegA = RegA;
   Reg->RegB = RegB;
   Reg->SR   = SR;
   Reg->PC   = PC;

   return Stop;
}


/*******************************************************************************
 * Function       :  Run
 * Description    :  Runs the machine code at given location, till 
 *                   stop encountered, or error
 ******************************************************************************/

void Run (REGISTERS* Registers, unsigned char* Memory, int Start)
{
   printf ("Beginning execution at address %04X\n", Start);

   Registers->PC = Start;

   while (Execute (Memory, Registers) == FALSE)
      ;

   printf ("Break encountered at address %04X\n", Registers->PC);
}

/*******************************************************************************
 * Function       :  Step
 * Description    :  Executes one machine code instruction
 ******************************************************************************/
void Step (REGISTERS* Registers, unsigned char* Memory)
{
   if (Execute (Memory, Registers) == FALSE)
      printf ("Break encountered at address %04X\n", Registers->PC);
}


/*******************************************************************************
 * Function       :  SetStep
 * Description    :  Sets execution address
 *                   and executes one machine code instruction
 ******************************************************************************/
void SetStep (REGISTERS* Registers, unsigned char* Memory, int Start)
{
   Registers->PC = Start;
   if (Execute (Memory, Registers) == FALSE)
      printf ("Break encountered at address %04X\n", Registers->PC);
}


/*******************************************************************************
 * Function       :  DumpRegisters
 * Description    :  
 ******************************************************************************/

void DumpRegisters (REGISTERS* Registers)
{
   printf (" Reg A  : %02X\n", Registers->RegA);
   printf (" Reg B  : %02X\n", Registers->RegB);
   printf (" PC     : %02X\n", Registers->PC);
   printf (" SR     : %02X\n", Registers->SR);
}


/*******************************************************************************
 * Function       :  DumpMemory
 * Description    :  
 ******************************************************************************/

void DumpMemory (unsigned char* Memory, int start, int end)
{
   int i = 0;
   int j = 0;


   for (i=start; i < end; i+= 8)
   {
      printf ("%04X     ", i);

      for (j = i; j < i+8; j++)
      {
         printf ("%02X  ", Memory[j]);
      }
      printf("\n");
   }
}

/*******************************************************************************
 * Function       :  Disassemble
 * Description    :  Disassembles from start address to end address
 ******************************************************************************/

void Disassemble (int Start, int End, unsigned char* Memory)
{
   
   int   addr  = 0;

   unsigned char InsCode;

   INSTRUCTION*   Instruction = NULL;


   for (addr = Start; addr < End; addr++)
   {
      InsCode = Memory[addr];

      /* print the address of the instruction */

      printf("%04X %02X ", addr, InsCode);

      /* make sure the instruction code is within bounds */

      if (InsCode < sizeof InstructionTable / sizeof InstructionTable[0])
      {
         Instruction = &InstructionTable[InsCode]; 

         switch(Instruction->Operand)
         {
         case NONE:
            printf("        %-8s\n", Instruction->InstructionName);
            break;
            
         case IMMEDIATE:
            addr++;
            printf("%02X      %-8s #%02X\n", Memory[addr], 
               Instruction->InstructionName, 
               Memory[addr]);
            break;
            
         case ADDRESS:
            addr++;
            printf("%02X %02X   %-8s %04X\n", Memory[addr],
               Memory[addr+1],
               Instruction->InstructionName,
               LoadAddress(&Memory[addr]) );
            addr++;
            break;
         }
      }
      else
      {
         printf("        ???\n");
      }
   }
}

/*******************************************************************************
 * Function       :  HelpMenu
 * Description    :  
 ******************************************************************************/                  

void HelpMenu()
{
   printf ("h or help                        this help menu\n");
   printf ("load <filename> <memory_addr>    assemble file starting at address\n");
   printf ("go <memory addr>                 run program starting at address\n");
   printf ("s <memory addr>                  step one instruction, starting at address\n");
   printf ("s                                step one instruction, from current PC\n");
   printf ("d <start> <end>                  disassemble memory from start to end\n");
   printf ("r                                print contents of registers\n");
   printf ("m <start> <end>                  print memory from start to end\n");
   printf ("q or quit                        exits\n");
}


/*******************************************************************************
 * Function       :  Main
 * Description    :  
 ******************************************************************************/                  

void main ( int argc, char **argv)
{
   int   Exit     = FALSE;
   char  Seps[]   = " ,\t\n";
   char* Token    = NULL;

   char	Line1[MAX_LINE] = {0};
   char	Line2[MAX_LINE] = {0};

   char* LoadLocation = NULL;
   char* FileName     = NULL;
   char* Start        = NULL;
   char* End          = NULL;

   REGISTERS   Registers = {0};
   char        Memory[ADDRESS_SPACE] = {0};


   HelpMenu();

   /* loop until user asks to quit */
   while (!Exit)
   {
	   printf (">");
	   fgets (Line1, MAX_LINE, stdin);

      Token = strtok (Line1, Seps);

      /* parse the command line */
      if (NULL == Token)
      {
         continue;
      }

      if (0 == strcmp (Token, "load"))
      {
         /* see if there are any arguments specified */
         FileName = strtok (NULL, Seps);

         while (NULL == FileName)
         {
            printf ("File name :");
            fgets (Line1, MAX_LINE, stdin);
            FileName = strtok (Line1, Seps);
         }

         LoadLocation = strtok (NULL, Seps);

         while (NULL == LoadLocation)
         {
            printf ("Load to Memory Address :");
            fgets (Line2, MAX_LINE, stdin);
            LoadLocation = strtok (Line2, Seps);
         }

         ReadAssembly (Memory, strtol(LoadLocation, NULL, 16), FileName);
      }
      else if (0 == strcmp (Token, "go"))
      {
         /* see if there are any arguments specified */
         Token = strtok (NULL, Seps);

         while (NULL == Token)
         {
            printf ("address :");
            fgets (Line1, MAX_LINE, stdin);
            Token = strtok (Line1, Seps);
         }
         Run (&Registers, Memory, strtol(Token, NULL, 16));
      }
      else if (0 == strcmp (Token, "s"))
      {
         /* Step option specified */
         /* see if there are any arguments specified */
         Token = strtok (NULL, Seps);

         if (NULL != Token)
         {
            SetStep (&Registers, Memory, strtol(Token, NULL, 16));
         }
         else
         {
            Step (&Registers, Memory);
         }
      }
      else if (0 == strcmp (Token, "d"))
      {
         /* disassemble option specified */
         Start = strtok (NULL, Seps);

         while (NULL == Start)
         {
            printf ("Start Address :");
            fgets (Line1, MAX_LINE, stdin);
            Start = strtok (Line1, Seps);
         }

         End = strtok (NULL, Seps);
         while (NULL == End)
         {
            printf ("End Address :");
            fgets (Line2, MAX_LINE, stdin);
            End = strtok (Line2, Seps);
         }
         Disassemble (strtol(Start, NULL, 16), strtol(End, NULL, 16), Memory);
      }
      else if (0 == strcmp (Token, "r"))
      {
         DumpRegisters (&Registers);
      }
      else if (0 == strcmp (Token, "m"))
      {
         /* see if there are any arguments specified */
         Start = strtok (NULL, Seps);

         while (NULL == Start)
         {
            printf ("Start Address :");
            fgets (Line1, MAX_LINE, stdin);
            Start = strtok (Line1, Seps);
         }

         End = strtok (NULL, Seps);

         while (NULL == End)
         {
            printf ("End Address :");
            fgets (Line2, MAX_LINE, stdin);
            End = strtok (Line2, Seps);
         }

         DumpMemory (Memory, strtol(Start, NULL, 16), strtol(End, NULL, 16));
      }
      else if (0 == strcmp (Token, "quit") || 0 == strcmp (Token, "q"))
      {
         Exit = TRUE;
      }
      else if (0 == strcmp (Token, "help") || 0 == strcmp (Token, "h"))
      {
         HelpMenu();
      }
      else
      {
         printf ("Unknown command\n");
      }   
   }
}

#endif