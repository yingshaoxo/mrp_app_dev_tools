on error resume next

test = createobject("Scripting.FileSystemObject").GetFolder(".").Path
set sysenv = CreateObject("WScript.Shell").Environment("user")
sysenv.Remove("SKYHOME")
sysenv("SKYHOME")=test+"\"
msgbox "修改成功！",64,"提示"


