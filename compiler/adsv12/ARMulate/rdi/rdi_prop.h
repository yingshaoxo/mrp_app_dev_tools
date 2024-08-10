/* -*-C-*-
 *
 * $Revision: 1.4.12.3 $
 *   $Author: dsinclai $
 *     $Date: 2001/07/24 14:46:02 $
 *
 * Copyright (c) ARM Limited 1999.  All Rights Reserved.
 *
 * filename - rdi_prop.h
 */

#ifndef rdi_prop_h
#define rdi_prop_h


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
extern "C" {
#endif

/*
 * RDI Properties - target specific properties, controlled over RDI
 *
 * RDI Properties are new in RDI 1.51
 */

/*
 * Typedef: RDIProperty_GroupDesc
 * Purpose: Each module (processor or agent) can have a separate set or
 *          groups associated with it.  Each set of groups can be arranged as
 *          a non cyclic tree.
 */
typedef struct RDIProperty_GroupDesc {
  unsigned id;              /* ID must be unique over all groups in a module */
  char name[60];            /* Brief name of the group */
  char description[128];    /* Slightly longer description of group */
  int numProperties;        /* number of properties in this group */
  int numSubGroups;         /* number of subgroups in this group */
} RDIProperty_GroupDesc;

/*
 * Certain group IDs are pre-defined (and used by the debug controller to
 * extract further group IDs)
 */
#define RDIPropertyGroup_SuperGroup 0
#define RDIPropertyGroup_TopLevelGroup 1

/*
 * Typedef: RDIProperty_DisplayType
 * Purpose: These are display oriented types, which allow a GUI debugger to
 *          display suitable properties in a user friendly GUI way.  A
 *          debug controller can always ignore this and just display as the
 *          fundamental type instead.
 *
 * RDIProprty_PDT_Standard: no special GUI method can be used for this property
 *                          RDIProperty_GetPropertyDisplayDetails returns no
 *                          data, and the value RDIError_NoError.
 *
 * No other cases are defined yet.

 */
typedef enum RDIProperty_DisplayType
{
  RDIProperty_PDT_Standard = 0     /* No special display options possible */
} RDIProperty_DisplayType;


/* Property description structure
 *
 * Note that a property can be treated, as either numeric or a string or
 * even both! (Both is intended as a useful feature not just an oddity, eg.
 * for a variable which holds the type of a Cache (I or D), both numeric
 * (0, 1) and string ("I-Cache", "D-Cache") may be appropriate.  If a
 * variable advertises the capability of both string and numerical visibility,
 * the debug controller should by default use the string view if it is able to.
 *
 * This property description contains an ID which both uniquely identifies
 * a property, and distinguishes between "RDI defined" (or standard) 
 * and "RDI extension" (or non-standard) properties, as defined below:
 *
 * RDI defined properties are these are ones which
 * need to be understood by a debug controller eg. the command line
 * property needs to be understood by a debug controller (whereas most
 * properties mean nothing to the debug controller, (only to the debug agent
 * and user).
 *
 * RDI extension properties have the top bit set, and are intended for use
 * by 3rd parties to add new properties without having to
 * refer to ARM to do so.  Should one of these ever need to be known
 * about by a debug controller then the 3rd party should contact ARM and get
 * an RDI property allocated, since only then is it certain that a
 * debugger which uses that id will always be referring to the
 * intended property (rather than something another third party
 * has added).
 *
 * Note also that the id of a property must be unique for the module
 * handle.  The property names must also be unique within a group.
 * An implication of this is that in order for a debugger to be able
 * to use the "flat" list of properties provided when asking for all
 * the properites in the supergroup, the Debug Agent should ensure
 * that all property names in the "supergroup" are unique, which could
 * mean pre / post fixing to some of the names which would otherwise
 * have been identical.
 *
 * Note: bool_int is defined in host.h to be an integer
 */
typedef struct RDIProperty_Desc
{
  char name[60];          /* The name of the property - no spaces in name */
  char description[128];  /* A short description of the variable's function */
  unsigned32 id;          /* The id of a property is used to identify that
                           * property when setting / getting its value etc.
                           * The property id is a 32 bit unsigned integer
                           * which is split as follows:
                           * id[31] = 0 for RDI defined properies - see above
                           *        = 1 for extension properties - see above
                           * id[30:16] = 0  (currently reserved)
                           * id[15:0] = identifer for this property.  For
                           *            standard properties this value is
                           *            defined by the RDI; for non-standard
                           *            properties it may be any unique value
                           *            for that module handle.
                           */

  bool_int asString;      /* Can this property be treated as a string ? */
  unsigned maxLen;        /* if asString==1 then the maximum number of
                           * characters the property value may be set to
                           */
  bool_int asNumeric;     /* Can this property be treated as a numeric ? */
  bool_int isSigned;      /* if asNumeric==1 whether it is a signed numeric */
  unsigned width;         /* if asNumeric==1 the width in bits of the value
                           * Note that not all debug agents or debug hosts
                           * will be able to cope with all values.  For
                           * standard numeric properties use width=32
                           */
  unsigned readOnly;      /* 0 => read / write, otherwise read only */
  bool_int monotonicIncreasing;
                          /* 1 => This property is a monotonically
                           * increasing numerical value, and so it
                           * displaying differences between consecutive
                           * reads is useful - eg. time, cycle counts
                           */
  bool_int traceable;     /* 1 => This property is "traceable" in that it
                           * is useful to display consecutive values of it
                           * next to each other to produce a trace of values.
                           */

  RDIProperty_DisplayType display;
                          /* Display type for this property */
} RDIProperty_Desc;


/*
 * Function: RDI_InfoProc(mh, RDIInfo_Property, arg1, arg2)
 *
 *  Version: RDI 1.51
 *
 *  Purpose: Inquire whether RDI properties are supported by this target
 *
 *           This must be called before any other Property related info
 *           calls are made, and they will only be supported if this returns
 *           RDIError_NoError.
 *
 *  Returns: RDIError_NoError               Properties supported
 *           RDIError_UnimplementedMessage  Properties not supported
 */
#define RDIInfo_Properties 0x700 /* Sanity check - duplicated in rdi_info.h */

/*
 * Function: RDI_InfoProc(mh, RDIProperty_RequestGroups, arg1, arg2)
 *
 *  Version: RDI 1.51
 *
 *  Purpose: Called to get the group descriptions of groups within the
 *           specified group.  The data returned also gives how many properties
 *           and groups are in the requested groups (so that further calls can
 *           be made and buffers of the appropriate sizes passed in.
 *
 *  Params:
 *      Input:  mh     handle identifies the debug agent / processor
 *
 *      Input:  type   RDIProperty_RequestGroups
 *
 *      Input:  arg1   unsigned groupId
 *
 *                     If groupId == RDIPropertyGroup_SuperGroup:
 *                       This is a request for information about the 
 *                       "supergroup", a group which contains ALL properties
 *                       for this module.  It always exists.  Only one
 *                       RDIProperty_GroupDesc is written to in this case.  On
 *                       return this will contain the total number of
 *                       properties for this module.  Additionally, it will
 *                       contain the number of groups there are in the top
 *                       level group (RDIPropertyGroup_TopLevelGroup) for this
 *                       module.
 *
 *                     If groupId == RDIPropertyGroup_TopLevelGroup:
 *                       This is a request for information about the groups
 *                       contained in the top level.  The number of group
 *                       descriptions written to will be the number of groups
 *                       returned when this call was made with a groupId of
 *                       RDIPropertyGroup_SuperGroup.  No properties are
 *                       permitted in the top level group.
 *
 *                     If groupId == something else:
 *                       This is a request for the list of group
 *                       descriptions for the groups which are a subset of the
 *                       specified groupId.  The number of groups which will
 *                       be returned will already be known (from a previous
 *                       call to this fucntion).
 *
 *     In/Out:  arg2   RDIProperty_GroupDesc *descs
 *                     This is a buffer provided by the caller, which is large
 *                     enough to have a number of group descriptions filled
 *                     in, as determined  by the value of groupId and previous
 *                     calls.
 *
 *  Returns:
 *      OK:     RDIError_NoError
 *      OK:     RDIError_UnimplementedMessage - not understood
 *      Error:  RDIError_NoSuchHandle         - bad group id passed in
 */
#define RDIProperty_RequestGroups 0x701


/*
 * Function: RDI_InfoProc(mh, RDIProperty_RequestDescriptions, arg1, arg2)
 *
 *  Version: RDI 1.51
 *
 *  Purpose: Called to find out what RDI properties there are for this
 *           Processor / Debug Agent in the specified group.
 *
 *           Typically RDIProperty_RequestGroups would be called first
 *           to find out how many properties are in this group so that
 *           a buffer of sufficient size can be passed in.
 *
 *  Params:
 *      Input:  mh     handle identifies debug agent / processor for this
 *                     request.
 *
 *      Input:  type   RDIProperty_RequestDescriptions
 *
 *      Input:  unsigned groupId
 *                     If groupId == RDIPropertyGroup_SuperGroup:
 *                       This is a request for description of ALL
 *                       the properties in all groups for this module.
 *
 *                     If groupId == RDIPropertyGroup_TopLevelGroup:
 *                        This will return no data, since the top level group
 *                        may only contain groups.
 *
 *                     If groupId == something else:
 *                        Any other value must be a valid group id as returned
 *                        in one of the group descriptions returned by
 *                        RDIProperty_RequestGroups
 *
 *     In/Out:  arg2   RDIProperty_Desc *array.
 *                     An array of sufficient size for the number of properties
 *                     in this group.  The array will be filled in by this
 *                     function with the property descriptions.
 *
 *  Returns:
 *      OK:     RDIError_NoError
 *      Error:  RDIError_UnimplementedMessage - not understood
 *      Error:  RDIError_NoSuchHandle         - bad group id passed in
 */
#define RDIProperty_RequestDescriptions 0x702


/*
 * Function: RDI_InfoProc(mh, RDIProperty_GetAsNumeric, arg1, arg2)
 * 
 *  Version: RDI 1.51
 *
 *  Purpose: Get the numeric value of the specified property.
 *
 *           This is only supported for a particular module/agent handle
 *           and property id, if that property had indicated it supports
 *           'asNumeric' access.
 *
 *   Params:
 *       Input: mh      handle identifies agent/processor
 *
 *              type    RDIProperty_GetAsNumeric
 *
 *              arg1    unsigned32 id
 *                      Property id as returned in the RDIProperty_Desc struct.
 *
 *      In/Out: arg2    void *buf
 *                      The caller passes in a buffer of sufficient size for
 *                      all allowed values of the property to be written into
 *                      the buffer.
 *                      The size of the buffer required is all specified
 *                      by the width field in the RDIProperty_Desc struct.
 *   
 *   Returns:   RDIError_NoError - no error
 *              RDIError_NoSuchHandle - the property was specified incorrectly
 *                       (ie. the one specified does not exist).
 *              RDIError_Unset - the property should currently be greyed out.
 *                       However, a value is still returned in case greying out
 *                       is not supported.
 */
#define RDIProperty_GetAsNumeric 0x703


/*
 * Function: RDI_InfoProc(mh, RDIProperty_GetAsString, arg1, arg2)
 * 
 *  Version: RDI 1.51
 *
 *  Purpose: Get the string value of the specified property
 *
 *           This is only supported for a particular module/agent handle
 *           and property id, if that property had indicated it supports
 *           'asString' access.
 *
 *   Params:
 *       Input: mh      handle identifies agent/processor
 *
 *              type    RDIProperty_GetAsString
 *
 *              arg1    unsigned32 id
 *                      Property id as returned in the RDIProperty_Desc struct.
 *
 *      In/Out: arg2    void *buf
 *                      The caller passes in a buffer of sufficient size for
 *                      all allowed values of the property to be written into
 *                      the buffer.
 *                      The size of the buffer required is all specified
 *                      by the maxLen field in the RDIProperty_Desc struct.
 *   
 *   Returns:   RDIError_NoError - no error
 *              RDIError_NoSuchHandle - the property was specified incorrectly
 *                       (ie. the one specified does not exist).
 *              RDIError_Unset - the property should currently be greyed out.
 *                       However, a value is still returned in case greying out
 *                       is not supported.
 */
#define RDIProperty_GetAsString 0x704


/*
 * Function: RDI_InfoProc(mh, RDIProperty_SetAsNumeric, arg1, arg2)
 * 
 *  Version: RDI 1.51
 *
 *  Purpose: Set the numeric value of the specified property.
 *
 *           This is only supported for a particular module/agent handle
 *           and property id, if that property had indicated it supports
 *           'asNumeric' access.
 *
 *           Note that all properties in the same group are considered
 *           related, so that if any property in a group is changed then all
 *           others in that group should be reread.  This allows several
 *           properties in a group to be dependent on each other.
 *
 *           If the debug controller is offering a flat display of properties,
 *           using the RDIPropertyGroup_SuperGroup group, then it should
 *           re-read all properties whenever a single property gets set.
 *
 *   Params:
 *       Input: mh      handle identifies debug agent / processor / thread
 *
 *              type    RDIProperty_SetAsNumeric
 *
 *              arg1    unsigned32 id
 *                      Property id as returned in the RDIProperty_Desc struct.
 *
 *      In/Out: arg2    void *buf
 *                      The caller passes in a buffer containing the new value
 *                      for the specified property.
 *   
 *   Returns:
 *         OK:  RDIError_NoError - no error
 *       Error: RDIError_NoSuchHandle - the property was specified incorrectly
 *                                      (ie. the one specified does not exist).
 *       Error: RDIError_ReadOnly - this variable is readonly - request ignored
 *       Error: RDIError_BadValue - value specified was illegal for this
 *                                  variable request ignored.
 */
#define RDIProperty_SetAsNumeric 0x705


/*
 * Function: RDI_InfoProc(mh, RDIProperty_SetAsString, arg1, arg2)
 * 
 *  Version: RDI 1.51
 *
 *  Purpose: Set the string value of the specified property.
 *
 *           This is only supported for a particular module/agent handle
 *           and property id, if that property had indicated it supports
 *           'asString' access.
 *
 *           Note that all properties in the same group are considered
 *           related, so that if any property in a group is changed then all
 *           others in that group should be reread.  This allows several
 *           properties in a group to be dependent on each other.
 *
 *           If the debug controller is offering a flat display of properties,
 *           using the RDIPropertyGroup_SuperGroup group, then it should
 *           re-read all properties whenever a single property gets set.
 *
 *   Params:
 *       Input: mh      handle identifies debug agent / processor / thread
 *
 *              type    RDIProperty_SetAsString
 *
 *              arg1    unsigned32 id
 *                      Property id as returned in the RDIProperty_Desc struct.
 *
 *      In/Out: arg2    void *buf
 *                      The caller passes in a buffer containing the new value
 *                      for the specified property. Note that the string must
 *                      not be longer that the maxLen field in the
 *                      RDIProperty_Desc struct.
 *   
 *   Returns:
 *         OK:  RDIError_NoError - no error
 *       Error: RDIError_NoSuchHandle - the property was specified incorrectly
 *                                      (ie. the one specified does not exist).
 *       Error: RDIError_ReadOnly - this variable is readonly - request ignored
 *       Error: RDIError_BadValue - value specified was illegal for this
 *                                  variable request ignored.
 */
#define RDIProperty_SetAsString 0x706


/*
 * Function:    RDI_InfoProc(mh, RDIProperty_GetPropertyDisplayDetails,
 *              arg1, arg2)
 * 
 *  Version:    RDI 1.51
 *
 *  Purpose:    Get display details for the specified property.
 *
 *              This is only supported for a particular module/agent handle
 *              if RDIProperty_RequestCategories returns RDIError_NoError,
 *              and even then support of this function is optional.
 *
 *   Params:
 *       Input: mh      handle identifies agent/processor
 *
 *              type    RDIProperty_GetPropertyDisplayDetails
 *
 *              arg1    RDIProperty_Desc *prop
 *                      Contains property identification information.
 *
 *      In/Out: arg2    void *buf
 *                      The caller passes in a buffer of sufficient size for
 *                      whatever property display details are to be returned.
 *                      How much space will be required and what gets returned
 *                      depends entirely on the RDIProperty_DisplayType
 *                      for the specified property.  For details see the
 *                      descriptions of the various property display types,
 *                      given with the RDIProperty_DisplayType enumeration.
 *                     
 *   
 *   Returns:   RDIError_NoError - no error
 *              RDIError_NoSuchHandle - the property was specified incorrectly
 *                       (ie. the one specified does not exist).
 *              RDIError_BufferFull - the buffer passed was not big enough.
 */
#define RDI_GetPropertyDisplayDetails 0x707

/*
 * End of RDI Properties
 */


/* RESERVED PROPERTY-ID VALUES */
/*
 * 1 .. 8 are ARMSignal_IRQ thru ARMSignal_WaitForInterrupt -
 *   see armulif/armsignal.h in the plugin-model kit.
 *  ARMSignal_IRQ = 1,
 *  ARMSignal_FIQ,
 *  ARMSignal_Reset,
 *  ARMSignal_BigEnd,
 *  ARMSignal_HighException,
 *  ARMSignal_BranchPredictEnable,
 *  ARMSignal_LDRSetTBITDisable,
 *  ARMSignal_WaitForInterrupt = 8
 *
 * See also rdi_priv.h for  ARMulProp_* (values 100+)
 */


#if defined(__cplusplus) && !defined(CLX_CPP_LINKAGE)
}
#endif

#endif /* !def rdi_prop_h */

/* EOF rdi_prop.h */
