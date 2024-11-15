#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgumbo.h"

/*
 *----------------------------------------------------------------------
 *
 * Tclgumbo_Init --
 *
 *	Initialize the new package. 
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The TclGumbo package is created.
 *
 *----------------------------------------------------------------------
 */

int
Tclgumbo_Init(Tcl_Interp *interp)
{
    Tcl_Obj *strValue;
    Tcl_Obj *setupValue;
    
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }
    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    //Tclgunbo_InitHashTable();
    
    /* 
     *   Tcl_GetThreadData handles the auto-initialization of all data in 
     *  the ThreadSpecificData to NULL at first time.
     */
    Tcl_MutexLock(&myMutex);    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
        Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
        tsdPtr->initialized = 1;
        tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
        Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
          
        tsdPtr->gumbo_count = 0;
        tsdPtr->gumbo_root_count = 0;
        tsdPtr->gumbo_node_count = 0;
    }
    Tcl_MutexUnlock(&myMutex);    
    
    /* Add a thread exit handler to delete hash table */
    Tcl_CreateThreadExitHandler(Tclgunbo_Thread_Exit, (ClientData)NULL);    
    
    Tcl_CreateObjCommand(interp, "::gumbo::parse", tcl_gumbo_parse, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::destroy_output", tcl_gumbo_destroy_output, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::output_get_root", tcl_gumbo_output_get_root, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::node_get_type", tcl_gumbo_node_get_type, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::element_get_tag_name", tcl_gumbo_element_get_tag_name, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::element_get_children", tcl_gumbo_element_get_children, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::text_get_text", tcl_gumbo_text_get_text, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::element_get_attributes", tcl_gumbo_element_get_attributes, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::element_get_tag_open", tcl_gumbo_element_get_tag_open, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::gumbo::element_get_tag_close", tcl_gumbo_element_get_tag_close, (ClientData) NULL, NULL);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_DOCUMENT", -1 );
    setupValue = Tcl_NewIntObj( 0 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_ELEMENT", -1 );
    setupValue = Tcl_NewIntObj( 1 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_TEXT", -1 );
    setupValue = Tcl_NewIntObj( 2 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_CDATA", -1 );
    setupValue = Tcl_NewIntObj( 3 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_COMMENT", -1 );
    setupValue = Tcl_NewIntObj( 4 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    
    strValue = Tcl_NewStringObj( "::gumbo::GUMBO_NODE_WHITESPACE", -1 );
    setupValue = Tcl_NewIntObj( 5 );
    Tcl_ObjSetVar2 (interp, strValue, NULL, setupValue, TCL_NAMESPACE_ONLY);
    

    return TCL_OK;
}
