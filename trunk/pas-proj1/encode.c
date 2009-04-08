#include "encode.h"
#include "backend.h"
#include "types.h"

/* Function that outputs the necessary code for a global declaration */
void declareVariable(ST_ID id, TYPE type)
{
  /* Variables for the alignment and skip sizes */
  int align, skip;

  /* Gets the alignment value and skip value */
  align = getAlignSize(type);
  skip = getSkipSize(type);

  /* Calls the backend functions */
  b_global_decl(st_get_id_str(id), align);
  b_skip(skip);
}

/* Function that gets the required alignment value */
int getAlignSize(TYPE type)
{
  /* Gets the type tag from the type */
  TYPETAG tag = ty_query(type);

  /* Represents the index list */
  INDEX_LIST list;

  /* Low and high for subrange type */
  long low;
  long high;

  /* Switch based on typetag */
  switch(tag)
  {
    /* Array case */
    case TYARRAY:
      /* Recursive call to handle array */
      return getAlignSize(ty_query_array(type, &list));
      break;
    /* Pointer case */
    case TYPTR:
      /* Returns the size */
      return sizeof(char *);
      break;
    /* Subrange case */
    case TYSUBRANGE:
      return getAlignSize(ty_query_subrange(type, &low, &high));
      break;
    /* Double case */
    case TYDOUBLE:
      /* Returns the size of a double */
      return sizeof(double);
      break;
   /* Float */
   case TYFLOAT:
    /* Returns the size */
    return sizeof(float);
    break;
   /* Long double */
   case TYLONGDOUBLE:
    /* Returns the size */
    return sizeof(long double);
    break;
   /* Signed long int */
   case TYSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(signed long int);
    break;
   /* Signed short int */
   case TYSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(signed short int);
    break;
   /* Signed int */
   case TYSIGNEDINT:
    /* Returns the size */
    return sizeof(signed int);
    break;
   /* Unsigned long int */
   case TYUNSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(unsigned long int);
    break;
   /* Unsigned short int */
   case TYUNSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(unsigned short int);
    break;
   /* Unsigned integer */
   case TYUNSIGNEDINT:
    /* Returns the size */
    return sizeof(unsigned int);
    break;
   /* Unsigned char */
   case TYUNSIGNEDCHAR:
    /* Returns the size */
    return sizeof(unsigned char);
    break;
   /* Signed char */
   case TYSIGNEDCHAR:
    /* Returns the size */
    return sizeof(signed char);
    break;
  }
}

/* Function that gets the required skip value */
int getSkipSize(TYPE type)
{
  /* Gets the type tag from the type */
  TYPETAG tag = ty_query(type);

  /* Represents the index list */
  INDEX_LIST list;

  /* Low and high for subrange type */
  long low, high, size;

  /* Switch based on typetag */
  switch(tag)
  {
    /* Array case */
    case TYARRAY:
      /* Recursive call to handle array */
      size = getSkipSize(ty_query_array(type, &list));
      while (list) {
	ty_query_subrange(list->type, &low, &high); 
	size *= high - low + 1;           
	list = list->next;
      }
      return size;
      break;
    /* Pointer case */
    case TYPTR:
      /* Returns the size */
      return sizeof(char *);
      break;
    /* Subrange case */
    case TYSUBRANGE:
      return getSkipSize(ty_query_subrange(type, &low, &high));
      break;
    /* Double case */
    case TYDOUBLE:
      /* Returns the size of a double */
      return sizeof(double);
      break;
   /* Float */
   case TYFLOAT:
    /* Returns the size */
    return sizeof(float);
    break;
   /* Long double */
   case TYLONGDOUBLE:
    /* Returns the size */
    return sizeof(long double);
    break;
   /* Signed long int */
   case TYSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(signed long int);
    break;
   /* Signed short int */
   case TYSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(signed short int);
    break;
   /* Signed int */
   case TYSIGNEDINT:
    /* Returns the size */
    return sizeof(signed int);
    break;
   /* Unsigned long int */
   case TYUNSIGNEDLONGINT:
    /* Returns the size */
    return sizeof(unsigned long int);
    break;
   /* Unsigned short int */
   case TYUNSIGNEDSHORTINT:
    /* Returns the size */
    return sizeof(unsigned short int);
    break;
   /* Unsigned integer */
   case TYUNSIGNEDINT:
    /* Returns the size */
    return sizeof(unsigned int);
    break;
   /* Unsigned char */
   case TYUNSIGNEDCHAR:
    /* Returns the size */
    return sizeof(unsigned char);
    break;
   /* Signed char */
   case TYSIGNEDCHAR:
    /* Returns the size */
    return sizeof(signed char);
    break;
  }
}

/* Function that is called when a function block is entered */
void enter_func_body(char * global_func_name, TYPE type, int loc_var_size)
{
  /* Type, parameter list, and boolean variables for querying functions */
  TYPE fType;
  PARAM_LIST fParams;
  BOOLEAN checkArgs;

  /*Offset value for each variable, block number, data record*/
  int lVarOffset;
  int blockNum;
  ST_DR dataRec;

  /* Queries the function */
  fType = ty_query_func(type, &fParams, &checkArgs);

  /* Calls encoding function */
  b_func_prologue(global_func_name);

  /* While there are still elements in the list */
  while(fParams != NULL)
  {
    /* Gets the type tag */
    TYPETAG tag;
    tag = ty_query(fParams->type);

    /* Stores the formal parameter */
    lVarOffset = b_store_formal_param(tag);

    /*Gets the data record for the formal parameter*/
    dataRec = st_lookup(fParams->id, &blockNum);

    /*Compares the offset values*/
    if(lVarOffset != dataRec->u.decl.v.offset)
      bug("Local variable offset does not match");

    /* Moves to the next element in the list */
    fParams = fParams->next;
  }
  
  /*Allocates space for the local variables*/
  b_alloc_local_vars(loc_var_size);
}/* End enter_func_body */

/* Function that is called when a function block is exited */
void exit_func_body(char * global_func_name, TYPE type)
{
  /* Calls the encoding function */
  b_func_epilogue(global_func_name);

  /* Exits the block */
  st_exit_block();
}/* End exit_func body */

/* Function that calls backend routines to enter the main program body */
void enter_main_body()
{
  /* Calls the backend routine */
  b_func_prologue("main");
}/* End enter_main_body */

/* Function that calls backend routines to exit the main program body */
void exit_main_body()
{
  /* Calls the backend routine */
  b_func_epilogue("main");
}/* End exit_main_body */

/*Returns the local variable offset*/
int get_local_var_offset()
{
  /*Returns the local variable offset*/
  return b_get_local_var_offset();
}

/*Increments the stack top*/
void incrementStack()
{
  /*Increments*/
  bo_top++;
}

/*Decrements the stack top*/
void decrementStack()
{
  /*Decrements*/
  bo_top--;
}

/*Gets the formal parameter offset value for a certian type*/
int getFormalParameterOffset(TYPETAG tag)
{
  /*Returns the offset value*/
  return b_get_formal_param_offset(tag);
}
