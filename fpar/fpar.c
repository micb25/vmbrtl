/* This library is under the BSD license

   Changelog
      3. 6.2001 created. Michael Becker <michael.www@ijon.de>
     25.11.2002 translated comments in c-file and reference to english (M.Becker)
     20. 1.2003 bugfix in get_tokens. Stanislaw Szymczyk <sszymczy@manta.univ.gda.pl>
                translated comments in h-file into english (M.Becker)
      4. 6.2003 bugfix: two missing error-tests. (M.Becker)
      7. 1.2004 bugfix in fpar_parser. bug found by Matthias Peters <m.peters@aicoss.de>
      
      03.09.2008: 
            an Versatile MB angepasst (Michael Böhme)
      14.03.2010:
            bugfix: additional free() in free_node_tree() (Michael Böhme)

   Known bugs
     - fpar_parser simply return NULL after a parse error without freeing
       the so far allocated structure.
       
   http://www.ijon.de/comp/libs/index.html
   
 */

#include "../rtl_mem.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "fpar.h"
#include "basefunc.h"
#include "../build_options.h"

/* global variables */

/* the operations. Every operation consists of a term, giving its 
   syntax. This term consists of one or more tokens and #-signs. */
term_t op[] = {
  { "#+#", left , base_add },  /* addition */
  { "#-#", left , base_sub },  /* subtraction */
  { "#*#", left , base_mult},  /* multiplication */
  { "#/#", left , base_div },  /* division */
  { "-#",  left , base_neg },  /* the negative */
//  { "(#/#)", right, base_iddiv  },  /* identity, but with high priority */
//  { "(#*#)", right, base_idmul  },  /* identity, but with high priority */
  { "(#)", right, base_id  },  /* identity, but with high priority */
  { NULL,  right, NULL }
};

int **term_tok;    /* list of tokens in term i.   */
int *term_tok_num; /* number of tokens in term i. */
char **tokens;     /* list of all tokens          */


/* initializes global variables. */
char **FPAR_get_tokens ()
{
  int num=0, i, j, k, termnum, toknum;
  char **res, *p, *q, tmpch, *tmpstr;

  int count_tokens (char *s)
    {
      int res=1;
      char *p;
      for (p=s; *p; p++)
	if (*p=='#') res++;
      if (*s=='#') res--;
      if ((p-1>s) && (*(p-1)=='#')) res--;
      return res;
    }

  /* how many terms are there? */
  for (termnum=0; op[termnum].syntax!=NULL; termnum++);
  term_tok = (int**)VMBAlloc(sizeof(int*)*termnum);
  term_tok_num = (int*)VMBAlloc(sizeof(int)*termnum);

  /* how many tokens are there? */
  for (i=0; op[i].syntax!=NULL; i++) {
    j=count_tokens (op[i].syntax);
    term_tok[i]=(int*)VMBAlloc(sizeof(int)*2*(j+1));
    term_tok_num[i]=2*(j+1);
    num += j;
  }
  res=(char**)VMBAlloc(sizeof(char*)*(num+1));

  /* ok. Now write the tokens into the global variables */
  toknum=0;
  for (i=0; op[i].syntax!=NULL; i++) {
    k=0;
    tmpstr=strdup(op[i].syntax);
    p=tmpstr;
    while (*p!='\0') {
      if (*p=='#') term_tok[i][k++]=TERM_TOK;
      while (*p=='#') p++;
      if (*p!='\0') {
	q=p;
	while ((*q!='#') && (*q!='\0')) q++;
	tmpch = *q;
	*q='\0';
        /* check, whether token is already existent. */
	for (j=0; j<toknum; j++) 
	  if (strcmp(res[j],p)==0) {
	    term_tok[i][k++]=j;
	    break;
	  }
	if (j==toknum) {
	  res[toknum]=strdup(p);
	  term_tok[i][k++]=toknum;
	  toknum++;
	}
	*q=tmpch;
	p=q;
      } /* if */
    } /* while */
    term_tok[i][k]=END_TOK;
    VMBFree(tmpstr);
  } /* for */
  tokens=res;
  return res;
}


/* The lexer returns the next token. *s is set forward behind this token.
   If the token is NUM_TOK, i.e. a numerical value, it is returned in 
   *value. */
int fpar_lexer (char **s, scalar_t *value)
{
  int i;
  char *endptr;

  /* remove blanks */
  while ((**s==' ')||(**s=='\t')) (*s)++;

  if (**s=='\0') return END_TOK;

  /* check, whether it is a string token */
  for (i=0; tokens[i]; i++)
    if (strncmp (*s, tokens[i], strlen(tokens[i]))==0) {
      (*s)+=strlen(tokens[i]);
      return i;
    }

  /* else it is a number, or invalid. */  
  *value=strtod (*s, &endptr);
  if (endptr==*s) return NO_TOK;
  *s=endptr;
  return NUM_TOK;
}

#define FPAR_ERR_NONUM -1
#define FPAR_ERR_SYNTAX -2
#define FPAR_ERR_NOTOK -3
#define FPAR_ERR_NOOP -4

/* the parser
     **s is the string to be parsed.
     oldtok ist the token, which has to be interpreted next. After the
        function returns, it is set to the next token to be interpreted.
     depth is the actual priority. Normally only operations of higher 
        priority have to be interpreted before the return.
*/
node_t *fpar_parser (char **s, int depth, int *oldtok, scalar_t *oldval,
		     int *error)
{
  node_t *res=NULL;
  node_t *oldres=NULL;
  int i=0, starti=0, termnum=0, nextdepth;

  /* If the token starts a operation, we can reset depth. */
  for (i=0; ((i<depth)&&(term_tok[i][0]!=*oldtok)); i++);
  depth = i;

  if (op[depth].syntax==NULL) { /* then it must be a numerical value */
    if (*oldtok==NUM_TOK) {
      res=(node_t*)VMBAlloc(sizeof(node_t));
      res->value=*oldval;
      res->valid=1;
      res->type=NUM_TOK;
    } else {
      *error = FPAR_ERR_NONUM;
    }
    /* get next token */
    *oldtok = fpar_lexer (s, oldval);
    if (*oldtok==NO_TOK) {
      *error = FPAR_ERR_NOTOK;
      return NULL;
    }
    return res;
  }

  /* it is not a numerical token. So we start the recursion */

  /* if the operation starts with a term, we fetch this term */
  if (term_tok[depth][0]==TERM_TOK) {
    oldres=fpar_parser (s, depth+1, oldtok, oldval, error);
    if (oldres==NULL) return NULL;
    starti++; /* starti is the first non NUM_TOK token of operation op[depth]*/
  }

  /* check, whether oldtok is the first non NUM_TOK token of the operation */
  if (*oldtok==term_tok[depth][starti]) { /* yes! */
    do {
      i=starti;
      res=(node_t*)VMBAlloc(sizeof(node_t));
      res->type = depth;
      res->childs = (node_t**)VMBAlloc(term_tok_num[depth]*sizeof(node_t*));
      if (i==1) res->childs[0]=oldres;
      termnum=i;

      /* get the other terms */
      while (term_tok[depth][i]!=END_TOK) {
	if (term_tok[depth][i]==TERM_TOK) {
	  if ( ((i==1)&&(term_tok[depth][0]==TERM_TOK)) ||
               (term_tok[depth][i+1]==END_TOK) )
	    nextdepth = depth + ( ((i==1)^(op[depth].asc==left)) ? 1 : 0 );
	  else nextdepth = 0;
	  res->childs[termnum] = fpar_parser (s, nextdepth, oldtok, 
						oldval, error);
          if (res->childs[termnum]==NULL) return NULL;
          termnum++;
	}
	else {
	  if (term_tok[depth][i]!=*oldtok) {
	    *error = FPAR_ERR_SYNTAX;
	    return NULL;
	  }
	  else {
	    *oldtok=fpar_lexer (s, oldval);
	  }
	}
	i++;
      } /* while (term_tok ... */
      oldres = res;
    } while ((starti==1) && (*oldtok == term_tok[depth][1]));
    /* because if we are left associative, there could still be other terms.*/
    return res;
  } /* if (*oldtok ... */
  else { /* it must be another operation */
    if (oldres) { /* we already have parsed something and it must be an
                     operation of lower priority. */
      if ((depth>0)||(*oldtok==END_TOK)) return oldres;
      /* else there a no operations remaining. we have a syntax error. */
      *error = FPAR_ERR_NOOP;
      return NULL;
    }
    else return fpar_parser (s, depth+1, oldtok, oldval, error);
  }

  return res;
}
/* simple example. Consider the string 1*2+3. 
   1. The function assumes, it is an addition. It tries to get the first 
      term of the addition calling itself recursively and only looking for
      operations of higher priority. 
   2. It assumes the first term is a multiplication. It tries to get the
      first term. ....
   [many recursive calls]
   3. It reas the numerical token 1 with value 1.
   [many recursive returns]
   4. Because the next token to be interpreted (variable oldtok) is a "*",
      the function sees, that it now has the first term of the assumed
      multiplication (of 2.).
   5. It tries to get the remaining terms of the mulitplication.
   [many recursive calls and many recursive returns.]
   6. Beacause the next token is the "+" (with lower priority than "*"),
      the function sees, that is now has the complete first term of the
      addition. 
   7. It tries to get the remaining terms of the addition.
   [...]
*/



/* frees the memory for a node_t-tree */
void free_node_tree (node_t *n)
{
  int i;
  if (n==NULL) 
     return;
  if (n->childs)
  {
    for (i=0; n->childs[i]; i++) 
        free_node_tree(n->childs[i]);
    VMBFree(n->childs);
  }
  VMBFree(n);
  return;
}


/* This is the function, the user should call to parse a string and write
   it into a node_t-tree. In case of an error, NULL is returned. 
   (fpar_parser-wrapper) */
node_t *fpar_parserw (char *s)
{
  int tok, err=0;
  scalar_t val;
  node_t *res;

  tok = fpar_lexer (&s, &val);
  res = fpar_parser (&s, 0, &tok, &val, &err);
  if (!err) return res;
  free_node_tree (res);
  return NULL;
}


/* tries to print the parsed tree onto stdout. Call with currpos==0 */

/* this is the witdth of the columns of the output.  */
#define TREEOUT_WIDTH 10 
void tree_output (node_t *n, int currpos)
{
  int i;

  if (!n) return;
  if (n->type==NUM_TOK) {
#ifdef DEBUG                        
    VMBDebug("% *f\n", TREEOUT_WIDTH, n->value);
#endif    
    return;
  }
#ifdef DEBUG                          
  VMBDebug("%*s", TREEOUT_WIDTH, op[n->type].syntax);
#endif  
  currpos+=TREEOUT_WIDTH;
  for (i=0; n->childs[i]; i++) {
#ifdef DEBUG                              
    if (i!=0) VMBDebug("%*s", currpos, " ");
#endif
    tree_output (n->childs[i], currpos);
  }
#ifdef DEBUG                          
  VMBDebug("");
#endif
  return;
} 

/* recursively goes down the tree and calculates the value. */
void calc_value (node_t *n)
{
  int i;
  if (n->type==NUM_TOK) return;
  n->valid=1;
  for (i=0; n->childs[i]; i++) {
    calc_value (n->childs[i]);
    if (!(n->childs[i]->valid)) n->valid=0;
  }
  if (n->valid) op[n->type].calc_value (n);
  return;
}
