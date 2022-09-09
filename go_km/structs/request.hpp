#pragma once

typedef struct _REQUEST_CODE_INITIALIZE
{
    static constexpr auto                   code = CODE_INITIALIZE;
    
    // Offsets extraidos do pdb
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

    ULONG                                   zw_protect_virtual_memory = 0;
    ULONG                                   zw_query_virtual_memory = 0;
    ULONG                                   mm_allocate_independent_pages = 0;
    ULONG                                   mm_free_independent_pages = 0;

    bool                                    success = false;
} REQUEST_CODE_INITIALIZE, *PREQUEST_CODE_INITIALIZE;

typedef struct _REQUEST_ALLOCATE_MEMORY
{
    static constexpr auto                   code = CODE_ALLOCATE_MEMORY;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    ULONG                                   size;
    ULONG                                   state;
    ULONG                                   protect;
    ULONG                                   error_code = STATUS_SUCCESS;
} REQUEST_ALLOCATE_MEMORY, *PREQUEST_ALLOCATE_MEMORY;

typedef struct _REQUEST_FREE_MEMORY
{
    static constexpr auto                   code = CODE_FREE_MEMORY;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    ULONG                                   error_code = STATUS_SUCCESS;
} REQUEST_FREE_MEMORY, *PREQUEST_FREE_MEMORY;

typedef struct _REQUEST_WRITE_MEMORY
{
    static constexpr auto                   code = CODE_WRITE_MEMORY;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    ULONG_PTR                               buffer;
    ULONG                                   buffer_size;
    ULONG                                   num_bytes_writted;
    ULONG                                   error_code = STATUS_SUCCESS;
} REQUEST_WRITE_MEMORY, *PREQUEST_WRITE_MEMORY;

typedef struct _REQUEST_READ_MEMORY
{
    static constexpr auto                   code = CODE_READ_MEMORY;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    ULONG_PTR                               buffer;
    ULONG                                   buffer_size;
    ULONG                                   num_bytes_readed;
    ULONG                                   error_code = STATUS_SUCCESS;
} REQUEST_READ_MEMORY, *PREQUEST_READ_MEMORY;

typedef struct _REQUEST_GET_MODULE_HANDLE
{
    static constexpr auto                   code = CODE_GET_MODULE_HANDLE;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    char                                    name[260];
    ULONG                                   error_code = STATUS_SUCCESS;
};

typedef struct _REQUEST_GET_PROCESSID
{
    static constexpr auto                   code = CODE_GET_PROCESSID;
    char                                    name[260];
    ULONG                                   pid;
    ULONG                                   error_code = STATUS_SUCCESS;
};

typedef struct _REQUEST_CHANGE_MEMORY_PROTECTION
{
    static constexpr auto                   code = CODE_CHANGE_MEMORY_PROTECTION;
    ULONG                                   process_id;
    ULONG_PTR                               address;
    ULONG                                   size;
    SIZE_T                                  new_protection;
    SIZE_T                                  old_protection;
    ULONG                                   error_code = STATUS_SUCCESS;
} REQUEST_CHANGE_MEMORY_PROTECTION, *PREQUEST_CHANGE_MEMORY_PROTECTION;