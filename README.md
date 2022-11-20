# SysInfo

[SysInfo](https://anboto.github.io/srcdoc$SysInfo$SysInfo$en-us.html) is a library to get information and handle Desktop, OS and hardware internals.
 
It includes functions for:

- [Obtaining special folders](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#GetDesktopFolder())
- [Hardware, BIOS and OS info](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#GetSystemInfo(String&,String&,String&,int&))
- [Process handling](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#GetWindowsList(Array%3Clong%3E&,Array%3Clong%3E&,Array%3CString%3E&,Array%3CString%3E&,Array%3CString%3E&))
- [Windows handling](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#Window_GetRect(long,long&,long&,long&,long&))
- [Mouse and keyboard handling](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#Mouse_GetPos(long&,long&))
- [Screen recording](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#Mouse_GetPos(long&,long&))
- [Miscellaneous functions](https://anboto.github.io/src$SysInfo$SysInfo$en-us.html#LoadFile_Safe(String))

IMPORTANT: Before compiling please check 2. Compiler support and Requirements in Implementation details. In X11, SysInfo requires XTest extension (in debian based system, you need to install libxtst-dev package).
