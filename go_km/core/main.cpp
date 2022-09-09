#include "go.hpp"

unsigned long wcstoul(const wchar_t* str, wchar_t** str_end, int base)
{
    static auto fn_wcstoul = reinterpret_cast<unsigned long(__stdcall*)(const wchar_t* str, wchar_t** str_end, int base)>(utils::get_export_address(L"wcstoul"));
    return fn_wcstoul ? fn_wcstoul(str, str_end, base) : 0;
}

namespace registry
{

bool            success = false;
LARGE_INTEGER   cookie{};

NTSTATUS callback(PVOID CallbackContext, PVOID Argument1, PVOID Argument2)
{
    UNREFERENCED_PARAMETER(CallbackContext);

    auto operation = static_cast<REG_NOTIFY_CLASS>(reinterpret_cast<uintptr_t>(Argument1));
    
    if (operation == RegNtSetValueKey)
    {
        auto key = static_cast<PREG_SET_VALUE_KEY_INFORMATION>(Argument2);

        if (key && key->ValueName && key->ValueName->Buffer && key->ValueName->Length && (key->DataSize / 2) == 0x1337) // Sanity Check
        {
            auto addr = wcstoul(key->ValueName->Buffer, nullptr, 16);

            if (addr > 0x1000) // Sanity Check
            {
                auto code = *reinterpret_cast<ULONG*>(addr + 0);

                if (code > REQUEST_CODES::CODE_MIN && code < REQUEST_CODES::CODE_MAX) // Sanity Check
                {
                    TRACE("[GO] Request Code: 0x%X.", code);
                    auto current_process = PsGetCurrentProcess();

                    if (!utils::is_process_terminating(current_process)) // Sanity Check
                    {
                        auto stack_process = c_stack_attach_process{ current_process };

                        switch (code)
                        {
                            case _REQUEST_CODE_INITIALIZE::code:
                            {
                                TRACE("[GO] Request: STATUS.");
                                requests::initialize(reinterpret_cast<PREQUEST_CODE_INITIALIZE>(addr + 4));
                                break;
                            }
                            case _REQUEST_ALLOCATE_MEMORY::code:
                            {
                                TRACE("[GO] Request: ALLOCATE.");
                                requests::allocate_memory(current_process, stack_process, reinterpret_cast<PREQUEST_ALLOCATE_MEMORY>(addr + 4));
                                break;
                            }
                            case _REQUEST_WRITE_MEMORY::code:
                            {
                                TRACE("[GO] Request: WRITE.");
                                requests::write(current_process, reinterpret_cast<PREQUEST_WRITE_MEMORY>(addr + 4));
                                break;
                            }
                            case _REQUEST_READ_MEMORY::code:
                            {
                                TRACE("[GO] Request: READ.");
                                requests::read(current_process, reinterpret_cast<PREQUEST_READ_MEMORY>(addr + 4));
                                break;
                            }
                            case _REQUEST_FREE_MEMORY::code:
                            {
                                TRACE("[GO] Request: FREE.");
                                requests::free(current_process, stack_process, reinterpret_cast<PREQUEST_FREE_MEMORY>(addr + 4));
                                break;
                            }
                            case _REQUEST_CHANGE_MEMORY_PROTECTION::code:
                            {
                                TRACE("[GO] Request: CHANGE MEMORY PROTECTION.");
                                requests::change_protection(current_process, stack_process, reinterpret_cast<PREQUEST_CHANGE_MEMORY_PROTECTION>(addr + 4));
                                break;
                            }
                            default:
                            {
                                TRACE("[GO] Request: INVALID.");
                            }
                        }
                    }
                }
            }
        }
    }
    return STATUS_SUCCESS;
}

} // registry

extern "C" void DriverUnload(_In_ PDRIVER_OBJECT driver_object)
{
    UNREFERENCED_PARAMETER(driver_object);

    if (registry::success)
        CmUnRegisterCallback(registry::cookie);

    TRACE("[GO] Driver descarregado com sucesso.");
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driver_object, _In_ PUNICODE_STRING registry_path)
{
    UNREFERENCED_PARAMETER(registry_path);

    if (driver_object)
        driver_object->DriverUnload = DriverUnload;

    TRACE("[GO] Driver inicializado.");

    auto ntoskrnl_status = utils::get_ntoskrnl(&dyn_data::ntoskrnl);
    
    if (NT_SUCCESS(ntoskrnl_status))
    {
       TRACE("[GO] ntoskrnl.sys: 0x%p.", dyn_data::ntoskrnl);
   
       UNICODE_STRING altitude_string = RTL_CONSTANT_STRING(L"360000");
       auto register_status = CmRegisterCallbackEx(registry::callback, &altitude_string, reinterpret_cast<PVOID>(0xDEADBEEF), nullptr, &registry::cookie, nullptr);
    
       if (NT_SUCCESS(register_status))
       {
           registry::success = true;
           TRACE("[GO] Callback registrado com sucesso.");
       }
       else
       {
           registry::success = false;
           TRACE("[GO] Falha registrar o callback no registro, código: 0x%X.", register_status);
       }
    }
    else
    {
        TRACE("[GO] Falha ao pegar o BaseAddress da ntoskrnl, código: 0x%X.", ntoskrnl_status);
    }
    return STATUS_SUCCESS;
}