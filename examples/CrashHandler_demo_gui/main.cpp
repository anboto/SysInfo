// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <CtrlLib/CtrlLib.h>
#include <Functions4U/Functions4U_Gui.h>
#include <SysInfo/Crash.h>

using namespace Upp;


GUI_APP_MAIN
{	
	const Vector<String>& command = CommandLine();
	
	PromptOK(Format("Hello %d", command.size()));
	
	bool uselauncher = true;
	if (command.size() > 0) {
		if (Find(command, "-error") >= 0) {
			Exclamation("There has been an error");	
			uselauncher = false;
		} else if (Find(command, "-run") >= 0)
			uselauncher = false;
	}
	
	if (uselauncher) {
		Launcher launcher;
		if (!launcher.Launch())
			Exclamation("Impossible to launch application");
		while (launcher.Monitor())
			Sleep(5000);
	} else
		PromptOK("Normal application");
}
