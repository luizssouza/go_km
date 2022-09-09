#pragma once

namespace utils
{
    ULONG_PTR get_export_address(const wchar_t* name)
    {
        auto routine_name = UNICODE_STRING{};
        RtlUnicodeStringInit(&routine_name, name);
        return reinterpret_cast<ULONG_PTR>(MmGetSystemRoutineAddress(&routine_name));
    }

    NTSTATUS get_module(size_t name_hash, uintptr_t* base_address, size_t* size = nullptr)
    {
        auto extract_name = [](unsigned char* name) -> unsigned char*
        {
            if (name == nullptr || name[0] == '\0')
                return nullptr;

            auto len = 0u;

            while (name[len] != '\0')
                len++;
            
            for (auto i = len; i--; )
            {
                if (name[i] == '\\')
                    return reinterpret_cast<unsigned char*>(name + i + 1);
            }
            return nullptr;
        };

        auto bytes = ULONG{};
        auto nt_status = STATUS_SUCCESS;

        ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

        if (bytes)
        {
            auto mods = reinterpret_cast<PRTL_PROCESS_MODULES>(ExAllocatePool(NonPagedPool, bytes));

            if (mods)
            {
                RtlZeroMemory(mods, bytes);

                auto zw_query_info_status = ZwQuerySystemInformation(SystemModuleInformation, mods, bytes, &bytes);

                if (NT_SUCCESS(zw_query_info_status))
                {
                    auto mod = mods->Modules;

                    for (auto i = 0ul; i < mods->NumberOfModules; i++)
                    {
                        auto image_base = reinterpret_cast<ULONG_PTR>(mod[i].ImageBase);
                        auto image_size = static_cast<SIZE_T>(mod[i].ImageSize);

                        auto driver_name = extract_name(mod[i].FullPathName);

                        if (driver_name && rt_hashl(driver_name) == name_hash)
                        {
                            if (base_address)
                                *base_address = image_base;

                            if (size)
                                *size = image_size;

                            break;
                        }
                    }
                }
                else
                {
                    nt_status = zw_query_info_status;
                }

                ExFreePool(mods);
            }
            else
            {
                nt_status = STATUS_NO_MEMORY;
            }
        }
        else
        {
            nt_status = STATUS_INVALID_HANDLE;
        }
        return nt_status;
    }

    NTSTATUS get_ntoskrnl(uintptr_t* base_address, size_t* size = nullptr)
    {
        auto bytes = ULONG{};
        auto nt_status = STATUS_SUCCESS;

        ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

        if (bytes)
        {
            auto mods = reinterpret_cast<PRTL_PROCESS_MODULES>(ExAllocatePool(NonPagedPool, bytes));

            if (mods)
            {
                RtlZeroMemory(mods, bytes);

                auto ptr = get_export_address(L"NtOpenFile");

                if ( ptr )
                {
                    auto zw_query_info_status = ZwQuerySystemInformation(SystemModuleInformation, mods, bytes, &bytes);

                    if (NT_SUCCESS(zw_query_info_status))
                    {
                        auto mod = mods->Modules;

                        for (auto i = 0ul; i < mods->NumberOfModules; i++)
                        {
                            auto image_base = reinterpret_cast<uintptr_t>(mod[i].ImageBase);
                            auto image_size = static_cast<size_t>(mod[i].ImageSize);

                            if (ptr >= image_base && ptr < image_base + image_size)
                            {
                                if (base_address)
                                    *base_address = image_base;

                                if (size)
                                    *size = image_size;

                                break;
                            }
                        }
                    }
                    else
                        nt_status = zw_query_info_status;
                }
                else
                    nt_status = STATUS_ORDINAL_NOT_FOUND;

                ExFreePool( mods );
            }
            else
                nt_status = STATUS_NO_MEMORY;
        }
        else
            nt_status = STATUS_INVALID_HANDLE;

        return nt_status;
    }

    uintptr_t get_exported_function(uintptr_t base_address, const char* export_name)
    {
        return base_address ? reinterpret_cast<uintptr_t>(RtlFindExportedRoutineByName(reinterpret_cast<PVOID>(base_address), export_name)) : 0;
    }

    bool is_process_terminating(PEPROCESS pe_process)
    {
        auto zero_time = LARGE_INTEGER{0};
        return KeWaitForSingleObject(pe_process, Executive, KernelMode, FALSE, &zero_time) == STATUS_WAIT_0;
    }

    ANSI_STRING init_ansi_string(const char* ansistring)
    {
        auto output = ANSI_STRING{};
        RtlInitAnsiString(&output, ansistring);
        return output;
    }

    UNICODE_STRING init_unicode_string(const wchar_t* unicodestring)
    {
        auto output = UNICODE_STRING{};
        RtlInitUnicodeString(&output, unicodestring);
        return output;
    }

    bool cmp_ansi_string(const ANSI_STRING& ansi_str_1, const ANSI_STRING& ansi_str_2, bool case_sensitive = true)
    {
        return RtlCompareString(&ansi_str_1, &ansi_str_2, case_sensitive) == 0;
    }

    bool cmp_unicode_string(const UNICODE_STRING& unicode_str_1, const UNICODE_STRING& unicode_str_2, bool case_sensitive = true)
    {
        return RtlCompareUnicodeString(&unicode_str_1, &unicode_str_2, case_sensitive) == 0;
    }

    void sleep(ULONG milliseconds)
    {
        auto li = LARGE_INTEGER{};
        li.QuadPart = Int32x32To64(milliseconds * 1000, -10);
        KeDelayExecutionThread(KernelMode, FALSE, &li);
    }
}