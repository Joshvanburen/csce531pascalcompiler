/* Tree building functions */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "types.h"
#include "tree.h"
#include "symtab.h"
#include "message.h"


MEMBER_LIST insert_id(MEMBER_LIST list, ST_ID newid)
/* Function that inserts an ST_ID into a member list */
{
  MEMBER_LIST new;
  new = (MEMBER_LIST) malloc(sizeof(MEMBER));
  
  /*Inserts the element and returns the list*/
  new->id = newid;
  new->next = list;
  return new;
}

INDEX_LIST insert_index(INDEX_LIST list, TYPE newtype)
/* Function that inserts an index into the index list */
{
  /*Works by using the prev field to link back to the current list*/
  /*Creates the index list and allocates memory*/
  INDEX_LIST new;
  new = (INDEX_LIST) malloc(sizeof(INDEX));
  
  /*Inserts the element and returns the list*/
  new->type = newtype;
  new->next = list;
  new->prev = NULL;
  return new;
}

TYPE lookup_type(ST_ID id)
{
	ST_DR data_rec;
	int block;

	data_rec = st_lookup(id, &block); 
	if (!data_rec) {
		error("Undeclared type name: \"%s\"",st_get_id_str(id));
		return NULL;
	}
	else if (debug) { 
		printf("Denoter typename: %s\n", st_get_id_str(id) );
		printf("Reference TYPE:\n");
		ty_print_type(data_rec->u.typename.type); 
		printf("\n");
	}
	return data_rec->u.typename.type; 
}

TYPE make_array(INDEX_LIST list, TYPE newtype)
{
	if (!newtype) {
		error("Data type expected for array elements\n"); 
		return NULL;
	}
        else return ty_build_array(newtype, list); 
}

void make_type(ST_ID iden, TYPE newtype)
/* Function that makes a type data record and installs it in the symbol table */
{
  /* Creates the data record and allocates memory */
  ST_DR p;
  p = stdr_alloc();
  BOOLEAN resolved;	

  if (!newtype) {
	if (debug) printf("Type does not exist\n");
	return;
  }

  /* Sets the data record attributes and installs the type in the symbol table */
  p->tag = TYPENAME;
  p->u.typename.type = newtype;

  resolved = st_install(iden, p);

  if (!resolved) error("Duplicate definition of \"%s\" in block %d\n", st_get_id_str(iden), st_get_cur_block() );
  else if(debug) printf("TYPE name %s installed\n", st_get_id_str(iden) );

}


void make_var(MEMBER_LIST list, TYPE newtype)
/* Function that makes a variable data record and installs it in the symbol table */
{
  ST_DR p;  /* Creates a data record pointer and allocates memory */
  BOOLEAN resolved;
  
  if (!list) bug("Empty list passed to make_var"); /* empty list check */

  if (!newtype) {
	error("Variable(s) must be of data type\n"); /* check for undefined type */
	return;
  }
	
  while (list) {
	p = stdr_alloc();	  
	p->tag = GDECL;
	p->u.decl.type = newtype;

  	resolved = st_install(list->id, p); /* installs the variable in the symbol table */

	if (!resolved) error("Duplicate variable declaration: \"%s\"\n", st_get_id_str(list->id) );
	else if(debug)
  	{
  	  printf("GDECL created with type:\n");
	  ty_print_type(newtype);
	  printf("\n");
 	}

	list=list->next;
  }
}

MEMBER_LIST type_members(MEMBER_LIST list, TYPE newtype)
/* Function that takes a member list and assigns a type to each member */
{
  MEMBER_LIST p = list;

  if (!p) bug("Empty list passed to add members");

  while (p) {
	p->type = newtype;
	p=p->next;
  }
  return list;
}

MEMBER_LIST combine_members(MEMBER_LIST list1, MEMBER_LIST list2)
/* Function that adds two member lists together */
{
  MEMBER_LIST p = list1;

  if (!p) bug("Empty list passed to combine members");

  while (p->next) p=p->next;
  p->next = list2;	
  
  return list1;
}

void resolve_ptrs()
/* function that resolves pointers */
{
	ST_DR data_rec;
	TYPE list, ptr, next;
	ST_ID id;
	int block;
	BOOLEAN resolved;

	if (debug) printf("Attempting to resolve pointers.\n");

	list = ty_get_unresolved();		/* gets global unresolved list */

	if (!list) {
		if (debug) printf("No unresolved pointers\n");
		return;
	}
	
	while (list) {
		ptr = ty_query_ptr(list, &id, &next);	/* returns ID and next */
		
		data_rec = st_lookup(id, &block); 	/* lookup type for ID */
		
		if (!data_rec) error("Unresolved type name: \"%s\"",st_get_id_str(id));
				
		else {
			resolved = ty_resolve_ptr(list, data_rec->u.typename.type); /* assign type to pointer */
			if (!resolved) error("Unresolved pointer");	/* not used */
			else if (debug) {
				printf("Resolved %s ptr with type: \n",st_get_id_str(id));
				ty_print_type(data_rec->u.typename.type);
				printf("\n");
			}
		}

		list = next;				/* go to next ptr in list */

	} 	/* while (list) */
}


/* Function that evaluates the value of an identifier */
long eval_id(ST iden)
{
}

/*Function that makes an integer node*/
ST make_int(long n)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns it*/
  p->tag = INTCONST;
  p->u.intconst = n;
  if(debug)
  {
    printf("INTCONST %d created\n",(int)n);
  }
  return p;
}

/*Function that makes a real node*/
ST make_real(double n)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
	
  /*Sets the value of the node attributes and returns them*/
  p->tag = REALCONST;
  p->u.realconst = n;
  if(debug) 
  {
    printf("REALCONST %f created\n",n);
  }
  return p;
}

/*Function that makes an identifier node*/
ST make_id(ST_ID iden)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns the node*/
  p->tag = ID_NODE;
  p->u.id_node.id = iden;
  if(debug) 
  {
    printf("ID NODE %s created\n", st_get_id_str(iden) );
  }
  return p;
}

/*Function that makes a unary operator node*/
ST make_unop(char c, ST a)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));
  
  /*Sets the value of the node attributes and returns the node*/
  p->tag = UNOP;
  p->u.unop.op = c;
  p->u.unop.arg = a;
  if(debug) 
  {
    printf("UNOP %c created\n",c);
  }
  return p;
}

/*Function that makes a binary operator node*/
ST make_binop(ST a1, char c, ST a2)
{
  /*Creates the node and allocates memory*/
  ST p;
  p = (ST)malloc(sizeof(ST_NODE));

  /*Sets the value of the node attributes and returns the node*/
  p->tag = BINOP;
  p->u.binop.op = c;
  p->u.binop.arg1 = a1;
  p->u.binop.arg2 = a2;
  if(debug) 
  {
    printf("BINOP %c created\n",c);
  }
  return p;
}

