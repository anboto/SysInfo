// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <CtrlLib/CtrlLib.h>
#include <Functions4U/Functions4U_Gui.h>
#include <SysInfo/Crash.h>

using namespace Upp;

#define LAYOUTFILE <examples/CrashHandler_demo_gui/main.lay>
#include <CtrlCore/lay.h>

#include "../CrashHandler_demo_cl/problems.h"


struct Memory : public Withmemory<StaticRect> {
	Memory() {
		CtrlLayout(*this);
		
		butNullPointerAssignment << [] 	{NullPointerAssignment();};
		butUseAfterFree << [] 		   	{UseAfterFree();};
		butMemoryAllocation << [] 		{MemoryAllocation();};
		butDoubleFree << [] 			{DoubleFree();};
	}
};

struct Other : public Withother<StaticRect> {
	Other() {
		CtrlLayout(*this);
		
		butIllegalInstruction << [] 	{IllegalInstruction();};
		butStackOverflow << [] 			{StackOverflow();};
		butFloatingPointOverflow << [] 	{FloatingPointOverflow();};
	}
};

struct Main : public Withmain<TopWindow> {
	Memory memory;
	Other other;
	
	Main() {
		CtrlLayout(*this, "Error Monitor demo");
		
		tab.Add(memory.SizePos(), "Memory");
		tab.Add(other.SizePos(), "Other");
		
		butClose << [=] {Close();};
		butLog << [=] {EM().OpenLogWindow();};
	}
};
	

GUI_APP_MAIN {	
#if defined(PLATFORM_WIN32) 
	GetCrashHandler().Enable();
	#ifndef flagDEBUG
	InstallPanicMessageBox([](const char *title, const char *text) {
		EM().Log(Format("%s: %s", title, text));
		throw Exc(text);
	});
	if (EM().Init("CrashHandler", "CrashHandler", EM().DefaultExitError, Null))
		return;
	#endif
#endif
	
	Main main;
	main.OpenMain();

	Ctrl::EventLoop();
}
