#pragma once

#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

#include <cstddef>
#include <ntdef.h>
#include <ntifs.h>
#include <ntstrsafe.h>
#include <intrin.h>

#ifdef _DEBUG
#define TRACE( fmt, ... )                   DbgPrintEx( DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, __VA_ARGS__ )
#define TRY                                 __try
#define EXCEPT( name )                      __except ( EXCEPTION_EXECUTE_HANDLER ) { TRACE( "Exceção na função %s, código: 0x%X.", name, GetExceptionCode() ); }
#else
#define TRACE( fmt, ... )
#define TRY
#define EXCEPT( name )
#endif

#include "../dependencies/hash/hash.hpp"
#include "../dependencies/hash/hash_n.hpp"
#include "../structs/pe.hpp"
#include "../structs/natives.hpp"
#include "../enums/system_information.hpp"
#include "../enums/request.hpp"
#include "../structs/request.hpp"
#include "../structs/modules.hpp"
#include "dyn_data.hpp"
#include "imports.hpp"
#include "../utils/misc.hpp"
#include "../utils/raii.hpp"
#include "requests.hpp"