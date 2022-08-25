#include "load_library.hpp"

#include <filesystem>
#include <Windows.h>

bool load_library_injection(const std::string &process,
                            const std::string &dll) {
    const auto process_id = utils::get_process_id_by_name(process);
    if (process_id == -1) {
        std::cout << "Failed to find ID for " << process << std::endl;
        return false;
    }

    char dll_path[MAX_PATH] = {};
    if (GetFullPathNameA(dll.c_str(), MAX_PATH, dll_path, nullptr) == 0) {
        std::cout << "Failed to get full file path for " << dll << std::endl;
        return false;
    }

    if (!std::filesystem::exists(dll.c_str())) {
        std::cout << dll << " does not exist" << std::endl;
        return false;
    }

    auto *const process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
    if (process_handle == nullptr) {
        std::cout << "Failed to open process handle for " << process
                  << std::endl;
        return false;
    }

    auto *const base_address =
        VirtualAllocEx(process_handle, nullptr, MAX_PATH,
                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base_address == nullptr) {
        std::cout << "Could not allocate memory for DLL in " << process
                  << std::endl;
        return false;
    }

    // Write the path into the memory region allocated earlier
    if (WriteProcessMemory(process_handle, base_address, dll_path, MAX_PATH,
                           nullptr) == 0) {
        std::cout << "Could not write to allocated memory region in " << process
                  << std::endl;
        return false;
    }

    // Create a remote thread within the process; that will call LoadLibraryA
    // as the start routine, with the parameter that was written in memory.
    auto *const thread_handle = CreateRemoteThread(
        process_handle, nullptr, 0, LPTHREAD_START_ROUTINE(LoadLibraryA),
        base_address, 0, nullptr);

    if (thread_handle == nullptr) {
        std::cout << "Could not create remote thread in " << process
                  << std::endl;
        return false;
    }

    CloseHandle(process_handle);
    VirtualFreeEx(process_handle, base_address, 0, MEM_RELEASE);

    return true;
}
