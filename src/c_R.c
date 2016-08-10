
#include "igraph.h"
#include "igraph_error.h"

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>

//#include "rinterface.h"
//#include "config.h"


int R_SEXP_to_igraph_vs(SEXP rit, igraph_t *graph, igraph_vs_t *it) {
  
  igraph_vector_t *tmpv=(igraph_vector_t*)R_alloc(1,sizeof(igraph_vector_t));
  igraph_vs_vector(it, igraph_vector_view(tmpv, REAL(rit), 
					  GET_LENGTH(rit)));
  return 0;
}

int R_SEXP_to_vector(SEXP sv, igraph_vector_t *v) {
  v->stor_begin=REAL(sv);
  v->stor_end=v->stor_begin+GET_LENGTH(sv);
  v->end=v->stor_end;
  return 0;
}

int R_SEXP_to_igraph(SEXP graph, igraph_t *res) {
  
  res->n=(igraph_integer_t) REAL(VECTOR_ELT(graph, 0))[0];
  res->directed=LOGICAL(VECTOR_ELT(graph, 1))[0];
  R_SEXP_to_vector(VECTOR_ELT(graph, 2), &res->from);
  R_SEXP_to_vector(VECTOR_ELT(graph, 3), &res->to);
  R_SEXP_to_vector(VECTOR_ELT(graph, 4), &res->oi);
  R_SEXP_to_vector(VECTOR_ELT(graph, 5), &res->ii);
  R_SEXP_to_vector(VECTOR_ELT(graph, 6), &res->os);
  R_SEXP_to_vector(VECTOR_ELT(graph, 7), &res->is);
  
  /* attributes */
  REAL(VECTOR_ELT(VECTOR_ELT(graph, 8), 0))[0] = 1; /* R objects refcount */
  REAL(VECTOR_ELT(VECTOR_ELT(graph, 8), 0))[1] = 0; /* igraph_t objects */
  res->attr=VECTOR_ELT(graph, 8);
  
  return 0;
}


int igraph_expectedforce_opt(const igraph_t *graph, igraph_vector_t *ans,igraph_vs_t vids, igraph_bool_t directed,igraph_neimode_t mode, const igraph_vector_t *weights,igraph_bool_t calc_hub);


SEXP R_igraph_expectedforce(SEXP graph, SEXP pvids, SEXP pdirected,
			  SEXP pmode, SEXP weights, SEXP pcalc_hob) {

	igraph_t g;
	igraph_vs_t vs;
	igraph_bool_t directed=LOGICAL(pdirected)[0];
	igraph_bool_t calc_hob=LOGICAL(pcalc_hob)[0];
	igraph_integer_t mode=(igraph_integer_t) REAL(pmode)[0];
	igraph_vector_t res;
	igraph_vector_t v_weights, *pweights=0;
	SEXP result;

	R_SEXP_to_igraph(graph, &g);
	R_SEXP_to_igraph_vs(pvids, &g, &vs);
	igraph_vector_init(&res, 0);
	if (!isNull(weights)) {
		pweights=&v_weights; R_SEXP_to_vector(weights, &v_weights);
	}
	igraph_expectedforce_opt(&g, &res, vs, directed, (igraph_neimode_t) mode, pweights, calc_hob);

	PROTECT(result=NEW_NUMERIC(igraph_vector_size(&res)));
	igraph_vector_copy_to(&res, REAL(result));
	igraph_vector_destroy(&res);
	igraph_vs_destroy(&vs);

	UNPROTECT(1);
	return result;
}


