#pragma once

class pdb_reader
{
private:
    enum BasicType : size_t
    {
        btNoType = 0,
        btVoid = 1,
        btChar = 2,
        btWChar = 3,
        btInt = 6,
        btUInt = 7,
        btFloat = 8,
        btBCD = 9,
        btBool = 10,
        btLong = 13,
        btULong = 14,
        btCurrency = 25,
        btDate = 26,
        btVariant = 27,
        btComplex = 28,
        btBit = 29,
        btBSTR = 30,
        btHresult = 31
    };
public:
    struct sym_child_entry_t
    {
        std::string                     name;
        std::string                     type_name;
        uint64_t                        elements_count = 0;
        uint64_t                        size = 0;
        uint32_t                        offset = 0;
        int32_t                         is_bit_field = 0;
        uint32_t                        bit_position = 0;
    };

    struct sym_info_t
    {
        std::string                     name;
        uint64_t                        size = 0;
        uint32_t                        offset = 0;
        std::vector<sym_child_entry_t>  entries;
    };

    static bool                         initialize();
    static void                         shutdown();

    static bool                         load_module(const char* module_path, bool disable_wow64_redirection, uint64_t* module_Base);
    static bool                         dump_symbol(uint64_t& module_base, const char* symbol_name, sym_info_t& sym_info);
    static bool                         get_offset(uint32_t* output, uint64_t& module_base, const char* symbol_name, const char* member_name = nullptr);

private:
    static inline int32_t               last_status = 0;

    static std::string                  get_name(uint64_t& module_base, uint32_t index);
    static std::string                  get_type_name(uint64_t& module_base, uint32_t index, uint64_t* base_type_size = nullptr);
    static uint64_t                     get_size(uint64_t& module_base, uint32_t index);
    static uint32_t                     get_offset(uint64_t& module_base, uint32_t index);
    static uint32_t                     get_address_offset(uint64_t& module_base, uint32_t index);
    static uint32_t                     get_bit_position(uint64_t& module_base, uint32_t index);
    static uint32_t                     get_typeid(uint64_t& module_base, uint32_t index);
    static uint32_t                     get_array_typeid(uint64_t& module_base, uint32_t index);
    static size_t                       get_tag(uint64_t& module_base, uint32_t index);
    static size_t                       get_type(uint64_t& module_base, uint32_t index);
    static size_t                       get_base_type(uint64_t& module_base, uint32_t index);
};

bool pdb_reader::initialize()
{
    constexpr auto symbols_path = std::string_view("srv*%TEMP%*https://msdl.microsoft.com/download/symbols");
    return LoadLibraryA("dbghelp.dll") != nullptr && LoadLibraryA("symsrv.dll") != nullptr &&
        SymInitialize(GetCurrentProcess(), symbols_path.data(), 0) != 0;
}

void pdb_reader::shutdown()
{
    SymCleanup(GetCurrentProcess());
}

bool pdb_reader::load_module(const char* module_path, bool disable_wow64_redirection, uint64_t* module_base)
{
    if (module_base == nullptr)
        return false;

    if (sizeof(uintptr_t) == 4 && disable_wow64_redirection)
    {
        PVOID old = nullptr;
        Wow64DisableWow64FsRedirection(&old);

        *module_base = SymLoadModuleEx(GetCurrentProcess(), 0, module_path, 0, 0, 0, 0, 0);

        Wow64RevertWow64FsRedirection(old);
    }
    else
    {
        *module_base = SymLoadModuleEx(GetCurrentProcess(), 0, module_path, 0, 0, 0, 0, 0);
    }
    return *module_base != 0;
}

bool pdb_reader::get_offset(uint32_t* output, uint64_t& module_base, const char* symbol_name, const char* member_name)
{
    if (output == nullptr)
        return false;

    sym_info_t info = {};

    dump_symbol(module_base, symbol_name, info);

    if (member_name)
    {
        for (const auto& entry : info.entries)
        {
            if (entry.name == member_name)
            {
                *output = entry.offset;
                return true;
            }
        }
    }
   
    if (info.offset)
    {
        *output = info.offset;
        return true;
    }
    return false;
}

std::string pdb_reader::get_name(uint64_t& module_base, uint32_t index)
{
    wchar_t* name = nullptr;
    if (SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_SYMNAME, &name) && name)
    {
        std::string symname{ name, name + wcslen(name) };
        LocalFree(name);
        return symname;
    }
    return std::string{};
}

std::string pdb_reader::get_type_name(uint64_t& module_base, uint32_t index, uint64_t* base_type_size)
{
    if (!index)
        return std::string{};

    auto size = get_size(module_base, index);

    if (base_type_size)
        * base_type_size = size;

    auto name = get_name(module_base, index);

    if (!name.empty())
        return name;

    auto tag = get_tag(module_base, index);
    switch (tag)
    {
        case SymTagBaseType:
        {
            auto type = get_base_type(module_base, index);
            switch (type)
            {
                case btNoType:
                    name = "NO_TYPE";
                    break;
                case btVoid:
                    name = "VOID";
                    break;
                case btChar:
                    name = "CHAR";
                    break;
                case btWChar:
                    name = "WCHAR";
                    break;
                case btInt:
                    name = size == sizeof(int64_t) ? "INT64" : "INT";
                    break;
                case btUInt:
                    name = size == sizeof(uint64_t) ? "UINT64" : "UINT";
                    break;
                case btFloat:
                    name = "float";
                    break;
                case btBCD:
                    name = "BCD"; // Binary-coded decimal
                    break;
                case btBool:
                    name = "BOOL";
                    break;
                case btLong:
                    name = size == sizeof(uint64_t) ? "LONGLONG" : "LONG";
                    break;
                case btULong:
                    name = size == sizeof(uint64_t) ? "ULONGLONG" : "ULONG";
                    break;
                case btCurrency:
                    name = "CurrencyType"; // ???
                    break;
                case btDate:
                    name = "DateType"; // ???
                    break;
                case btVariant:
                    name = "VariantType"; // ???
                    break;
                case btComplex:
                    name = "ComplexType"; // ???
                    break;
                case btBit:
                    name = "Bit";
                    break;
                case btBSTR:
                    name = "BSTR"; // Binary string
                    break;
                case btHresult:
                    name = "HRESULT";
                    break;
            }
            break;
        }
        case SymTagPointerType:
        {
            name = get_type_name(module_base, get_type(module_base, index), base_type_size) + "*";
            break;
        }
        case SymTagArrayType:
        {
            name = get_type_name(module_base, get_array_typeid(module_base, index), base_type_size);
            break;
        }
        default:
        {
            name = get_type_name(module_base, get_type(module_base, index), base_type_size);
        }
    }
    return name;
}

uint64_t pdb_reader::get_size(uint64_t& module_base, uint32_t index)
{
    uint64_t size = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_LENGTH, &size);
    return size;
}

uint32_t pdb_reader::get_offset(uint64_t& module_base, uint32_t index)
{
    uint32_t offset = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_OFFSET, &offset);
    return offset;
}

uint32_t pdb_reader::get_address_offset(uint64_t& module_base, uint32_t index)
{
    uint32_t offset = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_ADDRESSOFFSET, &offset);
    return offset;
}

uint32_t pdb_reader::get_bit_position(uint64_t& module_base, uint32_t index)
{
    uint32_t bit_position = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_BITPOSITION, &bit_position);
    return bit_position;
}

uint32_t pdb_reader::get_typeid(uint64_t& module_base, uint32_t index)
{
    uint32_t type_id = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_TYPEID, &type_id);
    return type_id;
}

uint32_t pdb_reader::get_array_typeid(uint64_t& module_base, uint32_t index)
{
    uint32_t type_id = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_ARRAYINDEXTYPEID, &type_id);
    return type_id;
}

size_t pdb_reader::get_tag(uint64_t& module_base, uint32_t index)
{
    uint32_t tag = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_SYMTAG, &tag);
    return tag;
}

size_t pdb_reader::get_type(uint64_t& module_base, uint32_t index)
{
    uint32_t type = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_TYPE, &type);
    return type;
}

size_t pdb_reader::get_base_type(uint64_t& module_base, uint32_t index)
{
    uint32_t base_type = 0;
    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, index, TI_GET_BASETYPE, &base_type);
    return base_type;
}

bool pdb_reader::dump_symbol(uint64_t& module_base, const char* symbol_name, sym_info_t& sym_info)
{
    uint8_t root_symbol_info_buffer[sizeof(SYMBOL_INFO) + 0x1000] = { 0 };

    auto root_symbol_info = reinterpret_cast<PSYMBOL_INFO>(&root_symbol_info_buffer[0]);

    root_symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
    root_symbol_info->MaxNameLen = MAX_SYM_NAME;

    last_status = SymGetTypeFromName(GetCurrentProcess(), module_base, symbol_name, root_symbol_info);

    if (!last_status)
        return false;

    auto root_index = root_symbol_info->Index;

    sym_info.name = get_name(module_base, root_index);
    sym_info.size = get_size(module_base, root_index);
    sym_info.offset = get_offset(module_base, root_index);

    if (!last_status)
        sym_info.offset = get_address_offset(module_base, root_index);

    uint32_t children_count = 0;

    last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, root_index, TI_GET_CHILDRENCOUNT, &children_count);

    if (!last_status)
        return false;

    sym_info.name = symbol_name;

    if (!SymGetTypeInfo(GetCurrentProcess(), module_base, root_index, TI_GET_LENGTH, &sym_info.size))
        return false;

    if (children_count)
    {
        std::vector<BYTE> FindChildrenParamsBuffer(sizeof(TI_FINDCHILDREN_PARAMS) + children_count * sizeof(uint32_t));

        auto Children = reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(&FindChildrenParamsBuffer[0]);

        Children->Count = children_count;

        last_status = SymGetTypeInfo(GetCurrentProcess(), module_base, root_index, TI_FINDCHILDREN, Children);

        if (!last_status)
            return false;

        for (size_t i = 0; i < children_count; i++)
        {
            sym_child_entry_t entry{};

            auto child_index = Children->ChildId[i];
            auto type_id = get_typeid(module_base, child_index);

            entry.name = get_name(module_base, child_index);
            entry.size = get_size(module_base, type_id);
            entry.offset = get_offset(module_base, child_index);
            entry.bit_position = get_bit_position(module_base, child_index);
            entry.is_bit_field = last_status;

            uint64_t base_type_size = 0;

            entry.type_name = get_type_name(module_base, type_id, &base_type_size);
            entry.elements_count = base_type_size != 0 ? entry.size / base_type_size : 1;

            sym_info.entries.emplace_back(entry);
        }
    }
    return true;
}