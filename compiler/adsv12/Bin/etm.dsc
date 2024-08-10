;; ARMulator configuration file type 3
;; - etm.dsc -
;; Copyright (c) 2000 - 2001  ARM Limited. All Rights Reserved.

;; RCS $Revision: 1.4.4.3 $
;; Checkin $Date: 2001/08/24 13:32:32 $
;; Revising $Author: lmacgreg $
 
;;
;; This is the configuration file for the ETM model 
;;

{ Processors

;; ARM7 with ETM

{ ARM7TDMI-ETM(L)=ARM7TDM
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=16
DataComp=8
Counters=4
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=45
}
}

{ ARM7TDMI-ETM(M)=ARM7TDM
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=8
DataComp=2
Counters=2
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=20
}
}

{ ARM7TDMI-ETM(S)=ARM7TDM
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=2
DataComp=0
Counters=1
Sequencer=0
MMDecode=0
FifoFULL=1
DepthFIFO=10
MaxPortWidth=8
}
}

;; ARM920 with ETM

{ ARM920T-ETM(L)=ARM920T
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=16
DataComp=8
Counters=4
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=45
}
}

{ ARM920T-ETM(M)=ARM920T
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=8
DataComp=2
Counters=2
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=20
}
}

{ ARM920T-ETM(S)=ARM920T
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=2
DataComp=0
Counters=1
Sequencer=0
MMDecode=0
FifoFULL=1
DepthFIFO=10
MaxPortWidth=8
}
}
;; ARM946 with ETM

{ ARM946E-S-ETM(L)=ARM946E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=16
DataComp=8
Counters=4
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=45
}
}

{ ARM946E-S-ETM(M)=ARM946E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=8
DataComp=2
Counters=2
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=20
}
}

{ ARM946E-S-ETM(S)=ARM946E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=2
DataComp=0
Counters=1
Sequencer=0
MMDecode=0
FifoFULL=1
DepthFIFO=10
MaxPortWidth=8
}
}
;; ARM966 with ETM

{ ARM966E-S-ETM(L)=ARM966E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=16
DataComp=8
Counters=4
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=45
}
}

{ ARM966E-S-ETM(M)=ARM966E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=8
DataComp=2
Counters=2
Sequencer=1
MMDecode=0
FifoFULL=1
DepthFIFO=20
}
}

{ ARM966E-S-ETM(S)=ARM966E-S
{ ETM
MODEL_DLL_FILENAME=etm
Protocol=2

AddrComp=2
DataComp=0
Counters=1
Sequencer=0
MMDecode=0
FifoFULL=1
DepthFIFO=10
MaxPortWidth=8
}
}


}
