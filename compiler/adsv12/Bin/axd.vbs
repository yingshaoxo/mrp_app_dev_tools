'***********************************************************
'   AXD: AXD.vbs
'   Copyright (C) ARM Limited 1999-2000. All rights reserved
'***********************************************************/

'   $Revision: 1.174.4.5 $
'   $Date: 2001/08/17 09:46:21 $

'''''''''''''''''''''''''''''''''''''''
' Property Enumeration Variables
'
'''''''''''''''''''''''''''''''''''''''


'    Dim HexFmt, DecFmt, OctFmt, BinFmt
'    HexFmt = "hex" : DecFmt = "dec" : OctFmt = "oct" : BinFmt = "bin"

    Dim AxdFile, AxdMsgbox, AxdIgnore, AxdReplace
    AxdFail = "Fail" : AxdMsgbox = "Msgbox" : AxdIgnore = "Ignore" : AxdReplace = "Replace"

    Dim SYNC, ASYNC
    ASYNC = 0 : SYNC = 1

    Dim stpIn, stpLine, stpInstr, stpOut
    stpIn = 0: stpOut = 1
    stpInstr = 0 : stpLine = 1
    
    Dim Bit8, Bit16, Bit32
    Bit8 = 1 : Bit16 = 2 : Bit32 = 4
    
    Dim THUMB, ARM, AUTO, BYTECODE
    THUMB = 0 : ARM = 1 : AUTO = 2 : BYTECODE = 3
  
    Dim AxdGLOBAL, AxdLOCAL, AxdCLASS
    AxdGLOBAL = 0 : AxdLOCAL = 1 : AxdCLASS = 2

    Dim sOn, sOff
    sOn = 0 : sOff =1

    Dim PosImgObj, PosFileObj, PosLineObj, PosAsmObj
    PosImgObj = 0 : PosFileObj = 1 : PosLineObj = 2 : PosAsmObj = 3

    Dim DefPlatForm
	Dim ImageLoadFlag
	ImageLoadFlag = FALSE
    
'''''''''''''''''''''''''''''''''''''''
' Property Flags
'''''''''''''''''''''''''''''''''''''''
    Dim DefDisplayFormat
    Dim DefDisplayFormatPar
    Dim DefInputBase
    Dim DefMemSize
    Dim DefInstrSize
    Dim DefMemRange
    Dim DefDisplayLine
    Dim DefRunMode
    Dim DefStepSize

''''''''''''''''''''''''''''''''''''''''''''
' Define global object and their list
' Lists of references to external objects
'''''''''''''''''''''''''''''''''''''''''''
    Dim CurrentThread
    Dim ProcList   
    Dim ImageList
    Dim FileList
    Dim RegBankList
    Dim MemBankList
    Dim BreakPtList
    Dim WatchPtList

''''''''''''''''''''
    Dim VariableList
    Dim FunctionList
    Dim ClassList
    Dim ClassVarList
    
'''''''''''''''''''''''''''''''
' Global strings for id and name.
'''''''''''''''''''''''''''''''''''''''''
    Dim ThreadIDString, ThreadNameString
    Dim ProcIDString, ProcNameString
    Dim ImageIDString, ImageNameString
    Dim FileIDString, FileNameString
    Dim RegBankIDString, RegBankNameString
    Dim ClassNameString
    Dim VariableNameSting
    Dim RegNameString
    Dim FunctionNameString
    Dim StackEntryString
    Dim FormatRDINameString

'''''''''''''''''''''''
'Text Strings
''''''''''''''''''''''
    Dim ImageListText
    Dim ProcListText
    Dim ImageFileListText
    Dim ImageFunctionListText
    Dim ImageVariableListText
    Dim FormatListText

''''''''''''''''''''''''''''''''''''''''''
' Column header
''''''''''''''''''''
    Dim HeaderText, HeadText, regheader
    headertext = "Index" & Chr(9) & "Name" & Chr(13) & Chr(10)
    regheader = "Index" & Chr(9) & "Name" & Chr(9) & "Value" & Chr(13) & Chr(10)
    HeadText = "Index" & Chr(9) & "ID" & Chr(9) & "Name" & Chr(13) & Chr(10)
''''''''''''''''''''

''''''''''''''''''''''''''''''
' semi permanent async objects
''''''''''''''''''''''''''''''
    Dim AsyncSource
    Dim AsyncAsm

'''''''''''''''''''''''''''''''
' local flag only
'''''''''''''''''''''''''''''''
    Dim ListFormatPar, ListCounter
    ListFormatPar = -1 : ListCounter = false
    
'''''''''''''''''''''''''
'Property Functions
''''''''''''''''''
Function MemSize(MemorySize)

    Select Case MemorySize
        Case Bit8
            DefMemSize = 1
        Case Bit16
            DefMemSize = 2
        Case Bit32
            DefMemSize = 4
    End Select
End Function

Function ConvertMemSize(MemorySize)
    Dim Result
    Select Case MemorySize
        Case Bit8
            Result = 1
        Case Bit16
            Result = 2
        Case Bit32
            Result = 4
    End Select
    ConvertMemSize = Result
End Function

Function MemRange(MemoryRange)
    DefMemRange = MemoryRange
End Function

Function StepSize(stpSize)                            '<--- CLI command
On Error Resume Next 
    StepSize = StepSizeInt(stpSize)
    DisplayError
End Function

Function StepSizeInt(stpSize)
    If stpSize = stpLine Then
        DefStepSize = 1
    ElseIf stpSize = stpInstr Then
        DefStepSize = 0
    ElseIf stpSize = -1 Then
        DefStepSize = DefStepSize
    Else
        err.raise 1000, "VBScript", "Invalid step size specified"
    End If
End Function


Function RunMode(aRunMode, stpSize)                     '<--- CLI command
On Error Resume Next 
    RunMode = RunModeInt(aRunMode, stpSize)
    DisplayError
End Function

Function RunModeInt(aRunMode, stpSize)
    If aRunMode = ASYNC Then
        DefRunMode = 0
    ElseIf aRunMode = SYNC Then
        DefRunMode = 1
    ElseIf aRunMode = -1 Then
        DefRunMode = DefRunMode
    Else
        err.raise 1000, "VBScript", "Invalid mode specified"
    End If
    If stpSize = stpLine Then
        DefStepSize = 1
    ElseIf stpSize = stpInstr Then
        DefStepSize = 0
    ElseIf stpSize = -1 Then
        DefStepSize = DefStepSize
    Else
        err.raise 1000, "VBScript", "Invalid step size specified"
    End If
End Function

Function InstrSize(aInstrSize)
    If aInstrSize = THUMB Then
        DefInstrSize = 0
    ElseIf aInstrSize = ARM Then
        DefInstrSize = 1
    ElseIf aInstrSize = AUTO Then
        DefInstrSize = 2
    Else
        adw3cli.Display("Unknown instruction type."& Chr(13) & Chr(10))
    End If
End Function

'Function format(InputParam, FormatParam)                    '<-- CLI command
'On Error Resume Next
'    If InputParam = HexFmt Then
'        DefInputBase = "H"
'    ElseIf InputParam = DecFmt Then
'        DefInputBase = "D"
'    ElseIf InputParam = OctFmt Then
'        DefInputBase = "O"
'    Else
'        adw3cli.Display("Invalid input option."& Chr(13) & Chr(10))
'    End If

'    If FormatParam = HexFmt Then
'        DefDisplayFormat = "hex"
'    ElseIf FormatParam = DecFmt Then
'        DefDisplayFormat = "dec"
'    ElseIf FormatParam = OctFmt Then
'        DefDisplayFormat = "oct"
'    ElseIf FormatParam = BinFmt Then
'        DefDisplayFormat = "bin"
'    Else
'        adw3cli.Display("Invalid format option."& Chr(13) & Chr(10))
'    End If
'    DisplayError
'End Function

Function Format(fmtStr, fmtPar)
On Error Resume Next

    If FormatListPar = -1 Then
        DefDisplayFormat = fmtStr
    Else
        DefDisplayFormat = GetFormatFromKey(fmtStr)
    End If
    DefDisplayFormatPar = CStr(fmtPar)    
'    DefDisplayFormat = CStr(fmtStr) & CStr(fmtPar)
    DisplayError
End Function

Function Platform(iPlatform)
    DefPlatForm = iPlatForm
End Function   

Function ThreadChange()
    Dim CurImageList
	set CurrentThread = session.GetCurrentThread
	'In case user start up CLI after loaded an image.
	set CurImageList = session.Images
	If CurImageList.count > 0 Then
		ImageLoaded
	End If
End Function

Function ImageLoaded()
	ImageLoadFlag = TRUE
    call UpdateImageList
End Function

'''''''''''''''''''''''''''''''''''''
' ImportFormat
'''''''''''''''''''''''''''''''''''''
Function ImportFormat(FmtFile, FailureOpt)                 '<--- CLI command
On Error Resume Next
    call ImportFormatInt(FmtFile, FailureOpt)
    DisplayError
End Function

Function ImportFormat(FmtFile, FailureOpt)               
    Dim ImportRes

    If FailureOpt = "-1" Then
        FailureOpt = "Fail" 
    End If

    ImportRes = session.ImportFormat(FmtFile, FailureOpt)
    adw3cli.Display(ImportRes)

End Function


Function LoadSession(SessionFile)                       '<---CLI command
On Error Resume Next 
    call LoadSessionInt(SessionFile)
    DisplayError
End Function

Function LoadSessionInt(SessionFile)
    Dim LoadStatus
    LoadStatus = session.LoadSession(SessionFile)
End Function

Function SaveSession(SessionFile)                       '<---CLI command
On Error Resume Next 
    call SaveSessionInt(SessionFile)
    DisplayError
End Function

Function SaveSessionInt(SessionFile)
    SaveStatus = session.SaveSession(SessionFile)
    If SaveStatus = 0 Then
        err.raise 1000, "VBScript", "Open file error."
    ElseIf SaveStatus = 2 Then
        err.raise 1000, "VBScript", "Error in saving session."
    End If
End Function

Function TraceLoad(TCFName)                             '<---CLI command
On Error Resume Next 
    TraceLoad = TraceLoadInt(TCFName)
    DisplayError
End Function


Function TraceLoadInt(TCFName)
    LoadState = session.TraceLoad(TCFName)
    If LoadState = 0 Then
        err.raise 1000, "VBScript", "Not a valid trace target."
    ElseIf LoadState = 1 Then
        err.raise 1000, "VBScript", "Failed to load TCF file."
    End If
End Function

Function Trace(Toggle)                                  '<---CLI command
On Error Resume Next 
    call TraceInt(Toggle)
    DisplayError
End Function

Function TraceInt(Toggle)
    If Toggle = SON Then
        res = session.TraceToggle(TRUE)
    Else
        res = session.TraceToggle(FALSE)
    End If
    If res = 0 Then
        err.raise 1000, "VBScript", "Not a valid trace target."
    ElseIf res = 1 and Toggle = SON Then
        err.raise 1000, "VBScript", "Failed to start trace."
    ElseIf res = 1 and Toggle = SOFF Then
        err.raise 1000, "VBScript", "Failed to stop trace."
    End If

End Function
''''''''''''''''''''''''''
' ListFormat
''''''''''''''''''''''''''
Function ListFormat(SizeOption)                           '<--- CLI command
On Error Resume Next 
    ListFormatInt(SizeOption)
    DisplayError
End Function

Function ListFormatInt(SizeOption)                           

    Dim HelperText
    Dim FormatList
    Dim GenericList, EightBitList, SixteenBitList, ThirtytwoBitList, SixtyfourBitList, EightyBitList, UserDefinedList
    Dim DataSize
    Dim ListHeader
    Dim FormatListText

    ListHeader = "Index" & Chr(9) & AddSpace("ShortName", 25) & "RDIName" & Chr(13) & Chr(10)
    set FormatList = session.ListFormat(SizeOption)
    ListFormatPar = SizeOption
    ListCounter = TRUE

    count = FormatList.count
    If count = 0 Then
        FormatListText = "No display format is available for this domain." & chr(13) & chr(10)
    End If
    iter = 0 : index = 0
    do while iter < count
        set ThisFormat = FormatList(iter)
        DataSize = ThisFormat.GetSize
        FmtRDIName = ThisFormat.GetRDIName
        FmtName = ThisFormat.GetName

        FormatListText = FormatListText & Chr(35) & CStr(index + 1) & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)

        If DataSize <> 0 Then
           FormatRDINameString = FormatRDINameString  & CStr(FmtRDIName) & "%" & CStr(iter) & "%"
           RegBankNameString = RegBankNameString & CStr(FmtName) & "%" & CStr(iter) & "%" 
        End If
        index = index + 1
        iter = iter + 1
    loop

    If SizeOption = -1  Then
        BitName = "All"
    Else
        BitName = CStr(SizeOption) & " bits data"
    End If
    
    ListTitle = BitName & " display formats:" & chr(13) & chr(10)
    txt = txt & ListTitle & ListHeader & FormatListText            

    adw3cli.Display(txt)
    
End Function


Function ListFormatIntOld(SizeOption)                           
                           

    Dim HelperText
    Dim FormatList
    Dim GenericList, EightBitList, SixteenBitList, ThirtytwoBitList, SixtyfourBitList, EightyBitList, UserDefinedList
    Dim DataSize
    Dim ListHeader
    ListCounter = true
    ListFormatPar = SizeOption

'HelperText = "Helper info about this format"

    GenericList = "" : EightBitList = "" : SixteenBitList = "" : ThirtytwoBitList = "" : SixtyfourBitList = "" : EightyBitList = "" : UserDefinedList = ""
    ListHeader = "Index" & Chr(9) & AddSpace("ShortName", 25) & "RDIName" & Chr(13) & Chr(10)
    set FormatList = session.ListFormat(SizeOption)
    ListFormatPar = SizeOption
'    Set FormatRDINameString = CreateObject("Scripting.Dictionary")
'    Set FormatIndexString = CreateObject("Scripting.Dictionary")

    count = FormatList.count
    iter = 0 : index = 0
    do while iter < count
        set ThisFormat = FormatList(iter)
        DataSize = ThisFormat.GetSize
        FmtRDIName = ThisFormat.GetRDIName
        FmtName = ThisFormat.GetName
        select case DataSize
            case 0:
                GenericList = GenericList & Chr(35) & CStr(index + 1) & Chr(9) & AddSpace(FmtName, 25) & FmtRDIName & chr(13) & chr(10)
            case 8:
                EightBitList = EightBitList & Chr(35) & CStr(index + 1) & Chr(9) & AddSpace(FmtName, 25) & FmtRDIName & chr(13) & chr(10)
            case 16:
                SixteenBitList = SixteenBitList & Chr(35) & CStr(index + 1)  & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)
            case 32:
                ThirtytwoBitList = ThirtytwoBitList & Chr(35) & CStr(index + 1)  & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)
            case 64:
                SixtyfourBitList = SixtyfourBitList & Chr(35) & CStr(index + 1)  & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)
            case 80:
                EightyBitList = EightyBitList & Chr(35) & CStr(index + 1) & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)
            case else:
'                If DataSize <> 0 Then
                    UserDefinedList = UserDefinedList & Chr(35) & CStr(index + 1) & Chr(9) & AddSpace(ThisFormat.GetName, 25) & FmtRDIName & chr(13) & chr(10)
'                End If
        End Select
'        If DataSize <> 0 Then
           FormatRDINameString = FormatRDINameString  & CStr(FmtRDIName) & "%" & CStr(iter) & "%"
           RegBankNameString = RegBankNameString & CStr(FmtName) & "%" & CStr(iter) & "%" 
           'FormatRDINameString.Add Chr(35) & CStr(index + 1), FmtRDIName
           'FormatIndexString.Add FmtRDIName, Chr(35) & CStr(index + 1)
            index = index + 1
'        End If
        iter = iter + 1
        'might need to generate old list for compatibility..., or take the 0 size off totally.
    loop
    If  SizeOption = -1 Then
        ListTitle = "Generic display formats:" & chr(13) & chr(10)   
        txt = txt & ListTitle & ListHeader & GenericList            
    ElseIf IsEmpty(GenericList) = True and SizeOption = -1 Then
        txt = txt & "Generic display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If (SizeOption = 8 or SizeOption = -1 ) and IsEmpty(EightBitList) = False Then
        ListTitle = "8 bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & EightBitList            
    ElseIf IsEmpty(EightBitList) = True and SizeOption = 8 Then
        txt = txt & "8 bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If IsEmpty(SixteenBitList) = False  and (SizeOption = 16 or SizeOption = -1 ) Then
        ListTitle = "16 bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & SixteenBitList            
    ElseIf IsEmpty(SixteenBitList) = True and SizeOption = 16 Then
        txt = txt &  "16 bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If ThirtytwoBitList <> "" and (SizeOption = 32 or SizeOption = -1 ) Then
        ListTitle = "32 bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & ThirtytwoBitList            
    ElseIf ThirtytwoBitList = "" and SizeOption = 32 Then
        txt = txt & "32 bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If SixtyfourBitList <> ""  and (SizeOption = 64 or SizeOption = -1 ) Then
        ListTitle = "64 bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & SixtyfourBitList            
    ElseIf SixtyfourBitList = "" and SizeOption = 64 Then
        txt = txt & "64 bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If  EightyBitList <> "" and (SizeOption = 80 or SizeOption = -1) Then
        ListTitle = "80 bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & EightyBitList         
    ElseIf EightyBitList = "" and SizeOption = 80 Then
        txt = txt & "80 bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    If (SizeOption <> -1 or (SizeOption Mod 8 <> 0 or SizeOption > 80)) and UserDefinedList <> "" Then
        If SizeOption = -1 Then
            BitName = "Other"
        Else
            BitName = CStr(SizeOption)
        End If
        ListTitle = BitName & " bits data display formats:" & chr(13) & chr(10)
        txt = txt & ListTitle & ListHeader & UserDefinedList            
    ElseIf UserDefinedList = "" and (SizeOption Mod 8 <> 0 or SizeOption > 80) Then
        txt = txt & BitName & " bits data display formats:" & chr(13) & chr(10) & "No display format is available for this domain." & chr(13) & chr(10)
    End If

    FormatListText = txt
    adw3cli.Display(FormatListText)
    
End Function

''''''''''''''''''''''''''
' Loadings
''''''''''''''''''''''''''
Function Load(FileName, ProcIndex)                          '<--- CLI command
On Error Resume Next 
    Load = LoadInt(FileName, ProcIndex)
    DisplayError
End Function
 
Function LoadInt(FileName, ProcIndex)
    Dim loadStatus, index
    Dim curProc
    set curProc = CheckInitProc(ProcIndex)
    loadStatus = session.LoadImage(FileName, curProc.Handle)
	ImageLoadFlag = TRUE
End Function
 

Function ImgProperties(ImageIndex)                          '<--- CLI command
On Error Resume Next    
    ImgProperties = ImgPropertiesInt(ImageIndex)
    DisplayError
End Function

Function ImgPropertiesInt(ImageIndex)
    Dim CurImage, cmdLineText, argText
    cmdLineText = "CmdLine:"
    
    set CurImage = CheckForImage(ImageIndex)

    argText = CurImage.GetCmdLine
    If argText = "" Then
        argText = "Command line argument has not been set."
    End If

    cmdLineText = cmdLineText & Chr(9) & argText & Chr(13) & Chr(10)
    adw3cli.Display(cmdLineText)
        
End Function

Function SetImgProp(ImageIndex, PropType, ArgString)
On Error Resume Next    
    SetImgProp = SetImgPropInt(ImageIndex, PropType, ArgString)
    DisplayError
End Function

Function SetImgPropInt(ImageIndex, PropType, ArgString)
    
    set CurImage = CheckForImage(ImageIndex)
    
    If PropType = "cmdline" Then
        CurImage.SetCmdLine(ArgString)
    Else
        err.raise 1000, "VBScript", "Invalid property parameter."
    End If      
End Function

Function CheckInitProc(ProcIndex)
dim pindex
    pindex = ProcIndex
    ProcListText = UpdateProcList
    If ProcIndex = -1 Then
        set CheckInitProc = GetCurrentProcessor
    Else
        set CheckInitProc = GetObjFromKey(pindex, ProcList, ProcNameString, ProcIDString, index)
    End If
End Function

Function ProcProperties(ProcIndex)                              '<--- CLI command
On Error Resume Next    
    ProcProperties = ProcPropertiesInt(ProcIndex)
    DisplayError
End Function

Function ProcPropertiesInt(ProcIndex)
    Dim CurProc, thisThread, PropText, PropList, PropObj
    set CurProc = CheckInitProc(ProcIndex)
    set thisThread = CurProc.Thread
    
    set PropList = thisThread.GetProcProperties
    
    iter = 0
    do while iter < 7
        set PropObj = PropList(iter)
        Select Case iter
            case 0:
                propType = "vector_catch"
            case 1:
                propType = "comms_channel"
            case 2:
                propType = "semihosting_enabled"
            case 3:
                propType = "semihosting_vector"
            case 4:
                propType = "semihosting_dcchandler_address"
            case 5:
                propType = "arm_semihosting_swi"
            case 6:
                propType = "thumb_semihosting_swi"
        End Select
        'var = PropObj.GetPropValue
        varText = PropObj.FormatValue(iter)
        PropText = PropText & propType & Chr(9) & varText & Chr(13) & Chr(10)
        iter = iter + 1     
    Loop
    adw3cli.Display(PropText)
End Function

Function SetProcProp(AnyType, NewValue, ProcIndex)                 '<--- CLI command
On Error Resume Next    
    SetProcProp = SetProcPropInt(AnyType, NewValue, ProcIndex)
    DisplayError
End Function

Function SetProcPropInt(AnyType, NewValue, ProcIndex)
    Dim CurProc, thisThread, index, Value, propType
    propType = -1
	
	If ProcIndex = -1 Then
		set CurProc = CheckInitProc(ProcIndex)
	Else
		set CurProc = CheckInitProc(ProcIndex)
	End If

    set thisThread = CurProc.Thread
	Select Case AnyType
		case "vector_catch":
			propType = 0
		case "comms_channel":
			propType = 1
		case "semihosting_enabled":
			propType = 2
		case "semihosting_vector":
			propType = 3
		case "semihosting_dcchandler_address":
			propType = 4
		case "arm_semihosting_swi":
			propType = 5
		case "thumb_semihosting_swi":
			propType = 6
	End Select

    If propType < 0 or propType > 6 Then
        err.raise 1000, "VBScript", "Parameter name specified is not recognised."
    End If
    If AnyType = "vector_catch" or propType = 1 Then
		If IsNumeric(NewValue) = FALSE Then
			Dim NewVar
			Dim PropList
			set PropList = thisThread.GetProcProperties
			
			NewVar = PropList(0).GetValue
			OldVarStr = PropList(0).FormatValue(0)

			If InStr(NewValue, Chr(82)) <> 0 Then
				NewVar = NewVar or &h0001
			End If
			If InStr(NewValue, Chr(114)) <> 0 and InStr(OldVarStr, Chr(114)) <> 1 Then
				NewVar = NewVar xor &h0001
			End If
			If InStr(NewValue, Chr(85)) <> 0 Then
				NewVar = NewVar or &h0002
			End If
			If InStr(NewValue, Chr(117)) <> 0 and InStr(OldVarStr, Chr(117)) <> 2 Then
				NewVar = NewVar xor &h0002
			End If
			If InStr(NewValue, Chr(83)) <> 0 Then
				NewVar = NewVar or &h0004
			End If
			If InStr(NewValue, Chr(115)) <> 0 and InStr(OldVarStr, Chr(115)) <> 3 Then
				NewVar = NewVar xor &h0004
			End If
			If InStr(NewValue, Chr(80)) <> 0 Then
				NewVar = NewVar or &h0008
			End If
			If InStr(NewValue, Chr(112)) <> 0 and InStr(OldVarStr, Chr(112)) <> 4 Then
				NewVar = NewVar xor &h0008
			End If
			If InStr(NewValue, Chr(68)) <> 0 Then
				NewVar = NewVar or &h0010
			End If
			If InStr(NewValue, Chr(100)) <> 0 and InStr(OldVarStr, Chr(100)) <> 5 Then
				NewVar = NewVar xor &h0010
			End If
			If InStr(NewValue, Chr(73)) <> 0 Then
				NewVar = NewVar or &h0040
			End If
			If InStr(NewValue, Chr(105)) <> 0 and InStr(OldVarStr, Chr(105)) <> 6 Then
				NewVar = NewVar xor &h0040
			End If
			If InStr(NewValue, Chr(70)) <> 0 Then
				NewVar = NewVar or &h0080
			End If
			If InStr(NewValue, Chr(102)) <> 0 and InStr(OldVarStr, Chr(102)) <> 7 Then
				NewVar = NewVar xor &h0080
			End If

		NewValue = NewVar
		End If
	Else
		NewValue = Unsigned(ExprToAddress(NewValue))
	End If
    call thisThread.SetProcProperties(propType, NewValue)

End Function

Function LoadSymbols(FileName, ProcIndex)                       '<--- CLI command
On Error Resume Next    
    LoadSymbols = LoadSymbolsInt(FileName, ProcIndex)
    DisplayError
End Function

Function LoadSymbolsInt(FileName, ProcIndex)
    Dim res, index
    Dim CurProc
    If ProcIndex = -1 Then
        set CurProc = GetCurrentProcessor
    Else
        set CurProc = GetObjFromKey(ProcIndex, ProcList, ProcNameString, ProcIDString, index)
    End If
    call session.LoadSymbols(FileName, CurProc.Handle)
       
End Function
 
Function Reload(ImageIndex)                     '<--- CLI command
On Error Resume Next    
    Reload = ReloadInt(ImageIndex)
    DisplayError
End Function


Function GetImageHandle(ImageIndex)    
    Dim CurImage
    Set CurImage = CheckForImage(ImageIndex)
    GetImageHandle = CurImage.Handle
End Function


Function ReloadInt(ImageIndex)

    Dim ImageHandle

    ImageHandle = GetImageHandle(ImageIndex)
    call session.ReloadImage(ImageHandle)
	ImageLoadFlag = TRUE
    call UpdateImageList

End Function

Function LoadBinary(FileName, ExprStr)                      '<--- CLI command
On Error Resume Next    
    LoadBinary = LoadBinaryInt(FileName, ExprStr)
    DisplayError
End Function

Function LoadBinaryInt(FileName, ExprStr)
    Dim CurProc, Address
    set CurProc = GetCurrentProcessor
    Address = ExprToAddress(ExprStr)
    call session.LoadFileToMem(FileName, Address, CurProc.Handle)
End Function

Function SaveBinary(FileName, Expr1, Expr2)                     '<--- CLI command
On Error Resume Next    
    SaveBinary = SaveBinaryInt(FileName, Expr1, Expr2)
    DisplayError
End Function

Function SaveBinaryInt(FileName, StartAddress, EndAddress)
    Dim CurProc, Range

    If Left(EndAddress, 1) = Chr(43) Then        
        Range = ComRange(StartAddress, EndAddress) -1 
    Else
        Range = ComRange(StartAddress, EndAddress) - 1
    End If
    
    set CurProc = GetCurrentProcessor
    call session.DumpMemToFile(FileName, StartAddress, StartAddress+Range, CurProc.Handle)
End Function


'''''''''''''''''''''''''''''''''''''''
' Create global processor list
''''''''''''''''''''''''''''''''''''''
Function processors()                       '<--- CLI command
On Error Resume Next    
    processors = processorsInt()    
    DisplayError
End Function

Function processorsInt()                        '<---- CLI function
    dim txt
    call CheckInitProc(-1)
    txt = headtext + ProcListText
    adw3cli.Display(txt)
End Function

Function Images()                       '<--- CLI command
On Error Resume Next    
    Images = ImagesInt()
    DisplayError
End Function

Function CheckForImage(ImageIndex)
	dim iindex
    iindex = ImageIndex
    
    If ImageIndex = -1 Then
        dim context
        set context = currentthread.Context
        set CheckForImage = context.Image
        UpdateImageList
    Else 
        if CheckListStatus(ImageList) = False Then
            err.raise 1000, "VBScript", "Please Load an Image."
        End If
        set CheckForImage = GetObjFromKey(iindex, ImageList, ImageNameString, ImageIDString, index)
    End If
    call UpdateImageFileList(CheckForImage)
    set ClassList = CheckForImage.Classes   
End Function

Function ImagesInt()
    dim txt
    call CheckForImage(-1)
    txt = headtext + ImageListText
    adw3cli.Display(txt) 
End Function


''''''''''
' Get files from an image
''''''''''''
Function Files(ImageIndex)              '<--- CLI command
On Error Resume Next    
    Files = FilesInt(ImageIndex)
    DisplayError
End Function

Function FilesInt(ImageIndex)
        dim txt

        call CheckForImage(ImageIndex)
        
        txt = headtext + ImageFileListText
        adw3cli.Display(txt) 
End Function

'''''''''''''''''''''''''''
' List all the classes in the spricfied image
'
'''''''''''''''''''''''
Function Classes(ImageIndex)                '<--- CLI command
On Error Resume Next    
    Classes = ClassesInt(ImageIndex)
    DisplayError
End Function

Function ClassesInt(ImageIndex)  
    dim ThisClass
    dim count
    dim iter
    dim txt
    
    call CheckForImage(ImageIndex)
    count = ClassList.count

    If count = 0 Then
        err.raise 1000, "VBScript", "Image does not have any classes."
    End If

    iter = 0
    do while iter < count
        set ThisClass = ClassList(iter)
        txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & ThisClass.Name & chr(13) & chr(10)
        iter = iter + 1
    loop
    txt = headertext & txt
    adw3cli.Display(txt) 
    call UpdateClassClasslist(ClassList)
    
End Function


'''''''''''''
' Get function from an image 
''''''''''''''''
Function Functions(ImageIndex)              '<--- CLI command
On Error Resume Next    
    Functions = FunctionsInt(ImageIndex)
    DisplayError
End Function

Function FunctionsInt(ImageIndex)
    Dim CurImage   
    Dim index
    dim iter
    dim txt

    Set CurImage = CheckForImage(ImageIndex)
    
    UpdateImageFunctionList CurImage
    Dim Head
    Head = "Index" & Chr(9) & "Function" & Chr(13) & Chr(10)
    txt = head & ImageFunctionListText
    adw3cli.Display(txt) 
End Function

'''''''''''''''''''
'Variarbles of an image
''''''''''''''''
Function Variables(ImageIndex)              '<--- CLI command
On Error Resume Next    
    Variables = VariablesInt(ImageIndex)
    DisplayError
End Function

Function VariablesInt(ImageIndex)
    Dim CurImage   
    
    Set CurImage = CheckForImage(ImageIndex)
    UpdateImageVariableList CurImage
    
    dim text 
    text = text & "Index" & Chr(9) & "Name" & Chr(13) & Chr(10)    
    txt = text & ImageVariableListText
    adw3cli.Display(txt) 
End Function

Function CVariables(ClassIndex)             '<--- CLI command
On Error Resume Next    
    CVariables = CVariablesInt(ClassIndex)
    DisplayError
End Function

Function CVariablesInt(ClassIndex)
    Dim CurClass   
    Dim ClassVarList
    Dim index
    Dim listStatus, listInit
    
    If CheckListStatus(ClassList) = False Then
        call CheckForImage(-1)              'retrieves top level classlist
    End If
    
    set CurClass = GetObjFromKey(ClassIndex, ClassList, ClassNameString, ClassIDString, index)
    set ClassVarList = CurClass.Variables()

    dim ThisVariable
    dim count
    dim iter
    dim txt

        count = ClassVarList.count
    If count = 0 Then
        err.raise 1000, "VBScript", "No variables in this class."
    End If

    iter = 0
    do while iter < count
        set ThisVariable = ClassVarList(iter)
        txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & ThisVariable.Type & Chr(9) & ThisVariable.Name & chr(13) & chr(10)
        iter = iter + 1
    loop
    
    Dim Head
    Head = "Index" & Chr(9) & "Type" & Chr(9) & Chr(9) & "Name" & Chr(13) & Chr(10)
    txt = head & txt
    adw3cli.Display(txt) 
End Function

Function CFunctions(ClassIndex)             '<--- CLI command
On Error Resume Next    
    CFunctions = CFunctionsInt(ClassIndex)
    DisplayError
End Function

Function CFunctionsInt(ClassIndex)
    Dim CurClass   
    Dim FunctionList
    Dim index
    Dim listStatus, listInit

    If CheckListStatus(ClassList) = False Then
        call CheckForImage(-1)              'retrieves top level classlist
    End If
    set CurClass = GetObjFromKey(ClassIndex, ClassList, ClassNameString, ClassIDString, index) 
    set FunctionList = CurClass.Functions()

    dim ThisFunction
    dim count
    dim iter
    dim txt

        count = FunctionList.count
    If count = 0 Then
        err.raise 1000, "VBScript", "No functions in this class."
    End If

    iter = 0
        do while iter < count
            set ThisFunction = FunctionList(iter)
            FuncText = ThisFunction.Type & ThisFunction.Name & ThisFunction.Params
            txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & FuncText & chr(13) & chr(10)
            iter = iter + 1
        loop
    
    Dim Head
    Head = "Index" & Chr(9) & "Function" & Chr(13) & Chr(10)
    txt = head & txt
    adw3cli.Display(txt) 
End Function

Function CClasses(ClassIndex)               '<--- CLI command
On Error Resume Next    
    CClasses = CClassesInt(ClassIndex)
    DisplayError
End Function

Function CClassesInt(ClassIndex)
    dim CurClass
    dim SubClassList
    
    If CheckListStatus(ClassList) = False Then
        call CheckForImage(-1)                      'retrieves top level classlist
    End If
    set CurClass = GetObjFromKey(ClassIndex, ClassList, ClassNameString, ClassIDString, index)
    set SubClassList = CurClass.Classes()

    dim subclass
    dim count
    dim iter
    dim txt

    count = subclasslist.count
    If count = 0 Then
        err.raise 1000, "VBScript", "No classes in this class."
    End If
    iter = 0
    do while iter < count
        set subclass = subclasslist(iter)
        txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) &subclass.Name & chr(13) & chr(10)
        iter = iter + 1
    loop
    dim header 
    header = "Index" & Chr(9) & "Class Name" & Chr(13) & Chr(10)
    txt = header & txt
    adw3cli.Display(txt)
    call UpdateClassClasslist(SubClassList)
End Function

Function Source(StartLine, Line, FileIndex)             '<--- CLI command
On Error Resume Next    
    Source = SourceInt(StartLine, Line, FileIndex)
    DisplayError
End Function

Function SourceInt(StartLine, Line, FileIndex)
    Dim CurFile, SourceLineList, SrcLine  
    Dim ret, size, iter, txt, Header
    
    If Left(Line, 1) = Chr(43) Then
        CheckFormat = true
    Else
        CheckFormat = false
    End If

    call CheckForImage(-1)

    Header = "Line No." & Chr(9) & "Source" & Chr(13) & Chr(10)

    StartLine = ExprToAddress(StartLine) - 1
    If StartLine = -1  Then
        StartLine = 0
    End If

    If CheckFormat = true Then
        Line = ComRange(StartLine, Line)
    Else
        Line = ComRange(StartLine, Line) - 1
    End If 

    If FileIndex = -1 Then
        dim cxt 
        set cxt = currentThread.Context
        If cxt.PosObjectStatus(PosFileObj) = False Then
            err.raise 1000, "VBScript", "No source file associated with current context."
        End If
        set CurFile = cxt.File
    Else
        call CheckListStatus(FileList)
        set CurFile = GetObjFromKey(FileIndex, FileList, FileNameString, FileIDString, index)
    End If

    set SourceLineList = CurFile.Lines(Unsigned(StartLine), Unsigned(Line))
    size = sourcelinelist.count
    If size = 0 Then
        err.raise 1000, "VBScript", "Could not find any source lines in this range"
    End If
    iter = 0
    do while iter < size
        set SrcLine = SourceLineList(iter)
        txt = txt & Chr(9) & CStr(SrcLine.LineNumber + 1) & Chr(9) & Chr(9) & SrcLine.Text & chr(13) & chr(10)
        iter = iter + 1
    loop
    txt = Header & txt
    adw3cli.Display(txt)
End Function

Function Disassemble(StartAddress, Address, asm)                '<--- CLI command
On Error Resume Next    
    Disassemble = DisassembleInt(StartAddress, Address, asm)
    DisplayError
End Function

Function DisassembleInt(StartAddress, Address, asm)
    Dim CurImage, LastTime   
    Dim Range, SmallRange
    dim line, count, iter, txt, linelist
    
	If Address = -1 Then
		Address = Chr(43) & CStr(DefMemRange)
	End If

    LastTime = False
    If asm = -1 Then
        asm = DefInstrSize
    End If
    
    do 
        Range = InitRange(SmallRange, LastTime, StartAddress, Address)      
        set linelist = CurrentThread.AsmLines(StartAddress, Range, asm)
        count = linelist.count
        iter = 0
        do while iter < count
            set line = linelist(iter)
            txt = line.FormatAddress & Chr(9) & line.Text & chr(13) & chr(10)
            If adw3cli.Display(txt) = 0 Then 'user aborted
                Exit Function
            End If
            iter = iter + 1
        loop
        If LastTime Then
            Exit Function
        End If
        StartAddress = Unsigned(StartAddress+Range)
        Address = Unsigned(Address)
    Loop
End Function

Function LowLevel(ImageIndex)               '<--- CLI command
On Error Resume Next    
    LowLevel = LowLevelInt(ImageIndex)
    DisplayError
End Function

Function LowLevelInt(ImageIndex)
    Dim CurImage, index   
    Dim LowLevelList

    Set CurImage = CheckForImage(ImageIndex)
    
    set LowLevelList = CurImage.LowLevel

    dim ThisLowLevel
    dim count
    dim iter
    dim txt

    count = LowLevelList.count
    If count = 0 Then
        err.raise 1000, "VBScript", "No LowLevel available."
    End If

    iter = 0
    do while iter < count
        set ThisLowLevel = LowLevelList(iter)
        txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & ThisLowlevel.FormatAddress & Chr(9) & ThisLowLevel.Name & chr(13) & chr(10)
        LowLevelNameString = LowLevelNameString &  CStr(ThisLowLevel.Name) & "%" & CStr(iter) & "%"
        iter = iter + 1
    loop

    dim text
    text = "Index" & Chr(9) & "Address" & Chr(9) & Chr(9) & "Name" & Chr(13) & Chr(10)
    txt = text & txt
    adw3cli.Display(txt) 
End Function

Function RegBanks(ProcIndex)                '<--- CLI command
On Error Resume Next    
    RegBanks = RegBanksInt(ProcIndex)
    DisplayError
End Function

Function RegBanksInt(ProcIndex)
    dim txt
    txt = UpdateRegBankList(ProcIndex)
    txt = HeadText & txt
    adw3cli.Display(txt) 
End Function

Function Registers(RegBankIndex, fmt, fmtPar)           '<--- CLI command
On Error Resume Next    
    Registers = RegistersInt(RegBankIndex, fmt, fmtPar)
    DisplayError
End Function

Function RegistersInt(RegBankIndex, fmtKey, fmtPar)
    Dim CurRegBank
    dim Register
    dim count
    dim iter
    dim txt, RegisterList
    dim regtext, ListText
    
    If RegBankIndex = -1 Then
        RegBankIndex = "current"
    End If  

    If InStr(1, RegBankIndex, "User", 1) <> 0 or InStr(1, RegBankIndex, "System", 1) <> 0 Then
        RegBankIndex = "User/System"
    End If  

    fmt = GetFormatFromKey(fmtKey)
    If fmtPar = -1 Then
        fmtPar = DefDisplayFormatPar
    End If
    
    call UpdateRegBankList(-1)
    set CurRegBank = GetObjFromKey(RegBankIndex, RegBankList, RegBankNameString, RegBankIDString, index)   
    set RegisterList = CurRegBank.Registers()
    
    count = RegisterList.count
	BankName = CurRegBank.Name
	If BankName = "User" Then
		BankName = "User/System"
	End If
   
    iter = 0
    do while iter < count
        set Register = RegisterList(iter)
        errText = Register.Format(fmt, fmtPar)
        txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & Register.Name & Chr(9) & errText & chr(13) & chr(10)
        RegNameString = RegNameString &  CStr(Register.Name) & "%" & CStr(iter) & "%"
        iter = iter + 1
    loop
    ListText = "Registers Bank" & Chr(58) & Chr(32) & BankName & chr(13) & chr(10) & regheader & txt
    adw3cli.Display(ListText) 
End Function

'Generic Padd/Remove Function
Function Pad(a, b)
Dim Pads, i
    i = Len(a)
    if i > b Then
        a = mid(a, i-b+1)   'String bigger then b so truncate to b
        i = b
    End If
    Pads = b-i

    i = 0
    do while i < Pads
        a = "0" & a     'Pad zeros
        i = i + 1
    Loop
    Pad = a
End Function

'Function to pad a string with zeros to the next even number 
Function PadEven(a)
Dim l
    l = Len(a)
    l = Int((l+1)/2)*2
    PadEven = Pad(a,l)
End Function

Function Unsigned(a)
    Unsigned = "&h" & HexLong(a)
End Function
' Function to get a full unsigned string representation of a 32bit value , Hex() can't
Function HexLong(a)
Dim word16, upper, lower
Dim Supper, Slower
    word16 = &h10000
    upper = Int(a/word16)
    if upper > 65536 Then
        err.raise 1000, "VBScript", "Specified number or address larger then 0xFFFFFFFF"
    end if
    lower  = a - upper*word16
    Supper = Cstr(Hex(upper))
    Slower = Cstr(Hex(lower))
    HexLong = Pad(Supper,4) & Pad(Slower,4)
End Function

Function Memory(StartAddress, Address, MemSize, MemFormatKey, MemFmtPar)          '<--- CLI command
On Error Resume Next    
    Memory = MemoryInt(StartAddress, Address, MemSize, MemFormatKey, MemFmtPar)
    DisplayError
End Function
    
'Function MemoryInt(StartAddress, Address, MemSize, MemFormat)
Function MemoryInt(StartAddress, Address, MemSize, MemFormatKey, MemFmtPar)
    Dim DesireMemSize
    Dim MemObj, memText, MemRange
    Dim SmallRange, LastTime, Range
    dim iter, LineCount, OffSet
    Dim Pads, iLoop, OutPads, iOutLoop

	If Address = -1 Then
		Address = Chr(43) & CStr(DefMemRange)
	End If
   
    If Left(Address, 1) = Chr(43) Then
        CheckFormat = true
    Else
        checkformat = false
    End If

    If MemSize = -1 Then
        DesireMemSize = DefMemSize
    Else
        DesireMemSize = ConvertMemSize(MemSize)
    End If

'    If MemFormat = -1 Then
'        MemFormat = DefDisplayFormat
'    End If
    MemFormat = GetFormatFromKey(MemFormatKey)

    LastTime = False
    do
        Range = InitRange(SmallRange, LastTime, StartAddress, Address)

    If checkformat = true Then
        If MemSize = 4 Then
            If Range <= 4 Then
                MemRange = 4
            Else
                'MemRange = 4*Fix((Range)/4)
                If (Range Mod 4) = 0 Then
                    MemRange = Range
                Else
'                   MemRange = Range + 1
                    MemRange = Range + (4 - BitsLeft)
                'MemRange = 2*Fix((Range)/2)
                End If

            End If
        ElseIf MemSize = 2 Then
            If Range <= 2 Then
                MemRange = 2
            Else
                If (Range Mod 2) = 0 Then
                    MemRange = Range
                Else
                    MemRange = Range + 1
                'MemRange = 2*Fix((Range)/2)
                End If
            End If
        Else
            MemRange = Range
        End If
    Else
        If MemSize = 4 Then
            If Range <= 4 Then
                MemRange = 4
            Else
                BitsLeft = Range Mod 4
                If BitsLeft = 0 Then
                    MemRange = Range
                Else
                    MemRange = Range + (4 - BitsLeft)
                'MemRange = 2*Fix((Range)/2)
                End If
                'MemRange = 4*Fix((Range + 4)/4)
            End If
        ElseIf MemSize = 2 Then
            If Range <= 2 Then
                MemRange = 2
            Else
                If (Range Mod 2) = 0 Then
                    MemRange = Range
                Else
                    MemRange = Range + 1
                End If
'               MemRange = 2*Fix((Range + 2)/2)
            End If
        Else
            MemRange = Range
        End If
    End If
        set MemObj = currentthread.CreateMemory(StartAddress, MemRange)
' PM 64863 create memory uses "create and update" - we don't need create and read
'        MemObj.GetFromTarget

        ret = Round(CLng(MemRange)/16)
        ' now work out how many loops we need.
        If ret * 16 < CLng(MemRange) Then
            LineCount = ret
        ElseIf ret = 0 Then
            LineCount = 1
        Else
            LineCount = ret -1
        End If
        iter = 0
        OffSet = 0
        do while iter < LineCount 
            memText = MemObj.Format(MemFormat, MemFmtPar, DesireMemSize, OffSet, 16)
            FormatAddress  = "0x" & HexLong(StartAddress + OffSet)
            outText = FormatAddress & Chr(9) & memText & chr(13) & chr(10)
            If adw3cli.Display(outText) = 0 Then    'user aborted
                Exit Function
            End If 
            OffSet = OffSet + 16
            iter = iter + 1
        loop
        BytesRemain = Clng(MemRange) - LineCount*16    
        If BytesRemain >= 0 Then
            FormatAddress  = "0x" & HexLong(StartAddress + OffSet)
            memText = MemObj.Format(MemFormat, MemFmtPar, DesireMemSize, OffSet, BytesRemain)
            outText = FormatAddress & Chr(9) & memText & chr(13) & chr(10)
            If adw3cli.Display(outText) = 0 Then 'user aborted
                Exit Function
            End If
        End If   
        If LastTime Then
            Exit Function
        End If
        StartAddress = Unsigned(StartAddress + Range)
        Address = Unsigned(Address) 
        set MemObj = Nothing
    Loop
End Function

Function FillMem(StartAddress, Address, NewValue, RequiredMemSize)      '<--- CLI command
On Error Resume Next    
    FillMem = FillMemInt(StartAddress, Address, NewValue, RequiredMemSize)
    DisplayError
End Function

Function FillMemInt(StartAddress, Address, NewValue, RequiredMemSize)
    Dim MemObj, MemRange, DesireMemSize
    Dim SmallRange, LastTime
    
    If RequiredMemSize = -1 Then
        DesireMemSize = DefMemSize
    Else
        DesireMemSize = ConvertMemSize(RequiredMemSize)
    End If

    If Left(Address, 1) = Chr(43) Then
        CheckFormat = true
    Else
        checkformat = false
    End If

    If CheckFormat = false Then
    
        If RequiredMemSize = 4 Then
            address = 4*Fix((address + 4)/4)
        ElseIf RequiredMemSize = 2 Then
            address = 2*Fix((address + 2)/2)
        Else
            address = address
        End If
    End If

	If Left(NewValue, 1) = Chr(37) Then				'^, but changed to %
		NewValue = ExprToAddress(NewValue)
	End If

    LastTime = False   

    do
        MemRange = InitRange(SmallRange, LastTime, StartAddress, Address)    

        currentthread.FillMemory StartAddress, MemRange, NewValue, DesireMemSize
        
        If LastTime Then
            Exit Function
        End If
        AddressFound = "0x" & HexLong(StartAddress) & " - 0x" & HexLong(StartAddress+MemRange) & chr(13) & chr(10)
        If adw3cli.Display(AddressFound) = 0 Then 'user aborted
            Exit Function
        End If
        StartAddress = Unsigned(StartAddress+MemRange)
        Address = Unsigned(Address)
    Loop
End Function

''''''
' set the memory locateion at expr1 to the value given.
'''''''''
Function SetMem(StartAddress, NewValue, MemSize)        '<--- CLI command
On Error Resume Next    
    SetMem = SetMemInt(StartAddress, NewValue, MemSize)
    DisplayError
End Function

Function SetMemInt(StartAddress, NewValue, MemSize)
    Dim MemRange, DesireMemSize

    StartAddress = ExprToAddress(StartAddress)

    If MemSize = -1 Then
        DesireMemSize = DefMemSize
    Else
        DesireMemSize = ConvertMemSize(MemSize)
    End If

    call GetRange(StartAddress, StartAddress+DesireMemSize-1)
    NewValue = Unsigned(ExprToAddress(NewValue))
    currentthread.FillMemory StartAddress, DesireMemSize, NewValue, DesireMemSize
End Function

' function to calculate range and overcomes problem with signed arithemetic with unsigned
Function GetRange(StartAddr, EndAddr)
    Dim y, z, Range
        y=-2147483648
        z=2147483647
    
    Range = EndAddr - StartAddr

    If EndAddr < 0 and StartAddr >= 0  Then
        y= Endaddr - y
        z= z - StartAddr
        Range = y + z + 1
    End If
    If EndAddr >= 0 and StartAddr < 0 Then
        Range = -1
    End If

    If Range < 0 Then   
        err.raise 1000, "VBScript", "Invalid Range Specified."
    End If

    GetRange = Range
End Function
    
Function FindValue(SearchString, StartAddress, EndAddress)
On Error Resume Next    
    FindValue = FindValueInt(SearchString, StartAddress, EndAddress)
    DisplayError
End Function

Function InitRange(SmallRange, LastTime, StartAddress, EndAddress)
        SmallRange = &h10000        '64k is jugged to be best compromise
        LastTime = False 
        InitRange = ComRange(StartAddress, EndAddress)
        If InitRange > SmallRange Then
            InitRange = SmallRange
        Else
            LastTime = True
        End if
End Function

Function FindValueInt(SearchString, StartAddress, EndAddress)
    Dim MemObj, AddressFound, InString, Range 
    Dim SmallRange, LastTime
        
    InString = "&h" & PadEven(Hex(ExprToAddress(SearchString)))
    LastTime = False

    do      
        Range = InitRange(SmallRange, LastTime, StartAddress, EndAddress)

        set MemObj = CurrentThread.CreateMemory(StartAddress, Range)
'        MemObj.GetFromTarget
        
        if LastTime Then
            AddressFound = MemObj.FindInMemory(InString, StartAddress) 
        Else
            AddressFound = "0x" & HexLong(StartAddress) & " - 0x" & HexLong(StartAddress+Range) & "  "
            AddressFound = AddressFound & MemObj.FindInMemory(InString, StartAddress) 
        End If
    
        If adw3cli.Display(AddressFound) = 0 Then 'user aborted
            Exit Function
        End If
        
        If LastTime Then
            Exit Function
        End If
        set MemObj = Nothing
        StartAddress = Unsigned(StartAddress+Range)
        EndAddress   = Unsigned(EndAddress)
    Loop
End Function
        
' routine to translate comand parameters to range and addresses
' a1 - Start Address, 
' a2 - End address (it is converted to End Address if range input)
' a2 - End address (according to v1.1, it is not covered to end address if range input)
Function ComRange(a1, a2)   
    Dim ParseVal, Range
    a1   = ExprToAddress(a1)
    ParseVal = a2
    Range = Parse(ParseVal)
    If Range = "Range" Then
        ComRange = GetRange(0, ExprToAddress(ParseVal)) 
        a2 = a1 + ComRange - 1
    Else
        a2   = ExprToAddress(a2)

        If a1 = -1  and a2 = -1 Then            'full range specified
            a1 = 0
            a2   = &hffffffff
        End If
        ComRange = GetRange(a1, a2) + 1
    End If
End Function

Function FindString(SearchString, StartAddress, EndAddress)
On Error Resume Next    
    FindString = FindStringInt(SearchString, StartAddress, EndAddress)
    DisplayError
End Function

Function FindStringInt(SearchString, StartAddress, EndAddress)
    Dim MemObj, AddressFound, Range
    Dim SmallRange, LastTime
    LastTime = False
    
    do 
        Range = InitRange(SmallRange, LastTime, StartAddress, EndAddress)
    
        set MemObj = CurrentThread.CreateMemory(StartAddress, Range)
'        MemObj.GetFromTarget
    
        if LastTime Then
            AddressFound = MemObj.FindInMemory(SearchString, StartAddress) 
        Else
            AddressFound = "0x" & HexLong(StartAddress) & " - 0x" & HexLong(StartAddress+Range) & "  "
            AddressFound = AddressFound & MemObj.FindInMemory(SearchString, StartAddress) 
        End If
    
        If adw3cli.Display(AddressFound) = 0 Then 'user aborted
                Exit Function
        End If
        if LastTime Then
            Exit Function
        End if
        set MemObj = Nothing
        StartAddress = Unsigned(StartAddress+Range)
        EndAddress   = Unsigned(EndAddress)
    Loop
    
End Function

Function Where(ContextParam)
On Error Resume Next    
    Where = WhereInt(ContextParam)
    DisplayError
End Function
Function WhereInt(ContextParam)
    Dim CurContext, ExprObj, CurImgObj, CurFileObj, CurLineObj, CurAssObj, thisThread
    Dim ContextText, ContextImgText, ContextLineText, curStkObj
    Dim ret, StkObj, lineExist, entrystatus
    Dim ImageName, FileName, LineNum, AssAddress
    Dim NumOfEntry, RequiredIndex
    
    Dim StackEntryList
    set StackEntryList = currentthread.GetStackEntries(0)

    dim size
    size = StackEntryList.count
    if size <= 1 then
        Context(-1)
    Else
        If ContextParam = -1 Then
            set curStkObj = CurrentThread.CurrentStackEntry
            if curStkObj.Handle = 0 then 
                Context(-1)
                Exit Function
            Else
                set StkObj = curStkObj
            End If
        Else
            ret = Parse(ContextParam)
            NumOfEntry = StackEntryList.count
            RequiredIndex = ParseStackIndex(ContextParam, NumOfEntry)
            set StkObj = StackEntryList(RequiredIndex)    
        End If

        If StkObj.PosObjectStatus(PosImgObj) = True Then 
            set CurImgObj = StkObj.Image
            ImageName = CurImgObj.Name
            If StkObj.PosObjectStatus(PosFileObj) = True Then
                set CurFileObj = StkObj.File
                FileName = CurFileObj.Name
                If StkObj.PosObjectStatus(PosLineObj) = True Then
                    set CurLineObj = StkObj.SourceLine
                    LineNum = Chr(124) & (CurLineObj.LineNumber + 1)
                Else
                    LineNum = "N/A"
                End If
            Else
                FileName = "No file information available."
            End If
        Else
            ImageName = "No image"
        End If

        If StkObj.PosObjectStatus(PosAsmObj) = True Then
            set CurAssObj = StkObj.AsmLine
            AssAddress = CurAssObj.FormatAddress
        Else
            AssAddress = "Disassembly not available."
        End If
        
        ContextImgText = "Image:" & Chr(9) & ImageName & Chr(124) & Chr(64) & AssAddress & Chr(13) & Chr(10)
        ContextLineText = "File:" & Chr(9) & FileName & LineNum & Chr(13) & Chr(10)
        
        ContextText = ContextImgText & ContextLineText
        adw3cli.Display(ContextText)
    End If
End Function

Function StackEntries(StackIndex)
On Error Resume Next    
    StackEntries = StackEntriesInt(StackIndex)
    DisplayError
End Function

Function StackEntriesInt(StackIndex)        
    Dim txt
    Dim StackEntryList
    set StackEntryList = currentthread.GetStackEntries(StackIndex) 
    txt = GetStackListText(StackEntryList, StackIndex)
    adw3cli.Display(txt)
End Function

Function GetAllScopeVar(ContextStr, VarScope, fmtKey, fmtStr, scopeFlag, tempStr)
    Dim iter,  tmp
    iter = 0
    do while iter < 3 
        tmp = ContextStr
		If iter = 2 Then
			scopeFlag = FALSE
		End If
        ConVariablesInt tmp, iter, fmtKey, fmtStr, scopeFlag, tempStr
        iter = iter + 1
    Loop
	If scopeFlag = FALSE Then
		adw3cli.Display(tempStr)
	End If
End Function

Function ConVariables(ContextStr, VarScope, fmtKey, fmtStr)
On Error Resume Next
	scopeFlag = FALSE
	tempStr = AddSpace("Name", 24) & AddSpace("Type",14) & "Value" & Chr(13) & Chr(10)
    ConVariables = ConVariablesInt(ContextStr, VarScope, fmtKey, fmtStr, socpeFlag, tempStr)
    DisplayError
End Function

Function ConVariablesInt(ContextStr, VarScope, fmtKey, fmtStr, scopeFlag, tempStr)
    Dim varList, ctxx, currentContext, expr, exprName, ConVariable
	Dim GlobalHeader, LocalHeader, ClassHeader, NoHeader, VarText, columnheader 
	Dim AllScopeFlag
    Dim StackEntryList
    call CheckForImage(-1)
    set currentContext = CurrentThread.CurrentStackEntry
 
    If VarScope = -1 Then
		scopeFlag = TRUE
        GetAllScopeVar ContextStr, VarScope, fmtKey, fmtStr, scopeFlag, tempStr
        Exit Function
    End If
   
    If ContextStr <> -1 Then
        call ContextInt(ContextStr)
    End If
    
    set ConVariable = CurrentThread.ContextVariable
    Select Case VarScope
        Case AxdGlobal
            set varList = ConVariable.GlobalVars
            GlobalHeader = "Global Variables:" & Chr(13) & Chr(10)
			VarText = tempStr & GlobalHeader
			'adw3cli.Display("Global Variables:" & Chr(13) & Chr(10))
        Case AxdLocal
            set varList = ConVariable.LocalVars
            LocalHeader = "Local Variables:" & Chr(13) & Chr(10)
			VarText = tempStr & LocalHeader
			'adw3cli.Display("Local Variables:" & Chr(13) & Chr(10))
        Case AxdClass
            set varList = ConVariable.ClassVars
            ClassHeader = "Class Variables:" & Chr(13) & Chr(10)
			VarText = tempStr & ClassHeader
			'adw3cli.Display("Class Variables:" & Chr(13) & Chr(10))
    End Select

    dim size
    size = varList.count
    If size = 0 Then
        call Restore(currentContext)    'restore old stack  
		NoHeader = "No variable available within this scope." & Chr(13) & Chr(10)
		'VarText = tempStr & NoHeader
		'adw3cli.Display("No variable available within this scope." & Chr(13) & Chr(10))
		tempStr = VarText & NoHeader
        Exit Function
    End If

    'columnheader = AddSpace("Name", 24) & AddSpace("Type",14) & "Value" & Chr(13) & Chr(10)
    'adw3cli.Display(columnheader)
    'VarText = VarText & columnheader

    dim iter, thisvar, ctvarType, tf
    iter = 0
    fmt = GetFormatFromKey(fmtKey)
    If fmtStr = -1 Then
        fmtStr = DefDisplayFormatPar
    End If
    do while iter < size
        set thisvar = varList(iter)
        ctvarName = thisvar.GetName
        tf = FormatValueSimple(thisvar, fmt, fmtStr)
        txt = AddSpace(ctvarName,24) & tf & Chr(13) & Chr(10)
        VarText = VarText & txt
        iter = iter + 1
    Loop
	
	If scopeFlag = False Then
		adw3cli.Display(VarText)
	Else
		tempStr = VarText
    End If

    call Restore(currentContext)        'restore old stack
    
End Function


Function Restore(currentContext)
    If IsNull(currentContext) = True Then
        err.raise 1000, "VBScript", "No stack entry found."
    End If
    CurrentThread.CallStackLocate currentContext.Handle
End Function

Function FormatValueSimple(ByRef ContextObj, fmt, fmtStr)
    Dim ctvarType, TypeString, txt, min
    ctvarType = ContextObj.GetType
    TypeString = TypeValueToName(ctvarType)

    If fmt = -1 Then
        fmt = DefDisplayFormat
    End If
    If fmtStr = -1 Then
        fmtStr = DefDisplayFormatPar
    End If
    
    If ContextObj.IsParent = True and ContextObj.GetSize > 0 Then
        If ContextObj.GetType = &H2000 Then
            FormatValueSimple = AddSpace(TypeString, 14) & ContextObj.Format(fmt, fmtStr)
        Else
            FormatValueSimple = AddSpace(TypeString, 14)
        End If
    Else
        FormatValueSimple = AddSpace(TypeString, 14) & ContextObj.Format(fmt, fmtStr)
    End If
    
End function

Function FormatValue(ByRef ContextObj, fmt, fmtStr)
    Dim ctvarType, TypeString, txt, min
    ctvarType = ContextObj.GetType
    TypeString = TypeValueToName(ctvarType)

    If fmt = -1 Then
        fmt = DefDisplayFormat
    End If
    If fmtStr = -1 Then
        fmtStr = DefDisplayFormatPar
    End If

    If ContextObj.IsParent = True Then
    '
    ' and ContextObj.NumOfChildren > 0 Then
    '
        Dim subList, subSize, subItem, subIter
        set subList = ContextObj.DeReference
        subSize = subList.count
        subIter = 0
        subText = Chr(13) & Chr(10)
        do while subIter < subSize
            set subItem = subList(subIter)
            subName = subItem.GetName
            subType = subItem.GetType
            subValue = subItem.Format(fmt, fmtStr)
            subTypeString = TypeValueToName(subType)
            subText = subText & Chr(9) & AddSpace(subName, 14) & AddSpace(subTypeString, 14) & subValue & Chr(13) & Chr(10)
            subIter = subIter + 1
        Loop
        If ContextObj.GetType = &H2000 Then
            FormatValue = AddSpace(TypeString, 14) & ContextObj.Format(fmt, fmtStr) & subText
        Else
            FormatValue = AddSpace(TypeString, 14) & subText
        End If
    Else
        FormatValue = AddSpace(TypeString, 14) & ContextObj.Format(fmt, fmtStr) & Chr(13) & Chr(10)
    End If
End function

Function Context(StackIndex)                    '<--- CLI command
On Error Resume Next    
    Context = ContextInt(StackIndex)
    DisplayError
End Function

Function ContextInt(StackIndex)
    Dim CurContext, ExprObj, CurImgObj, CurFileObj, CurLineObj, CurAddObj
    Dim ContextText, ContextImgText, ContextLineText
    Dim ret, StkObj, EntryText, lineExist
    Dim ImageName, FileName, LineNum, AssAddress

    If StackIndex = -1 Then
        set CurContext = CurrentThread.Context
        If CurContext.PosObjectStatus(PosImgObj) = True Then 
            set CurImgObj = CurContext.Image
            ImageName = CurImgObj.Name
            If CurContext.PosObjectStatus(PosFileObj) = True Then
                set CurFileObj = CurContext.File
                FileName = CurFileObj.Name
                If CurContext.PosObjectStatus(PosLineObj) = True Then
                    set CurLineObj = CurContext.SourceLine
                    LineNum = Chr(124) & (CurLineObj.LineNumber + 1)
                Else
                    LineNum = "N/A"
                End If
            Else
                FileName = "No file information available."
            End If
        Else
            ImageName = "No image"
        End If

        If CurContext.PosObjectStatus(PosAsmObj) = True Then
            set CurAssObj = CurContext.AsmLine
            AssAddress = CurAssObj.FormatAddress
        Else
            AssAddress = "Disassembly not available."
        End If
        
        ContextImgText = "Image:" & Chr(9) & ImageName & Chr(124) & Chr(64) & AssAddress & Chr(13) & Chr(10)
        ContextLineText = "File:" & Chr(9) & FileName & LineNum & Chr(13) & Chr(10)
        
        ContextText = ContextImgText & ContextLineText
        adw3cli.Display(ContextText)
    Else    
        ret = Parse(StackIndex)
        Dim STackEntryList    
        set StackEntryList = currentthread.GetStackEntries(0) 
        Dim NumOfEntry, RequiredIndex
        NumOfEntry = StackEntryList.count
        RequiredIndex = ParseStackIndex(StackIndex, NumOfEntry)
        set StkObj = StackEntryList(RequiredIndex)
        If IsNull(StkObj) = True Then
            err.raise 1000, "VBScript", "No stack entry found."
        End If
        CurrentThread.CallStackLocate StkObj.Handle
    End If
End Function

Function SetPC(PCAddress)           '<-----  CLI command
On Error Resume Next
    SetPC = SetPCInt(PCAddress)
    DisplayError
End Function

Function SetPCInt(PCAddress)
    Dim NewValue
    NewValue = ExprToAddress(PCAddress)
    CurrentThread.SetPC(NewValue)   
End Function

Function Statistics(StatEntryName)               '<-----  CLI command
On Error Resume Next
    Statistics = StatisticsInt(StatEntryName)
    DisplayError
End Function

Function StatisticsInt(StatEntryName)
    Dim StatList, ListSize
    Dim StatObj
    Dim iter
    iter = 0
    If StatEntryName = -1 Then
        set StatList = session.GetStatistics
        ListSize = StatList.count
        HeaderText = AddSpace("Reference Points", 20) & StatList(0).GetCycleNames & Chr(13) & Chr(10)
        do while iter < ListSize
            set StatObj = StatList(iter)
            EntryName = StatObj.GetEntryName
            EntryValues = StatObj.GetEntryValues

            DisplayText = DisplayText & AddSpace(EntryName, 20) & EntryValues & Chr(13) & Chr(10)
            iter = iter + 1
        Loop
        DisplayText = HeaderText & DisplayText
        adw3cli.Display(DisplayText)
        set StatList = Nothing
    Else
        res = session.AddStatEntry(StatEntryName)
        If res = FALSE Then
            err.raise 1000, "VBScript", "Reference point's name already exists."
        End If
    End If
End Function

Function ClearStat(StatEntryName)                  '<-----  CLI command
On Error Resume Next
    If StatEntryName = "$statistics" Then
        err.raise 1000, "VBScript", "Only user defined statistics reference points can be deleted."
    Else
        ClearStat = ClearStatInt(StatEntryName)
    End If    
    DisplayError
End Function

Function ClearStatInt(StatEntryName)
    Dim StatList
    set StatList = session.GetStatistics
    size = StatList.count
    If size = 0 Or size = 1 Then
        err.raise 1000, "VBScript", "There is no user reference point."
    End If
    res = session.DeleteStatEntry(StatEntryName)
    If res = FALSE Then
        err.raise 1000, "VBScript", "Failed to delete reference point."
    End If

End Function

Function DbgInternals               '<-----  CLI command
On Error Resume Next
    DbgInternals = DbgInternalsInt
    DisplayError
End Function

Function DbgInternalsInt
    Dim DbgInterList, size, iter, thisInter, interText, text
    Dim entryList, entrySize, entryItem, entryIter, entryExpr, entryText, entryValue, entryName
    Dim arrayList, arraySize, arrayItem, arrayIter, arrayExpr
    set DbgInterList = session.GetDbgInternals
    size = DbgInterList.count
    iter = 0
    do while iter < size
        set thisInter = DbgInterList(iter)
        varName = thisInter.GetName
        If varName = "$statistics" Then
            Dim subList, subSize, subItem, subIter, curContext, interExpr
            set interExpr = CurrentThread.CreateExpression(varName)
            set curContext = CurrentThread.Context
            interExpr.SetContext(curContext.Handle)
            set subList = interExpr.DeReference
            subSize = subList.count
            subIter = 0
            subText = Chr(13) & Chr(10)
            do while subIter < subSize
                set subItem = subList(subIter)
                subName = subItem.GetName
                subName = AddSpace(subName, 20)
                subValue = subItem.Format(DefDisplayFormat, DefDisplayFormatPar)
                subText = subText & Chr(9) & subName & subValue & Chr(13) & Chr(10)
                subIter = subIter + 1
            Loop
            varFormat = varFormat & subText
        Else
            If varName = "$memstats" Then
                set arrayExpr = CurrentThread.CreateExpression(varName)
                set curContext = CurrentThread.Context
                arrayExpr.SetContext(curContext.Handle)
                set arrayList = arrayExpr.DeReference
                arraySize = arrayList.count
                arrayIter = 0
                entryText = Chr(13) & Chr(10)
                Err.clear
                do while arrayIter < arraySize
                   
                    set arrayItem = arrayList(arrayIter)
                    subName = "$memstats" & arrayItem.GetName
                    entryText = entryText & subName & Chr(13) & Chr(10) 
                    set entryExpr = CurrentThread.CreateExpression(subName)
                    set curContext = CurrentThread.Context
                    entryExpr.SetContext(curContext.Handle)
                    set entryList = entryExpr.DeReference
                    entrySize = entryList.count
                    entryIter = 0
                    do while entryIter < entrySize
                        set entryItem = entryList(entryIter)
                        entryName = entryItem.GetName
                        If entryName <> ".name" then
                            entryValue = entryItem.Format(DefDisplayFormat, DefDisplayFormatPar)
                        Else
                            entryValue = entryItem.GetString
                        End If
                        entryName = AddSpace(entryName, 20)
                        entryText = entryText & Chr(9) & entryName & entryValue
                        entryText = entryText & Chr(13) & Chr(10)
                        entryIter = entryIter + 1
                    Loop
                    arrayIter = arrayIter + 1
                Loop
                varFormat = entryText
            Else
                varFormat = thisInter.Format(DefDisplayFormat, DefDisplayFormatPar) & Chr(13) & Chr(10)
            End If  
        End If
        text = varName & Chr(9) & varFormat    
        interText = interText & text
'        adw3cli.Display(interText) move to outside loop DE41815
        iter = iter + 1
    loop
	adw3cli.Display(intertext)

End Function

Function GetRegDetails(RegisterIndex, BankIndex)
    Dim ret, localIndex
    localIndex = RegisterIndex
    ret = Parse(localIndex)
    If ret = "Position" Then
        GetRegDetails = SplitIndex(RegisterIndex, BankIndex)
 	Else
        GetRegDetails = RegisterIndex
        BankIndex = "current"
    End If
End Function

Function GetBankRegister(RegisterIndex, RegBankIndex)
    Dim CurRegBank
    dim Register
    dim count
    dim iter
    dim txt, RegisterList
    dim regtext, ListText
    dim fmt
    fmt = 1     'doing decimal

    If InStr(1, RegBankIndex, "User", 1) <> 0 or InStr(1, RegBankIndex, "System", 1) <> 0 Then
		RegBankIndex = "User/System"
	End If

    call UpdateRegBankList(-1)
    set CurRegBank = GetObjFromKey(RegBankIndex, RegBankList, RegBankNameString, RegBankIDString, index)   
    set RegisterList = CurRegBank.Registers()
    count = RegisterList.count 
    iter = 0
    do while iter < count
        set Register = RegisterList(iter)
        if Register.Name = RegisterIndex Then
            GetBankRegister = GetRange(0, Register.Format("dec", "-1"))
            Exit Function
        End If
        iter = iter + 1
    loop
    err.raise 1000, "VBScript", "Source register not specified correctly." 
End Function

Function GetValue(RegisterIndex)
    Dim ret, localIndex, BankIndex
    localIndex = RegisterIndex
    ret = Parse(localIndex)
    If ret = "Position" Then
        RegisterIndex = SplitIndex(RegisterIndex, BankIndex)
		If RegisterIndex = "r15" Then
			RegisterIndex = "pc"
		End If
		GetValue = GetBankRegister(RegisterIndex, BankIndex)
    Else
        GetValue = ExprToAddress(RegisterIndex)
    End If
End Function

Function SetReg(RegisterIndex, NewValueExpr)            '<-----  CLI command
On Error Resume Next
    SetReg = SetRegInt(RegisterIndex, NewValueExpr)
    DisplayError
End Function

Function SetRegInt(RegisterIndex, NewValueExpr)
    Dim index, BankIndex, RegIndex, RegCount, iter
    Dim ThisBank, LocalRegList, ThisReg, CurReg, BankText
    RegIndex = GetRegDetails(RegisterIndex, BankIndex)
	NewValue = GetValue(NewValueExpr)
	If RegIndex = "r15" Then
		currentthread.SetPc(NewValue)
	Else
		call UpdateRegBankList(-1)
		set ThisBank = GetObjFromKey(BankIndex, RegBankList, RegBankNameString, RegBankIDString, index)
		set LocalRegList = ThisBank.Registers
		RegCount = LocalRegList.Count
		iter = 0
		do while iter < RegCount
			set ThisReg = LocalRegList(iter)
			LocalRegNameString = LocalRegNameString &  CStr(ThisReg.Name) & "%" & CStr(iter) & "%"
			iter = iter + 1
		loop
		set CurReg  = GetObjFromKey(RegIndex, LocalRegList, LocalRegNameString, RegIDString, index)
		If CurReg.Name = "pc" Then
			currentthread.SetPC(NewValue)
		Else
			CurReg.SetValue(NewValue)
		End If
	End If
End Function


Function SetProc(ProcIndex)         '<-----  CLI command
On Error Resume Next
    SetThread ProcIndex
    DisplayError
End Function


Function DisplayLine(LinesToDisplay)
    DefDisplayLine = LinesToDisplay
End Function


Function SetThread(ProcIndex)
    Dim threadObj, ProcObj
    set ProcObj = CheckInitProc(ProcIndex)
    set threadObj = ProcObj.Thread
    session.SetCurrentThread(threadObj.Handle)
    set CurrentThread = session.GetCurrentThread
End Function

Function StackIn()          '<-----  CLI command
On Error Resume Next
        StackIn = StackInInt()
    DisplayError
End Function

Function StackInInt()
On Error Resume Next
    Dim StackEntryList, StkObj, curStkObj
    set StackEntryList = currentthread.GetStackEntries(0) 
    set curStkObj = CurrentThread.CurrentStackEntry
    if curStkObj.Handle = 0 then 
         adw3cli.Display("Already at bottom of call stack."& Chr(13) & Chr(10))
         Exit Function
    End If

    dim size
    size = StackEntryList.count
    if size = 1 then 
        adw3cli.Display("Already at bottom of call stack."& Chr(13) & Chr(10))
        Exit Function
    End If
    
    dim index
    index = size - 1

    do while index >= 0 
        set StkObj = StackEntryList(index)
        If StkObj.Handle = curStkObj.Handle Then
            if index = size - 1 then 
                 adw3cli.Display("Already at bottom of call stack."& Chr(13) & Chr(10))
            Exit Function
            End If
            Dim EntryToSet
            set EntryToSet = StackEntryList(index + 1)
            CurrentThread.CallStackLocate EntryToSet.Handle
            Exit Function            
        End If
        index  = index - 1
    Loop
    adw3cli.Display("Failed to locate current stack position." & Chr(13) & Chr(10))
End Function

Function StackOut()         '<-----  CLI command
On Error Resume Next
        StackOut = StackOutInt()
    DisplayError
End Function

Function StackOutInt()
    Dim StackEntryList, StkObj, curStkObj
    Dim EntryToSet
    
    set StackEntryList = currentthread.GetStackEntries(0) 
    
    set curStkObj = CurrentThread.CurrentStackEntry
    dim size
    size = StackEntryList.count
    
    if size = 1 then
        adw3cli.Display("Already at top of call stack."& Chr(13) & Chr(10))
        Exit Function
    End If

    if curStkObj.handle = 0 then
        set EntryToSet = StackEntryList(size - 2)   
        CurrentThread.CallStackLocate EntryToSet.Handle
        Exit Function
    End If
            
    dim index
    index = 0

    do while index < size 
        set StkObj = StackEntryList(index)
        If StkObj.Handle = curStkObj.Handle Then
            if index = 0 then
                adw3cli.Display("Already at top of call stack."& Chr(13) & Chr(10))
                Exit Function
            End If
    
            set EntryToSet = StackEntryList(index - 1)
            CurrentThread.CallStackLocate EntryToSet.Handle
            Exit Function            
        End If
        index  = index + 1
    Loop
End Function

Function WatchPt(Expr, BkCount)                         '<-----  CLI command
On Error Resume Next
    WatchPt = WatchPtInt(Expr, BkCount)
    DisplayError
End Function
Function WatchPtInt(Expr, BkCount)
    Dim SingleWatch, VarScope', WatchPointList
    dim count, iter, value, txt, HeaderText, tmp
    If Expr = -1 Then
        dim thiswpt
        set WatchPtList = session.WatchPoints
        count = WatchPtList.Count
        If count = 0 Then
            adw3cli.Display("No watch point available."& Chr(13) & Chr(10))
            Exit Function
        End If
        iter = 0
        do while iter < count
            set thiswpt = WatchPtList(iter)
            VarScope = thiswpt.SwProperty
            If VarScope = TRUE Then
                Glb = "HW"
                Glb = Glb & Chr(40) & thiswpt.GetHWResource & Chr(41)
            Else
                Glb = "SW"
            End If

            ActionState = thiswpt.IsEnabled
            If ActionState = TRUE Then
                StateStr = ""'"Enabled"
            Else
                StateStr = Chr(88)'"Disabled"
            End If
'!!! 15/01/01, waiting for decision of watch point scope check.
'            ActionScope = thiswpt.IsInScope
'			If ActionScope = TRUE Then
'                ScopeStr = ""'"within scope"
'            Else
'                ScopeStr = Chr(63)'"out of scope"
'            End If
'            StateText = StateStr & ScopeStr' & Space(1) & Glb
            StateText = StateStr' & Space(1) & Glb
            WatchExpr2 = thiswpt.GetExpr2
            if WatchExpr2 = "" Then
                WatchExpr2 = ""
            Else
                WatchExpr2 = " == " & WatchExpr2
            End If

            ItemStr = thiswpt.GetExpr1 & WatchExpr2

            set currProc = GetCurrentProcessor
            ProcName = currProc.Name
        
            If thiswpt.GetAction = 0 Then
                ActionStr = "Break"
            Else
                ActionStr = "Log: " & thiswpt.GetActionString
            End If

            WatchCount = thiswpt.GetCount
            ConditionStr = thiswpt.GetCondition
            If ConditionStr = "" Then
                ConditionStr = "N/A"
            End If

            WatchSizeVar = thiswpt.GetWPSize
            If WatchSizeVar > 1 Then
                WatchSizeVar = CStr(WatchSizeVar) & "bytes"
            ElseIf WatchSizeVar = 0 Then
                WatchSizeVar = ""
            Else    
                WatchSizeVar = CStr(WatchSizeVar) & "byte"
            End If
            
            If thiswpt.IsFixedMemory = TRUE Then
                WatchingStr = "Fixed memory"
                address = Chr(91) & thiswpt.GetWatchMemAddress & Chr(93)
                If thiswpt.GetWatchIsForced = TRUE Then
					ForceText = "Forced"
				Else
					ForceText = ""
				End If
				WatchingStr =WatchingStr & address & ForceText
            Else
                If thiswpt.IsRegister = TRUE Then
                    WatchingStr ="Register" 
                ElseIf thiswpt.IsMemory Then
                    WatchingStr ="Memory" 
                    address = Chr(91) & thiswpt.GetWatchMemAddress & Chr(93)
                    If thiswpt.GetWatchIsForced = TRUE Then
						ForceText = "Forced"
					Else
						ForceText = ""
					End If
					WatchingStr = WatchingStr & address & ForceText
                End If
            End If

            WatchingStr = WatchingStr & Space(1) & WatchSizeVar

            txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & AddSmallSpace(StateText, 2) & AddSmallSpace(ItemStr, 18) & AddSmallSpace(WatchingStr, 38) & AddSmallSpace(WatchCount, 10) & AddSmallSpace(ConditionStr, 20) & Glb & Space(1) & ProcName & Space(1) & ActionStr & Chr(13) & Chr(10)
            iter = iter + 1
        loop
        WatchPtHeader = "Index" & Chr(9) & Space(2) & AddSmallSpace("Item", 18) & AddSmallSpace("Watching", 38) & AddSmallSpace("Count", 10) & AddSmallSpace("Condition", 20)  & "Additional" & Chr(13) & Chr(10)
        DisplayText = WatchPtHeader & txt
        adw3cli.Display(DisplayText) 
    Else
        set SingleWatch = session.CreateWatchPoint
        
        tmp = Expr                                      'added for error checking DE34283
        call ExprToAddress(tmp)                         'make sure expression is valid

        'following added for DE34791, when a vb form hex is passed to vc source.
        If Left(CStr(Expr), 2) = "&h" Then
            Dim newHexStr
            newHexStr = Replace(Expr, "&h", "0x", 1, 1)
            Expr = newHexStr
        End If
		If Left(CStr(Expr), 1) = Chr(37) Then
			Expr = ExprToAddress(Expr)
		End If
        SingleWatch.SetExpr1(Expr)
        If BkCount = -1 Then
            BkCount = 1
        End If
        SingleWatch.InitCount(BkCount)
        SingleWatch.InitThread(CurrentThread.Handle)
        SingleWatch.InitSet
    End If  
End Function

Function ClearWatch(WatchPoint)                         '<-----  CLI command
On Error Resume Next
    ClearWatch = ClearWatchInt(WatchPoint)
    DisplayError
End Function

Function ClearWatchInt(WatchPoint)
    Dim  watptList
	Dim IndexCheck
	IndexCheck = Parse(WatchPoint)
    
	If IndexCheck = "Name" and WatchPoint = "all" Then
        ClearAllWatchInt
        Exit Function
    End If

    If IndexCheck <> "Index" Then
        err.raise 1000, "VBScript", "Index not specified correctly"
    End If
    set watptList = session.WatchPoints
    If watptList.count = 0 Then
        err.raise 1000, "VBScript", "No more watchpoints to clear"
    End If
    session.DeleteWatchPoint WatchPoint
End Function

Function Watch(WatchExpr, fmtKey, fmtPar)                          '<-----  CLI command
On Error Resume Next
    Watch = WatchInt(WatchExpr, fmtKey, fmtPar)
    DisplayError
End Function

Function WatchInt(WatchExpr, fmtKey, fmtPar)
    Dim txt, expr
    set expr = currentthread.CreateExpression(WatchExpr)
    if expr.IsValid() <> FALSE then
        fmt = GetFormatFromKey(fmtKey)
        txt = FormatValue(expr, fmt, fmtPar)    
    Else
        txt = expr.GetTarget() & Chr(13) & Chr(10)
    End if            
    txt = AddSpace(WatchExpr, 22) & txt 
    adw3cli.Display(txt)
End Function


Function SetWatchProps(WatchPoint, PorpertyNameStr, PropExpr)
On Error Resume Next
    call SetWatchPropsInt(WatchPoint, PorpertyNameStr, PropExpr)
    DisplayError
End Function

Function SetWatchPropsInt(WatchPoint, PropertyNameStr, PropExpr)
    
    Dim wptlist, ThisWatch
    if Parse(WatchPoint) <> "Index" Then
        err.raise 1000, "VBScript", "Index not specified correctly"
    End If
    
    set wptlist = session.WatchPoints

    If wptlist.count = 0 Then
        err.raise 1000, "VBScript", "No watch point is set"
    End If

    set ThisWatch = wptlist(WatchPoint)
    Select Case PropertyNameStr
        case "state":
            If PropExpr <> "enable" and PropExpr <> "disable" Then
                err.raise 1000, "VBScript", "Please use on/off to toggle break state"               
                Exit Function
            ElseIf PropExpr = "enable" Then
                PropExprVar = 1
            Else
                PropExprVar = 0
            End If
            Status = ThisWatch.SetEnabled(PropExprVar)
'           If Status <> TRUE or Status <> 1 Then
'               err.raise 1000, "VBScript", "Failed to disable watch point"
'           End If
        case "processor":
            Dim CurProc, ProcThread
            set CurProc = CheckInitProc(PropExpr)
            set ProcThread = CurProc.Thread
            Status = ThisWatch.ModifyTargetProcessor(ProcThread.Handle)           
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify processor on target"
            End If
        case "condition":
            Status = ThisWatch.ModifyTargetCondition(PropExpr)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify condition on target"
            End If
        case "log_text":
            ThisWatch.SetAction(1)
            ThisWatch.SetActionString(PropExpr)
        case "value":
			If (IsNumeric(propexpr)) = TRUE Then
				If Left(CStr(PropExpr), 2) = "&h" Then
					Dim newHexStr
					newHexStr = Replace(PropExpr, "&h", "0x", 1, 1)
					PropExpr = newHexStr
				ElseIf Left(CStr(PropExpr), 2) = "&o" Then
					Dim newDecStr
					newDecStr = CLng(PropExpr)
					PropExpr = newDecStr
				End If
				Status = ThisWatch.ModifyTargetValue(PropExpr)
				If Status <> TRUE Then
					err.raise 1000, "VBScript", "Failed to modify value on target"
				End If
			Else
				err.raise 1000, "VBScript", "Invalid comparison value"
			End If
        case "watch_size":	'de40825
			If PropExpr <> 32 and PropExpr <> 16 and PropExpr <> 8 Then
				err.raise 1000, "VBScript", "Invalid size parameter, use 8/16/32"
               Exit Function
            ElseIf PropExpr = 32 Then
			    PropVar = 4
            ElseIf PropExpr = 16 Then
			    PropVar = 2             
			Else
				PropVar = 1
			End If
            Status = ThisWatch.ModifyTargetWPSize(1, PropVar)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to force watch point size on target"
            End If
		case "break_size":
            If strcomp(PropExpr, "arm", 1) <> 0 and strcomp(PropExpr, "thumb", 1) <> 0 and strcomp(PropExpr, "auto", 1) <> 0 Then
				err.raise 1000, "VBScript", "Invalid size parameter, use ARM/THUMB/AUTO"
               Exit Function
            ElseIf strcomp(PropExpr, "arm", 1) = 0 Then
			    PropVar = 4
            ElseIf strcomp(PropExpr, "thumb", 1) = 0 Then
			    PropVar = 2             
			Else
				PropVar = 1
			End If
            Status = ThisWatch.ModifyTargetWPSize(1, PropVar)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to force watch point size on target"
            End If
        case "count":
            Status = ThisWatch.ModifyTargetCount(CInt(PropExpr))
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify count on target"
            End If
        case else
            err.raise 1000, "VBScript", "Invalid property name string"                      
            Exit Function        
        End Select
    
End Function

Function SetWatch(Expr1, Expr2)                         '<-----  CLI command
On Error Resume Next
    SetWatch = SetWatchInt(Expr1, Expr2)
    DisplayError
End Function

Function SetWatchInt(Expr1, Expr2)
    Dim ThisWatch
    Dim NewValue
    set ThisWatch = CurrentThread.CreateExpression(Expr1)
	If ThisWatch.IsValid = FALSE Then
		err.raise 1000, "VBScript", "Invalid expression"
	Else
		NewValue = ExprToAddress(Expr2)
		ThisWatch.SetValue(NewValue)
	End If
End Function


Function RunToPos(Position, ProcIndex)                  '<-----  CLI command
On Error Resume Next
    RunToPos = RunToPosInt(Position, ProcIndex)
    DisplayError
End Function

Function RunToPosInt(Position, ProcIndex)
    dim ImagePosObj, FilePosObj, LinePosObj, ThisThread, LineObjList, AsmObjList
    dim ImagePos, FilePos, LinePos, Address, Col, Lev, Index, AsmPosObj

    Lev = EvaluatePosition(Position, ImagePos, FilePos, LinePos, Col, Address)

    set ThisThread = GetThreadFromProc(ProcIndex)

    Select Case Lev
    Case 0
        set ImagePosObj = GetCurrentImage       
        If ImagePosObj is Nothing Then
            'just an address
				set AsmObjList = CurrentThread.AsmLines(Address, 2, DefInstrSize)
                set AsyncAsm = Nothing
                set AsyncAsm = AsmObjList(0)
                ThisThread.RunToAddress 0, AsyncAsm.Handle
        Else
                call CheckForImage(-1)
                set AsmObjList = ImagePosObj.AsmLines(Address, 2, DefInstrSize)'startline, 
                set AsyncAsm = Nothing
                set AsyncAsm = AsmObjList(0)
                ThisThread.RunToAddress ImagePosObj.Handle, AsyncAsm.Handle
        End If
     Case 1
        set ImagePosObj = CheckForImage(-1)
        If ImagePos = "xx" Then  
            set FilePosObj = GetObjFromKey(FilePos, FileList, FileNameString, FileIDString, index)
            set LineObjList = FilePosObj.Lines(LinePos - 1, 1)
            index = LineObjList.count
            If index <> 0 Then   
                set AsyncSource = nothing
                set AsyncSource = LineObjList(0)
                ThisThread.RunToLine ImagePosObj.Handle, FilePosObj.Handle, AsyncSource.Handle, 0
            Else
                err.raise 1000, "VBScript", "Line not found in file."                    
            End If        
         Else
            set ImagePosObj = GetObjFromKey(ImagePos, ImageList, ImageNameString, ImageIDString, index)
            set AsmObjList = ImagePosObj.AsmLines(Address, 2, DefInstrSize)'startline, numlines
            set AsyncAsm = Nothing
            set AsyncAsm = AsmObjList(0)
            ThisThread.RunToAddress ImagePosObj.Handle, AsyncAsm.Handle
        End If
    Case 2
        call CheckForImage(-1)
        set ImagePosObj = GetObjFromKey(ImagePos, ImageList, ImageNameString, ImageIDString, index)
        set FilePosObj = GetObjFromKey(FilePos, FileList, FileNameString, FileIDString, index)
        set LineObjList = FilePosObj.Lines(LinePos - 1, 1)'startline, numlines
        index = LineObjList.count
        If index <> 0 Then   
            set AsyncSource = nothing
            set AsyncSource = LineObjList(0)
            ThisThread.RunToLine ImagePosObj.Handle, FilePosObj.Handle, AsyncSource.Handle, 0
        Else
            err.raise 1000, "VBScript", "Line not found in file." 
        End If
    End Select
End Function

Function Run(ProcIndex, InstrSize)                  '<-----  CLI command
On Error Resume Next
    Run = RunInt(ProcIndex, InstrSize)
    DisplayError
End Function

Function RunInt(ProcIndex, InstrSize)
    Dim res, ThisThread

    set ThisThread = GetThreadFromProc(ProcIndex)

    If InstrSize = -1 Then
        InstrSize = DefInstrSize
    End If 

    res = ThisThread.Run(InstrSize)
End Function

Function Break(Position, BreakCount)                '<-----  CLI command
On Error Resume Next
        Break = BreakInt(Position, BreakCount)
    DisplayError
End Function

Function BreakInt(Position, BreakCount)
    Dim ImagePos, FilePos, LinePos, Col, Address
    Dim currProc
    Dim source, bkFile, asm, img, VarScope
    Dim ImagePosObj, FilePosObj, LinePosObj, SingleBreak', BreakPointList
    Dim size, AsmPosObj
    Dim NextLineFlag

    BreakCount = CLng(BreakCount)
    If Position = -1 Then
        dim iter, thisbpt, txt, count
        set BreakPtList = session.BreakPoints
        count = BreakPtList.count
        If count = 0 Then
            err.raise 1000, "VBScript", "No break point is set up."
        End If
        iter = 0
        do while iter < count
            set thisbpt = BreakPtList(iter)
            set source = thisbpt.SourceLine
            set asm = thisbpt.AsmLine
            set bkFile = thisbpt.File
            
            If thisbpt.PosObjectStatus(PosLineObj) = True Then
                If thisbpt.PosobjectStatus(PosFileObj) = True Then 
                    fileName = bkFile.Name
                    'fileName = "I:\Program files\arm\examples\c\dhry\dhry_1.c"
                    charLen = Len(fileName)
                    NextLineFlag = FALSE
                    If charLen > 14 Then
                        If DefPlatForm = 1 Then 'unix
                            Pos = InstrRev(fileName, Chr(47), -1, 1)
                        Else
                            Pos = InstrRev(fileName, Chr(92), -1, 1)
                        End If
                        If(Pos > 0) Then
                            FullFileName = fileName
                            NextLineFlag = TRUE
                            fileName = Right(fileName, charLen - Pos)
                        End If
                    End If
                End If
                LineNo = source.LineNumber + 1  'this is 0 based
            Else
                fileName = "File"'"No file information available."
                LineNo = "N/A"
            End If

            If thisbpt.PosObjectStatus(PosAsmObj) = True Then
                Address = Chr(91) & asm.FormatAddress & Chr(93)
            Else
                Address = "No assemble information available."
            End If       
            If NextLineFlag = TRUE Then
                PositionTextDummy = Address & Chr(123) & FullFileName & Chr(58) & LineNo & Chr(125)
            End If

            ActionState = thisbpt.IsEnabled
            If ActionState = TRUE Then
                StateStr = ""'"Enabled"
            Else
                StateStr = Chr(88)'"Disabled"
            End If
            StateText = StateStr' & Space(1) & Glb

            PositionText = address & Chr(123) & fileName  & Chr(58) & LineNo & Chr(125)

            VarScope = thisbpt.SwProperty     
            If VarScope = TRUE Then
                Glb =  "HW"
                Glb = Glb & Chr(40) & thisbpt.GetHWResource & Chr(41)
            Else
                Glb = "SW"
            End If
            
            set currProc = GetCurrentProcessor
            ProcName = currProc.Name

            BreakCount = thisbpt.GetCount
            ConditionStr = thisbpt.GetCondition
            If ConditionStr = "" Then
                ConditionStr = "N/A"
            End If

            If thisbpt.GetAction = 0 Then
                ActionStr = "Break"
            Else
                ActionStr = "Log: " & thisbpt.GetActionString
            End If
            BreakSizeVar = thisbpt.GetBPSize
            If BreakSizeVar = 16 Then
                BreakSize = "THUMB"
            ElseIf BreakSizeVar = 32 Then
                BreakSize = "ARM"
            ElseIf  BreakSizeVar = 0 Then
                BreakSize = "AUTO"
            Else
                BreakSize = CStr(BreakSizeVar)
            End If

            OtherInfoText = ProcName & Space(1) & Glb & Space(1) & ActionStr
            If NextLineFlag = TRUE Then
                txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & AddSmallSpace(StateText, 2) & AddSmallSpace(PositionText, 30) & AddSmallSpace(CStr(BreakCount), 7) & AddSmallSpace(BreakSize, 6) & AddSmallSpace(ConditionStr, 18) & OtherInfoText & Chr(13) & Chr(10) & Chr(9) & "Position: " & PositionTextDummy & Chr(13) & Chr(10)
            Else
                txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & AddSmallSpace(StateText, 2) & AddSmallSpace(PositionText, 30) & AddSmallSpace(CStr(BreakCount), 7) & AddSmallSpace(BreakSize, 6) & AddSmallSpace(ConditionStr, 18) & OtherInfoText & Chr(13) & Chr(10)
            End If
            iter = iter + 1
        loop
        BreakPtHeader = "Index" & Chr(9) & Space(2) & AddSmallSpace("Position", 30) & AddSmallSpace("Count", 7) & AddSmallSpace("Size", 6) & AddSmallSpace("Condition", 18) & "Additional" & Chr(13) & Chr(10)
        DisplayText = BreakPtHeader & txt
        adw3cli.Display(DisplayText)
    Else
        Lev = EvaluatePosition(Position, ImagePos, FilePos, LinePos, Col, Address)
        If Err.Number > 0 Then
            DisplayError
            Exit Function
        End If
        set SingleBreak = session.CreateBreakPoint
        Select Case Lev
        case 0 
            set ImagePosObj = GetCurrentImage       
            If ImagePosObj is Nothing Then
                'just an address
                    set AsmObjList = CurrentThread.AsmLines(Address, 2, DefInstrSize)
                    set AsmPosObj = AsmObjList(0)
                    SingleBreak.SetPosFromAsm 0, AsmPosObj.Handle
            Else
				'call CheckForImage(-1)
                set AsmObjList = ImagePosObj.AsmLines(Address, 2, DefInstrSize)'startline, 
                set AsmPosObj = AsmObjList(0)
                SingleBreak.SetPosFromAsm ImagePosObj.Handle, AsmPosObj.Handle
            End If
        Case 1
            set ImagePosObj = CheckForImage(-1)
            If ImagePos = "xx" Then
                set ImagePosObj = GetCurrentImage
                set FilePosObj = GetObjFromKey(FilePos, FileList, FileNameString, FileIDString, index)
                set LineObjList = FilePosObj.Lines(LinePos -1, 1)'startline, numlines
                size = LineObjList.count
                If size <> 0 Then     
                    set LinePosObj = LineObjList(0)
                    SingleBreak.SetPosFromSource ImagePosObj.Handle, FilePosObj.Handle, LinePosObj.Handle
                Else
                    err.raise 1000, "VBScript", "Line not found in file."                    
                End If
            Else
                set ImagePosObj = GetObjFromKey(ImagePos, ImageList, ImageNameString, ImageIDString, index)
                set AsmObjList = ImagePosObj.AsmLines(Address, 2, DefInstrSize)'startline, bytes
                set AsmPosObj = AsmObjList(0)
                SingleBreak.SetPosFromAsm ImagePosObj.Handle, AsmPosObj.Handle
            End If
        Case 2
            call CheckForImage(-1)
            set ImagePosObj = GetObjFromKey(ImagePos, ImageList, ImageNameString, ImageIDString, index)
            set FilePosObj = GetObjFromKey(FilePos, FileList, FileNameString, FileIDString, index)
            If LinePos = "xx" Then 'this is the form of file|address
                set ImagePosObj = GetObjFromKey(ImagePos, ImageList, ImageNameString, ImageIDString, index)
                set AsmObjList = ImagePosObj.AsmLines(Address, 2, DefInstrSize)'startline, bytes
                set AsmPosObj = AsmObjList(0)
                SingleBreak.SetPosFromObject Address                
            Else
                set LineObjList = FilePosObj.Lines(LinePos - 1, 1)'startline, numlines
                size = LineObjList.count
                If size <> 0 Then     
                    set LinePosObj = LineObjList(0)
                    SingleBreak.SetPosFromSource ImagePosObj.Handle, FilePosObj.Handle, LinePosObj.Handle 
                Else
                    err.raise 1000, "VBScript", "Line not found in file."                   
                End If
            End If
        End Select
        If BreakCount = -1 Then
            BreakCount = 1
        End If
        SingleBreak.InitCount(BreakCount)
        SingleBreak.InitThread(CurrentThread.Handle)
        SingleBreak.InitSet
        End If
End Function

Function StopExe(ProcIndex)                 '<-----  CLI command
On Error Resume Next
        StopExe = StopExeInt(ProcIndex)
    DisplayError
End Function

Function StopExeInt(ProcIndex)
    Dim threadObj
    If DefRunMode = 0 Then 'ASYNC Then
        set threadObj = GetThreadFromProc(ProcIndex)
        threadObj.Break
    Else
        err.raise 1000, "VBScript", "Cannot stop execution in sync mode, wait for process to finish."
    End If
End Function

Function ClearBreak(BreakPoint)                 '<-----  CLI command
On Error Resume Next
    ClearBreak = ClearBreakInt(BreakPoint)
    DisplayError
End Function

Function ClearBreakInt(BreakPoint)
    Dim bptlist
    Dim IndexCheck
    IndexCheck = Parse(BreakPoint)
    If IndexCheck = "Name" and BreakPoint = "all" Then
        ClearAllBreakInt
        Exit Function
    End If
    If IndexCheck <> "Index" Then
        err.raise 1000, "VBScript", "Index not specified correctly"
    End If
    set bptlist = session.BreakPoints
    If bptlist.count = 0 Then
        err.raise 1000, "VBScript", "No more breakpoints to clear"
    End If

    session.DeleteBreakPoint BreakPoint
End Function

Function ClearAllBreakInt()
    session.DeleteAllActionPt(1)
End Function

Function ClearAllWatchInt()
    session.DeleteAllActionPt(2)
End Function


Function SetBreakProps(BreakPoint, PorpertyNameStr, PropExpr)
On Error Resume Next
    call SetBreakPropsInt(BreakPoint, PorpertyNameStr, PropExpr)
    DisplayError
End Function

Function SetBreakPropsInt(BreakPoint, PropertyNameStr, PropExpr)

    Dim bptlist, ThisBreak

    if Parse(BreakPoint) <> "Index" Then
        err.raise 1000, "VBScript", "Index not specified correctly"
    End If
    
    set bptlist = session.BreakPoints

    If bptlist.count = 0 Then
        err.raise 1000, "VBScript", "No breakpoint has been set"
    End If

    set ThisBreak = bptlist(BreakPoint)

    Select Case PropertyNameStr
        case "state":
            If PropExpr <> "enable" and PropExpr <> "disable" Then
                err.raise 1000, "VBScript", "Please use enable/disable to toggle break state"               
                Exit Function
            ElseIf PropExpr = "enable" Then
                NewPropEnableVar = 1
            Else
                NewPropEnableVar = 0
            End If
            Status = ThisBreak.SetEnabled(NewPropEnableVar)
'           If Status <> TRUE Then
'               If NewPropEnableVar = 1 Then
'                   err.raise 1000, "VBScript", "Failed to enable break point"
'               ElseIf NewPropEnableVar = 0 Then
'                   err.raise 1000, "VBScript", "Failed to disable break point"
'               End If
'           End If
        case "processor":
            Dim NewProc, ProcThread
            set NewProc = CheckInitProc(PropExpr)
            set ProcThread = NewProc.Thread
            Status = ThisBreak.ModifyTargetProcessor(ProcThread.Handle)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify processor on target"
            End If
        case "condition":
            Status = ThisBreak.ModifyTargetCondition(PropExpr)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to force watch point size on target"
            End If
        case "log_text":
            ThisBreak.SetAction(1)
            ThisBreak.SetActionString(PropExpr)
'        case "value":
'            If Left(CStr(PropExpr), 2) = "&h" Then
'                Dim newHexStr
'                newHexStr = Replace(PropExpr, "&h", "0x", 1, 1)
'                PropExpr = newHexStr
'            End If
'            Status = ThisBreak.ModifyTargetCondition(PropExpr)
'            If Status <> TRUE Then
'                err.raise 1000, "VBScript", "Failed to modify condition on target"
'            End If
        case "break_size":
            If strcomp(PropExpr, "arm", 1) <> 0 and strcomp(PropExpr, "thumb", 1) <> 0 and strcomp(PropExpr, "auto", 1) <> 0 Then
                err.raise 1000, "VBScript", "Invalid size parameter, use ARM/THUMB/AUTO"                
                Exit Function
            ElseIf strcomp(PropExpr, "arm", 1) = 0 Then
                PropVar = 32
            ElseIf strcomp(PropExpr, "thumb", 1) = 0 Then
                PropVar = 16
            Else
                PropVar = 0
            End If
            Status = ThisBreak.ModifyTargetBPSize(PropVar)
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify break point size on target"
            End If
        case "count":
            Status = ThisBreak.ModifyTargetCount(CInt(PropExpr))
            If Status <> TRUE Then
                err.raise 1000, "VBScript", "Failed to modify count on target"
            End If
        case else
            err.raise 1000, "VBScript", "Invalid property name string"              
            Exit Function
    End Select
    set ThisBreak = Nothing
End Function

Function Step(StpCount, StpVar, StpSize)                    '<-----  CLI command
On Error Resume Next
    Step = StepInt(StpCount, StpVar, StpSize)
    DisplayError
End Function

Function StepInt(StpCount, StpVar, StpSize)    

    If StpCount = -1 Then
        StpCount = 1
    End If 
   
    If StpSize = -1 Then
        StpSize = DefStepSize
    End If

	If CurrentThread.Handle = 0 Then
		err.raise 1000, "VBScript", "Invalid thread"
		Exit Function
	End If

    If StpVar = -1 Then
        CurrentThread.Step StpSize, StpCount
    ElseIf StpVar = stpIn Then
        CurrentThread.StepIn StpSize, StpCount
    ElseIf StpVar = stpOut Then
        CurrentThread.StepOut StpSize, StpCount
    Else
        adw3cli.Display("Illegal step parameter." & Chr(13) & Chr(10))
    End If
   
    
End Function

Function SourceDir()                                      '<-----  CLI command
On Error Resume Next
    SourceDir = SourceDirInt()
    DisplayError
End Function

Function SourceDirInt()    
    Dim iter, DelimiterPos
    separatedStr = session.SourceDir
    If separatedStr = "" Then
        adw3cli.Display("No source directory is set."& Chr(13) & Chr(10))
        Exit Function
    End If

    iter = 0
    If DefPlatForm = 0 Then 'for windows    
        Delimiter = Chr(59)     ';
    Else
        Delimiter = Chr(58)     ':
    End If

    DelimiterPos = Instr(separatedStr, Delimiter)
    
    do while DelimiterPos > 0
        PathStr = Left(separatedStr, DelimiterPos -1 )
        separatedStr = Right(separatedStr, Len(separatedStr) - DelimiterPos)
        DelimiterPos = Instr(separatedStr, Delimiter)
        DisplayStr = DisplayStr & Chr(35) & CStr(iter + 1) & Chr(9) & PathStr & Chr(13) & Chr(10)
        iter = iter + 1
    Loop
    DisplayStr = DisplayStr & Chr(35) & CStr(iter + 1) & Chr(9) & separatedStr & Chr(13) & Chr(10)

    adw3cli.Display(DisplayStr)
End Function

Function SetSourceDir(strPath, VirtualIndex)                  '<-----  CLI command
On Error Resume Next
    SetSourceDir = SetSourceDirInt(strPath, VirtualIndex)
    DisplayError
End Function

Function SetSourceDirInt(strPath, VirtualIndex)    
    If Left(strPath, 1) = chr(32) Then
		err.raise 1000, "VBScript", "Invalid path name"
		Exit Function
	End If
    If VirtualIndex = -1 Then
        If strPath = -1 Then
            strPath = ""
        End If
        SetSourceDirInt = session.SetSourceDir(strPath)   
    Else
        call AddSourceDirInt(strPath, VirtualIndex)    
    End If
End Function

Function AddSourceDir(strPath, index)                   '<-----  CLI command
On Error Resume Next
    ASDErrorText = "Debug >AddSourceDir is obsolete, please use SetSourceDir to set a source directory." & Chr(13) & Chr(10)
    adw3cli.Display(ASDErrorText)
'    AddSourceDir = AddSourceDirInt(strPath, index)
    DisplayError
End Function

Function AddSourceDirInt(strPath, DirIndex)
    If strPath = "" or Left(strPath, 1) = chr(32) Then
		err.raise 1000, "VBScript", "Invalid path name"
		Exit Function
	End If
    If DirIndex = -1 Then
        AddSourceDirInt = session.AddSourceDir(strPath, DirIndex)   
    Else
        If Parse(DirIndex) <> "Index" Then
            err.raise 1000, "VBScript", "Index not specified correctly"
        End If
		AddSourceDirInt = session.AddSourceDir(strPath, DirIndex + 1)   
    End If
End Function

Function Parse(IndexVar)
    Dim length
    Dim newName
    Dim newValue
    Dim x 'position of "|"
    x = 0
    If InStr(IndexVar, "|") > 0 Then
        Parse = "Position"
    Else    
        newName = Asc(Left(IndexVar, 1))
        length = Len(IndexVar)
        If newName > 47 and newName < 58  Then 'numbers
            Parse = "ID"
        ElseIf newName = 35 Then '#
            newValue = CLng(Right(IndexVar, (length - 1)))
            Parse = "Index"
            IndexVar = CStr(CLng(newValue) -1)
        ElseIf newName = 64 Then '@
            newValue = Right(IndexVar, (length - 1))

            If Left(newValue, 2) = "0x" Then
                    newStr = Replace(newValue, "0x", "&h", 1, 1)
                    newValue = newStr
            End If
            Parse = "Address"
            IndexVar = CLng(newValue)
            'IndexVar = newValue
        ElseIf newName = 36 Then '$
            newValue = CLng(Right(IndexVar, (length - 1)))
            Parse = "Symbol"
            IndexVar = newValue
        ElseIf newName = 43 Then '+
            newValue = Right(IndexVar, (length - 1))        'ML changed leave IndexVar in naitive form
            Parse = "Range"
            IndexVar = newValue
        ElseIf newName = 45 Then '-
            newValue = Right(IndexVar, (length - 1))
            If newValue = 1 Then
                Parse = "Default"
                IndexVar = -1
            Else
                Parse = "Other"
            End If
        Else    'must be string
                Parse = "Name"
        End If
    End If
End Function


Function ParseStackIndex(StackIndex, ObjectCount)
    ParseStackIndex = ObjectCount - StackIndex - 1
    if ParseStackIndex < 0 or ParseStackIndex >= ObjectCount then
        err.raise 1000, "VBScript", "Please specify a valid context"
    end if
End Function

Function SplitIndex(FullIndex, LeftIndex)
    Dim Pos
    Pos = InStr(FullIndex, "|")
    LeftIndex = Left(FullIndex, Pos - 1)
    If InStr(1, LeftIndex, "User", 1) <> 0 or InStr(1, LeftIndex, "System", 1) <> 0 Then
		LeftIndex = "User/System"
	End If
    SplitIndex = Mid(FullIndex, Pos + 1)
End Function

Function FindOccurencies (PositionStr, StringToFind)
    Dim complete, counter
    Dim dummy

    counter = 0
    complete = 1
    dummy = PositionStr

    do while complete <> 0
        complete = InStr(dummy, StringToFind)
        dummy = Mid(dummy, complete+1)
        counter = counter +1
    loop

    FindOccurencies = counter - 1

End Function

' This routine finds Entry number in a string 
' This function finds the Entry elelment separated by StringToFind, i.e. file|line|col will
' find file if Entry=1, line if Entry=2, and col if Entry=3

Function FindEntry (PositionStr, StringToFind, Entry)
    Dim complete, counter
    Dim dummy

    counter = 0
    complete = 1
    dummy = PositionStr 

    do while counter <> Entry
        complete = InStr(dummy, StringToFind)
        If complete = 0 Then 'string not found, we reached last level
            FindEntry = dummy   
        Else
            FindEntry = Left(dummy, complete-1)
            dummy = Mid(dummy, complete+1)
        End If
        counter = counter +1
    loop
End Function
    
Function EvaluatePosition(PositionStr, ImageName, FileName, LineNumber, ColumnNumber, Address)
        ImageName = "xx": FileName = "xx": LineNumber = "xx" : ColumnNumber = "xx" : Address = 0

        EvaluatePosition = FindOccurencies(PositionStr, "|")

    Select Case EvaluatePosition
        Case 0
            Address = ExprToAddress(PositionStr)
        Case 1  'file|line  --> type image|@address discontinued
            FileName   = FindEntry(PositionStr, "|", 1)
            LineNumber = FindEntry(PositionStr, "|", 2)
            If Parse(LineNumber) = "Address" Then
                Address = "@" & "0x" & Hex(LineNumber)
                LineNumber = "xx"
            End If
        Case 2  'image|file|line
            ImageName  = FindEntry(PositionStr, "|", 1)
            FileName   = FindEntry(PositionStr, "|", 2)
            LineNumber = FindEntry(PositionStr, "|", 3)
        End Select
End Function

Function GetIndexFromKey(IndexS, List)
    Dim pos
    Dim index
    dim start
    dim poss
    Dim IndexKey, ListString
    IndexKey = LCase(IndexS)
    KeyToSearch = IndexKey & Chr(37)
    ListString = LCase(List)
    pos = InStr(ListString, KeyToSearch)
    If pos = 0 Then
            GetIndexFromKey = -1
        Exit Function
    End If
    start = pos + len(IndexKey) + 1 
    poss = Instr(start, ListString, "%")
    index = CLng(Mid(ListString, start, (poss - start)))

    GetIndexFromKey = index
End Function

Function GetFormatFromKey(fmtKey)
    If fmtKey = -1 Then
        GetFormatFromKey = DefDisplayFormat
'    ElseIf Left(fmtKey, 1) = Chr(35) Then
'        If FormatRDINameString.Exists(fmtKey) Then
'            fmtItem = FormatRDINameString.Items
'            fmtIndex = Parse(fmtKey)
'            GetFormatFromKey = fmtItem(fmtKey)
'        Else
'            Err.raise 1000, "VBScript", "Index out of range."
'        End If    
    ElseIf Parse(fmtKey) = "Index" Then
        If ListCounter = false Then
            Err.raise 1000, "VBScript", "Index must refer to an existing format list."
        Else 
            Dim FormatList
            set FormatList = UpdateFormatList(ListFormatPar)
            If CInt(fmtKey) > FormatList.count - 1 Then
                Err.raise 1000, "VBScript", "Index out of range."
            Else    
                Dim LocalFormat
                set LocalFormat = FormatList(fmtKey)
                GetFormatFromKey = LocalFormat.GetRDIName
            End If
        End If
    Else
        GetFormatFromKey = fmtKey
    End If
End Function

Function GetObjFromKey(IndexKey, ObjList, NameString, IDString, index)
    Dim Value
    Value = Parse(IndexKey)
    set GetObjFromKey = Nothing
    Select Case Value
        Case "Index"
            index = CLng(IndexKey)
            set GetObjFromKey = ObjList(index)
        If  GetObjFromKey Is Nothing Then
            err.raise 1000, "VBScript", "Specified Index does not correspond to a valid object"
        End If
        Case "ID"
            index = GetIndexFromKey(IndexKey, IDString)
            If index <> -1 Then
                set GetObjFromKey = session.GetObjectFromID(IndexKey)
            End If
        If  GetObjFromKey Is Nothing Then
            err.raise 1000, "VBScript", "Specified ID does not correspond to a valid object"
        End If
        Case "Name"
            index = GetIndexFromKey(IndexKey, NameString)
            If index <> -1 Then
                set GetObjFromKey = ObjList(index)
            End If
        If  GetObjFromKey Is Nothing Then
            err.raise 1000, "VBScript", "Specified name does not correspond to a valid object"
        End If
    End Select
End Function

Function IsOctal(Expr)
    IsOctal = False
    If Left(Expr,2) = "&o" Then
        IsOctal = True
    End If
End Function

' evaluate octal decimal number assumes format is &oa1a2a3a4a5a6a7a8a9a10a11
Function COct(Expr)
    Dim length, iter, charval, result, value
    length = Len(Expr)
' Cannot handle more then 11 octal digits, so 11+2 or less then one
    If length > 13 Then
        err.raise 1000, "VBScript", "Specified octal value is too large"
    End If

' Cannot handle less then 1+2 octal digits
    If length < 3 Then
        err.raise 1000, "VBScript", "Specified octal value is too small"
    End If
' evaluate octal decimal number
    iter = 0
    result = 0
    value = 1
    do while iter < length-2
        charval = Clng(Mid(Expr, length-iter, 1))
        if charval > 7 Then
            err.raise 1000, "VBScript", "At least one illegal octal digit specified"
        End If
        result = result + value*charval
        value = value*8
        iter = iter + 1
    loop
    Coct = result
End Function

Function ExprToAddress(ExprStr)
    Dim Expr, newStr
' first check whether string is octal IsNumeric pick is octal OK but then CLng cannot evaluate
    If IsOctal(ExprStr) = True Then
        newStr = COct(ExprStr)
        ExprToAddress = newStr  
    Elseif IsNumeric(ExprStr) = True Then
        newStr = CLng(Unsigned(ExprStr))
'       If Left(CStr(newStr), 2) = "&h" Then
'           Dim newHexStr
'           newHexStr = Replace(newStr, "&h", "0x", 1, 1)
'           newStr = newHexStr
'       End If
        ExprToAddress = newStr
    Else    'look for symbolic expression
        If Left(CStr(ExprStr), 2) = "&h" Then
            ExprToAddress = Replace(ExprStr, "&h", "0x", 1, 1)
        Else
            set Expr = CurrentThread.CreateExpression(ExprStr)
			If Expr.IsValid = FALSE Then
	            err.raise 1000, "VBScript", "Invalid expression"
			Else
				ExprToAddress = Expr.GetValue
			End If	
        End If
    End If
End Function

Function ExprToName(ExprStr)
    Dim Expr
    set Expr = CurrentThread.CreateExpression(ExprStr)
	If Expr.IsValid = FALSE Then
	    err.raise 1000, "VBScript", "Invalid expression"
	Else
	    ExprToName = Expr.GetName
	End If	
    ExprToName = Expr.GetName
End Function

Function VarDeRefView(ByRef ExprObj, fmt)
    Dim txt, size, thisvar, iter, tf, ctvarName
    set varlist = ExprObj.Dereference
    size = varlist.count
    iter = 0
    do while iter < size
        set thisvar = varlist(iter)
        ctvarName = thisvar.GetName
        tf = FormatValue(thisvar, fmt)
        txt = txt & ctvarName & Chr(9) & tf
        iter = iter + 1
    Loop
    'need to get variable again?
    VarDeRefView = txt  
End Function

Function TypeValueToName(convarType)
    Dim TypeStr
    Select Case convarType
        Case &H01
            TypeStr = "unknown"
        Case &H02
            TypeStr = "signed"   
        Case &H04
            TypeStr = "unsigned"   
        Case &H08
            TypeStr = "char"   
        Case &H10
            TypeStr = "enum"              
        Case &H20
            TypeStr = "union"             
        Case &H40
            TypeStr = "structure"         
        Case &H80
            TypeStr = "array"             
        Case &H100
            TypeStr = "float"             
        Case &H200
            TypeStr = "double"           
        Case &H400
            TypeStr = "string"            
        Case &H800
            TypeStr = "function"          
        Case &H1000
            TypeStr = "class"             
        Case &H2000
            TypeStr = "pointer"           
        Case &H4000
            TypeStr = "reference"          
        Case &H2807
            TypeStr = "simple"           
    End Select
    TypeValueToName = TypeStr
End Function

Function CheckListStatus(ObjList)
    If IsNull(ObjList) or IsEmpty(ObjList) Then
        CheckListStatus = False
    Else
        CheckListStatus = True
    End If
End Function

Function UpdateRegBankList(ProcIndex)

    Dim ThisThread
    Dim value
    dim RegBank
    dim count
    dim iter
    dim txt
   
    set ThisThread = GetThreadFromProc(ProcIndex)
    set RegBankList = ThisThread.RegBanks

	RegBankIDString = ""
	RegBankNameString = ""
    count = RegBankList.count
    iter = 0
	BankIter = 0
    do while bankiter < count
        set RegBank = RegBankList(bankiter)
		BankName = RegBank.Name
		BankID = RegBank.ID
		If InStr(1, BankName, "User", 1) <> 0 Then 
			BankName = "User/System"
		End If
		If BankName <> "System" Then
			txt = txt & Chr(35) & CStr(iter + 1) & Chr(9) & CStr(BankID) & Chr(9) & BankName & chr(13) & chr(10)
			RegBankIDString = RegBankIDString  & CStr(BankID) & "%" & CStr(iter) & "%"
			RegBankNameString = RegBankNameString & BankName & "%" & CStr(iter) & "%" 
			iter = iter + 1
		End If
		Bankiter = Bankiter + 1
    loop
    UpdateRegBankList = txt
End Function


Function UpdateProcList()
    dim Size, Proc, iter, IdStr, NameStr
    ProcIDString = Empty
    ProcNameString = Empty
    ProcListText = Empty
    set ProcList = session.Processors
    Size = ProcList.count
    iter = 0
    procIter = 0
    do while iter < Size
        set Proc = ProcList(iter)
        If  Proc.CheckDefThread <> 0 Then           
            NameStr = Proc.Name
            IdStr = CStr(Proc.ID)
            ProcListText = ProcListText & Chr(35) & CStr(procIter + 1) & Chr(9) & IdStr & Chr(9) & NameStr & Chr(13) & Chr(10)
            ProcIDString = ProcIDString &  IdStr & "%" & CStr(procIter) & "%"
            ProcNameString = ProcNameString &  NameStr & "%" & CStr(procIter) & "%"
            iter = iter + 1
            procIter = procIter + 1
        Else
            iter = iter + 1
        End If
    loop 
    UpdateProcList = ProcListText
End Function

Function UpdateImageList()
    dim Size, Image, iter, IdStr, NameStr
    
    set AsyncAsm = Nothing
    set AsyncSource = Nothing
    
    ImageIDString = Empty
    ImageNameString = Empty
    ImageListText = Empty
    set ImageList = session.Images
    Size = ImageList.count
    If Size = 0 Then
        err.raise 1000, "VBScript", "Please load an image."
    End If
    iter = 0
    do while iter < Size
        set Image = ImageList(iter)
        NameStr = Image.Name
        IdStr = CStr(Image.ID)
        ImageListText = ImageListText & Chr(35) & CStr(iter + 1) & Chr(9) & IdStr & Chr(9) & NameStr & Chr(13) & Chr(10)
        ImageIDString = ImageIDString &  IdStr & "%" & CStr(iter) & "%"
        ImageNameString = ImageNameString &  NameStr & "%" & CStr(iter) & "%"
        iter = iter + 1
        UpdateImageClassList(Image)
    loop 
    UpdateImageList = ImageListText
End Function

Function UpdateImageFileList(ByRef ImageObj)
    dim ThisFile
    dim count, NameStr, IdStr, Size
    dim iter
    FileNameString = Empty
    FileIDString = Empty
    ImageFileListText = Empty
    set FileList = ImageObj.Files
    Size = FileList.count
    If Size = 0 Then 
        err.raise 1000, "VBScript","Cannot create files list for this image"
    End If
    iter = 0
    do while iter < Size
        set thisfile = FileList(iter)
        NameStr = ThisFile.Name
        IdStr = CStr(ThisFile.ID)
        ImageFileListText = ImageFileListText & Chr(35) & CStr(iter + 1) & Chr(9) & IdStr & Chr(9) & NameStr & Chr(13) & Chr(10)
        FileIDString = FileIDString &  CStr(ThisFile.ID) & "%" & CStr(iter) & "%"
        FileNameString = FileNameString &  CStr(ThisFile.Name) & "%" & CStr(iter) & "%"
        iter = iter + 1
    loop
    UpdateImageFileList = ImageFileListText
End Function

Function UpdateImageClassList(ByRef ImageObj)
    dim ThisClass
    dim count
    dim iter
    ClassNameString = Empty
    set ClassList = ImageObj.Classes
    count = ClassList.count
    iter = 0
    do while iter < count
        set ThisClass = ClassList(iter)
        ClassNameString = ClassNameString & CStr(ThisClass.Name) & "%" & CStr(iter) & "%"
        iter = iter + 1
    loop
End Function

Function UpdateClassClassList(ByRef ClassObj)
    dim ThisClass
    dim count
    dim iter
    ClassNameString = Empty
    set ClassList = ClassObj
    count = ClassList.count
    iter = 0
    do while iter < count
        set ThisClass = ClassList(iter)
        ClassNameString = ClassNameString & CStr(ThisClass.Name) & "%" & CStr(iter) & "%"
        iter = iter + 1
    loop
End Function

Function UpdateImageVariableList(ByRef ImageObj)
    dim ThisVariable, TypeStr, NameStr
    dim count
    dim iter
    VariableNameString = Empty
    ImageVariableListText = Empty
    set VariableList = ImageObj.Variables
    count = VariableList.count
    iter = 0
    do while iter < count
        set ThisVariable = VariableList(iter)
        NameStr = ThisVariable.Name
        VariableNameString = VariableNameString & NameStr & "%" & CStr(iter) & "%"
        ImageVariableListText = ImageVariableListText & Chr(35) & CStr(Iter + 1) & Chr(9) & NameStr & Chr(13) & Chr(10)        
        iter = iter + 1
    loop
End Function

Function UpdateImageFunctionList(ByRef ImageObj)
    dim ThisFunction, TypeStr, ParaStr
    dim count, iter
    FunctionNameString = Empty
    ImageFunctionListText = Empty
    set FunctionList = ImageObj.Functions
    count = FunctionList.count
    iter = 0
    do while iter < count
        set ThisFunction = FunctionList(iter)
        FunctionStr = ThisFunction.Type & ThisFunction.Name & ThisFunction.Params
        FunctionNameString = FunctionNameString &  FunctionStr & "%" & CStr(iter) & "%"
        ImageFunctionListText = ImageFunctionListText & Chr(35) & CStr(Iter + 1) & Chr(9) & FunctionStr & Chr(13) & Chr(10)
        iter = iter + 1
    loop
End Function


Function UpdateFormatList(SizeOption)
    Dim FormatList
'    Dim DataSize

    set FormatList = session.ListFormat(SizeOption)

    count = FormatList.count
    iter = 0 ': index = 0
    do while iter < count
        set ThisFormat = FormatList(iter)
'        DataSize = ThisFormat.GetSize
        FmtRDIName = ThisFormat.GetRDIName

        If DataSize <> 0 Then
           FormatRDINameString = FormatRDINameString  & CStr(FmtRDIName) & "%" & CStr(iter) & "%"
'           RegBankNameString = RegBankNameString & CStr(FmtName) & "%" & CStr(iter) & "%" 
        End If
        iter = iter + 1

    loop
    set UpdateFormatList = FormatList

End Function


Function GetStackListText(StackEntryList, StackIndex)
    dim ThisEntry, ret
    dim count, iter, StackEntryText
    
    If StackIndex = -1 Then
        StackIndex = 0
    End If
    
    count = StackEntryList.count
    iter = 1
    do while count > 0
        set ThisEntry = StackEntryList(count-1)
        StackEntryString = FunctionNameString &  NameStr & "%" & CStr(iter) & "%"
    StackEntryText = StackEntryText & Chr(35) & CStr(iter) & Chr(9) & ThisEntry.Type & ThisEntry.Name & ThisEntry.Params & chr(13) & chr(10)
        count = count - 1
        iter = iter + 1
    loop

    GetStackListText = StackEntryText
End Function

Function UpDateWatchPTList()
    set WatchPtList = session.WatchPoints
End Function

Function UpDateBreakPTList()
    set BreakPTList = session.BreakPoints
End Function

Function GetCurrentImage()
    dim ctx
    set ctx = CurrentThread.Context
    set GetCurrentImage = ctx.Image
End Function

Function GetCurrentProcessor()
    dim ctx
    set ctx = CurrentThread.Context
    set GetCurrentProcessor = ctx.GetProcessor
End Function

Function GetThreadFromProc(ProcIndex)
    Dim CurProc
    set CurProc = CheckInitProc(ProcIndex)
    set GetThreadFromProc = CurProc.Thread
End Function

Function QuitDebugger()                     '<---- CLI command does not need wrapper
On Error Resume Next

    session.QuitDebugger
    DisplayError
End Function

Function DisplayError()
    DisplayError = Err.Number
        If Err.Number <> 0 Then
            adw3cli.Display(Err.Description & Chr(13) & Chr(10))
        Err.Clear
    End If
End Function

Function AddSpace(strOrig, ichars)
    OrigLen = Len(strOrig)
    WantedLen = iChars
    if OrigLen+3 > WantedLen then
        AddSpace = AddSpace(strOrig,ichars+6)
        Exit Function
    End If
    iter = WantedLen - OrigLen
    strOrig = strOrig & string(iter, Chr(32))
    AddSpace = strOrig
End Function

Function AddSmallSpace(strOrig, ichars)
    OrigLen = Len(strOrig)
    WantedLen = iChars
    if OrigLen+1 > WantedLen then
        AddSmallSpace = AddSmallSpace(strOrig,ichars+2)
        Exit Function
    End If
    iter = WantedLen - OrigLen
    strOrig = strOrig & string(iter, Chr(32))
    AddSmallSpace = strOrig
End Function

Function sth(ThreadIndex)
On Error Resume Next
    SetThread ThreadIndex
    DisplayError
End Function

Sub params(a1,a2,a3)
    adw3cli.Display(a1)
    adw3cli.Display(Chr(13) & Chr(10))
    adw3cli.Display(a2)
    adw3cli.Display(Chr(13) & Chr(10))
    adw3cli.Display(a3)
    adw3cli.Display(Chr(13) & Chr(10))
End Sub


Function SetAci(strArgs)                  '<-----  CLI command
On Error Resume Next
    SetAci = SetAciInt(strArgs)
    DisplayError
End Function

Function SetAciInt(strArgs)    
    If Left(strArgs, 1) = chr(32) Then
		err.raise 1000, "VBScript", "Invalid argument"
		Exit Function
	End If
    SetAciInt = session.SetAci(strArgs)   
    If SetAciInt = False Then
	    err.raise 1000, "VBScript", "Failed to set string property"
		Exit Function
    End If 
End Function
