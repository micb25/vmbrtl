#ifndef __fpar_h_
#define __fpar_h_

#define scalar_t double

/* special tokens */
#define TERM_TOK -1
#define END_TOK -2
#define NUM_TOK -3
#define NO_TOK -4


/* the function tree will consist of node of the following type */
typedef struct node_t_s {
  scalar_t value;           /* possibly value of this partial tree */
  int type;                 /* what kind of operation it is (number of term) */
  int valid;                /* is the value valid? */
  struct node_t_s **childs; /* NULL terminated array of child-nodes */
} node_t;

/*left or right associativity */
enum assoc { left, right };

/* every variable of the following type represents a possible opertion */
typedef struct {
  char *syntax;
  enum assoc asc;
  void (*calc_value) (node_t *n);
  /* the function, which realizes this operation. the children of the node
     must have their values in this case */
} term_t;

/* the syntax of the operations is as follows: The character # represents an
   arbitrary sub-expression. Example:
   #*#   -> multiplication
   #-#   -> subtraction
   (#)   -> brackets
   [#;#] -> derivation oerator
   ^#    -> negation
*/


char **get_tokens ();
int fpar_lexer (char **s, scalar_t *value);
node_t *fpar_parser (char **s, int depth, int *oldtok, scalar_t *oldval,
		     int *error);
void free_node_tree (node_t *n);
node_t *fpar_parserw (char *s);
//void tree_output (node_t *n, int currpos);
void calc_value (node_t *n);



#endif
