// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _CrashHandler_Crash_h_
#define _CrashHandler_Crash_h_

#if defined(flagGUI) //&& (defined(__WIN32) || defined(_WIN32) || defined(WIN32))
#include <CtrlLib/CtrlLib.h>
#include <Functions4U/Functions4U_Gui.h>
#else
#include <Core/Core.h>
#endif

#include <SysInfo/SysInfo.h>

namespace Upp {

#ifdef __GNUC__
#define __cdecl __attribute__((__cdecl__))
#endif


class CrashHandler {
public:
    CrashHandler();
    virtual ~CrashHandler() {};

	void Enable()		{enabled = true;}
	void Disable()		{enabled = false;}
	bool IsEnabled()	{return enabled;}
	
private:
#if defined(PLATFORM_WIN32) 
    static LONG WINAPI UnhandledHandler(EXCEPTION_POINTERS *p);
    //static void __cdecl SEHHandler(unsigned u, EXCEPTION_POINTERS* p);
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
    static void __cdecl TerminateHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression, 
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static void __cdecl NewHandler();
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    static void SigabrtHandler(int);
    static void SigfpeHandler(int);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);
    static void SigbusHandler(int);
    
    bool enabled;
};

CrashHandler &GetCrashHandler();


#if defined(flagGUI) && defined(PLATFORM_WIN32)

class ErrorMonitorLog : public TopWindow {
public:
	typedef ErrorMonitorLog CLASSNAME;
	
	ErrorMonitorLog(Event<> whenClose);
	void Close() override {
		WhenClose();
		delete this;
	}
	void AddStr(const char *str) {
		array.Add(str);
		array.GoEnd();
	}
	
private:
	ArrayCtrl array;
	Event<> WhenClose;
};

class ErrorMonitor {
public:
	bool Init(const char *title, const char *folder, Function<bool(const char *title, const Vector<String>&)> ExitError, Function <void()> ExitOK);
	bool IsInit()	{return !appname.IsEmpty();}
	~ErrorMonitor();
	
	void Log(const char *str);
	void OpenLogWindow();
	
	static bool DefaultExitError(const char *title, const Vector<String> &);
	
private:
	String fileLog;
	String appname;
	bool isChild = false;
	ErrorMonitorLog *window = nullptr;
	Vector<String> logList;
};

ErrorMonitor &EM();

class ButtonEM : public Button {
private:
	virtual void LeftDown(Point p, dword d) {
		EM().Log(Format("Button %s", GetLabel()));
		Button::LeftDown(p, d);
	}
	virtual void KeyPush() {
		EM().Log(Format("Button %s", GetLabel()));
		Button::KeyPush();
	}
};

class TabCtrlEM : public TabCtrl {
private:
	virtual void Paint(Draw& draw) {
		int actual_t = Get();
		if (prev_t < 0 || prev_t != actual_t) {
			prev_t = actual_t;
			if (GetCount() > 0 && GetItem(prev_t).IsEnabled()) 
				EM().Log(Format("Tab %s", GetItem(prev_t).GetText()));
		}
		TabCtrl::Paint(draw);
	}
	int prev_t = -1;
};	

#else

#define ButtonEM 	Button
#define TabCtrlEM	TabCtrl

#endif
}

#endif
