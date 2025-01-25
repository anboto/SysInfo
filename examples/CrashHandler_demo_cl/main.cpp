// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <Core/Core.h>
#include <SysInfo/Crash.h>

using namespace Upp;

#include "problems.h"


bool Test(void (*Fun)()) {
	try {
		Fun();

	} catch (Exc e) {
		UppLog() << "\nError: " << e;  
		return true;
	} catch (const std::out_of_range& e) {
		UppLog() << "Out-of-bounds access caught: " << e.what();
		return true;
	} catch (const std::bad_alloc& e) {
		UppLog() << "Memory allocation failure: " << e.what();
		return true;
	} catch (const std::runtime_error& e) {
		UppLog() << "Runtime error: " << e.what();
		return true;
	} catch (...) {
		UppLog() << "\nUnknown Error";  
		return true;
	}
	return false;
}

CONSOLE_APP_MAIN
{	
	StdLogSetup(LOG_COUT|LOG_FILE);
	SetExitCode(0);
	
	GetCrashHandler().Enable();
	
	Test(DivisionByZero);
	Test(NullPointerAssignment);
	Test(OutOfBounds);
	Test(MemoryAllocation);
	Test(IllegalInstruction);
	Test(StackOverflow);
	Test(FloatingPointOverflow);
	Test(MisalignedMemoryAccess);
	Test(DoubleFree);
	Test(InvalidCast);
	Test(UseAfterFree);
}
