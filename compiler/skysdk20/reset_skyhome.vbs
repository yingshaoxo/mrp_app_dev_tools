on error resume next

test = createobject("Scripting.FileSystemObject").GetFolder(".").Path
set sysenv = CreateObject("WScript.Shell").Environment("user")
sysenv.Remove("SKYHOME")
sysenv("SKYHOME")=test+"\"
msgbox "�޸ĳɹ���",64,"��ʾ"


