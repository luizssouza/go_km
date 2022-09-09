#define _NO_CVCONST_H

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <array>
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include <functional>
#include <fstream>
#include <limits>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp")

#include "../win/structs.hpp"
#include "../helpers/file.hpp"
#include "../helpers/pdb_reader.hpp"
#include "../go_mapped_drv/go_mapped_drv_request.hpp"
#include "../go_mapped_drv/go_mapped_drv.hpp"

c_go_mapped_drv                go_mapped_drv;

int32_t main( )
{
    setlocale( LC_ALL, "Portuguese" );

    go_mapped_drv.set_error_callback( [&]( const std::string& last_fn, const uint32_t last_err, const uint32_t last_win_err )
    {
        std::cout << "[go_drv] Aconteceu um erro, na função: " << last_fn << std::endl;

        if (last_err != c_go_mapped_drv::FAIL_SUCCESS)
            std::cout << "[go_drv] last_err: " << last_err << std::endl;

        if (last_win_err != ERROR_SUCCESS)
            std::cout << "[go_drv] last_win_err: " << last_win_err << std::endl;

        Sleep(10000);

        go_mapped_drv.unload_driver();

        exit(0);
    } );

    try
    {
        if (!pdb_reader::initialize())
            throw std::runtime_error("Falha ao inicializar o PDB.");

        go_mapped_drv.load_driver("go_km.sys", "GO", SERVICE_ERROR_NORMAL);

        uint64_t ntoskrnl = 0;

        if (!pdb_reader::load_module("C:\\Windows\\System32\\ntoskrnl.exe", true, &ntoskrnl))
            throw std::runtime_error("Falha ao carregar a ntoskrnl.exe");

        if (go_mapped_drv.initialize(ntoskrnl))
        {
            auto pid = uint32_t{};
            auto allocated_memory = uint64_t{};

            std::cout << "Insira o PID: ";
            std::cin >> pid;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Memory allocation
            {
                auto err_code = go_mapped_drv.allocate(pid, 0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE, &allocated_memory);

                if (err_code)
                    throw std::runtime_error("Falha ao alocar a memória no processo, código: " + std::to_string(err_code));

                std::cout << "Memória alocada no processo: 0x" << std::hex << std::uppercase << allocated_memory << std::endl;
            }

            // Memory write
            {
                if (allocated_memory)
                {
                    uint8_t write_buffer[7] = { 0xCC, 0xDE, 0xAD, 0xBE, 0xEF, 0xCC, 0x90 };

                    auto err_code = go_mapped_drv.write(pid, allocated_memory, write_buffer, sizeof(write_buffer));

                    if (err_code)
                        throw std::runtime_error("Falha ao escrever no processo, código: " + std::to_string(err_code));
                }
            }

            // Memory read
            {
                if (allocated_memory)
                {
                    uint8_t read_buffer[7] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
                    auto num_bytes_readed = size_t{};

                    auto err_code = go_mapped_drv.read(pid, allocated_memory, read_buffer, sizeof(read_buffer), &num_bytes_readed);

                    if (err_code)
                        throw std::runtime_error("Falha ao ler o processo, código: " + std::to_string(err_code));
                    
                    if (num_bytes_readed)
                    {
                        for (auto i = 0u; i < num_bytes_readed; i++)
                            std::cout << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<size_t>(read_buffer[i]) << " ";

                        std::cout << std::endl;
                    }
                }
            }
        }
    }
    catch (std::runtime_error err)
    {
        MessageBoxA(nullptr, err.what(), "Erro de execução!", MB_ICONERROR);
    }

    std::cout << "Pressione enter para sair e descarregar o driver." << std::endl;
    std::cin.get();

    go_mapped_drv.unload_driver( );

    return 1;
}