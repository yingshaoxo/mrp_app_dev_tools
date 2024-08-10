;; ARMulator configuration file type 3
;; - XScale.dsc -
;; Copyright (c) 1996-2001 ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.4.2.3 $
;; Checkin $Date: 2001/08/28 14:51:13 $
;; Revising $Author: dsinclai $
 
;;
;; This is a non-user-edittable configuration file for ARMulator
;;



;;
;; This is the list of XScale processors supported by ARMISS.
;;

{ Processors

{XScale=Processors_Common_ARMISS
MODEL_DLL_FILENAME=ARMISS
ARMulator=XSCALE
Architecture=5TE
CP15_IDREGVALUE=0x69052000
LSU_Width=32
PFU_Width=32
BUS_Width=64

; This tells the main memory model to ignore CP15.
;ENDIANNESS=L
; L means main memory is fixed LittleEnd (R+W)
; C means listen to BIGEND from CP15.
ENDIANNESS=C

{RDIMSVR=RDIMSVR
target_controller_type=armulate
}

{ meta_moduleserver_processor=XScale
XScale_DSP=true
XScale_InterruptCtrl=true
XScale_Performance=true
XScale_SoftwareDebug=false
XScale_ClockAndPower=false
XScale_SystemControl=true
}

;Tell Pagetab how to treat XScale.
HASMMU=True


;End XScale
}

;End Processors
}

;; EOF XScale.dsc








