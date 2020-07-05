#ifndef INTEL_HPP
#define INTEL_HPP

#define iinline		__forceinline

#define HWD_MT_BIT         0x10000000     // EDX[28]  Bit 28 is set if HT or multi-core is supported
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with 
                                          // eax set to 1
#define NUM_CORE_BITS      0xFC000000     // EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
                                          // per physical processor when execute cpuid with 
                                          // eax set to 4. 


#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
                                          // initial APIC ID for the processor this code is running on.
                                          


// Status Flag
#define SINGLE_CORE_AND_HT_ENABLED					1
#define SINGLE_CORE_AND_HT_DISABLED					2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE				4
#define MULTI_CORE_AND_HT_NOT_CAPABLE				5
#define MULTI_CORE_AND_HT_ENABLED					6
#define MULTI_CORE_AND_HT_DISABLED					7
#define USER_CONFIG_ISSUE							8

class Intel
{
public:
	Intel();
	~Intel();

	bool IsSSE();
	bool IsIntelCPU();
	dword CpuIDSupported(void);
	dword HWD_MTSupported(void);
	dword MaxLogicalProcPerPhysicalProc(void);
	dword MaxCorePerPhysicalProc(void);
	byte CPUCount(dword *TotAvailLogical, dword * TotAvailCore, dword * PhysicalNum);

private:

};

iinline Intel::Intel()
{
}

iinline Intel::~Intel()
{
}

iinline byte GetAPIC_ID(void)
{
	dword Regebx = 0;
	                                             
	__asm
	{
		mov eax, 1
		cpuid
		mov Regebx, ebx
	}

	return (byte) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);
}

iinline dword find_maskwidth(dword CountItem)
{
	dword MaskWidth, count = CountItem;
	
	__asm
	{
		mov eax, count
		mov ecx, 0
		mov MaskWidth, ecx
		dec eax
		bsr cx, ax
		jz next
		inc cx
		mov MaskWidth, ecx
next:
		mov eax, MaskWidth
	}
	return MaskWidth;
}

iinline byte GetNzbSubID(byte FullID, byte MaxSubIDValue, byte ShiftCount)
{
	dword MaskWidth;
	byte MaskBits;

	MaskWidth = find_maskwidth((dword) MaxSubIDValue);
	MaskBits  = (0xff << ShiftCount) ^ 
				((byte) (0xff << (ShiftCount + MaskWidth)));

	return (FullID & MaskBits);
}

iinline bool Intel::IsSSE()
{
	DWORD dwEdx;
	bool bSSE = false;
	
	if (!((CpuIDSupported() >= 1) && IsIntelCPU())) return bSSE;

	__asm 
	{
		mov  eax,1
		cpuid
		mov dwEdx, edx
	}

	if (dwEdx & (1<<25)) 
	{
		// SSE может быть запрещено OS, либо отсутствовать расширенная поддержка 
		// контекста (24 бит edx CPUID(1)) ! 
		if (dwEdx & (1<<24))
		{      
			__try
			{
				// попытаемся исполнить комманду...
				__asm xorps xmm0, xmm0
        
				bSSE = true;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}
	return bSSE;
}

iinline bool Intel::IsIntelCPU()
{
	dword VendorID[3] = {0, 0, 0};

	__try    // If CPUID instruction is supported
	{
		__asm
		{
			xor eax, eax			// call cpuid with eax = 0
        	cpuid					// Get vendor id string
			mov VendorID, ebx
			mov VendorID + 4, edx
			mov VendorID + 8, ecx
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return(0);                   // cpuid instruction is unavailable
	}

	return ( (VendorID[0] == 'uneG') &&
			 (VendorID[1] == 'Ieni') &&
			 (VendorID[2] == 'letn')); 

}

iinline dword Intel::CpuIDSupported(void)
{
	dword MaxInputValue = 0;

	__try    // If CPUID instruction is supported
	{
		__asm
		{
			xor eax, eax			// call cpuid with eax = 0
        	cpuid					
			mov MaxInputValue, eax
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return(0);                   // cpuid instruction is unavailable
	}

	return MaxInputValue;
}

iinline dword Intel::HWD_MTSupported(void)
{
	dword Regedx      = 0;

	if ((CpuIDSupported() >= 1) && IsIntelCPU())
	{
		__asm
		{
			mov eax, 1
			cpuid
			mov Regedx, edx
		}
	}

	return (Regedx & HWD_MT_BIT);  

  
}

iinline dword Intel::MaxLogicalProcPerPhysicalProc(void)
{
	dword Regebx = 0;

	if (!HWD_MTSupported()) return (dword) 1;
		__asm
		{
			mov eax, 1
			cpuid
			mov Regebx, ebx
		}
		return (dword) ((Regebx & NUM_LOGICAL_BITS) >> 16);
}

iinline dword Intel::MaxCorePerPhysicalProc(void)
{
	dword Regeax = 0;
	
	if (!HWD_MTSupported()) return (dword) 1;  // Single core

		__asm
		{
			xor eax, eax
			cpuid
			cmp eax, 4			// check if cpuid supports leaf 4
			jl single_core		// Single core
			mov eax, 4			
			mov ecx, 0			// start with 1st or 2nd cache level using index = 0 or 1
			cpuid
			mov Regeax, eax
			jmp multi_core

single_core:
			xor eax, eax		

multi_core:
			
		}

	return (dword)((Regeax & NUM_CORE_BITS) >> 26)+1;

}

iinline byte Intel::CPUCount(dword *TotAvailLogical, dword * TotAvailCore, dword * PhysicalNum)
{
	byte StatusFlag = 0;
	DWORD dwProcessAffinity, dwSystemAffinity;
	DWORD dwAffinityMask;
	dword numLPEnabled = 0;
	int j = 0, MaxLPPerCore;
	byte apicID, PackageIDMask;
	byte tblPkgID[256], tblCoreID[256], tblSMTID[256];
	*TotAvailCore = 1;
	*PhysicalNum  = 1;

	GetProcessAffinityMask(GetCurrentProcess(), 
						   &dwProcessAffinity,
						   &dwSystemAffinity);

	if (dwProcessAffinity != dwSystemAffinity)  // not all CPUs are enabled
	{
		StatusFlag = USER_CONFIG_ISSUE;		
		return StatusFlag;
	}

	MaxLPPerCore = MaxLogicalProcPerPhysicalProc() / MaxCorePerPhysicalProc();

	dwAffinityMask = 1;
	while (dwAffinityMask && dwAffinityMask <= dwSystemAffinity)
	{
		if (SetThreadAffinityMask(GetCurrentThread(), dwAffinityMask))
		{
			Sleep(0);  // Ensure system to switch to the right CPU
			apicID = GetAPIC_ID();

			tblSMTID[j]  = GetNzbSubID(apicID, MaxLPPerCore, 0);
			tblCoreID[j] = GetNzbSubID(apicID, 
						   MaxCorePerPhysicalProc(),
						   (byte) find_maskwidth(MaxLPPerCore));

			PackageIDMask = (byte) (0xff << 
							find_maskwidth(MaxLogicalProcPerPhysicalProc()));

			tblPkgID[j] = apicID & PackageIDMask;

			numLPEnabled ++;   // Number of available logical processors in the system.
		} // if

		j++;
		dwAffinityMask = 1 << j;


	} // while

	
	*TotAvailLogical = numLPEnabled;

	//
	// Count available cores (TotAvailCore) in the system
	//
	byte CoreIDBucket[256];
	DWORD ProcessorMask, pCoreMask[256];
	dword i, ProcessorNum;

	CoreIDBucket[0] = tblPkgID[0] | tblCoreID[0];
	ProcessorMask = 1;
	pCoreMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *TotAvailCore; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if ((tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum]) == CoreIDBucket[i])
			{
				pCoreMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *TotAvailCore)   // did not match any bucket.  Start a new one.
		{
			CoreIDBucket[i] = tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum];
			pCoreMask[i] = ProcessorMask;

			(*TotAvailCore)++;	// Number of available cores in the system

		}

	}  // for ProcessorNum


	//
	// Count physical processor (PhysicalNum) in the system
	//
	byte PackageIDBucket[256];
	DWORD pPackageMask[256];

	PackageIDBucket[0] = tblPkgID[0];
	ProcessorMask = 1;
	pPackageMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *PhysicalNum; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if (tblPkgID[ProcessorNum]== PackageIDBucket[i])
			{
				pPackageMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *PhysicalNum)   // did not match any bucket.  Start a new one.
		{
			PackageIDBucket[i] = tblPkgID[ProcessorNum];
			pPackageMask[i] = ProcessorMask;

			(*PhysicalNum)++;	// Total number of physical processors in the system

		}

	}  // for ProcessorNum

	//
	// Check to see if the system is multi-core 
	// Check if the system is hyper-threading
	//
	if (*TotAvailCore > *PhysicalNum) 
	{
		// Multi-core
		if (MaxLPPerCore == 1)
			StatusFlag = MULTI_CORE_AND_HT_NOT_CAPABLE;
		else if (numLPEnabled > *TotAvailCore)
			StatusFlag = MULTI_CORE_AND_HT_ENABLED;
			else StatusFlag = MULTI_CORE_AND_HT_DISABLED;
	}
	else
	{
		// Single-core
		if (MaxLPPerCore == 1)
			StatusFlag = SINGLE_CORE_AND_HT_NOT_CAPABLE;
		else if (numLPEnabled > *TotAvailCore)
			StatusFlag = SINGLE_CORE_AND_HT_ENABLED;
			else StatusFlag = SINGLE_CORE_AND_HT_DISABLED;
	}
	
	return StatusFlag;
}

#endif