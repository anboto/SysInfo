// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <Core/Core.h>
#include <SysInfo/Crash.h>

using namespace Upp;


void DivisionByZero() {
	UppLog() << "\nTesting division by zero";  
	
	double a = 4, b = 0;
	
	double c = a/b;
}

void NullPointerAssignment() {
	UppLog() << "\nTesting null pointer assignment";  
	
	int* p = nullptr;
	*p = 10;  
}

void OutOfBounds() {
	UppLog() << "\nTesting out of bounds";  
	
	int vect[30];
	int id = 30;
	vect[id] = 23;  
}

void MemoryAllocation() {
	UppLog() << "\nTesting memory allocation failure";  
	
	Buffer<int> data;
	data.Alloc(123456789012, 0);
}

void IllegalInstruction() {
    UppLog() << "\nTesting illegal instruction";  
    
#ifdef MSVC
    __asm {
        ud2;  // Execute an undefined instruction (illegal instruction)
    }
#else
	__asm__("ud2"); 
#endif
}

int64 StackOverflow0(int64 val) {
	return StackOverflow0(val++);
}

void StackOverflow() {
    UppLog() << "\nTesting stack overflow";  
    
    StackOverflow0(0);
}

void FloatingPointOverflow() {
    UppLog() << "\nTesting floating-point overflow";  
    
    double largeValue = 1e308;
    double result = largeValue * largeValue;  // This should trigger floating-point overflow
}

void MisalignedMemoryAccess() {
    UppLog() << "\nTesting misaligned memory access";  
    
    char data[8];  // 8-byte array
    int* misalignedPtr = (int*)(data + 1);  // Misaligned access (on platforms requiring alignment)
    *misalignedPtr = 42;  // Write to misaligned address
}

void DoubleFree() {
    UppLog() << "\nTesting double free";  
    
    int* ptr = new int[10];
    delete[] ptr;  // Free memory
    
    delete[] ptr;  // Double free (invalid)
}	

class Base {};
class Derived : public Base {};

void InvalidCast() {
    UppLog() << "\nTesting invalid cast";  
    
    Base* basePtr = new Base;
    Derived* derivedPtr = static_cast<Derived*>(basePtr);  // Invalid cast
    derivedPtr->~Derived();  // Attempt to call Derived-specific code on a Base object
}

void UseAfterFree() {
    UppLog() << "\nTesting use after free";  
    
    int* ptr = new int[10];
    delete[] ptr;  // Free the memory
    
    ptr[0] = 42;  // Use memory after it has been freed
}

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
