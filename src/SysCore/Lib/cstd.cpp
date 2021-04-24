
//! Function pointer typedef for less typing
typedef void (* _PVFV)(void);

//! __xc_a points to beginning of initializer table
// #pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = { 0 };

//! __xc_z points to end of initializer table
// #pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = { 0 };

//! Select the default data segment again (.data) for the rest of the unit
// #pragma data_seg()

//! Now, move the CRT data into .data section so we can read/write to it
// #pragma comment(linker, "/merge:.CRT=.data")

/*
===========================
	Globals
===========================
*/

//! function pointer table to global deinitializer table
static _PVFV * pf_atexitlist = 0;

//! Maximum entries allowed in table. Change as needed
static unsigned max_atexitlist_entries = 32;

//! Current amount of entries in table
static unsigned cur_atexitlist_entries = 0;

/*
===========================
	CRT Routines
===========================
*/

//! initialize all global initializers (ctors, statics, globals, etc..)
void _initterm ( _PVFV * pfbegin, _PVFV * pfend ) {

	//! Go through each initializer
    while ( pfbegin < pfend )
    {
	  //! Execute the global initializer
      if ( *pfbegin != 0 )
            (**pfbegin) ();

	    //! Go to next initializer inside the initializer table
        ++pfbegin;
    }
}

//! initialize the de-initializer function table
void _atexit_init(void) {

    max_atexitlist_entries = 32;

	// Warning: Normally, the STDC will dynamically allocate this. Because we have no memory manager, just choose
	// a base address that you will never use for now
	pf_atexitlist = (_PVFV *)0x5000;
}

//! adds a new function entry that is to be called at shutdown
int atexit(_PVFV fn) {

	//! Insure we have enough free space
	if (cur_atexitlist_entries>=max_atexitlist_entries)
		return 1;
	else {

		//! Add the exit routine
		*(pf_atexitlist++) = fn;
		cur_atexitlist_entries++;
	}
	return 0;
}

//! shutdown the C++ runtime; calls all global de-initializers
void Exit () {

	//! Go through the list, and execute all global exit routines
	while (cur_atexitlist_entries--) {

			//! execute function
			(*(--pf_atexitlist)) ();
	}
}

//! execute all constructors and other dynamic initializers
void InitializeConstructors() {

   _atexit_init();
   _initterm(__xc_a, __xc_z); 
}

// Disable C4100 ('Unused Parameter') warning for now, 'til we could get these written
// #pragma warning (disable:4100)

//! purecall function handler
int _purecall() { for(;;) {}; return 0; }

//! global new and delete operators
void* operator new (unsigned long size) { return 0; }
void* operator new[] (unsigned long size) { return 0; }
void operator delete (void * p) {}
void operator delete[] (void * p) { }

extern "C" float _CIcos() {
#ifdef ARCH_X86
   _asm fcos
#endif
};

extern "C" float _CIsin() {
#ifdef ARCH_X86
   _asm fsin
#endif
};

extern "C" float _CIsqrt() {
#ifdef ARCH_X86
   _asm fsqrt
#endif
};

//! called by MSVC++ to convert a float to a long
extern "C" long _ftol2_sse() {
	int a;
#ifdef ARCH_X86
	_asm {
		fistp [a]
		mov	ebx, a
	}
#endif
}

//! required by MSVC++ runtime for floating point operations (Must be 1)
extern "C" int _fltused = 1;

//! enable warning
// #pragma warning (default:4100)
