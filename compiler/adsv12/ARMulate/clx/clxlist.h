/* -*-C-*-
 *
 * $Revision: 1.2 $
 *   $Author: dsinclai $
 *     $Date: 1999/12/08 18:28:07 $
 *
 * Copyright (c) 1999 ARM Limited
 * All Rights Reserved
 *
 * clxlist.h - definitions of list manipulation routines
 */
#ifndef clxlist__h
#define clxlist__h


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif


/*
 * if the first field in any linked-list structure is the pointer to
 * the next list member, then with the use of some unpleasant casts,
 * we can provide a generalised set of routines for list manipulation
 */
typedef struct ClxList
{
    struct ClxList *li_next;
} ClxList;

/**********************************************************************/

/*
 *  Function: ClxList_InsertAtHead
 *   Purpose: Add a new ClxList element to an existing linked ClxList member
 *
 *    Params:
 *      In/Out: prev    The ClxList member for new element to be inserted after
 *
 *              newel   The element to be added
 *
 *   Returns: The address of the new element
 */
extern ClxList *ClxList_InsertAtHead(ClxList *prev, ClxList *newel);

/*
 *  Function: ClxList_Append
 *   Purpose: Add a new ClxList element to the tail of a linked ClxList
 *
 *    Params:
 *      In/Out: head    The ClxList for new element to be added to
 *
 *              new     The element to be added
 *
 *   Returns: The address of the new element
 */
extern ClxList *ClxList_Append(ClxList *head, ClxList *newel);

/*
 *  Function: ClxList_CreateMemberAtHead
 *   Purpose: Create a new ClxList member, chain it to the ClxList head
 *
 *    Params:
 *       Input: head    The ClxList for new member to be added to
 *
 *              size    The space required for a ClxList member
 *
 *   Returns:
 *          OK: Pointer to new ClxList member
 *       Error: NULL
 *
 * Post Conditions: New member will be initialised to zero
 */
extern ClxList *ClxList_CreateMemberAtHead(ClxList *head, unsigned int size);

/*
 *  Function: ClxList_CreateMemberAtTail
 *   Purpose: Create a new ClxList member, chain it to the ClxList tail
 *
 *    Params:
 *       Input: head    The ClxList for new member to be added to
 *
 *              size    The space required for a ClxList member
 *
 *   Returns:
 *          OK: Pointer to new ClxList member
 *       Error: NULL
 *
 * Post Conditions: New member will be initialised to zero
 */
extern ClxList *ClxList_CreateMemberAtTail(ClxList *head, unsigned int size);

/*
 * Functions: ClxListElementDestructorProc
 *  Purpose: Destroy a single element of a ClxList, during a call to ClxList_Destroy.
 *
 * Params: Inout: li - address of ClxList element to clear and free.
 */
typedef void ClxListElementDestructorProc(ClxList *li);

/*
 *  Function: ClxList_Destroy
 *   Purpose: Destroy a linked ClxList, possibly free'ing ClxList members
 *              along the way.
 *
 *    Params:
 *       Input: tidy    If non-NULL then used as a function to tidy the
 *                      ClxList member before it is destroyed.  If tidy is
 *                      called, then it is responsible for free'ing the
 *                      member, otherwise this routine will call free().
 *
 *      In/Out: head    The ClxList to be destroyed
 *
 *   Returns: Nothing
 */
extern void ClxList_Destroy(ClxList *head, void (*const tidy)(ClxList *li));

/*
 *  Function: ClxList_RemoveEntry
 *   Purpose: remove an old entry from a linked ClxList
 *
 *    Params:
 *      In/Out: head    The ClxList for the entry to be removed from
 *
 *              old     The entry to be removed
 *
 *   Returns:   Ok: 0
 *            Fail: 164  (RDIError_BadValue)
 */
extern unsigned ClxList_RemoveEntry(ClxList *head, ClxList *old);

/*
 *  Function: ClxList_RemoveHead
 *   Purpose: remove the head element from the ClxList, and return it
 *
 *    Params:
 *      In/Out: head    The ClxList for the entry to be removed from
 *
 *   Returns: old ClxList head, NULL if none
 */
extern ClxList *ClxList_RemoveHead(ClxList *head);

#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* ndef clxlist__h */

/* EOF ClxLists.h */
