;; ARMulator configuration file type 3
;; Copyright (c) 1996-2001 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.1.2.5 $
;; Checkin $Date: 2001/09/04 14:27:41 $
;; Revising $Author: lmacgreg $



{ Processors

{ARM925T=ARM9TDMI-REV1
;Formerly =ARM9TDMIr1 : Retrieve that if '9TDMI diverges.
meta_moduleserver_processor=ARM925T
HASMMU=True
Memory=ARM925T
}

}
;End of Processors.


{ Memories
;;;#if MEMORY_ARM925CacheMMU
;; The "ARM925CacheMMU" model provides the cache/MMU model for the ARM925T
ARM925T=ARM925CacheMMU

{ ARM925CacheMMU

Config=Enhanced

ICache_Associativity=2
DCache_Associativity=2
ICache_Lines=1024
DCache_Lines=512
ChipNumber=0x925
Revision=0
ProcessId

;; Clock speed
CCCFG=0

Memory=Default
}
;;;#endif
;; end of memories
}


;EOF arm925.dsc