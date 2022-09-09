#pragma once

class c_go_mapped_drv
{
    bool                        set_last_err(const uint32_t last_err, uint32_t last_win_err) noexcept;
    void                        reset_last_err(const std::string& last_fn_called) noexcept;
    void                        registry_request(uintptr_t ptr)  const noexcept;
public:

    using err_callback_t = std::function< void(const std::string& last_fn_called, uint32_t last_err, uint32_t last_win_err) >;

    enum errors_codes
    {
        FAIL_SUCCESS,

        FAIL_LOAD_DRIVER_GET_CURRENT_APP,
        FAIL_LOAD_DRIVER_GET_CURRENT_DIRECTORY,
        FAIL_LOAD_DRIVER_CURRENT_DIRECTORY_EMPTY,
        FAIL_LOAD_DRIVER_DRIVER_NOT_FOUND,
        FAIL_LOAD_DRIVER_OPEN_SC_MANAGER,
        FAIL_LOAD_DRIVER_OPEN_SERVICE,
        FAIL_LOAD_DRIVER_START_SERVICE,
        FAIL_UNLOAD_DRIVER_CONTROL_SERVICE,
        FAIL_UNLOAD_DRIVER_DELETE_SERVICE,
        FAIL_UNLOAD_DRIVER_CLOSE_SERVICE_HANDLE,
        FAIL_UNLOAD_DRIVER_CLOSE_SERVICE_MANAGER_HANDLE,
        FAIL_SEND_REQUEST_DRIVER_NOT_LOADED,
        FAIL_SEND_REQUEST_DRIVER_NOT_CONNECTED,


        FAIL_MAX
    };

    void                        set_error_callback(const err_callback_t& err_fn);
    uint32_t                    last_err() const noexcept;
    uint32_t                    last_win_err() const noexcept;
    std::string                 last_fn_called();
    bool                        load_driver(std::string directory, std::string name, uint32_t error_lvl);
    bool                        unload_driver();
    bool                        is_loaded() const noexcept;

    template < typename T >
    bool                        send_request(T& s_request) const noexcept;

    bool                        initialize(uint64_t& ntoskrnl) const noexcept;

    uint32_t                    allocate(uint32_t pid, uint64_t address, size_t size, uint32_t type, uint32_t protect, uint64_t* output) const noexcept;
    uint32_t                    free(uint32_t pid, uint64_t address, uint32_t type) const noexcept;
    uint32_t                    write(uint32_t pid, uint64_t address, uint8_t* buffer, size_t buffer_size, size_t* num_bytes_writted = nullptr) const noexcept;
    uint32_t                    read(uint32_t pid, uint64_t address, uint8_t* buffer, size_t buffer_siz, size_t* num_bytes_readed = nullptr) const noexcept;
private:
    err_callback_t              _err_callback = nullptr;
    std::string                 _last_fn_called;
    uint32_t                    _last_err = FAIL_SUCCESS;
    uint32_t                    _last_win_err = 0;
    SC_HANDLE                   _sc_manager = nullptr;
    SC_HANDLE                   _service = nullptr;
    SERVICE_STATUS              _service_status;
};

void c_go_mapped_drv::reset_last_err( const std::string& last_fn_called ) noexcept
{
    SetLastError(ERROR_SUCCESS);
    _last_err = FAIL_SUCCESS;
    _last_win_err = ERROR_SUCCESS;
    _last_fn_called = std::move(last_fn_called);
}

bool c_go_mapped_drv::set_last_err( const uint32_t last_err, const uint32_t last_win_err ) noexcept
{
    if (last_err != FAIL_SUCCESS)
        _last_err = last_err;

    if (last_win_err != ERROR_SUCCESS)
        _last_win_err = last_win_err;
    
    if (_err_callback)
        _err_callback(_last_fn_called, last_err, last_win_err);

    return false;
}

void c_go_mapped_drv::set_error_callback( const err_callback_t& err_fn )
{
    _err_callback = std::move( err_fn );
}

void c_go_mapped_drv::registry_request(uintptr_t ptr) const noexcept
{
    uint8_t buffer[0x1338] = { 0 };
    char addr_str[10] = { 0 };
    sprintf_s(addr_str, sizeof(uintptr_t) == 4 ? "0x%X" : "0x%016X", ptr);
    RegSetKeyValueA(HKEY_CURRENT_USER, "Software", addr_str, REG_SZ, buffer, sizeof(buffer) - 1);
}

uint32_t c_go_mapped_drv::last_err() const noexcept
{
    return _last_err;
}

uint32_t c_go_mapped_drv::last_win_err() const noexcept
{
    return _last_win_err;
}

std::string c_go_mapped_drv::last_fn_called()
{
    return _last_fn_called;
}

bool c_go_mapped_drv::load_driver(std::string directory, std::string name, const uint32_t error_lvl)
{
    reset_last_err("load_driver");

    if (!helpers::file::is_directory(directory))
    {
        char current_directory[MAX_PATH] = { 0 };
        auto current_app = GetModuleHandleA( nullptr );

        if (current_app == nullptr)
            return set_last_err(FAIL_LOAD_DRIVER_GET_CURRENT_APP, GetLastError());

        if (!GetModuleFileNameA(current_app, current_directory, MAX_PATH))
            return set_last_err(FAIL_LOAD_DRIVER_GET_CURRENT_DIRECTORY, GetLastError());

        if (current_directory[0] == '\0')
            return set_last_err(FAIL_LOAD_DRIVER_CURRENT_DIRECTORY_EMPTY, GetLastError());

        auto last_bar_directory = strrchr(current_directory, '\\');

        if (last_bar_directory)
            strncpy_s(current_directory, current_directory, (last_bar_directory - current_directory) + 1u);

        directory = current_directory + directory;

        if (!helpers::file::file_exist(directory))
            return set_last_err(FAIL_LOAD_DRIVER_DRIVER_NOT_FOUND, 0);
    }

    if ((_sc_manager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE)) == nullptr)
        return set_last_err(FAIL_LOAD_DRIVER_OPEN_SC_MANAGER, GetLastError());

    _service = CreateServiceA(_sc_manager,
                              name.c_str(),
                              name.c_str(),
                              SERVICE_START | DELETE | SERVICE_STOP,
                              SERVICE_KERNEL_DRIVER,
                              SERVICE_DEMAND_START,
                              error_lvl,
                              directory.c_str(),
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr,
                              nullptr);

    if (_service == nullptr && (_service = OpenServiceA(_sc_manager, name.c_str(), SERVICE_START | DELETE | SERVICE_STOP)) == nullptr)
        return set_last_err(FAIL_LOAD_DRIVER_OPEN_SERVICE, GetLastError());

    if (!StartServiceA(_service, 0, nullptr))
        return set_last_err(FAIL_LOAD_DRIVER_START_SERVICE, GetLastError());

    return true;
}

bool c_go_mapped_drv::unload_driver()
{
    reset_last_err("unload_driver");

    if (_sc_manager && _service)
    {
        if (!ControlService(_service, SERVICE_CONTROL_STOP, &_service_status))
            return set_last_err(FAIL_UNLOAD_DRIVER_CONTROL_SERVICE, GetLastError());

        if ( !DeleteService(_service))
            return set_last_err( FAIL_UNLOAD_DRIVER_DELETE_SERVICE, GetLastError());

        if ( !CloseServiceHandle(_service))
            return set_last_err(FAIL_UNLOAD_DRIVER_CLOSE_SERVICE_HANDLE, GetLastError());

        if ( !CloseServiceHandle(_sc_manager))
            return set_last_err(FAIL_UNLOAD_DRIVER_CLOSE_SERVICE_MANAGER_HANDLE, GetLastError());
    }
    return true;
}

bool c_go_mapped_drv::is_loaded() const noexcept
{
    return (_sc_manager != nullptr && _service != nullptr);
}

bool c_go_mapped_drv::initialize(uint64_t& ntoskrnl) const noexcept
{
    auto initialize = s_request_initialize{};

    if (!pdb_reader::get_offset(&initialize.kprocess_execopt, ntoskrnl, "_KPROCESS", "Flags"))
        throw std::runtime_error("Falha ao pegar a KPROCESS::Flags");

    if (!pdb_reader::get_offset(&initialize.eprocess_protection, ntoskrnl, "_EPROCESS", "Protection"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::Protection");

    if (!pdb_reader::get_offset(&initialize.eprocess_objtable, ntoskrnl, "_EPROCESS", "ObjectTable"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::ObjectTable");

    if (!pdb_reader::get_offset(&initialize.eprocess_vadroot, ntoskrnl, "_EPROCESS", "VadRoot"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::VadRoot");

    if (!pdb_reader::get_offset(&initialize.eprocess_uniqueprocessid, ntoskrnl, "_EPROCESS", "UniqueProcessId"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::UniqueProcessId");

    if (!pdb_reader::get_offset(&initialize.eprocess_ownerprocessid, ntoskrnl, "_EPROCESS", "OwnerProcessId"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::OwnerProcessId");

    if (!pdb_reader::get_offset(&initialize.eprocess_activeprocesslinks, ntoskrnl, "_EPROCESS", "ActiveProcessLinks"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::ActiveProcessLinks");

    if (!pdb_reader::get_offset(&initialize.eprocess_inheritedfromuniqueprocessid, ntoskrnl, "_EPROCESS", "InheritedFromUniqueProcessId"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::InheritedFromUniqueProcessId");

    if (!pdb_reader::get_offset(&initialize.eprocess_rotateinprogress, ntoskrnl, "_EPROCESS", "RotateInProgress"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::RotateInProgress");

    if (!pdb_reader::get_offset(&initialize.eprocess_forkinprogress, ntoskrnl, "_EPROCESS", "ForkInProgress"))
        throw std::runtime_error("Falha ao pegar a EPROCESS::ForkInProgress");

    if (!pdb_reader::get_offset(&initialize.zw_protect_virtual_memory, ntoskrnl, "ZwProtectVirtualMemory"))
        throw std::runtime_error("Falha ao pegar a ZwProtectVirtualMemory");

    if (!pdb_reader::get_offset(&initialize.zw_query_virtual_memory, ntoskrnl, "ZwQueryVirtualMemory"))
        throw std::runtime_error("Falha ao pegar a ZwQueryVirtualMemory");

    if (!pdb_reader::get_offset(&initialize.mm_allocate_independent_pages, ntoskrnl, "MmAllocateIndependentPages"))
        throw std::runtime_error("Falha ao pegar a MmAllocateIndependentPages");

    if (!pdb_reader::get_offset(&initialize.mm_free_independent_pages, ntoskrnl, "MmFreeIndependentPages"))
        throw std::runtime_error("Falha ao pegar a MmFreeIndependentPages");

    std::cout << "KPROCESS::Flags: 0x" << std::hex << std::uppercase << initialize.kprocess_execopt << std::endl;
    std::cout << "EPROCESS::Protection: 0x" << std::hex << std::uppercase << initialize.eprocess_protection << std::endl;
    std::cout << "EPROCESS::UniqueProcessId: 0x" << std::hex << std::uppercase << initialize.eprocess_uniqueprocessid << std::endl;
    std::cout << "EPROCESS::ObjTable: 0x" << std::hex << std::uppercase << initialize.eprocess_objtable << std::endl;
    std::cout << "EPROCESS::ActiveProcessLinks: 0x" << std::hex << std::uppercase << initialize.eprocess_activeprocesslinks << std::endl;
    std::cout << "EPROCESS::InheritedFromUniqueProcessId: 0x" << std::hex << std::uppercase << initialize.eprocess_inheritedfromuniqueprocessid << std::endl;
    std::cout << "EPROCESS::VadRoot: 0x" << std::hex << std::uppercase << initialize.eprocess_vadroot << std::endl;

    // Não exportadas no Win 7
    std::cout << "ZwQueryVirtualMemory: 0x" << std::hex << std::uppercase << initialize.zw_query_virtual_memory << std::endl;
    std::cout << "ZwProtectVirtualMemory: 0x" << std::hex << std::uppercase << initialize.zw_protect_virtual_memory << std::endl;

    // Não exportadas
    std::cout << "MmAllocateIndependentPages: 0x" << std::hex << std::uppercase << initialize.mm_allocate_independent_pages << std::endl;
    std::cout << "MmFreeIndependentPages: 0x" << std::hex << std::uppercase << initialize.mm_free_independent_pages << std::endl;

    return send_request(initialize) && initialize.success;
}

template < typename T >
bool c_go_mapped_drv::send_request(T& s_request) const noexcept
{
    constexpr auto extra_size = 10u;

    auto request_struct = std::make_unique< uint8_t[] >(sizeof(uint32_t) + sizeof(T) + extra_size);

    if (request_struct)
    {
        *reinterpret_cast<uint32_t*>(request_struct.get() + 0) = T::code;

        std::memcpy(reinterpret_cast<void*>(request_struct.get() + 4), reinterpret_cast<void*>(&s_request), sizeof(T));

        registry_request(reinterpret_cast<uintptr_t>(request_struct.get()));
        
        std::memcpy(reinterpret_cast<void*>(&s_request), reinterpret_cast<void*>(request_struct.get() + 4), sizeof(T));
        
        return true;
    }
    return false;
}

uint32_t c_go_mapped_drv::allocate(uint32_t pid, uint64_t address, size_t size, uint32_t type, uint32_t protect, uint64_t* output) const noexcept
{
    auto allocation_memory = s_request_allocate_memory{};

    allocation_memory.process_id = pid;
    allocation_memory.address = address;
    allocation_memory.state = type;
    allocation_memory.protect = protect;
    allocation_memory.size = size;

    if (send_request( allocation_memory ) && allocation_memory.error_code == 0)
    {
        if (output)
            *output = allocation_memory.address;

        return 0;
    }
    return allocation_memory.error_code;
}

uint32_t c_go_mapped_drv::free(uint32_t pid, uint64_t address, uint32_t type) const noexcept
{
    auto free_memory = s_request_free_memory{};

    free_memory.process_id = pid;
    free_memory.address = address;

    return (send_request(free_memory) && free_memory.error_code == 0) ? 0 : free_memory.error_code;
}

uint32_t c_go_mapped_drv::write(uint32_t pid, uint64_t address, uint8_t* buffer, size_t buffer_size, size_t * num_bytes_writted) const noexcept
{
    auto write_memory = s_request_write_memory{};

    write_memory.process_id = pid;
    write_memory.address = address;
    write_memory.buffer = reinterpret_cast<uint64_t>(buffer);
    write_memory.buffer_size = buffer_size;

    if (send_request(write_memory) && write_memory.error_code == 0 && write_memory.num_bytes_writted)
    {
        if (num_bytes_writted)
            *num_bytes_writted = write_memory.num_bytes_writted;

        return 0;
    }
    return write_memory.error_code;
}

uint32_t c_go_mapped_drv::read(uint32_t pid, uint64_t address, uint8_t* buffer, size_t buffer_size, size_t* num_bytes_readed) const noexcept
{
    auto read_memory = s_request_read_memory{};

    read_memory.process_id = pid;
    read_memory.address = address;
    read_memory.buffer = reinterpret_cast<uint64_t>(buffer);
    read_memory.buffer_size = buffer_size;

    if (send_request(read_memory) && read_memory.error_code == 0 && read_memory.num_bytes_readed)
    {
        if (num_bytes_readed)
            *num_bytes_readed = read_memory.num_bytes_readed;

        return 0;
    }
    return read_memory.error_code;
}