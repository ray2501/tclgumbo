#ifndef _TCLGUMBO
#define _TCLGUMBO

#include <tcl.h>


/*
 * For C++ compilers, use extern "C"
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ThreadSpecificData {
  int initialized;                /* initialization flag */
  Tcl_HashTable *gumbo_hashtblPtr; /* per thread hash table. */
  int gumbo_count;
  int gumbo_root_count;
  int gumbo_node_count;
} ThreadSpecificData;

static Tcl_ThreadDataKey dataKey;    

TCL_DECLARE_MUTEX(myMutex);

/*
 * Only the _Init function is exported.
 */

extern DLLEXPORT int	Tclgumbo_Init(Tcl_Interp * interp);

int tcl_gumbo_parse (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_destroy_output (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_output_get_root (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_node_get_type (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_element_get_tag_name (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_element_get_children (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_text_get_text (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_element_get_attributes (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_element_get_tag_open (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int tcl_gumbo_element_get_tag_close (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);


//void Tclgunbo_InitHashTable _ANSI_ARGS_(());
void Tclgunbo_Thread_Exit(ClientData clientdata);


/*
 * end block for C++
 */

#ifdef __cplusplus
}
#endif

#endif /* _TCLGUMBO */
