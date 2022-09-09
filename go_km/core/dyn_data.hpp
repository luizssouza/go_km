#pragma once

namespace dyn_data
{

uintptr_t                               ntoskrnl = 0;
ULONG                                   zw_protect_virtual_memory = 0;
ULONG                                   zw_query_virtual_memory = 0;
ULONG                                   mm_allocate_independent_pages = 0;
ULONG                                   mm_free_independent_pages = 0;

ULONG                                   kprocess_execopt = 0;                       // KPROCESS::ExecuteOptions(UCHAR)
ULONG                                   eprocess_protection = 0;                    // EPROCESS::Protection(_PS_PROTECTION)
ULONG                                   eprocess_objtable = 0;                      // EPROCESS::ObjectTable(_HANDLE_TABLE)
ULONG                                   eprocess_vadroot = 0;                       // EPROCESS::VadRoot(_RTL_AVL_TREE)
ULONG                                   eprocess_uniqueprocessid = 0;               // EPROCESS::UniqueProcessId(PVOID)
ULONG                                   eprocess_ownerprocessid = 0;                // EPROCESS::OwnerProcessId(uint8)
ULONG                                   eprocess_activeprocesslinks = 0;            // EPROCESS::ActiveProcessLinks(_LIST_ENTRY)
ULONG                                   eprocess_inheritedfromuniqueprocessid = 0;  // EPROCESS::InheritedFromUniqueProcessId(PVOID)
ULONG                                   eprocess_rotateinprogress = 0;              // EPROCESS::RotateInProgress(_EHTREAD)
ULONG                                   eprocess_forkinprogress = 0;                // EPROCESS::ForkInProgress(_ETHREAD)

} // dyn_data