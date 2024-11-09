#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tgumbo.h"
#include "gumbo.h"

void Tclgunbo_Thread_Exit(ClientData clientdata)
{
  ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

  if(tsdPtr->gumbo_hashtblPtr) {
    Tcl_DeleteHashTable(tsdPtr->gumbo_hashtblPtr);
    ckfree(tsdPtr->gumbo_hashtblPtr);
  }
}


int tcl_gumbo_parse (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{    
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    char *htmltext = NULL;
    Tcl_Size len = 0;
    char handleName[16 + TCL_INTEGER_SPACE];
    int newvalue;
    GumboOutput* output;    
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }
    
    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "HtmlString");
        return TCL_ERROR;
    }

    htmltext = Tcl_GetStringFromObj(obj[1], &len);
    if(!htmltext || len < 1) {
        return TCL_ERROR;
    }
    output = gumbo_parse(htmltext);        
    
    Tcl_MutexLock(&myMutex);
    sprintf( handleName, "gumbo%d", tsdPtr->gumbo_count++ );   
        
    returnvalue = Tcl_NewStringObj( handleName, -1 );
    hashEntryPtr = Tcl_CreateHashEntry(tsdPtr->gumbo_hashtblPtr, handleName, &newvalue);
    Tcl_SetHashValue(hashEntryPtr, output);
    Tcl_MutexUnlock(&myMutex);

    Tcl_SetObjResult(interp, returnvalue);     
        
    return TCL_OK;
}


int tcl_gumbo_destroy_output (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{    
    Tcl_HashEntry *hashEntryPtr;
    char *handle;
    Tcl_Size len;
    GumboOutput* output;    
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_output");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    output = Tcl_GetHashValue( hashEntryPtr );  
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    if(hashEntryPtr) Tcl_DeleteHashEntry(hashEntryPtr);

    return TCL_OK;
}


int tcl_gumbo_output_get_root (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;  
    GumboOutput* output;
    GumboNode* root;
    char handleName[16 + TCL_INTEGER_SPACE];
    char *handle;
    Tcl_Size len;
    int newvalue;
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_output");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo output handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    output = (GumboOutput*) Tcl_GetHashValue( hashEntryPtr );
    root = output->root;

    Tcl_MutexLock(&myMutex);  
    sprintf( handleName, "gumbo_root%d", tsdPtr->gumbo_root_count++); 

    returnvalue = Tcl_NewStringObj( handleName, -1 );
    hashEntryPtr = Tcl_CreateHashEntry(tsdPtr->gumbo_hashtblPtr, handleName, &newvalue);
    Tcl_SetHashValue(hashEntryPtr, root);
    Tcl_MutexUnlock(&myMutex);

    Tcl_SetObjResult(interp, returnvalue);
    

    return TCL_OK;
}


int tcl_gumbo_node_get_type (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    long nodetype;

    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );

            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );  
    
    nodetype = node->type;    
    
    
    returnvalue = Tcl_NewLongObj(nodetype);    
    Tcl_SetObjResult(interp, returnvalue);
    
    return TCL_OK;  
}


int tcl_gumbo_element_get_tag_name (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    const char *tag_name;

    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );

            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );  
    
    if ( node->type != GUMBO_NODE_ELEMENT ) {
        return TCL_ERROR;
    }    


    tag_name = gumbo_normalized_tagname(node->v.element.tag);
    
    returnvalue = Tcl_NewStringObj( tag_name, -1 );
    Tcl_SetObjResult(interp, returnvalue);
    
    
    return TCL_OK;  
}


int tcl_gumbo_element_get_children (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *listobj;  
    Tcl_Obj *returnvalue;  
    GumboNode *node;
    char handleName[16 + TCL_INTEGER_SPACE];
    char *handle;
    Tcl_Size len;
    int newvalue;

    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }
    
    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );
    
    if ( node->type != GUMBO_NODE_ELEMENT ) {
        return TCL_ERROR;
    }     
        
    listobj = Tcl_NewListObj(0, NULL);
    
    GumboVector *children = &node->v.element.children;
    for ( int i = 0; i < children->length; i++ ) {
        GumboNode *children_node = children->data[i];
        
        Tcl_MutexLock(&myMutex);  
        sprintf( handleName, "gumbo_node%d", tsdPtr->gumbo_node_count++);
        returnvalue = (Tcl_Obj *) Tcl_NewStringObj( handleName, -1 );

        Tcl_ListObjAppendElement(interp, listobj, returnvalue);

        hashEntryPtr = Tcl_CreateHashEntry(tsdPtr->gumbo_hashtblPtr, handleName, &newvalue);
        Tcl_SetHashValue(hashEntryPtr, children_node);
        Tcl_MutexUnlock(&myMutex); 
    }        

    Tcl_SetObjResult(interp, listobj);    
    return TCL_OK;  
}


int tcl_gumbo_text_get_text (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    const char *text;
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );  
    if ( node->type != GUMBO_NODE_TEXT ) {
        return TCL_ERROR;
    }

    text = node->v.text.text;
    
    returnvalue = Tcl_NewStringObj( text, -1 );
    Tcl_SetObjResult(interp, returnvalue);
    
    
    return TCL_OK; 
}


int tcl_gumbo_element_get_attributes (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;    
    Tcl_Obj *returnvalue, *nameobj, *valueobj;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );     
    if ( node->type != GUMBO_NODE_ELEMENT ) {
        return TCL_ERROR;
    }    
    
    returnvalue = Tcl_NewStringObj( "attribute", -1 );
    
    GumboVector * attributes = &node->v.element.attributes;
    for ( int i = 0; i < attributes->length; i++ ) {
        GumboAttribute * attribute = attributes->data[ i ];

        nameobj = Tcl_NewStringObj(  attribute->name, -1 );
        valueobj = Tcl_NewStringObj(  attribute->value, -1 );
        Tcl_ObjSetVar2 (interp, returnvalue, nameobj, valueobj, 0);
    }    
    
    Tcl_SetObjResult(interp, returnvalue);
    return TCL_OK;  
}


int tcl_gumbo_element_get_tag_open (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );
    if ( node->type != GUMBO_NODE_ELEMENT ) {
        return TCL_ERROR;
    }

    GumboElement *element = &node->v.element;
    GumboStringPiece *openTag = &element->original_tag;
    returnvalue = Tcl_NewStringObj( openTag->data, -1 );
    Tcl_SetObjResult(interp, returnvalue);
      
    return TCL_OK;  
}


int tcl_gumbo_element_get_tag_close (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *returnvalue;    
    GumboNode *node;
    char *handle;
    Tcl_Size len;
    
    ThreadSpecificData *tsdPtr = (ThreadSpecificData *)
      Tcl_GetThreadData(&dataKey, sizeof(ThreadSpecificData));

    if (tsdPtr->initialized == 0) {
      tsdPtr->initialized = 1;
      tsdPtr->gumbo_hashtblPtr = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(tsdPtr->gumbo_hashtblPtr, TCL_STRING_KEYS);
    }

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "Gumbo_node");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( tsdPtr->gumbo_hashtblPtr, handle );    
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid gumbo node handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    node = (GumboNode *) Tcl_GetHashValue( hashEntryPtr );  
    if ( node->type != GUMBO_NODE_ELEMENT ) {
        return TCL_ERROR;
    }    

    GumboElement * element = &node->v.element;
    GumboStringPiece * closeTag = &element->original_end_tag;
    
    returnvalue = Tcl_NewStringObj( closeTag->data, -1 );
    Tcl_SetObjResult(interp, returnvalue);
    
    return TCL_OK;  
}
