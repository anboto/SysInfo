// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _SysInfo_SysInfo_h
#define _SysInfo_SysInfo_h

#include <Functions4U/Functions4U.h>

namespace Upp {

/////////////////////////////////////////////////////////////////////
// Processor Info
void GetSystemInfo(String &manufacturer, String &productName, String &version, int &numberOfProcessors, String &mbSerial);
void GetBiosInfo(String &biosVersion, Date &biosReleaseDate, String &biosSerial);
bool GetProcessorInfo(int number, String &vendor, String &identifier, String &architecture, int &speed);		
int GetCpuSpeed();
double GetCpuTemperature();

struct NetAdapter : DeepCopyOption<NetAdapter> {
	String description;
	String fullname;
	String dnsSuffix;
	String mac;
	String type;
	String ip4, ip6;
	String gatewayip4, gatewayip6;
	bool connected;
	
	void Xmlize(XmlIO &xml);
	void Jsonize(JsonIO &json);
	void Serialize(Stream &stream);
	void Copy(const NetAdapter& src);
	NetAdapter(const NetAdapter& src, int) 	{Copy(src);}
	//NetAdapter(const NetAdapter& src) 		{Copy(src);}
	NetAdapter() {}
};

Array<NetAdapter> GetAdapterInfo();

bool GetNetworkInfo(String &name, String &domain, String &ip4, String &ip6);

int GetAvailableSocketPort(int from = 49152);
bool IsPortFree(int port);

#if defined(PLATFORM_WIN32) 
bool GetVideoInfo(Upp::Array <Value> &name, Upp::Array <Value> &description, Upp::Array <Value> &videoProcessor, 
				  Upp::Array <Value> &ram, Upp::Array <Value> &videoMode);
#ifdef DEPRECATED
bool GetPackagesInfo(Upp::Array <Value> &name, Upp::Array <Value> &version, Upp::Array <Value> &vendor, 
			Upp::Array <Value> &installDate, Upp::Array <Value> &caption, Upp::Array <Value> &description, Upp::Array <Value> &state, Upp::Array <Value> &path);
#endif
#endif

String GetHDSerial();

/////////////////////////////////////////////////////////////////////
// Memory Info
bool GetMemoryInfo(int &memoryLoad, uint64 &totalPhys, uint64 &freePhys, uint64 &totalPageFile, uint64 &freePageFile, uint64 &totalVirtual, uint64 &freeVirtual);

/////////////////////////////////////////////////////////////////////
// Windows list
// They get arrays with handles to all the opened windows with additional info as 
// pid: Handle to the process that manages the window
// name: Window name
// fileName: Window process program file name
// title: Window title (caption)
void GetWindowsList(Array<int64> &wid, Array<int64> &pid, Array<String> &name, 
					Array<String> &fileName, Array<String> &title, bool getAll = true);
Array<int64> GetWindowsList();

Rect GetDesktopRect();
	
bool Window_GetRect(int64 windowId, int &left, int &top, int &right, int &bottom);
bool Window_SetRect(int64 windowId, int left, int top, int right, int bottom);
#if defined(PLATFORM_WIN32) 
void Window_Bottom(int64 windowId);
void Window_Top(int64 windowId);
void Window_TopMost(int64 windowId);
#endif

enum WINDOW_STATUS {
	WINDOW_MAXIMIZED, WINDOW_MINIMIZED, WINDOW_NORMAL};
int Window_GetStatus(int64 windowId);

/////////////////////////////////////////////////////////////////////
// Process list
bool GetProcessList(Array<int64> &pid, Array<String> &pNames);
Array<int64> GetProcessList();
String GetProcessName(int64 pid);
String GetProcessFileName(int64 processID);
int64 GetParentProcessId();
bool ProcessExists(int64 processID);

bool UnSetEnv(const char *var);

#if defined(PLATFORM_WIN32) 
int GetProcessCPUUsage(int64 pid);
#endif

int64 GetProcessIdFromWindowCaption(String windowCaption, bool exactMatch = false);
int64 GetProcessIdFromName(String name);

int64 GetWindowIdFromCaption(String windowCaption, bool exactMatch = false);

int64 GetProcessIdFint64romWindowId(int64 wid);
int64 GetWindowIdFromProcessId(int64 pid);

bool ProcessTerminate(int64 pid, int timeout = 500);

int GetProcessPriority(int64 pid);
bool SetProcessPriority(int64 pid, int priority);

bool ProcessExists(int64 pid);

/////////////////////////////////////////////////////////////////////
// Os Info
bool GetOsInfo(String &kernel, String &kerVersion, String &kerArchitecture, String &distro, String &distVersion, String &desktop, String &deskVersion);
/////////////////////////////////////////////////////////////////////
// Get drives info
bool GetDriveSpace(String drive, uint64 &freeBytesUser, uint64 &totalBytesUser, uint64 &totalFreeBytes);
bool GetDriveInformation(String drive, String &type, String &volume, /*uint64 &serial, */int &maxName, String &fileSystem);

/////////////////////////////////////////////////////////////////////
// Others
int64 GetProcessId();

bool Shutdown(String action);

void GetCompilerInfoAux(String &name, int &version, Upp::Time &time, String &mode, int &bits, const char *sdate, const char *stime);
#define GetCompilerInfo(name, version, time, mode, bits) 	GetCompilerInfoAux(name, version, time, mode, bits, __DATE__, __TIME__)

bool GetBatteryStatus(bool &discharging, int &percentage, int &remainingMin);
bool GetBatteryInfo(bool &present/*, int &designCapacity, int &lastFullCapacity, String &vendor, String &type, String &model, String &serial*/);	

bool OpenCDTray(String drive);
bool CloseCDTray(String drive);

/////////////////////////////////////////////////////////////////////
// Key and mouse keys
bool Mouse_GetPos(int &x, int &y);
bool Mouse_SetPos(int x, int y, int64 windowId = 0);

#if defined(PLATFORM_WIN32) || !defined(flagNO_XTEST)
void Mouse_LeftClick();
void Mouse_LeftDown();
void Mouse_LeftUp();
void Mouse_MiddleClick(); 
void Mouse_MiddleDown();
void Mouse_MiddleUp();
void Mouse_RightClick();
void Mouse_RightDown();
void Mouse_RightUp();
void Mouse_LeftDblClick();
void Mouse_MiddleDblClick();
void Mouse_RightDblClick();

void Keyb_SendKeys(String text, long finalDelay = 100, long delayBetweenKeys = 50);
#endif

bool Window_SaveCapture(int64 windowId, String fileName, int left = -1, int top = -1, int width = -1, int height = -1);
Image Window_SaveCapture(int64 windowId, int left = -1, int top = -1, int width = -1, int height = -1);

bool Snap_Desktop(String fileName);
bool Snap_DesktopRectangle(String fileName, int left, int top, int width, int height);
bool Snap_Window(String fileName, int64 handle);
Image Snap_Desktop();
Image Snap_DesktopRectangle(int left, int top, int width, int height);
Image Snap_Window(int64 handle);

#if !defined(flagNO_XTEST)
bool GetKeyLockStatus(bool &caps, bool &num, bool &scroll);
bool SetKeyLockStatus(bool caps, bool num, bool scroll);
#endif

#if defined(PLATFORM_WIN32)

bool Record_Desktop(String fileName, unsigned duration, double secsFrame = 1, bool viewMouse = true);
bool Record_DesktopRectangle(String fileName, unsigned duration, int left, int top, int width, int height, double secsFrame = 1, bool viewMouse = true);
bool Record_Window(String fileName, unsigned duration, uint64 handle, double secsFrame = 1, bool viewMouse = true);

#endif

void SetDesktopWallPaper(char *path);

struct SystemSignature : DeepCopyOption<SystemSignature> {
	String manufacturer, productName, version, mbSerial;
	int numberOfProcessors;	
	String hdSerial;
	String userName;
	Upp::Array <NetAdapter> netAdapters;
	
	void Load();
	String ToString() const			{return StoreAsJson(*this, true);}
	operator String() const			{return ToString();}
	operator const char *() const	{return ToString();}
	String operator~() const		{return ToString();}
	void Copy(const SystemSignature& src);
	SystemSignature(const SystemSignature& src, int) {Copy(src);}
	SystemSignature()	{}
	void Xmlize(XmlIO &xml);
	void Jsonize(JsonIO &json);
	void Serialize(Stream &stream);
	bool operator==(const SystemSignature &other) const;
	bool operator!=(const SystemSignature &other) const {return !(*this == other);}	
	Vector<String> GetDiff(const SystemSignature &other) const;
};

struct SystemOverview : DeepCopyOption<SystemOverview> {
	SystemSignature signature;
	String biosVersion, biosSerial;
	Date biosReleaseDate;
	String computerName;
	String kernel;
	String kerVersion;
	String kerArchitecture;
	String distro;
	String distVersion;
	String desktop;
	String deskVersion;
	String compilerName;
	int compilerVersion;
	Upp::Time compilerTime;
	String compilerMode;
	
	void Load();
	String ToString() const			{return StoreAsJson(*this, true);}
	operator String() const			{return ToString();}
	operator const char *() const	{return ToString();}
	String operator~() const		{return ToString();}
	void Copy(const SystemOverview& src);
	SystemOverview(const SystemOverview& src, int) { Copy(src); }	
	SystemOverview() {}
	void Xmlize(XmlIO &xml);
	void Jsonize(JsonIO &json);
	void Serialize(Stream &stream);
};

#if defined(PLATFORM_WIN32) || !defined(flagNO_XTEST)

class SimulateActivity {
public:
	SimulateActivity(int _deltaTime, 
		int _fromH = Null, int _fromM = Null, int _toH = Null, int _toM = Null) : 
		deltaTime(_deltaTime), fromH(_fromH), fromM(_fromM), toH(_toH), toM(_toM), rng(123987) {
#if defined(PLATFORM_WIN32) 
		if (::CreateMutex(0, true, "__msdxp__")) {
			if (GetLastError() == ERROR_ALREADY_EXISTS) 
				active = false;
		}
#endif
		nextDeltaTime = deltaTime + std::uniform_int_distribution<int>(-22*1000, 35*1000)(rng);
	}
	void DoActivity() {
		if (!active)
			return;
		
		Time tm = GetSysTime();
		if (!IsNull(fromH) && 
			(tm.hour < fromH || (tm.hour == fromH && tm.minute < fromM)) && 
			(tm.hour > toH   || (tm.hour == toH   && tm.minute > toM)))
			return;
		
		int x, y;
		Mouse_GetPos(x, y);
		if (x != x0 && y != y0) {
			timer.Reset();
			x0 = x;
			y0 = y;
			nextDeltaTime = deltaTime + std::uniform_int_distribution<int>(-22*1000, 35*1000)(rng);
		} else {
			if (timer.Elapsed() > nextDeltaTime*1000) {
				Keyb_SendKeys("{CTRL}", 10);
				timer.Reset();
				std::uniform_int_distribution<int> rand1min(53*1000, 67*1000);	
				nextDeltaTime = deltaTime + std::uniform_int_distribution<int>(-22*1000, 35*1000)(rng);
			}
		}
	}	
	bool IsActive() {return active;}
	
private:
	TimeStop timer;
	int x0 = -1, y0 = -1;
	dword deltaTime, nextDeltaTime;
	int fromH, fromM, toH, toM;
	bool active = true;
	std::mt19937 rng;
};
#endif

}

#endif

// Known bugs
// GetWindowsList does not get the window title in Kde
// Shutdown in Linux only works with option "logoff", probably because of user permissions
