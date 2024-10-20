// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <Core/Core.h>
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

namespace Upp {


#if defined(PLATFORM_WIN32)
#pragma float_control(except, on)
#endif

void CrashPanic(const char *msg) {
	throw Exc(msg);
}

void CrashMessageBox(const char *title, const char *text) {
	throw Exc(text);
}

#ifndef flagDEBUG
#define Panic CrashPanic
#endif

CrashHandler::CrashHandler() {
#if defined(PLATFORM_WIN32)
	#ifdef flagDEBUG	// It doesn't work properly in release
	_clearfp();
	_controlfp(_controlfp(0, 0) & ~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW), _MCW_EM);
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

#ifndef flagDEBUG
	InstallPanicMessageBox(CrashMessageBox);
#endif
	
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
		CrashPanic(msg);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif 

void __cdecl CrashHandler::TerminateHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("Terminate exception");
	}
}

void __cdecl CrashHandler::PureCallHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("Pure virtual function call");
	}
}

void __cdecl CrashHandler::InvalidParameterHandler(const wchar_t* expression, const wchar_t *function, 
	const wchar_t* file, unsigned int line, uintptr_t) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		if (line == 0)
			Panic("Invalid parameter");
		else
			Panic(Format("Invalid parameter in %s, function %s, file %s, line %d", AsString(expression),
				AsString(function), AsString(file), int(line)));
	}
}

void __cdecl CrashHandler::NewHandler() {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("Not enough memory available");
	}
}

void CrashHandler::SigabrtHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGABRT: Process has aborted");
	}
}

void CrashHandler::SigfpeHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGFPE: Floating point error");
	}
}

void CrashHandler::SigillHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGILL: Executable code seems corrupted");
	}
}

void CrashHandler::SigintHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGINT: Process has been asked to terminate by user");
	}
}

void CrashHandler::SigsegvHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGSEGV: Trying to read or write from/to a memory area that your process does not have access to");
	}
}

void CrashHandler::SigtermHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGTERM: Process has been asked to terminate by other application");
	}
}

void CrashHandler::SigbusHandler(int) {
	ONCELOCK {
		if (!GetCrashHandler().IsEnabled())
			return;
		Panic("SIGBUS: Process is trying to access memory that the CPU cannot physically address");
	}
}
	
CrashHandler &GetCrashHandler() {
	static CrashHandler clss;
	return clss;
}
	
	
}