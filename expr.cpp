/******************************************************************************
      Expression Resolver For C programming language v1.6 
        Designed by Daipayan Bhowal
       Using Priority linked list to solve expression evaluation
*******************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define OPERAND 255
#define OPERATOR -255
//#define INT 33
#define INIT_PRIORITY -1


enum Ty
{
    INT,
    FLOAT,
    CHAR,
    STRING,
    FCONST,
    I_ARRAY,
    F_ARRAY,
    CONST = 256
};

enum Priority
{
    ZERO_OP=129,
    OR_EQ_OP, /* 130 */
    XOR_EQ_OP, /* 131 */
    AMP_EQ_OP, /* 132 */
    RSHR_EQ_OP, /* 133 */
    LSHR_EQ_OP, /* 134 */
    MOD_EQ_OP, /* 135 */
    DIV_EQ_OP, /* 136 */
    MUL_EQ_OP, /* 137 */
    SUB_EQ_OP, /* 138 */
    ADD_EQ_OP, /* 139 */
    MOV_OP, /* 140 */
    OR_OR_OP, /* 141 */
    AND_AND_OP, /* 142 */
    OR_OP,    /* 143 */
    XOR_OP,  /* 144 */
    AND_OP,  /* 145 */
    NOT_EQ_OP, /* 146 */
    EQ_EQ_OP, /* 147 */
    LSR_EQ_OP, /* 148 */
    LSR_OP,  /* 149 */
    GTR_EQ_OP,  /* 150 */
    GTR_OP, /* 151 */
    RSHR_OP, /* 152 */
    LSHR_OP, /* 153 */
    SUB_OP,  /* 155 */
    ADD_OP,  /* 156 */
	MOD_OP,  /* 154 */
    DIV_OP,  /* 157 */
    MUL_OP,  /* 158 */
    DEREF_OP,
    ADDR_OP,
    BNOT_OP,
    NOT_OP,
    SUB_SUB_OP,
    ADD_ADD_OP,
    LAST_OP
};
enum Operation
{
    NULL_OP,
    UNARY_OP,
    BINARY_OP,
    TERNARY_OP
};


enum Registers
{
    ax,
    bx,
    cx,
    dx,
    eax,
    ebx,
    LAST_REG
};
char *register_str[7] = {(char*)"ax",(char*)"bx",(char*)"cx",(char*)"dx", (char*)"eax", (char*)"ebx", (char*)"ecx"};
struct Symbol
{
   char *symbol;
   char type;
   char *value;
   enum Registers r;
   char qual;
};
struct Symbol symtbl[256];
int max_sym=0;
static int reg_count = 0;
void Symbol_insert(char *sym,char typ,char *val,enum Registers rtemp)
{
    symtbl[max_sym].symbol =(char*) calloc(strlen(sym)+1, sizeof(char));
    strncpy(symtbl[max_sym].symbol,sym,strlen(sym));
    symtbl[max_sym].type = typ;
    symtbl[max_sym].value = (char*) calloc(strlen(val)+1, sizeof(char));
    strncpy(symtbl[max_sym].value,val, strlen(sym));
    symtbl[max_sym].r = rtemp;

   if(reg_count >= 6)
      reg_count = reg_count%5;
    
    reg_count++;

    max_sym++;
}
int Symbol_search(char *sym)
{
    for(int i=0; i<max_sym; i++)
    {
        if(strcmp(sym,symtbl[i].symbol)==0)
        {
            
            return i;
        }
    }
   return -1;
}
struct ListNode
{
    struct ListNode* prev;
    int isOperatorOrOperand;
    int priority;
    struct Symbol *str;
    int op;
    int derefLevel;
    enum Operation opType; /* 1 for unary, 2 for binary, for ternary we don't need this */
    enum Ty type;
    struct ListNode* next;
};
struct ListNode *tail,*head ; // Priority Queue
int size =0;

void DecodeNodeOp(struct ListNode *ptr) // used for debugging purpose only, not a functionality
{
   if(ptr == NULL)
   {
      printf("ptr is NULL\n");
      return;
   }
  if(ptr->isOperatorOrOperand == OPERATOR)
  {
 
     switch(ptr->op)
     {
       case OR_EQ_OP:
       printf("OR_EQ_OP\n");
       break;
       case XOR_EQ_OP:
       printf("XOR_EQ_OP\n");
       break;
       case AMP_EQ_OP:
       printf("AMP_EQ_OP\n");
       break;
       case RSHR_EQ_OP:
       printf("RSHR_EQ_OP\n");
       break;
       case LSHR_EQ_OP:
       printf("LSHR_EQ_OP\n");
       break;
       case MOD_EQ_OP:
       printf("MOD_EQ_OP\n");
       break;
       case DIV_EQ_OP:
       printf("DIV_EQ_OP\n");
       break;
       case MUL_EQ_OP:
       printf("MUL_EQ_OP\n");
       break; 
       case SUB_EQ_OP:
       printf("SUB_EQ_OP\n");
       break;
       case ADD_EQ_OP: 
       printf("ADD_EQ_OP\n");
       break;
       case MOV_OP: 
       printf("MOV_EQ_OP\n");
       break;

       case OR_OR_OP: 
       printf("OR_OR_OP\n");
       break;
       case AND_AND_OP: 
       printf("AND_AND_OP\n");
       break;
       case OR_OP: 
       printf("OR_OP\n");
       break;
       case XOR_OP: 
       printf("XOR_OP\n");
       break;
       case AND_OP: 
       printf("AND_OP\n");
       break;
       case NOT_EQ_OP: 
       printf("NOT_EQ_OP\n");
       break;
       case EQ_EQ_OP: 
       printf("EQ_EQ_OP\n");
       break;
       case LSR_EQ_OP:
       printf("LSR_EQ_OP\n");
       break;
       case LSR_OP:
       printf("LSR_OP\n");
       break;
       case GTR_EQ_OP:
       printf("GTR_EQ_OP\n");
       break;
       case GTR_OP:
       printf("GTR_OP\n");
       break;
       case RSHR_OP:
       printf("RSHR_OP\n");
       break;
       case LSHR_OP:
       printf("LSHR_OP\n");
       break;
       case MOD_OP:
       printf("MOD_OP\n");
       break;
       case SUB_OP:
       printf("SUB_OP\n");
       break;
       case ADD_OP:
       printf("ADD_OP\n");
       break;
       case DIV_OP:
       printf("DIV_OP\n");
       break;
       case MUL_OP:
       printf("MUL_OP\n");
       break;
       case DEREF_OP:
       printf("DEREF_OP\n");
       break;
       case ADDR_OP:
       printf("ADDR_OP\n");
       break; 
       case BNOT_OP:
       printf("BNOT_OP\n");
       break;
       case NOT_OP:
       printf("NOT_OP\n");
       break;
       case SUB_SUB_OP:
       printf("SUB_SUB_OP\n");
       break;
       case ADD_ADD_OP:
       printf("ADD_ADD_OP\n");
       break;
       default:
       printf("\nOperator is %d\n", ptr->op);
       break; 
     }

     switch(ptr->opType)
     {
      
       case NULL_OP:
        printf("NULL_OP\n");
       break;
       case UNARY_OP:
        printf("UNARY_OP\n");
       break;
       case BINARY_OP:
        printf("BINARY_OP\n");
       break;
       case TERNARY_OP:
        printf("TERNARY_OP\n");
       break;
       default:
        printf("\nOperator Type is %d\n", ptr->opType);
       break;
     }
  }
  else if(ptr->isOperatorOrOperand == OPERAND)
  {
      printf("Symbol is %s priority is %d\n", ptr->str->value, ptr->priority); 
  }
}
void erasell() /* erase the last 1 or 2 elements left in the priority linked list after calculation or travesal */
{
   struct ListNode * ptr = head;
   struct ListNode * temp = NULL;
   while(ptr != NULL)
   {  
      temp = ptr;
      ptr = ptr->next;    
      printf("memory is freed!\n"); 
      free(temp);
      printf("memory is freed!\n"); 
      temp = NULL; 
   }
   head = NULL;
   tail = NULL;
   return;
}

void printll() /* used for debugging purpose not a functionality */
{
    struct ListNode *ptr = head;

    while(ptr != NULL)
    {
        if(ptr->isOperatorOrOperand == OPERAND)
        {
             printf("Symbol is %s priority is %d\n", ptr->str->value, ptr->priority);
        }
        if(ptr->isOperatorOrOperand == OPERATOR)
        {
             printf("Symbol is %c priority is %d\n", ptr->op, ptr->priority);
        }
        ptr = ptr->next;
    }
    
    printf("\n");
}

struct ListNode* maximum()
{
    int i=size - 1;
    int max = -1;
    struct ListNode* ptr = head;
    struct ListNode* max_ptr= NULL;
    //printll();
    while(ptr != NULL)
    {
        //DecodeNodeOp(ptr);
        if(max < ptr->priority && ptr->isOperatorOrOperand == OPERATOR)
        {  max = ptr->priority;
           max_ptr = ptr;
          // printf("maximum value hit ! %c %d\n", ptr->op, ptr->priority);
        }
        
        ptr = ptr->next;
    }
   // printf("\noperator is %c\n", max_ptr->op);
    return max_ptr;
}

void fill(int isType,int pr, struct Symbol *s, int op,enum Operation opType,int derefLevel)
{
  struct ListNode *ptr = (struct ListNode*)malloc(sizeof(struct ListNode));
  ptr->isOperatorOrOperand = isType;
  ptr->priority = pr;
  ptr->next = NULL;
  ptr->prev = NULL;
  if(isType ==  OPERAND)
  {
    ptr->str = s;
    ptr->type = INT;
    ptr->isOperatorOrOperand = OPERAND;
    ptr->derefLevel = derefLevel;
  }
  else if(isType == OPERATOR)
  {  ptr->op = op;
     ptr->opType = opType;
     ptr->isOperatorOrOperand = OPERATOR;
  }
  else if(isType == CONST)
  {
      ptr->str = s;
      ptr->type = CONST;
      ptr->isOperatorOrOperand = CONST;
  }

  if(tail == NULL)
  {
      tail = ptr;
      head = ptr;
  }
  else
  {
      tail->next =ptr;
      ptr->prev = tail;
      tail = tail->next;
  }
  size++;
}

void expr_resolver()
{  int sum;
    char buffer[64];
    struct ListNode* new_ptr;
    
   // char *reg[] = { "r0" ,"r1" ,"r2" , "r3" , "r4"}; // for register/temp variable insertion
   //    static int rcount =0;
    do
    {
        
        struct ListNode* p = maximum();
         if(p== NULL)
         {    
           //   printf("operation completed\n"); 
              return;
         }
        struct ListNode* left_op = p->prev;
        struct ListNode* right_op = p->next;
       
        //printf("DecodeNode Op is \n"); 
        //DecodeNodeOp(p);
        // start the reduce functionality
        if(left_op == NULL && right_op == NULL)
         return ;
        if((left_op == NULL && right_op != NULL) || (left_op != NULL && right_op == NULL))
        {
           //printf("Expression string is for pure unary op 1 !\n");
           if(left_op == NULL && right_op != NULL && p->opType==UNARY_OP)
           {
               switch(p->op)
               {
                   case ADD_ADD_OP:
                   {  printf("++ op with one operand left\n");
                      printf("add %s,1\n", register_str[right_op->str->r]);
                   }
                   break;
                   case SUB_SUB_OP:
                      printf("sub %s,1\n", register_str[right_op->str->r]);
                   break;
                   case MUL_OP:
                      // do nothing
                   break;
                   case SUB_OP:
                    // do nothing, legal statement but no opeartion
                   break;
                   case NOT_OP:
                     printf("not %s\n", register_str[right_op->str->r]);
                   break;
                   case ADDR_OP:
                     // do nothing, legal statement but no opeartion
                   break;

               }
                free(right_op);
                right_op = NULL;
           }
           else if(left_op != NULL && right_op == NULL && p->opType==UNARY_OP)
           {
               printf("Expression string is for pure unary op 2 !\n");
               switch(p->op)
               {
                   case ADD_ADD_OP:
                     printf("add %s,1\n", register_str[left_op->str->r]);
                   break;
                   case SUB_SUB_OP:
                     printf("sub %s,1\n", register_str[left_op->str->r]);
                   break;
                   case MUL_OP:
                   case SUB_OP:
                   case NOT_OP:
                   case AND_OP:
                   {
                       printf("Compilation error!");
                       exit(0);
                   }
                   break;

               }
             /*  free(left_op); free is not required as left_op is being deleted by erase_ll, left_op is considered to be residual
               left_op = NULL; */
           }

           return; // terminate as only 2 element one operator & one operand present
        }
        else if(left_op == NULL && right_op == NULL)
        {
            printf("Compilation error!");
            exit(0);
        }
      //  printf("p->op is %c %s %s!\n", p->op, left_op->str->value, right_op->str->value);
      if(p->opType == UNARY_OP)
      {
      
        if(left_op->isOperatorOrOperand == OPERAND) // post increment
        {
           // printf("left op is Operand\n");
            switch(p->op)
            {
              case ADD_ADD_OP:
                printf("add %s,1\n", register_str[left_op->str->r]);
              break;
              case SUB_SUB_OP:
                printf("sub %s,1\n", register_str[left_op->str->r]);
              break;
            }
            //if(right_op == NULL)
        //  printf("\nleft_op %s right_op %d left_op_prev %d", left_op->str->symbol,right_op->op, left_op->prev->op);
          left_op->next = right_op;
          if(right_op != NULL)
            right_op->prev = left_op;
            
          if(left_op->prev != NULL && right_op != NULL)
          {
              if(left_op->prev->priority > right_op->priority)
              {
                  left_op->priority = left_op->prev->priority;
              }
              else
              {
                  left_op->priority = right_op->priority;
              }
              
          }
          else if(left_op->prev != NULL)
          {
              left_op->priority = left_op->prev->priority;
          }
          else if(right_op != NULL)
          {
              left_op->priority = right_op->priority;
          }
           // printf("\nleft_op pr %d left_op->next %d\n", left_op->priority, left_op->next->op );
            free(p);
            p=NULL;
            continue;
        }
        else if(right_op->isOperatorOrOperand == OPERAND) // pre increment
        {
              printf("right op is Operand\n");
            switch(p->op)
            {
              case ADD_ADD_OP:
                printf("add %s,1", register_str[right_op->str->r]);
              break;
              case SUB_SUB_OP:
                printf("sub %s,1", register_str[right_op->str->r]);
              break;
              case SUB_OP:
               printf("neg %s", register_str[right_op->str->r]);
              break;
              case BNOT_OP:  // Binary not
               printf("not %s", register_str[right_op->str->r]);
              break;
              case NOT_OP:
               printf("xor %s", register_str[right_op->str->r]);
              break;
              case ADDR_OP:
               printf("mov %s,[%s]" , register_str[right_op->str->r],right_op->str->symbol);
              break;
              case DEREF_OP:
              {
                 int i=0;
                 while(i < right_op->derefLevel)
                   printf("lea %s, [%s] ", register_str[right_op->str->r], register_str[right_op->str->r] ); 	
              	
	      }	
              break;	
            }
          //  printf("left_op %s right_op %s\n", left_op->str->symbol,right_op->str->symbol);
            right_op->prev = p->prev;
            if(p->prev != NULL)
            p->prev->next = right_op;
            
            if(right_op->next != NULL && p->prev != NULL)
            {
                if(p->prev->priority > right_op->next->priority)
                {
                    right_op->priority = p->prev->priority;
                }
                else
                {
                    right_op->priority = right_op->next->priority;
                }
            }
            else if(right_op->next != NULL)
            {
                right_op->priority = right_op->next->priority;
            }
            else if(p->prev != NULL)
            {
                right_op->priority = p->prev->priority;
            }
            free(p);
            p=NULL;
            continue;
        }
        else
        {
            printf("lvalue is required for post/pre-suffix operation\n");
        }
      }
      else if(p->opType == BINARY_OP)
      {
        switch(p->op)
        {
            case LSR_OP:
            {   printf("cmp %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r]);
                printf("jle\n");
            }
            break;
            case GTR_OP:
            {
               printf("cmp %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r]);
               printf("jg\n");
            }
            break;
            case LSHR_OP:
              printf("shl %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r]);
            break;
            case RSHR_OP:
              printf("shr %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r]);
            break;
            case SUB_OP:
               printf("sub %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case SUB_EQ_OP:
               printf("sub %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case ADD_OP:
               printf("add %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case ADD_EQ_OP:
               printf("add %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case XOR_OP:
               printf("xor %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case XOR_EQ_OP:
               printf("xor %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case OR_OP:
               printf("or %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case OR_OR_OP:
               printf("cmp %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case AND_AND_OP:
               printf("cmp %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case DIV_OP:
               printf("div %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case DIV_EQ_OP:
               printf("div %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case MUL_OP:
               printf("mul %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case MUL_EQ_OP:
               printf("mul %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case MOV_OP:
               printf("mov %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r]);
            break;
            case AND_OP:
               printf("and %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case AMP_EQ_OP:
               printf("and %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            case NOT_EQ_OP:
            {
				
               printf("cmp %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
               printf("jne\n");
            }
            break;
            
            case MOD_OP:
               printf("mod %s,%s\n", register_str[left_op->str->r], register_str[right_op->str->r] );
            break;
            
        }
        
        if(left_op->prev == NULL && right_op->next == NULL)
        {
         // printf("left_op prev is NULL and right_op next is NULL");
          return;
        } 
        // printf("sum is %d!\n", sum);
        //sprintf(buffer, "%d", sum);
     //   Symbol_insert(reg[rcount++],INT ,buffer); // for register/temp variable insertion
        // printll();
        // make left_op as the temporary variable which will store the result of left_op + right_op
        left_op->next = right_op->next;
        if(right_op->next != NULL)
         right_op->next->prev = left_op;

        //strcpy(left_op->str->value, buffer);
        
        // Assign the neighbour node priority to out result node left_op
        if(left_op->prev != NULL && right_op->next != NULL)
        {
          if(left_op->prev->priority > right_op->next->priority)
          {
             left_op->priority = left_op->prev->priority;
          }
          else
          {
             left_op->priority = right_op->next->priority;
          }
        } else if(left_op->prev != NULL)
        {
            left_op->priority = left_op->prev->priority;
        } else if(right_op->next != NULL )
        {
            left_op->priority = right_op->next->priority;
        }
        free(right_op);
        right_op =NULL;
        free(p);
        p=NULL;
      }
      //   printll();
        //printf("maximum() pr is %d!\n", maximum());

        
    } while(1);
    
    
}

int getNextPriority(char* str) /* need this function to assign operands or variables with priority value */
{
    int pr=0;
      char *s =str;
    while(*s != '+' && *s != '-' && *s != '/' && *s != '*' && *s != '%' && *s !='=' && *s != '>' && *s != '<' && *s != '!' && *s != '&' && *s != '^' && *s != '|' && *s != '?' && *s != ':')
     s++;
   if(strncmp(str,"+=", 2) == 0 || strncmp(str,"*=", 2)==0 || strncmp(str,"/=", 2)==0 || strncmp(str,"-=", 2)==0)
   {
        switch(*s)
    {
      case '+':
      pr=ADD_EQ_OP;
      break;
      case '-':
      pr=SUB_EQ_OP;
      break;
      case '/':
      pr=DIV_EQ_OP;
      break;
      case '*':
      pr=MUL_EQ_OP;
      break;
     
     }
       
   }
   else if(strncmp(str, "&&",2)==0 || strncmp(str, "||",2)==0)
   {
       if(strncmp(str, "&&",2)==0)
       {
           pr=AND_AND_OP;
       }
       else if(strncmp(str, "||",2)==0)
       {
           pr=OR_OR_OP;
       }
   }
   else if(strncmp(str, "==",2) == 0 || strncmp(str, ">=",2) == 0 || strncmp(str, "<=",2) == 0 || strncmp(str, "!=",2) == 0)
   {
      switch(*s)
      {
        case '=':
        pr=EQ_EQ_OP;
        break;
        case '>':
        pr=GTR_EQ_OP;
        break;
        case '<':
        pr=LSR_EQ_OP;
        break;
        case '!':
        pr=NOT_EQ_OP;
        break;
      }
   }
   else if(strncmp(str, "&=",2) == 0 || strncmp(str, "^=",2) == 0 || strncmp(str, "|=",2) == 0)
   {
       switch(*s)
      {
        case '&':
        pr=AMP_EQ_OP;
        break;
        case '^':
        pr=XOR_EQ_OP;
        break;
        case '|':
        pr=OR_EQ_OP;
        break;

      }
   }
   else if(strncmp(str, "<<=",3) == 0 || strncmp(str, ">>=",3) == 0)
   {
       if(strncmp(str, "<<",2) == 0)
      {
        pr=LSHR_EQ_OP;
      }
      else if(strncmp(str, ">>",2) == 0)
      {
        pr=RSHR_EQ_OP;
      }
   }
   else if(strncmp(str, ">>",2) == 0 || strncmp(str, "<<",2) == 0)
   {
      if(strncmp(str, "<<",2) == 0)
      {
        pr=LSHR_OP;
      }
      else if(strncmp(str, ">>",2) == 0)
      {
        pr=RSHR_OP;
      }
   }
   else
   {
   
    switch(*s)
    {
      case '+':
      pr=ADD_OP;
      break;
      case '-':
      pr=SUB_OP;
      break;
      case '/':
      pr=DIV_OP;
      break;
      case '*':
      pr=MUL_OP;
      break;
      case '<':
      pr=LSR_OP;
      break;
      case '>':
      pr=GTR_OP;
      break;
      case '&':
      pr=AND_OP;
      break;
      case '|':
      pr=OR_OP;
      break;
      case '!':
      pr=NOT_OP;
      break;
      case '=':
      pr=MOV_OP;
      break;
      case '%':
      pr=MOD_OP;
      break;
     }
   }
    return pr;
}
int mapBlocklevel(int blocklevel)
{
    return (LAST_OP-ZERO_OP)*blocklevel;
}
int insert(char *str,int *k) /* inserts operators and operands into the priority linked list */
{
    static int blocklevel = 0;
   static int priority =INIT_PRIORITY;
   char *s;
   char string_c[100];
   struct Symbol *sym_ptr;
   if(*str == ' ' || *str == '\t' || *str == '\v' || *str == '\n' )
   {
       // do nothing, just ignore whitespaces
       return 1;
   }
   else if(*str == '<')
   {
      int count = 1;
      int i =1;
      while(*(str+i) == '<')
      {
        count++;
        i++;
      }
      if(count == 2)
      {
         priority = LSHR_OP;
        if(blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,LSHR_OP,BINARY_OP,0);
        } else
        {
          fill(OPERATOR,priority,NULL, LSHR_OP,BINARY_OP,0);
        }
         *k=*k+1;
      }
      else if(count == 2 && *(str+2) == '=')
      {
         priority = LSHR_EQ_OP;
        if(blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,LSHR_EQ_OP,BINARY_OP,0);
        } else
        {
          fill(OPERATOR,priority,NULL, LSHR_EQ_OP,BINARY_OP,0);
        }
         *k=*k+2;
      }
      else if(count == 1 && *(str+1) == '=')
      {
          priority = LSR_EQ_OP;
          if(blocklevel > 0)
          { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,LSR_EQ_OP,BINARY_OP,0);
          } else
          {
              fill(OPERATOR,priority,NULL, LSR_EQ_OP,BINARY_OP,0);
          }

         *k=*k+1;
      }
      else if( count == 1)
      {
        priority = LSR_OP;
        if(blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,LSR_OP,BINARY_OP,0);
        } else
        {
          fill(OPERATOR,priority,NULL, LSR_OP,BINARY_OP,0);
        }
      }
      return 1;
   }
   else if( *str == '>')
   {
       int count = 1;
       int i = 1;
       
       while(*(str+i) == '>')
       {
         count++;
         i++;
       }
       
       if(count == 2)
       {
          priority = RSHR_OP;
          if(blocklevel > 0)
          { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,RSHR_OP,BINARY_OP,0);
          } else
          {
              fill(OPERATOR,priority,NULL, RSHR_OP,BINARY_OP,0);
          }
          *k=*k+1;
       }
      else if(count == 2 && *(str+2) == '=' )
       {
          priority = RSHR_EQ_OP;
          if(blocklevel > 0)
          { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,RSHR_EQ_OP,BINARY_OP,0);
          } else
          {
              fill(OPERATOR,priority,NULL, RSHR_EQ_OP,BINARY_OP,0);
          }
          *k=*k+2;
       }
       else  if(count == 1 && *(str+1) == '=')
       {
          priority = GTR_EQ_OP;
          if(blocklevel > 0)
          { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,GTR_EQ_OP,BINARY_OP,0);
          } else
          {
              fill(OPERATOR,priority,NULL, GTR_EQ_OP,BINARY_OP,0);
          }
       
         *k=*k+1;
       } 
       else if(count == 1)
       { 
          priority = GTR_OP;
          if(blocklevel > 0)
          { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,GTR_OP,BINARY_OP,0);
          } else
          {
              fill(OPERATOR,priority,NULL, GTR_OP,BINARY_OP,0);
          }
       }
       return 1;
   }
   else if(*str == '*')
   {
       int count = 1;
       int i = 1;
       int l = 1;
       int j = 1;
      while(*(str+i) == '*')
      {
         count++;
         i++;
      }
       if(count > 1)
        j=count;  

        while(*(str-l) == ' ' || *(str-l) == '\t' || *(str-l) == '\v' || *(str-l) == '\n')
        {
            l++;
        }
        while(*(str+j) == ' ' || *(str+j) == '\t' || *(str+j) == '\v' || *(str+j) == '\n')
        {
            j++;
        }
      if( count >= 1 && !isalnum(*(str-l)) && isalnum(*(str+j)) ) /* Dereference operation ex:- y = ***p  */
      {
          if(*(str-l) == '=' || *(str-l) == '+' || *(str-l) == '-' || *(str-l) == '*' || *(str-l)=='/' || *(str-l) == '>' || *(str-l) == '<' || *(str-l) == '&' || *(str-l) == '^' || *(str-l) == '%' || *(str-l) == '|' )
          ;
          else
          {
             printf("error:Compilation failed!\n");
             exit(0);
          }

           priority = DEREF_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , DEREF_OP, UNARY_OP,count); // fill count as dereference level due to double & triple pointer
           } else
           {
              fill(OPERATOR,priority,NULL, DEREF_OP,UNARY_OP,count);
           }
          *k = *k+ count - 1; // need to debug
      }
      else if(count == 1 && *(str+1) == '=')
      {

        priority = MUL_EQ_OP;
        if(blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,MUL_EQ_OP,BINARY_OP,0);
        } else
        {
           fill(OPERATOR,priority,NULL, MUL_EQ_OP,BINARY_OP,0);
        }
        *k = *k+1;

      }
      else if(count == 1)
      {
        priority = MUL_OP;
        if(blocklevel > 0)
        { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,MUL_OP,BINARY_OP,0);
        } else
        {
           fill(OPERATOR,priority,NULL, MUL_OP,BINARY_OP,0);
        }
      }  
       return 1;
   }
   else if(*str == '/')
   {
      int count = 1;
      int i = 1;

      while(*(str+i) == '/')
      {
         count++;
         i++;
      }

       if(count == 1 & *(str+1) == '=')
       {
         priority = DIV_EQ_OP;
         if(blocklevel > 0)
         { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,DIV_EQ_OP,BINARY_OP,0);
         } else
         {
              fill(OPERATOR,priority,NULL, DIV_EQ_OP,BINARY_OP ,0);
         }
        *k=*k+1;
       }
       else if(count == 1) 
       {
         priority = DIV_OP;
         if(blocklevel > 0)
         { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,DIV_OP,BINARY_OP,0);
         } else
         {
              fill(OPERATOR,priority,NULL, DIV_OP,BINARY_OP ,0);
         }
       }
       return 1;
   }
   else if(*str == '+')
   {
       int count =1;
       int i=1;
       
       while(*(str+i) == '+')
        { count++;
          i++;
        }
      
      if(count == 1 && *(str+1) == '=' )
      {
      
            priority = ADD_EQ_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_EQ_OP,BINARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_EQ_OP,BINARY_OP,0);
            }
            *k= *k+1;
      } 
      else if(count == 2)  // ++ only   ex:- ++i
      {
            priority = ADD_ADD_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_ADD_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_ADD_OP,UNARY_OP,0);
            }
            *k= *k+1;
        
      }
      else if(count == 3) // ++ and + ,ex:- i++ + j
      {
           priority = ADD_ADD_OP;
          //printf("+++ operation hit %d!\n", count);
            if(blocklevel > 0)
            { //printf("Symbol founded is %c, priority is %d !\n",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_ADD_OP,UNARY_OP,0);
            } else
            {
              // printf("Symbol founded is %c, priority is %d\n !\n", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_ADD_OP,UNARY_OP,0);
            }
           
           priority = ADD_OP;
           
            if(blocklevel > 0)
            {  printf("Symbol founded is %c, priority is %d!\n",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_OP,BINARY_OP,0);
            } else
            {
              printf("Symbol founded is %c, priority is %d !\n", *str , priority,0);
               fill(OPERATOR,priority,NULL ,ADD_OP,BINARY_OP,0);
            }
            *k=*k+2;
          
      }
      else if(count == 5)  //  ++ + ++ ex:- i++ + ++i
      {
            priority = ADD_ADD_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_ADD_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_ADD_OP,UNARY_OP,0);
            }
           
           priority = ADD_OP;
           
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_OP,BINARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_OP,BINARY_OP,0);
            }
            
            priority = ADD_ADD_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_ADD_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_ADD_OP,UNARY_OP,0);
            }
            
            *k=*k+4;
      }
      else if(count ==1)
      {
       
         priority = ADD_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,ADD_OP,BINARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,ADD_OP,BINARY_OP,0);
            }
            
      }
       return 1;
   }
   else if(*str == '-')
   {
       int count =1;
       int i=1;
       
       while(*(str+i) == '-')
        { count++;
          i++;
        }
      if(count == 1 && *(str+1) == '=' )
      {

            priority = SUB_EQ_OP;

            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_EQ_OP,BINARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_EQ_OP,BINARY_OP,0);
            }
            *k= *k+1;
      }
      else if(count == 2)  // -- only   ex:- --i
      {
            priority = SUB_SUB_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_SUB_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_SUB_OP,UNARY_OP,0);
            }
            *k= *k+1;
        
      }
      else if(count == 3) // -- and - ,ex:- i-- - j
      {
           priority = SUB_SUB_OP;
          //printf("+++ operation hit %d!\n", count);
            if(blocklevel > 0)
            { //printf("Symbol founded is %c, priority is %d !\n",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_SUB_OP,UNARY_OP,0);
            } else
            {
              // printf("Symbol founded is %c, priority is %d\n !\n", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_SUB_OP,UNARY_OP,0);
            }
           
           priority = SUB_OP;
           
            if(blocklevel > 0)
            {  printf("Symbol founded is %c, priority is %d!\n",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_OP,BINARY_OP,0);
            } else
            {
              printf("Symbol founded is %c, priority is %d !\n", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_OP,BINARY_OP,0);
            }
            *k=*k+2;
          
      }
      else if(count == 5)  //  -- - -- ex:- i-- - --i
      {
            priority = SUB_SUB_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_SUB_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_SUB_OP,UNARY_OP,0);
            }
           
           priority = SUB_OP;
           
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_OP,BINARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_OP,BINARY_OP,0);
            }
            
            priority = SUB_SUB_OP;
       
            if(blocklevel > 0)
            { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_SUB_OP,UNARY_OP,0);
            } else
            {
             //  printf("Symbol founded is %c, priority is %d\n !", *str , priority);
               fill(OPERATOR,priority,NULL ,SUB_SUB_OP,UNARY_OP,0);
            }
            
            *k=*k+4;
     }
     else if(count == 1)   
     {
          priority = SUB_OP;
          if(blocklevel > 0)
           {//  printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,SUB_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, SUB_OP ,BINARY_OP,0);
           }
     }
       return 1;
   }
    else if(*str == '=')
   {
   	  if(*(str+1) == '=')
   	  {
   	     priority = EQ_EQ_OP;
         if(blocklevel > 0)
         { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
               fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL ,  EQ_EQ_OP,BINARY_OP,0);
         } else
         {
              fill(OPERATOR,priority,NULL,  EQ_EQ_OP,BINARY_OP,0 );
         }
         *k=*k+1;
	  }
	  else
	  {
	  
          priority = MOV_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , MOV_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, MOV_OP,BINARY_OP,0 );
           }
       }
       return 1;
   }
   else if(*str == '!')
   {
   	   if(*(str+1) == '=')
   	   {
   	   	  priority = NOT_EQ_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , NOT_EQ_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, NOT_EQ_OP, BINARY_OP,0 );
           }
           *k=*k + 1;
	   }
	   else 
	   {
	      priority = NOT_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , NOT_OP,UNARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, NOT_OP, UNARY_OP,0 );
           }
       }
       return 1;
   }
   else if(*str == '~')
   {
       priority = BNOT_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , BNOT_OP,UNARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, BNOT_OP, UNARY_OP,0 );
           }
       return 1;
   }
   else if(*str == '^')
   {
       if(*(str+1) == '=' )
       {
          priority = XOR_EQ_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , XOR_EQ_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, XOR_EQ_OP, BINARY_OP,0);
           }
         *k=*k+1;
       }
       else
       {
          priority = XOR_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , XOR_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, XOR_OP, BINARY_OP,0);
           }
       }
       return 1;
   } 
   else if(*str == '&')
   {
       int count =1;
       int i=1;
       int l=1;
       int j=1;
       while(*(str+i) == '&')
        { count++;
          i++;
        }
       if(count == 2)  // for differentiating between '&' and '&&'
          j=2;
        
        while(*(str-l) == ' ' || *(str-l) == '\t' || *(str-l) == '\v' || *(str-l) == '\n')
        {
            l++;
        }
        while(*(str+j) == ' ' || *(str+j) == '\t' || *(str+j) == '\v' || *(str+j) == '\n')
        {
            j++;
        }
        printf("count:%d %d %d\n ", count, isalnum(*(str-l)), isalnum(*(str+j)) );
       if(count == 1 && isalnum(*(str-l)) && isalnum(*(str+j)) )  // for example i & j
       {
          priority = AND_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , AND_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, AND_OP,BINARY_OP,0);
           }
       }
        else if(count == 1 && isalnum(*(str+j))) // for address of( &k ) example addr = & k
       {
         priority = ADDR_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , ADDR_OP,UNARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, ADDR_OP, UNARY_OP,0);
           }
       }
       else if(count == 2 && isalnum(*(str-l)) && isalnum(*(str+j)))  // for logical or, ex:- i && j
       {
          priority = AND_AND_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , AND_AND_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, AND_AND_OP,BINARY_OP,0);
           }
            *k=*k+1;
       }
       else if(count == 1 && *(str+1)== '=')
       {
          priority = AMP_EQ_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , AMP_EQ_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, AMP_EQ_OP,BINARY_OP ,0);
           }
            *k=*k+1;

       }
       else {
           printf("Compilation error ! line:%d", __LINE__);
           exit(0);
       }
       return 1;
   }
    else if(*str == '|')
   {
       int count =1;
       int i=1;
       int l=1;
       int j=1;
       while(*(str+i) == '|')
        { count++;
          i++;
        }
        if(count == 2)  // for differentiating between '|' and '||'
          j=2;
          
        while(*(str-l) == ' ' || *(str-l) == '\t' || *(str-l) == '\v' || *(str-l) == '\n')
        {
            l++;
        }
        while(*(str+j) == ' ' || *(str+j) == '\t' || *(str+j) == '\v' || *(str+j) == '\n')
        {
            j++;
        }
        printf("count:%d %d %d\n ", count, isalnum(*(str-l)), isalnum(*(str+j)) );
       if(count == 1 && isalnum(*(str-l)) && isalnum(*(str+j)) )  // for example i | j
       {
          priority = OR_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , OR_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, OR_OP,BINARY_OP, 0);
           }
       }
        else if(count == 1 && *(str+1) == '=') // for address of( |=k ) example addr = |= k
       {
         priority = OR_EQ_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , OR_EQ_OP,UNARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, OR_EQ_OP, UNARY_OP,0 );
           }
           *k=*k+1;
       }
       else if(count == 2 && isalnum(*(str-l)) && isalnum(*(str+j)))  // for logical or, ex:- i || j
       {
          priority = OR_OR_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , OR_OR_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, OR_OR_OP,BINARY_OP,0);
           }
            *k=*k+1;
       }
       else {
           printf("Compilation error ! line:%d", __LINE__);
           exit(0);
       }
       return 1;
   }
   else if(*str == '%')
   {
      if(*(str+1)== '=' )
      {
          priority = MOD_EQ_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , MOD_EQ_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, MOD_EQ_OP,BINARY_OP,0);
           }
        *k = *k + 1;
      }
      else
      {
          priority = MOD_OP;
          if(blocklevel > 0)
           { // printf("Symbol founded is %c, priority is %d\n !",*str , priority+mapBlocklevel(blocklevel));
              fill(OPERATOR,priority + mapBlocklevel(blocklevel),NULL , MOD_OP,BINARY_OP,0);
           } else
           {
              fill(OPERATOR,priority,NULL, MOD_OP,BINARY_OP,0);
           }
      }
      return 1;
   }
   else if((*k==0 || !isalnum( *(str - 1))) && *str =='\"')
   {
   	    int i=1;
   	   while(*(str+i) != '\"')
   	   {
                  if(*(str+i) == '\n' || *(str+i) =='\r' || *(str+i) =='\b')
                  {
                    printf("error: compilation error as string not terminated!");
                    exit(0);
                  }
   	   	 i++;
	   }
   	   char *dup_str = (char*) calloc(i+1,sizeof(char));
   	   strncpy(dup_str,str,i+1);
            
           printf(".ascii %s\n", dup_str);
   	   int *addr_to_str = (int *)dup_str;
   	    /********* similar to inserting an integer constant, address integer constant defines the size of an object  *********/
   	    memset(dup_str,'\0',i);
        sprintf(dup_str,"%p",addr_to_str); // converting address value to string form 
        printf("mov %s,%s\n",register_str[reg_count],dup_str);

        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*) malloc(i);
        strcpy(sym->value,dup_str);
        sym->r = (Registers) reg_count++;
        if(blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority+mapBlocklevel(blocklevel));

              fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
        } else
        {
           //    printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority);
               fill(CONST,priority,sym ,'\0',NULL_OP,0);
        }
        free(dup_str);
      *k = *k + 2 + i; 
   }
   else if(!isalnum( *(str - 1)) && strncmp(str,"sizeof",6) == 0) // sizeof will be replaced by an integer value similar to a macro
   {
      char type_size = 1;
      char string_type_size[3];
      int j= 0,index = -1;
      char *s = str + 6;
      char *id;
	  while(*(s+j) == ' ' || *(s+j) == '\n' || *(s+j) == '\t' || *(s+j) == '\v') // ignore white-spacing after 'sizeof'
		  j++;
      while(*(s+j) != ')' && *(s+j) != ' ')
      {
         j++;
      }
      id = (char*) calloc(j ,sizeof(char)); // -1 added to j for opening bracket '('
      strncpy(id,s+1,j-1);
      
      //printf("id is %s s is %s j is %d\n", id,s,j ); 
      index=Symbol_search(id);
      if(strncmp(s,"(int)",5) == 0)
      {
            type_size =4;
      } 
      else if(strncmp(s,"(char)",6) == 0)
      {
            type_size =1;
      }
	   else if(strncmp(s,"(long long)",11) == 0)
      {
           type_size =8;
      }
	  else if(strncmp(s,"(unsigned char)",14) == 0)
      {
           type_size =1;
      }
	  else if(strncmp(s,"(unsigned int)",13) == 0)
      {
           type_size =4;
      }
	  else if(strncmp(s,"(double)",8) == 0)
      {
           type_size =8;
      }
	  else if(strncmp(s,"(float)",7) == 0)
      {
           type_size =4;
      }
      else if(strncmp(s,"(long)",6) == 0)
      {
           type_size =8;
      }
      else if(strncmp(s,"(short)",7) == 0)
      {
            type_size =2;
      }
	  else if(strncmp(s,"('",2) == 0)
	  {
		  if((*(s+3) == '\'' && *(s+4) == ')') || (*(s+4) == '\'' && *(s+5) == ')')) 
		  {
			  type_size = 1;
		  }
		  else
		  {
			  printf("error: Compilation error!");
			  exit(0);
		  }
	  }
      else if(index != -1)
      {
          if( symtbl[index].type == INT)
             type_size = 4;
		  else if( symtbl[index].type == FLOAT)
             type_size = 8;
		  else if( symtbl[index].type == CONST)
             type_size = 4;

      }
       free(id);
      /********* similar to inserting an integer constant, integer constant defines the size of an object  *********/
        sprintf(string_type_size,"%d",type_size); // converting integer to string 
        printf("mov %s,%s\n",register_str[reg_count], string_type_size);

        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*) malloc(strlen(string_type_size));
        strcpy(sym->value,string_type_size);
        sym->r = (Registers) reg_count++;
        if(blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority+mapBlocklevel(blocklevel));

              fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
        } else
        {
           //    printf("Symbol founded is %s, priority is %d\n !",string_type_size , priority);
               fill(CONST,priority,sym ,'\0',NULL_OP,0);
        }
      *k = *k + 5 + j; 
     // printf("next str is %s\n", str+*k ); 
   }
   else if(*str == '(')
   {
       blocklevel++;
       priority = getNextPriority(str+1);
       return 1;
   }
   else if(*str == ')')
   {
       blocklevel--;
       priority = getNextPriority(str+1);
       return 1;
   }
   else if(*str == '[')
   {
      int i=1;
      int w=1;
      int l=1;
      int index = -1;
      int index_id = -1;
      char *arr_def; 
      if( *(str - 1) == ' ' || *(str - 1) == '\t' || *(str - 1) == '\v' || *(str - 1) =='\n')
      {
         printf("error: array defination is wrong !\n");
         exit(0);
      }
      printf("Array is Hit!\n");
      while(isalpha( *(str-i) ) || *(str-i) == '_')
      {
          i++;
      }
      if(i == 1)
      {
        printf("error: Array declaration is invalid\n");
        exit(0);
      }    
      arr_def =(char*) calloc(i,sizeof(char) );
      strncpy(arr_def, str-i+1, i-1);
      printf("arr_def: %s",arr_def); 
      while(*(str+l) != ']')
      {
         l++;
      }
     
 
      if(isdigit(*(str+1)) )
      {
         while( isdigit( *(str+w) ) )   
         {
             w++;
         }     
         if( *(str+w) != ']')
         {
            printf("error: compilation failed !\n");
            exit(0);
         }
         char *digits = (char*) calloc(w ,sizeof(char)); // -1 added to j for opening bracket '('
         strncpy(digits,str+1,w-1);
          char *buffer = (char*) calloc(w+i+1 ,sizeof(char));


         sprintf(buffer, "%s#%s", arr_def, digits);
        printf("digits is %s buffer is %s i is %d\n", digits,buffer,i );
         index=Symbol_search(buffer);
         if(index != -1)
         {
              int len = strlen(symtbl[index].value);
              strncpy(string_c,symtbl[index].value , len);
              string_c[len] = '\0';
              printf("mov %s,%s\n",register_str[reg_count], string_c);

              struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
              sym->value = (char*) malloc(strlen(string_c));
              strcpy(sym->value,string_c);
              sym->r = (Registers) reg_count++;
              if(blocklevel > 0)
              { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

                  fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
              } else
              {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
                  fill(CONST,priority,sym ,'\0',NULL_OP,0);
              }

         }


      }
      else if(isalpha( *(str+1) ) || *(str+1) == '_' )
      {

         while( isalpha( *(str+w) ) || *(str+w) == '_' )
         {
             w++;
         }
         if( *(str+w) != ']')
         {
            printf("error: compilation failed !\n");
            exit(0);
         }



         char *id = (char*) calloc(w ,sizeof(char)); // -1 added to j for opening bracket '('
         strncpy(id,str+1,w-1);
         char *buffer = (char*) calloc(w+i+1 ,sizeof(char));
        //printf("id is %s s is %s j is %d\n", id,s,j );
         
         index_id = Symbol_search(id);
         if(index_id == -1)
         {
             printf("%s identifier is unknown!\n", id);
             exit(0);
         }        
         
         sprintf(buffer, "%s#%s", arr_def, symtbl[index_id].value);
         index=Symbol_search(buffer);
         if(index != -1)
         {
              int len = strlen(symtbl[index].value);
              strncpy(string_c,symtbl[index].value , len);
              string_c[len] = '\0';
              printf("mov %s,%s\n",register_str[reg_count], string_c);

              struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
              sym->value = (char*) malloc(strlen(string_c));
              strcpy(sym->value,string_c);
              sym->r = (Registers) reg_count++;
              if(blocklevel > 0)
              { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

                  fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
              } else
              {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
                  fill(CONST,priority,sym ,'\0',NULL_OP,0);
              }
            
         } 
      }
   // copy ll
   // delete ll
   // create new ll using expression call() 
   // restore ll 
      if(l > 1)
      {
          *k += l;
      }

   }
   else if( *str == '\'')
   {
       int i=1;
        int character, char_in_digit_form; 
            if((*(str+2) == '\'') || (*(str+3) == '\''))
            {
                if(*(str+2) == '\'')
                   i=2;               
                else if( *(str+3) == '\'')     
                   i=3;
            }
            else
            {
                          printf("error: Compilation error!");
                          exit(0);
            }
      *k+= i-1;
      if(i==3)
      {
        character = *(str +1); 
        char_in_digit_form = character - '0';
      }
       /************** adding the character as ascii integer constant ********/
       strncpy(string_c,str+1, 1);
       string_c[i] = '\0';
       printf("mov %s,%s\n",register_str[reg_count], string_c);

        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*) malloc(strlen(string_c));
        strcpy(sym->value,string_c);
        sym->r = (Registers) reg_count++;
        if(blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

              fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
        } else
        {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
               fill(CONST,priority,sym ,'\0',NULL_OP,0);
        }



   }
   else if(isdigit(*str))
   {
       int i=1;
       s = str;
       
       while(isdigit(*(str +i)))
       {
           str++; 
           i++;
       }

       if(*(s+i-1) == '.')
       {
        goto FP;
       }
       strncpy(string_c,s,i+1);
       string_c[i+1] = '\0';
       printf("mov %s,%s\n",register_str[reg_count], string_c);
       
        struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
        sym->value = (char*) malloc(strlen(string_c));
        strcpy(sym->value,string_c);
        sym->r = (Registers) reg_count++;
        if(blocklevel > 0)
        { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));
              
              fill(CONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
        } else
        {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
               fill(CONST,priority,sym ,'\0',NULL_OP,0);
        }
         *k+= i; // for updating the reading pointer in main() for removing multi-integer constants
       
   }
   else if(isalpha(*str))
   {   int i=0,index=-1;
       s = str;
       
       while(isalpha(*str) || isdigit(*str))
       {  str++;
           i++;
       }
       *k+=i-1; // for moving the reading pointer in main() for removing multi-charachter variable ex:-abc not just ex:-a
       strncpy(string_c,s,i);
       string_c[i]='\0';
      //  printf("Symbol inserted is %s\n !",string_c );
       string_c[i+1]='\0';
       index=Symbol_search(string_c);
       if(index != -1)
       {
           if(priority == INIT_PRIORITY)
              priority = getNextPriority(str);
           else if(*(str+1) != '\0' && priority < getNextPriority(str) )
              priority = getNextPriority(str);
              
            // if symbol search is successful then move it into a register and assign a register
        //    static int reg_count = 0;
            
          //  if(reg_count >= 6)
        //      reg_count = reg_count%5;
              
          printf("mov %s,%s\n",register_str[symtbl[index].r],symtbl[index].value );
            
              
          /*   sym_ptr = (struct Symbol*) malloc (sizeof(struct Symbol));
             sym_ptr->symbol = (char*) malloc (strlen()) */
           if(blocklevel > 0)
           { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));
              
              fill(OPERAND,priority + mapBlocklevel(blocklevel),&symtbl[index] ,'\0',NULL_OP,0);
           } else
           {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
               fill(OPERAND,priority,&symtbl[index] ,'\0',NULL_OP,0);
           }
           
       }
       else
       {
         // printf("[ERROR] Unknown Symbol %s not found !",string_c );
       }
       
       return i;
   }
   else if( *str == '.')
   {
FP:
      printf("decimal leg is hit\n"); 
      int i = 1;
      int j = 1;
      while( isdigit(*(str - i) ) )
      {
         i++;
      }
      while( isdigit( *(str + j)   ) )
      {
         j++;
      }
      *k+= j-1; // for updating the reading pointer in main() for removing multi-integer constants
      /****** insert the floating constant *******/     
      strncpy(string_c,s,i+j+1);
      string_c[i+j+2] = '\0';
      printf("mov %s, __?float32?__(%s)\n",register_str[reg_count], string_c);

      struct Symbol* sym = (struct Symbol*)malloc(sizeof(struct Symbol));
      sym->value = (char*) malloc(strlen(string_c));
      strcpy(sym->value,string_c);
      sym->r = (Registers) reg_count++;
      if(blocklevel > 0)
      { // printf("Symbol founded is %s, priority is %d\n !",string_c , priority+mapBlocklevel(blocklevel));

           fill(FCONST,priority + mapBlocklevel(blocklevel),sym ,'\0',NULL_OP,0);
      } 
      else
      {
           //    printf("Symbol founded is %s, priority is %d\n !",string_c , priority);
               fill(FCONST,priority,sym ,'\0',NULL_OP,0);
      }


 
   }



return 0;        
}
void symbols_entry()
{
    char a[]="a";
    char b[]="b";
    char c[]="c";
    char d[]="d";
    char e1[]="e#1";
    char e2[]="e#2";
    char e3[]="e#3";
    char a_value[] ="9";
    char b_value[] ="3";
    char c_value[] ="2";
    char d_value[] ="1";
    char e1_value[] = "55";
    char e2_value[] = "56";
    char e3_value[] = "57";
    char arr_value[] ="1";
    Symbol_insert(a,INT , a_value, ax);
    Symbol_insert(b,INT ,b_value, bx);
    Symbol_insert(c,INT ,c_value, cx);
    Symbol_insert(d,INT ,d_value, dx);
    Symbol_insert(e1,INT ,e1_value, ax);
    Symbol_insert(e2,INT ,e2_value, bx);
    Symbol_insert(e3,INT ,e3_value, cx);
    Symbol_insert((char*)"arr#0", I_ARRAY ,arr_value, eax);

  return;
}

int expression(char *arr)
{
    int i;
    //char arr[] = "a=b+++--c*c%99";
    for( i=0; i< strlen(arr); )
    {
        insert(arr+i,&i); // we are passing i for jumping for double ot more digit constant or multi-chaacter variables
         //printf("%c\n",arr[i]);
        i++;
    }
  //  printf("\nmaximum value is %d\n", maximum()->priority);
    expr_resolver();
    
    return 0;
}

int *searchstr(char *arr, char *find,int *numberOfOccurrance) /* searches multiple instances of find in str and returs the array with locations/indexes*/
{
    int k= strlen(arr);
    int l= strlen(find);
    int i;
    int *z = (int*) calloc(k/l, sizeof(int));
    int x =0;
    for(i=0;i<k;i++)
    {
        if(strncmp(arr+i,find,l)==0)
        {
            z[x] = i;
            x++;
            *numberOfOccurrance = x;
        }
    }
    return z;
}

int main()
{
      symbols_entry();
     // char arr[] = "a*sizeof(b)";
       char arr[] = "e[d]";
      int found=0,comma_found =0;
      int *d=searchstr(arr,(char*) "?", &found);
      int *comma = searchstr(arr,(char*) ",", &comma_found);
      if(found > 0)
      {
          if(d[0] > 0)
          {
             // printf("%d %d\n", d[0],found);
              char *sub_expr = (char*)calloc(d[0]+1,sizeof(char)); // use calloc for intializing string as '0' arrays for termination of str easily 
              strncpy(sub_expr,arr,d[0]);
              printf("%s\n", sub_expr);
              expression(sub_expr);
              erasell(); // cleaning the linked list for reusing the linked list again
              free(sub_expr); 
              found = 0;
              int *y=searchstr(arr, (char*)":", &found);
              //printf("%d %d\n", y[0],found);
              if(found > 0)
              {
                  if(y[0]> d[0])
                  {
                      char *sub_expr2 = (char*)calloc((y[0]-d[0]),sizeof(char));
                      strncpy(sub_expr2,arr+d[0]+1,y[0]-d[0]-1);
                      expression(sub_expr2);
                      erasell();
                      free(sub_expr2); 
                      found = 0;
                      int *q=searchstr(arr,(char*)";", &found);
                      if(found > 0)
                      {
                          if(q[0] > y[0])
                          {
                              char *sub_expr3 = (char*)calloc( (q[0]-y[0]),sizeof(char) );
                              strncpy(sub_expr3,arr+y[0]+1,q[0]-y[0]-1);
                              expression(sub_expr3);
                              erasell();
                              free(sub_expr3);
                          }
                          
                      }
                      
                  }
                  else printf("error: '?' expected before ':'");
              }
              else
                printf("error: incomplete ternary operation");
              
          }
          else printf("error: expression expected before '?'");
      }
      else if(comma_found > 0)
      {
      	 int start_charread=0;
      	 int i =0;
      	 char *read_str = arr;
         int len = strlen(arr);
      	while(i < comma_found)
      	{
      	    char *sub_expr = (char*)calloc((comma[i]-start_charread),sizeof(char));
            strncpy(sub_expr,read_str,comma[i]-start_charread);
            printf("sub_expr is :%s\n", sub_expr);
            expression(sub_expr);
            erasell();
            free(sub_expr); 
					 
	    read_str=arr+comma[i]+1;
	    start_charread = comma[i]+1;
	    i++;		  
	}
         char *sub_expr2 = (char*)calloc((len-start_charread),sizeof(char));
         strncpy(sub_expr2,read_str,len-start_charread);
         expression(sub_expr2);
         erasell();
         free(sub_expr2); 
      	
      }
      else
      {
         printf("expr is hit !\n");
         expression(arr);
         erasell();
      }
      //system("pause");
}
