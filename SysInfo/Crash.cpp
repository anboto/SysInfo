// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "Crash.h"
#include <signal.h>
#include <exception>
#include <fenv.h>
#if defined(PLATFORM_WIN32) 
#include <psapi.h>
#include <rtcapi.h>
#include <shellapi.h>
	#ifdef COMPILER_MSC
	#include <verrsrc.h>
	#endif
#include <dbghelp.h>
#include <eh.h>
#include <memory>
#endif

#include <Functions4U/EnableWarnings.h>

namespace Upp {

#if defined(PLATFORM_WIN32)
#pragma float_control(except, on)
#endif

static void CrashPanic(const char *msg) {
	throw Exc(msg);
}

#ifndef flagDEBUG
#define Panic CrashPanic
#endif

CrashHandler::CrashHandler() {
#if defined(PLATFORM_WIN32)
	#ifdef flagDEBUG	// It doesn't work properly in release
	_clearfp();
	_controlfp(_controlfp(0, 0) & unsigned(~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW)), _MCW_EM);
	#endif
	
	SetUnhandledExceptionFilter(UnhandledHandler);    
	_set_purecall_handler(PureCallHandler);    
	_set_invalid_parameter_handler(InvalidParameterHandler); 
	
#else
 	feclearexcept(FE_ALL_EXCEPT); 
	feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
	signal(SIGBUS, SigbusHandler);
#endif

	std::set_new_handler(NewHandler);
	std::set_terminate(TerminateHandler);

	signal(SIGABRT, SigabrtHandler);  
	signal(SIGINT,  SigintHandler);     
	signal(SIGTERM, SigtermHandler);          
	signal(SIGFPE,  SigfpeHandler);     
	signal(SIGILL,  SigillHandler);     
	signal(SIGSEGV, SigsegvHandler); 
	
	enabled = true;
}

#if defined(PLATFORM_WIN32)	
LONG WINAPI CrashHandler::UnhandledHandler(EXCEPTION_POINTERS *exceptionPtrs) { 
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return EXCEPTION_CONTINUE_EXECUTION;
		
		DWORD exceptionCode = exceptionPtrs->ExceptionRecord->ExceptionCode;

		String msg;
    	switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            msg << "Exception: Access Violation";
            if (exceptionPtrs->ExceptionRecord->NumberParameters >= 2) {
                msg << ". ";
                ULONG_PTR violationType = exceptionPtrs->ExceptionRecord->ExceptionInformation[0];
                msg << (violationType == 0 ? "Attempted to read" : "Attempted to write");
            }
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            msg << "Array Bounds Exceeded";						break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            msg << "Exception: Data Type Misalignment";			break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            msg << "Exception: Floating-point Division by Zero";break;
        case EXCEPTION_FLT_OVERFLOW:
            msg << "Exception: Floating-point Overflow";		break;
        case EXCEPTION_FLT_UNDERFLOW:
            msg << "Exception: Floating-point Underflow";		break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            msg << "Exception: Illegal Instruction";			break;
        case EXCEPTION_IN_PAGE_ERROR:
            msg << "Exception: In-Page Error";
            if (exceptionPtrs->ExceptionRecord->NumberParameters >= 3) {
                ULONG_PTR errorCode = exceptionPtrs->ExceptionRecord->ExceptionInformation[2];
                msg << ". Error Code: " << errorCode << "\n";
            }
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            msg << "Exception: Integer Division by Zero";	break;
        case EXCEPTION_PRIV_INSTRUCTION:
            msg << "Exception: Privileged Instruction";		break;
        case EXCEPTION_STACK_OVERFLOW:
            msg << "Exception: Stack Overflow";				break;
        case EXCEPTION_BREAKPOINT:
            msg << "Exception: Breakpoint";					break;    
        default:
            msg << Format("Default Exception %ld", int64(exceptionCode));     break;
    	}
    	EM().Log("CrashHandler: " + msg);
		CrashPanic(msg);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif 

void __cdecl CrashHandler::TerminateHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("Terminate exception");
		Panic("Terminate exception");
	}
}

void __cdecl CrashHandler::PureCallHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("Pure virtual function call");
		Panic("Pure virtual function call");
	}
}

void __cdecl CrashHandler::InvalidParameterHandler(const wchar_t* expression, const wchar_t *function, 
	const wchar_t* file, unsigned int line, uintptr_t) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		String str;
		if (line == 0)
			str = "Invalid parameter";
		else
			str = Format("Invalid parameter in %s, function %s, file %s, line %d", AsString(expression),
				AsString(function), AsString(file), int(line));
		EM().Log(str);
		Panic(str);
	}
}

void __cdecl CrashHandler::NewHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("Not enough memory available");
		Panic("Not enough memory available");
	}
}

void CrashHandler::SigabrtHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGABRT: Process has aborted");
		Panic("SIGABRT: Process has aborted");
	}
}

void CrashHandler::SigfpeHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGFPE: Floating point error");
		Panic("SIGFPE: Floating point error");
	}
}

void CrashHandler::SigillHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGILL: Executable code seems corrupted");
		Panic("SIGILL: Executable code seems corrupted");
	}
}

void CrashHandler::SigintHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGINT: Process has been asked to terminate by user");
		Panic("SIGINT: Process has been asked to terminate by user");
	}
}

void CrashHandler::SigsegvHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGSEGV: Trying to read or write from/to a memory area that your process does not have access to");
		Panic("SIGSEGV: Trying to read or write from/to a memory area that your process does not have access to");
	}
}

void CrashHandler::SigtermHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGTERM: Process has been asked to terminate by other application");
		Panic("SIGTERM: Process has been asked to terminate by other application");
	}
}

void CrashHandler::SigbusHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		EM().Log("SIGBUS: Process is trying to access memory that the CPU cannot physically address");
		Panic("SIGBUS: Process is trying to access memory that the CPU cannot physically address");
	}
}
	
CrashHandler &GetCrashHandler() {
	static CrashHandler clss;
	return clss;
}


bool ErrorMonitor::Init(const char *title, const char *folder, Function<bool(const char *title, const Vector<String>&)> ExitError, Function <void()> ExitOK) {
	appname = title;
	String sfolder(folder);
	if (!DirectoryExists(sfolder)) {
		sfolder = AppendFileNameX(GetAppDataFolder(), folder);
		RealizeDirectory(sfolder);
	}

	String name;
	if (GetEnv(Format("AnbotoWD%ld", GetParentProcessId())) != "1") {
		isChild = false;
		name = Format("AnbotoWD%ld", GetProcessId());
	} else {
		isChild = true;
		name = Format("AnbotoWD%ld", GetParentProcessId());
	}
	
	fileLog = AppendFileNameX(sfolder, name + ".log");
		
	if (!isChild) {
		SetEnv(name, "1");
		
		DeleteFile(fileLog);
		
		String args;
		for (const String &s : CommandLine())
			args << " " << s;
		
		bool restart = true;
		
		while (restart) {
			String kernel, kerVersion, kerArchitecture, distro, distVersion, desktop, deskVersion;
			if (GetOsInfo(kernel, kerVersion, kerArchitecture, distro, distVersion, desktop, deskVersion)) 
				Log(Format("Kernel: %s, Version: %s, Architecture: %s, Distro: %s, Version: %s, Desktop: %s, Version: %s", kernel, kerVersion, kerArchitecture, distro, distVersion, desktop, deskVersion));
			
			int memoryLoad;
			uint64 totalPhys, freePhys, totalPageFile, freePageFile, totalVirtual, freeVirtual;
			if (GetMemoryInfo(memoryLoad, totalPhys, freePhys, totalPageFile, freePageFile, totalVirtual, freeVirtual))
				Log(Format("Memory: %.1f Gb", double(totalPhys)/1024./1024./1024.));
			
			String nam, mode;
			Time time; 
			int version, bits;
			GetCompilerInfo(nam, version, time, mode, bits);	
			Log(Format("Compiler: %s, Version: %d, Time: %`, mode: %s, bits: %d", nam, version, time, mode, bits));
			
			int64 childpid = LaunchCommand(Format("\"%s\" %s", GetExeFilePath(), args));
			
			while(ProcessExists(childpid)) 
				Sleep(500);
			
			String slog = LoadFile(fileLog);
			Vector<String> vlog = Split(slog, '\n');
			if (!childpid)
				vlog << t_("Impossible to launch application");
			
			if (vlog.size() > 0 && vlog[vlog.size()-1].StartsWith("End")) {
				ExitOK();
				restart = false;
			} else
				restart = ExitError(title, vlog);
			
			DeleteFile(fileLog);
		}
		return true;
	} else 
		Log(Format("Start: %`", GetSysTime()));

	return false;
}

void ErrorMonitor::Log(const char *str) {
	if (!fileLog.IsEmpty()) 
		FileStrAppend(fileLog, str + S("\n"));
#if defined(flagGUI)
	if (window)
		window->AddStr(str);
#endif
	logList << str;
}

ErrorMonitor::~ErrorMonitor() {
	if (isChild)
		Log(Format("End: %`", GetSysTime()));	
}

ErrorMonitor &EM() {
	static ErrorMonitor w;	
	return w;
}

#if defined(flagGUI)

void ErrorMonitor::OpenLogWindow() {
	if (window)
		return;
	window = new ErrorMonitorLog([=](){window = nullptr;});
	for (const String &str : logList)
		window->AddStr(str);
	window->OpenMain();
}

bool ErrorMonitor::DefaultExitError(const char *appname, const Vector<String> &vs) {
	struct DialogError : TopWindow {
		DialogError(const char *appname) {
			Sizeable();
			Title(Format(t_("Problem detected in %s"), appname));
			SetRect(0, 0, Zx(320), Zy(200));
			
			Add(butCancel.SetLabel(t_("Cancel")).RightPosZ(8, 56).BottomPosZ(5, 20));
			Add(butOK.SetLabel(t_("Restart")).RightPosZ(68, 56).BottomPosZ(5, 20));
			Add(dv___2.SetLabel(t_("Click Restart to restart or Cancel to end.")).HSizePosZ(8, 0).TopPosZ(4, 19));
			Add(array.HSizePosZ(4, 4).VSizePosZ(30, 30));
			
			array.AddColumn(t_("Please copy and report this log to the developers:"));
			array.MultiSelect();
			array.WhenBar = [&](Bar &menu) {ArrayCtrlWhenBar(menu, array);};
		}
		Button butCancel;
		Button butOK;
		Label dv___2;
		ArrayCtrl array;
	};
	
	DialogError dialog(appname);
	
	for (const String &str : vs)
		dialog.array.Add(str);
	
	dialog.array.SetCursor(vs.size()-1);
	
	bool ok = true;
	dialog.butOK		<< [&] {dialog.Close();};
	dialog.butCancel	<< [&] {ok = false;	dialog.Close();}; 
	dialog.TopMost(true, true);
	dialog.Execute();
	return ok;
}

ErrorMonitorLog::ErrorMonitorLog(Event<> whenClose) {
	Sizeable();
	Title("Log");
	
	Add(array.SizePos());
	array.AddColumn("");
	array.MultiSelect().NoHeader();
	array.WhenBar = [&](Bar &menu) {ArrayCtrlWhenBar(menu, array);};
			
	SetRect(0, 0, Zx(150), Zy(300));
	
	WhenClose = whenClose;
}
#endif

}