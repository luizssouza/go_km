#pragma once

namespace requests
{

void initialize(PREQUEST_CODE_INITIALIZE r)
{
    dyn_data::mm_allocate_independent_pages = r->mm_allocate_independent_pages;
    dyn_data::mm_free_independent_pages = r->mm_free_independent_pages;
    dyn_data::zw_protect_virtual_memory = r->zw_protect_virtual_memory;
    dyn_data::zw_query_virtual_memory = r->zw_query_virtual_memory;

    dyn_data::kprocess_execopt = r->kprocess_execopt;
    dyn_data::eprocess_protection = r->eprocess_protection;
    dyn_data::eprocess_objtable = r->eprocess_objtable;
    dyn_data::eprocess_vadroot = r->eprocess_vadroot;
    dyn_data::eprocess_uniqueprocessid = r->eprocess_uniqueprocessid;
    dyn_data::eprocess_ownerprocessid = r->eprocess_ownerprocessid;
    dyn_data::eprocess_activeprocesslinks = r->eprocess_activeprocesslinks;
    dyn_data::eprocess_inheritedfromuniqueprocessid = r->eprocess_inheritedfromuniqueprocessid;
    dyn_data::eprocess_rotateinprogress = r->eprocess_rotateinprogress;
    dyn_data::eprocess_forkinprogress = r->eprocess_forkinprogress;

    r->success = true;
}

void allocate_memory(PEPROCESS current_process, c_stack_attach_process& stack_process, PREQUEST_ALLOCATE_MEMORY r)
{
    auto address = reinterpret_cast<PVOID>(r->address);
    auto size = static_cast<SIZE_T>(r->size);
    auto state = r->state;
    auto protect = r->protect;
    auto process_attach = c_stack_attach_process{};
    auto error_code = STATUS_SUCCESS;

    auto pe_process = c_pe_process{ r->process_id };

    if (pe_process.success())
    {
        if (!utils::is_process_terminating(pe_process.get_pe_process()))
        {
            stack_process.detach();
            process_attach.attach(pe_process.get_pe_process());

            error_code = ZwAllocateVirtualMemory(ZwCurrentProcess(), &address, 0, &size, state, protect);

            process_attach.detach();
            stack_process.attach(current_process);

            r->address = (ULONG_PTR)address;
            r->error_code = NT_SUCCESS(error_code) ? 0 : error_code;
        }
        else
        {
            r->address = 0;
            r->error_code = STATUS_PROCESS_IS_TERMINATING;
        }
    }
    else
    {
        r->address = 0;
        r->error_code = pe_process.status();
    }
}

void write(PEPROCESS current_process, PREQUEST_WRITE_MEMORY r)
{
    auto address = reinterpret_cast<PVOID>(r->address);
    auto size_buffer = r->buffer_size;
    auto buffer = reinterpret_cast<PVOID>(r->buffer);
    auto num_bytes_writted = SIZE_T{};

    auto pe_process = c_pe_process{ r->process_id };

    if (pe_process.success())
    {
        if (!utils::is_process_terminating(pe_process.get_pe_process()))
        {
            auto error_code = MmCopyVirtualMemory(PsGetCurrentProcess(), buffer, pe_process.get_pe_process(), address, size_buffer, KernelMode, &num_bytes_writted);

            r->num_bytes_writted = num_bytes_writted;
            r->error_code = NT_SUCCESS(error_code) ? 0 : error_code;
        }
        else
        {
            r->num_bytes_writted = 0;
            r->error_code = STATUS_PROCESS_IS_TERMINATING;
        }
    }
    else
    {
        r->num_bytes_writted = 0;
        r->error_code = pe_process.status();
    }
}

void read(PEPROCESS current_process, PREQUEST_READ_MEMORY r)
{
    auto address = reinterpret_cast<PVOID>(r->address);
    auto size = static_cast<SIZE_T>(r->buffer_size);
    auto buffer = reinterpret_cast<PVOID>(r->buffer);
    auto num_bytes_readed = SIZE_T{};
    auto pe_process = c_pe_process{ r->process_id };

    if (pe_process.success())
    {
        if (!utils::is_process_terminating(pe_process.get_pe_process()))
        {
            auto error_code = MmCopyVirtualMemory(pe_process.get_pe_process(), address, PsGetCurrentProcess(), buffer, size, KernelMode, &num_bytes_readed);

            r->num_bytes_readed = num_bytes_readed;
            r->error_code = NT_SUCCESS(error_code) ? 0 : error_code;
        }
        else
        {
            r->num_bytes_readed = 0;
            r->error_code = STATUS_PROCESS_IS_TERMINATING;
        }
    }
    else
    {
        r->num_bytes_readed = 0;
        r->error_code = pe_process.status();
    }
}

void free(PEPROCESS current_process, c_stack_attach_process& stack_process, PREQUEST_FREE_MEMORY r)
{
    auto address = reinterpret_cast<PVOID>(r->address);
    auto error_code = ULONG{};
    auto process_attach = c_stack_attach_process{};
    auto size = SIZE_T{};
    auto pe_process = c_pe_process{ r->process_id };

    if (pe_process.success())
    {
        if (!utils::is_process_terminating(pe_process.get_pe_process()))
        {
            stack_process.detach();

            process_attach.attach(pe_process.get_pe_process());
            error_code = ZwFreeVirtualMemory(ZwCurrentProcess(), &address, &size, MEM_RELEASE);
            process_attach.detach();

            stack_process.attach(current_process);

            r->error_code = NT_SUCCESS(error_code) ? 0 : error_code;
        }
        else
        {
            r->error_code = STATUS_PROCESS_IS_TERMINATING;
        }
    }
    else
    {
        r->error_code = pe_process.status();
    }
}

void change_protection(PEPROCESS current_process, c_stack_attach_process& stack_process, PREQUEST_CHANGE_MEMORY_PROTECTION r)
{
    auto address = reinterpret_cast<PVOID>(r->address);
    auto error_code = ULONG{};
    auto process_attach = c_stack_attach_process{};
    auto new_protection = r->new_protection;
    auto old_protection = ULONG{};
    auto size = (SIZE_T)r->size;
    auto pe_process = c_pe_process{ r->process_id };

    if (pe_process.success())
    {
        if (!utils::is_process_terminating(pe_process.get_pe_process()))
        {
            stack_process.detach();

            process_attach.attach(pe_process.get_pe_process());
            error_code = ZwProtectVirtualMemory(ZwCurrentProcess(), &address, &size, new_protection, &old_protection);
            process_attach.detach();

            stack_process.attach(current_process);

            r->old_protection = old_protection;
            r->error_code = NT_SUCCESS(error_code) ? 0 : error_code;
        }
        else
        {
            r->error_code = STATUS_PROCESS_IS_TERMINATING;
        }
    }
    else
    {
        r->error_code = pe_process.status();
    }
}

} // requests