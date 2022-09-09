#pragma once

extern "C" NTKERNELAPI
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader( PVOID Base );

extern "C" NTKERNELAPI
NTSTATUS
NTAPI
ZwProtectVirtualMemory
(
    IN HANDLE                       ProcessHandle,
    IN OUT PVOID*                   BaseAddress,
    IN OUT SIZE_T*                  NumberOfBytesToProtect,
    IN ULONG                        NewAccessProtection,
    OUT PULONG                      OldAccessProtection
);

extern "C" NTKERNELAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation
(
    IN SYSTEM_INFORMATION_CLASS     SystemInformationClass,
    OUT PVOID                       SystemInformation,
    IN ULONG                        SystemInformationLength,
    OUT PULONG                      ReturnLength OPTIONAL
);

extern "C" NTKERNELAPI
PVOID
NTAPI
PsGetProcessWow64Process
( 
    IN PEPROCESS                    Process 
);

extern "C" NTKERNELAPI
NTSTATUS
NTAPI
MmCopyVirtualMemory
(
    IN PEPROCESS                    FromProcess,
    IN PVOID                        FromAddress,
    IN PEPROCESS                    ToProcess,
    OUT PVOID                       ToAddress,
    IN SIZE_T                       BufferSize,
    IN KPROCESSOR_MODE              PreviousMode,
    OUT PSIZE_T                     NumberOfBytesCopied
);


extern "C" NTKERNELAPI
PVOID
NTAPI
RtlFindExportedRoutineByName
(
    _In_ PVOID                      ImageBase,
    _In_ PCCH                       RoutineNam
);