#pragma once

template < int n >
using const_int = std::integral_constant< int, n >;

template < typename T >
constexpr bool is32bit = std::is_same_v< T, uint32_t >;

template < typename T, typename T32, typename T64 >
using type_32_64 = std::conditional_t< is32bit< T >, T32, T64 >;

template<typename T, int v32, int v64>
constexpr int int_32_64 = std::conditional_t< is32bit< T >, const_int< v32 >, const_int< v64 > >::value;

enum so_index_t
{
    NONE,
    WIN_7,
    WIN_8,
    WIN_81,
    WIN_10
};

template < typename T >
struct list_entry_t
{
    T                                                           flink;
    T                                                           blink;
};

template < typename T >
struct unicode_string_t
{
    uint16_t                                                    length;
    uint16_t                                                    maximum_length;
    T                                                           buffer;
};

template < typename T >
struct nt_tib_t
{
    T                                                           exception_list;
    T                                                           stack_base;
    T                                                           stack_limit;
    T                                                           sub_system_tib;
    T                                                           fiber_data;
    T                                                           arbitrary_user_pointer;
    T                                                           self;
};

template < typename T >
struct client_id_t
{
    T                                                           unique_process;
    T                                                           unique_thread;
};

template < typename T >
struct gdi_teb_batch_t
{
    uint32_t                                                    offset;
    T                                                           hdc;
    uint32_t                                                    buffer[ 310 ];
};

template < typename T >
struct activation_context_stack_t
{
    T                                                           active_frame;
    list_entry_t< T >                                           frame_list_cache;
    uint32_t                                                    flags;
    uint32_t                                                    next_cookie_sequence_number;
    uint32_t                                                    stack_id;
};

template < typename T >
struct teb_t
{
    struct Specific32_1
    {
        uint8_t                                                 instrumentation_callback_disabled;
        uint8_t                                                 spare_bytes[ 23 ];
        uint32_t                                                tx_fs_context;
    };

    struct Specific64_1
    {
        uint32_t                                                tx_fs_context;
        uint32_t                                                instrumentation_callback_disabled;
    };

    struct Specific64_2
    {
        T                                                       tls_expansion_slots;
        T                                                       deallocation_bstore;
        T                                                       bstore_limit;
    };

    struct Specific32_2
    {
        T                                                       tls_expansion_slots;
    };

    nt_tib_t< T >                                               nttib;
    T                                                           environment_pointer;
    client_id_t< T >                                            clientid;
    T                                                           active_rpc_handle;
    T                                                           thread_local_storage_pointer;
    T                                                           process_environment_block;
    uint32_t                                                    last_error_value;
    uint32_t                                                    count_of_owned_critical_sections;
    T                                                           csr_client_thread;
    T                                                           win32_thread_info;
    uint32_t                                                    user32_reserved[ 26 ];
    uint32_t                                                    user_reserved[ 5 ];
    T                                                           wow32_reserved;
    uint32_t                                                    current_locale;
    uint32_t                                                    fp_software_status_register;
    T                                                           reserved_for_debugger_instrumentation[ 16 ];
    T                                                           system_reserved1[ int_32_64< T, 26, 30 > ];
    uint8_t                                                     place_holder_compatibility_mode;
    uint8_t                                                     place_holder_reserved[ 11 ];
    uint32_t                                                    proxied_process_id;
    activation_context_stack_t< T >                             activation_stack;
    uint8_t                                                     working_on_behalf_ticket[ 8 ];
    uint32_t                                                    exceptioncode;
    T                                                           activation_context_stack_pointer;
    T                                                           instrumentation_callback_sp;
    T                                                           instrumentationCallbackPreviousPc;
    T                                                           instrumentationCallbackPreviousSp;
    type_32_64< T, Specific32_1, Specific64_1 >                 spec1;
    gdi_teb_batch_t< T >                                        gdi_teb_batch;
    client_id_t< T >                                            real_client_id;
    T                                                           gdi_CachedProcessHandle;
    uint32_t                                                    gdi_client_pid;
    uint32_t                                                    gdi_client_tid;
    T                                                           gdi_thread_local_info;
    T                                                           win32_client_info[ 62 ];
    T                                                           gl_dispatch_table[ 233 ];
    T                                                           gl_reserved1[ 29 ];
    T                                                           gl_reserved2;
    T                                                           gl_sectionInfo;
    T                                                           gl_section;
    T                                                           gl_table;
    T                                                           gl_current_rC;
    T                                                           gl_context;
    uint32_t                                                    last_status_value;
    unicode_string_t< T >                                       static_unicode_string;
    wchar_t                                                     static_unicode_buffer[ 261 ];
    T                                                           deallocation_stack;
    T                                                           tls_slots[ 64 ];
    list_entry_t< T >                                           tls_links;
    T                                                           vdm;
    T                                                           reservedForNtRpc;
    T                                                           dbg_ss_reserved[ 2 ];
    uint32_t                                                    hard_error_mode;
    T                                                           instrumentation[ int_32_64<T, 9, 11> ];
    GUID                                                        activity_id;
    T                                                           sub_process_tag;
    T                                                           per_flib_data;
    T                                                           etw_trace_data;
    T                                                           win_sock_data;
    uint32_t                                                    gdi_batch_count;
    uint32_t                                                    ideal_processor_value;
    uint32_t                                                    guaranteed_stack_bytes;
    T                                                           reserved_for_perf;
    T                                                           reserved_for_ole;
    uint32_t                                                    waiting_on_loader_lock;
    T                                                           saved_priority_state;
    T                                                           reserved_for_code_coverage;
    T                                                           thread_pool_data;
    type_32_64< T, Specific32_2, Specific64_2 >                 spec2;
    uint32_t                                                    mui_generation;
    uint32_t                                                    is_impersonating;
    T                                                           nls_cache;
    T                                                           shim_data;
    uint16_t                                                    heap_virtual_affinity;
    uint16_t                                                    low_frag_heap_data_slot;
    T                                                           current_transaction_handle;
    T                                                           active_frame;
    T                                                           fls_data;
    T                                                           preferred_languages;
    T                                                           user_pref_languages;
    T                                                           merged_pref_languages;
    uint32_t                                                    mui_impersonation;
    uint16_t                                                    cross_teb_flags;
    union
    {
        uint16_t                                                same_teb_flags;
        struct
        {
            uint16_t                                            safe_thunk_call : 1;
            uint16_t                                            in_debug_print : 1;
            uint16_t                                            has_fiber_ata : 1;
            uint16_t                                            skip_thread_attach : 1;
            uint16_t                                            wer_in_ship_assert_code : 1;
            uint16_t                                            ran_process_init : 1;
            uint16_t                                            cloned_thread : 1;
            uint16_t                                            suppress_debug_msg : 1;
            uint16_t                                            disable_user_stack_walk : 1;
            uint16_t                                            rtl_exception_attached : 1;
            uint16_t                                            initial_thread : 1;
            uint16_t                                            session_aware : 1;
            uint16_t                                            load_owner : 1;
            uint16_t                                            loader_worker : 1;
            uint16_t                                            skip_loader_init : 1;
            uint16_t                                            spare_same_teb_bits : 1;
        };
    };
    T                                                           txn_scope_enter_callback;
    T                                                           txn_scope_exit_callback;
    T                                                           txn_scope_context;
    uint32_t                                                    lock_count;
    uint32_t                                                    wow_teb_offset;
    T                                                           resource_ret_value;
    T                                                           reserved_for_wdf;
    uint64_t                                                    reserved_for_crt;
    GUID                                                        effective_container_id;
};

template<typename T>
struct peb_t
{
    uint8_t                                                     inherited_address_pace;
    uint8_t                                                     read_image_file_exec_options;
    uint8_t                                                     being_debugged;
    union
    {
        uint8_t                                                 bit_field;
        struct
        {
            uint8_t                                             bmage_uses_large_pages : 1;
            uint8_t                                             is_protected_process : 1;
            uint8_t                                             is_image_dynamically_relocated : 1;
            uint8_t                                             skip_patching_user32_forwarders : 1;
            uint8_t                                             is_packagedProcess : 1;
            uint8_t                                             is_appContainer : 1;
            uint8_t                                             is_protectedProcessLight : 1;
            uint8_t                                             spare_bits : 1;
        };
    };
    T                                                           mutant;
    T                                                           image_base_address;
    T                                                           ldr;
    T                                                           process_arameters;
    T                                                           sub_system_data;
    T                                                           process_heap;
    T                                                           fast_peb_lock;
    T                                                           atl_thunks_list_ptr;
    T                                                           ife_okey;
    union
    {
        T                                                       cross_process_flags;
        struct
        {
            uint32_t                                            process_in_job : 1;
            uint32_t                                            process_initializing : 1;
            uint32_t                                            process_ising_veh : 1;
            uint32_t                                            process_ising_vch : 1;
            uint32_t                                            process_using_fth : 1;
            uint32_t                                            reserved_bits0 : 27;
        };
    };
    union
    {
        T                                                       kernel_callback_table;
        T                                                       user_shared_info_ptr;
    };
    uint32_t                                                    system_reserved;
    uint32_t                                                    atl_thunks_list_ptr32;
    T                                                           api_set_map;
    union
    {
        uint32_t                                                tls_expansion_counter;
        T                                                       padding2;
    };
    T                                                           tls_bitmap;
    uint32_t                                                    tls_bitmap_bits[ 2 ];
    T                                                           read_only_shared_memory_base;
    T                                                           spare_pvoid0;
    T                                                           read_only_static_server_data;
    T                                                           ansi_code_page_data;
    T                                                           oem_code_page_data;
    T                                                           unicode_case_table_data;
    uint32_t                                                    number_of_processors;
    uint32_t                                                    nt_global_flag;
    LARGE_INTEGER                                               critical_section_timeout;
    T                                                           heap_segment_reserve;
    T                                                           heap_segment_commit;
    T                                                           heap_decommit_total_free_threshold;
    T                                                           heap_decommit_free_block_threshold;
    uint32_t                                                    number_of_heaps;
    uint32_t                                                    maximum_number_of_eaps;
    T                                                           process_heaps;
    T                                                           gdi_shared_handle_table;
    T                                                           process_starter_helper;
    union
    {
        uint32_t                                                gdi_dc_attribute_list;
        T                                                       padding3;
    };
    T                                                           loader_lock;
    uint32_t                                                    os_major_ersion;
    uint32_t                                                    os_minor_version;
    uint16_t                                                    os_build_number;
    uint16_t                                                    os_csd_version;
    uint32_t                                                    os_platform_id;
    uint32_t                                                    image_sub_system;
    uint32_t                                                    image_sub_system_ajor_version;
    union
    {
        uint32_t                                                image_subsystem_minor_version;
        T                                                       padding4;
    };
    T                                                           active_process_affinity_mask;
    uint32_t                                                    gdi_handle_buffer[ int_32_64<T, 34, 60> ];
    T                                                           post_process_init_outine;
    T                                                           tls_expansion_bitmap;
    uint32_t                                                    tls_expansion_bitmap_bits[ 32 ];
    union
    {
        uint32_t                                                session_id;
        T                                                       padding5;
    };
    ULARGE_INTEGER                                              app_compat_flags;
    ULARGE_INTEGER                                              app_compat_flags_user;
    T                                                           shim_data;
    T                                                           app_compat_info;
    unicode_string_t< T >                                       csd_version;
    T                                                           activation_context_data;
    T                                                           process_assembly_storage_map;
    T                                                           system_default_activation_context_data;
    T                                                           system_assembly_storage_map;
    T                                                           minimum_stack_commit;
    T                                                           fls_callback;
    list_entry_t< T >                                           fls_listHead;
    T                                                           fls_bitmap;
    uint32_t                                                    fls_bitmap_bits[ 4 ];
    uint32_t                                                    fls_high_index;
    T                                                           wer_registration_data;
    T                                                           wer_ship_assert_ptr;
    T                                                           unused;
    T                                                           image_header_hash;
    union
    {
        uint64_t                                                tracing_flags;
        struct
        {
            uint32_t                                            heap_tracing_enabled : 1;
            uint32_t                                            crit_sec_tracing_enabled : 1;
            uint32_t                                            lib_loader_tracing_enabled : 1;
            uint32_t                                            spare_tracing_bits : 29;
        };
    };
    T                                                           csr_server_read_only_shared_memory_base;
};

template < typename T >
struct peb_ldr_data_t
{
    uint32_t                                                    length;
    uint8_t                                                     initialized;
    T                                                           ss_handle;
    list_entry_t< T >                                           in_load_order_module_list;
    list_entry_t< T >                                           in_memory_order_module_list;
    list_entry_t< T >                                           in_initialization_order_module_list;
    T                                                           entry_in_progress;
    uint8_t                                                     shutdown_in_progress;
    T                                                           shutdown_thread_id;
};

template < typename T >
struct ldr_data_table_entry_t
{
    list_entry_t< T >                                           in_load_order_links;
    list_entry_t< T >                                           in_memory_order_links;
    list_entry_t< T >                                           in_initialization_order_links;
    T                                                           dll_base;
    T                                                           entrypoint;
    uint32_t                                                    size_of_image;
    unicode_string_t< T >                                       full_dll_name;
    unicode_string_t< T >                                       base_dll_name;
    uint32_t                                                    flags;
    uint16_t                                                    load_count;
    uint16_t                                                    tls_index;
    list_entry_t< T >                                           hash_links;
    uint32_t                                                    time_date_stamp;
    T                                                           entrypoint_activation_context;
    T                                                           patch_information;
};


using unicode_string_32                 = unicode_string_t< uint32_t >;
using unicode_string_64                 = unicode_string_t< uint64_t >;
using unicode_string                    = unicode_string_t< uintptr_t >;

using peb32                             = peb_t< uint32_t >;
using peb64                             = peb_t< uint64_t >;
using peb                               = peb_t< uintptr_t >;

using teb32                             = teb_t< uint32_t >;
using teb64                             = teb_t< uint64_t >;
using teb                               = teb_t< uintptr_t >;

using peb_ldr_data_32                   = peb_ldr_data_t< uint32_t >;
using peb_ldr_data_64                   = peb_ldr_data_t< uint64_t >;
using peb_ldr_data                      = peb_ldr_data_t< uintptr_t >;

using ldr_data_table_entry32            = ldr_data_table_entry_t< uint32_t >;
using ldr_data_table_entry64            = ldr_data_table_entry_t< uint64_t >;
using ldr_data_table_entry              = ldr_data_table_entry_t< uintptr_t >;