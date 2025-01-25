// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _CrashHandler_Crash_h_
#define _CrashHandler_Crash_h_

#ifdef flagGUI
#include <CtrlLib/CtrlLib.h>
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


#ifdef flagGUI

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
	virtual void LeftDown(Point p, dword d) {
		int h = GetTab(p);
		if (h >= 0 && GetItem(h).IsEnabled() && h != prev_h) {
			EM().Log(Format("Tab %s", GetItem(h).GetText()));
			TabCtrl::LeftDown(p, d);
			prev_h = h;
		}
	}
	virtual bool Key(dword key, int count) {
		int h = Get();
		switch(key) {
	#ifdef PLATFORM_COCOA
		case K_ALT|K_TAB:
		case K_OPTION|K_TAB:
	#endif
		case CtrlCoreKeys::K_CTRL_TAB:
			h++;
			break;
	#ifdef PLATFORM_COCOA
		case K_SHIFT|K_ALT|K_TAB:
		case K_SHIFT|K_OPTION|K_TAB:
	#endif
		case K_SHIFT_CTRL_TAB:
			h--;
			break;
		}		
		if (h >= 0 && h < GetCount() && h != prev_h) {
			Item& item = GetItem(h);
			if (item.IsEnabled()) 
			EM().Log(Format("Tab %s", item.GetText()));
			prev_h = h;
		}
		return TabCtrl::Key(key, count);
	}
	virtual void Paint(Draw& draw) {
		if (prev_h < 0) {
			prev_h = 0;
			if (GetCount() > 0 && GetItem(0).IsEnabled()) 
				EM().Log(Format("Tab %s", GetItem(0).GetText()));
		}
		TabCtrl::Paint(draw);
	}
	int prev_h = -1;
};	

#endif
}

#endif
