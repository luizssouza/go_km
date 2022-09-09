#pragma once

enum REQUEST_CODES
{
    CODE_MIN = 0xDEADBEEF,

    CODE_INITIALIZE,
    CODE_ALLOCATE_MEMORY,
    CODE_FREE_MEMORY,
    CODE_WRITE_MEMORY,
    CODE_READ_MEMORY,
    CODE_CHANGE_MEMORY_PROTECTION,
    CODE_GET_MODULE_HANDLE,
    CODE_GET_PROCESSID,
    CODE_IS_WOW64,

    CODE_MAX
};

struct s_request_initialize
{
    static constexpr auto                   code = CODE_INITIALIZE;

    // Offsets extraidos do pdb
    uint32_t                                kprocess_execopt = 0;                       // KPROCESS::ExecuteOptions(UCHAR)
    uint32_t                                eprocess_protection = 0;                    // EPROCESS::Protection(_PS_PROTECTION)
    uint32_t                                eprocess_objtable = 0;                      // EPROCESS::ObjectTable(_HANDLE_TABLE)
    uint32_t                                eprocess_vadroot = 0;                       // EPROCESS::VadRoot(_RTL_AVL_TREE)
    uint32_t                                eprocess_uniqueprocessid = 0;               // EPROCESS::UniqueProcessId(PVOID)
    uint32_t                                eprocess_ownerprocessid = 0;                // EPROCESS::OwnerProcessId(uint8)
    uint32_t                                eprocess_activeprocesslinks = 0;            // EPROCESS::ActiveProcessLinks(_LIST_ENTRY)
    uint32_t                                eprocess_inheritedfromuniqueprocessid = 0;  // EPROCESS::InheritedFromUniqueProcessId(PVOID)
    uint32_t                                eprocess_rotateinprogress = 0;              // EPROCESS::RotateInProgress(_EHTREAD)
    uint32_t                                eprocess_forkinprogress = 0;                // EPROCESS::ForkInProgress(_ETHREAD)

    uint32_t                                zw_protect_virtual_memory = 0;
    uint32_t                                zw_query_virtual_memory = 0;
    uint32_t                                mm_allocate_independent_pages = 0;
    uint32_t                                mm_free_independent_pages = 0;

    bool                                    success = false;
};

struct s_request_allocate_memory
{
    static constexpr auto                   code = CODE_ALLOCATE_MEMORY;

    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    uint32_t                                size = 0;
    uint32_t                                state = 0;
    uint32_t                                protect = 0;
    uint32_t                                error_code = 0;
};

struct s_request_free_memory
{
    static constexpr auto                   code = CODE_FREE_MEMORY;
    
    
    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    uint32_t                                error_code = 0;
};

struct s_request_write_memory
{
    static constexpr auto                   code = CODE_WRITE_MEMORY;

    
    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    uint64_t                                buffer = 0;
    uint32_t                                buffer_size = 0;
    uint32_t                                num_bytes_writted = 0;
    uint32_t                                error_code = 0;
};

struct s_request_read_memory
{
    static constexpr auto                   code = CODE_READ_MEMORY;
    
    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    uint64_t                                buffer = 0;
    uint32_t                                buffer_size = 0;
    uint32_t                                num_bytes_readed = 0;
    uint32_t                                error_code = 0;
};

struct s_request_get_module_handle
{
    static constexpr auto                   code = CODE_GET_MODULE_HANDLE;
    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    char                                    name[260] = { 0 };
    uint32_t                                error_code = 0;
};

struct s_request_get_processid
{
    static constexpr auto                   code = CODE_GET_PROCESSID;
    char                                    name[260] = { 0 };
    uint32_t                                pid = 0;
    uint32_t                                error_code = 0;
};

struct s_request_change_memory_protection
{
    static constexpr auto                   code = CODE_CHANGE_MEMORY_PROTECTION;
    uint32_t                                process_id = 0;
    uint64_t                                address = 0;
    uint32_t                                size = 0;
    uint32_t                                new_protection = 0;
    uint32_t                                old_protection = 0;
    uint32_t                                error_code = 0;
};