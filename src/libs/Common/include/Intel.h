#pragma once

// espkk # remove inline asm # 30/Dec/2017
#include <intrin.h>

//#define inline        inline

#define HWD_MT_BIT 0x10000000       // EDX[28]  Bit 28 is set if HT or multi-core is supported
#define NUM_LOGICAL_BITS 0x00FF0000 // EBX[23:16] Bit 16-23 in ebx contains the number of logical
// processors per physical processor when execute cpuid with
// eax set to 1
#define NUM_CORE_BITS 0xFC000000 // EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
// per physical processor when execute cpuid with
// eax set to 4.

#define INITIAL_APIC_ID_BITS 0xFF000000 // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique
// initial APIC ID for the processor this code is running on.

// Status Flag
#define SINGLE_CORE_AND_HT_ENABLED 1
#define SINGLE_CORE_AND_HT_DISABLED 2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE 4
#define MULTI_CORE_AND_HT_NOT_CAPABLE 5
#define MULTI_CORE_AND_HT_ENABLED 6
#define MULTI_CORE_AND_HT_DISABLED 7
#define USER_CONFIG_ISSUE 8

class Intel
{
  public:
    bool IsSSE();
    bool IsIntelCPU();
    uint32_t CpuIDSupported(void);
    uint32_t HWD_MTSupported(void);
    uint32_t MaxLogicalProcPerPhysicalProc(void);
    uint32_t MaxCorePerPhysicalProc(void);
    uint8_t CPUCount(uint32_t *TotAvailLogical, uint32_t *TotAvailCore, uint32_t *PhysicalNum);
};

inline uint8_t GetAPIC_ID(void)
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    return static_cast<uint8_t>((cpuInfo[1] & INITIAL_APIC_ID_BITS) >> 24);

    /*uint32_t Regebx = 0;
    __asm
    {
      mov eax, 1
      cpuid
      mov Regebx, ebx
    }

    return (byte) ((Regebx & INITIAL_APIC_ID_BITS) >> 24);*/
}

inline uint32_t find_maskwidth(uint32_t CountItem)
{
    if (CountItem == 0)
        return 1;

    uint32_t mask_width;
    _BitScanReverse((DWORD *)&mask_width, CountItem - 1);
    return mask_width;

    /*uint32_t MaskWidth, count = CountItem;

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
    return MaskWidth;*/
}

inline uint8_t GetNzbSubID(uint8_t FullID, uint8_t MaxSubIDValue, uint8_t ShiftCount)
{
    uint32_t MaskWidth;
    uint8_t MaskBits;

    MaskWidth = find_maskwidth(static_cast<uint32_t>(MaxSubIDValue));
    MaskBits = (0xff << ShiftCount) ^ static_cast<uint8_t>(0xff << (ShiftCount + MaskWidth));

    return (FullID & MaskBits);
}

inline bool Intel::IsSSE()
{
    if (IsIntelCPU())
    {
        int cpuInfo[4];
        __cpuid(cpuInfo, 1);

        if (cpuInfo[3] >> 26 & 1) // SSE2
            return true;
    }

    return false;

    /*DWORD dwEdx;
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
      // SSE may be disabled by the OS, or extended support may be missing
      // context (24 bit edx CPUID (1))!
      if (dwEdx & (1<<24))
      {
        __try
        {
          // try to execute the command ...
          __asm xorps xmm0, xmm0

          bSSE = true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
      }
    }
    return bSSE;*/
}

inline bool Intel::IsIntelCPU()
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);
    return ((cpuInfo[1] == 'uneG') && (cpuInfo[3] == 'Ieni') && (cpuInfo[2] == 'letn'));

    /*    uint32_t VendorID[3] = {0, 0, 0};

      __try    // If CPUID instruction is supported
      {
        __asm
        {
          xor eax, eax            // call cpuid with eax = 0
              cpuid                    // Get vendor id string
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
           (VendorID[2] == 'letn'));*/
}

inline uint32_t Intel::CpuIDSupported(void)
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    return cpuInfo[0];
    /*uint32_t MaxInputValue = 0;

    __try    // If CPUID instruction is supported
    {
      __asm
      {
        xor eax, eax            // call cpuid with eax = 0
            cpuid
        mov MaxInputValue, eax
      }
    }

    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      return(0);                   // cpuid instruction is unavailable
    }

    return MaxInputValue;*/
}

inline uint32_t Intel::HWD_MTSupported(void)
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    return cpuInfo[3] & HWD_MT_BIT;

    /*uint32_t Regedx      = 0;

    if ((CpuIDSupported() >= 1) && IsIntelCPU())
    {
      __asm
      {
        mov eax, 1
        cpuid
        mov Regedx, edx
      }
    }

    return (Regedx & HWD_MT_BIT);*/
}

inline uint32_t Intel::MaxLogicalProcPerPhysicalProc(void)
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);

    return (cpuInfo[1] & NUM_LOGICAL_BITS) >> 16;

    /*uint32_t Regebx = 0;

    if (!HWD_MTSupported()) return (uint32_t) 1;
      __asm
      {
        mov eax, 1
        cpuid
        mov Regebx, ebx
      }
      return (uint32_t) ((Regebx & NUM_LOGICAL_BITS) >> 16);*/
}

inline uint32_t Intel::MaxCorePerPhysicalProc(void)
{
    if (!HWD_MTSupported() || CpuIDSupported() < 4)
        return 1;

    int cpuInfo[4];
    __cpuidex(cpuInfo, 4, 0);

    return ((cpuInfo[0] & NUM_CORE_BITS) >> 26) + 1;

    /*    uint32_t Regeax = 0;

      if (!HWD_MTSupported()) return (uint32_t) 1;  // Single core

        __asm
        {
          xor eax, eax
          cpuid
          cmp eax, 4            // check if cpuid supports leaf 4
          jl single_core        // Single core
          mov eax, 4
          mov ecx, 0            // start with 1st or 2nd cache level using index = 0 or 1
          cpuid
          mov Regeax, eax
          jmp multi_core

    single_core:
          xor eax, eax

    multi_core:

        }

      return (uint32_t)((Regeax & NUM_CORE_BITS) >> 26)+1;*/
}

inline uint8_t Intel::CPUCount(uint32_t *TotAvailLogical, uint32_t *TotAvailCore, uint32_t *PhysicalNum)
{
    uint8_t StatusFlag = 0;
    DWORD_PTR dwProcessAffinity, dwSystemAffinity;
    uint32_t dwAffinityMask;
    uint32_t numLPEnabled = 0;
    int j = 0, MaxLPPerCore;
    uint8_t apicID, PackageIDMask;
    uint8_t tblPkgID[256], tblCoreID[256], tblSMTID[256];
    *TotAvailCore = 1;
    *PhysicalNum = 1;

    GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinity, &dwSystemAffinity);

    if (dwProcessAffinity != dwSystemAffinity) // not all CPUs are enabled
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
            Sleep(0); // Ensure system to switch to the right CPU
            apicID = GetAPIC_ID();

            tblSMTID[j] = GetNzbSubID(apicID, MaxLPPerCore, 0);
            tblCoreID[j] =
                GetNzbSubID(apicID, MaxCorePerPhysicalProc(), static_cast<uint8_t>(find_maskwidth(MaxLPPerCore)));

            PackageIDMask = static_cast<uint8_t>(0xff << find_maskwidth(MaxLogicalProcPerPhysicalProc()));

            tblPkgID[j] = apicID & PackageIDMask;

            numLPEnabled++; // Number of available logical processors in the system.
        }                   // if

        j++;
        dwAffinityMask = 1 << j;
    } // while

    *TotAvailLogical = numLPEnabled;

    //
    // Count available cores (TotAvailCore) in the system
    //
    uint8_t CoreIDBucket[256];
    uint32_t ProcessorMask, pCoreMask[256];
    uint32_t i, ProcessorNum;

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
        } // for i

        if (i == *TotAvailCore) // did not match any bucket.  Start a new one.
        {
            CoreIDBucket[i] = tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum];
            pCoreMask[i] = ProcessorMask;

            (*TotAvailCore)++; // Number of available cores in the system
        }
    } // for ProcessorNum

    //
    // Count physical processor (PhysicalNum) in the system
    //
    uint8_t PackageIDBucket[256];
    uint32_t pPackageMask[256];

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
            if (tblPkgID[ProcessorNum] == PackageIDBucket[i])
            {
                pPackageMask[i] |= ProcessorMask;
                break;
            }
        } // for i

        if (i == *PhysicalNum) // did not match any bucket.  Start a new one.
        {
            PackageIDBucket[i] = tblPkgID[ProcessorNum];
            pPackageMask[i] = ProcessorMask;

            (*PhysicalNum)++; // Total number of physical processors in the system
        }
    } // for ProcessorNum

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
        else
            StatusFlag = MULTI_CORE_AND_HT_DISABLED;
    }
    else
    {
        // Single-core
        if (MaxLPPerCore == 1)
            StatusFlag = SINGLE_CORE_AND_HT_NOT_CAPABLE;
        else if (numLPEnabled > *TotAvailCore)
            StatusFlag = SINGLE_CORE_AND_HT_ENABLED;
        else
            StatusFlag = SINGLE_CORE_AND_HT_DISABLED;
    }

    return StatusFlag;
}
