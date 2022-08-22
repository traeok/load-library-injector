#include "load_library.hpp"

#include <Windows.h>

bool load_library_injection(const std::string &process_name,
                            const std::string &dll_name) {
    const auto process_id = utils::get_process_id_by_name(process_name);
    if (process_id == -1) {
        std::cout << "Failed to find ID for " << process_name << std::endl;
        return false;
    }

    char dll[MAX_PATH] = {};
    if (GetFullPathNameA(dll_name.c_str(), MAX_PATH, dll, nullptr) == 0) {
        std::cout << "Failed to get full file path for " << dll_name
                  << std::endl;
        return false;
    }

    auto *const process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
    if (process_handle == nullptr) {
        std::cout << "Failed to open process handle for " << process_name
                  << std::endl;
        return false;
    }

    // Allocate memory for the DLL filepath
    auto *const base_address =
        VirtualAllocEx(process_handle, nullptr, MAX_PATH,
                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base_address == nullptr) {
        std::cout << "Could not allocate memory for DLL in " << process_name
                  << std::endl;
        return false;
    }

    // Write the path into the memory region allocated earlier
    if (WriteProcessMemory(process_handle, base_address, dll, MAX_PATH,
                           nullptr) == 0) {
        std::cout << "Could not write to allocated memory region in "
                  << process_name << std::endl;
        return false;
    }

    // Create a remote thread within the process; that will call LoadLibraryA
    // as the start routine, with the parameter that was written in memory.
    auto *const thread_handle = CreateRemoteThread(
        process_handle, nullptr, 0, LPTHREAD_START_ROUTINE(LoadLibraryA),
        base_address, 0, nullptr);

    if (thread_handle == nullptr) {
        std::cout << "Could not create remote thread in " << process_name
                  << std::endl;
        return false;
    }

    // Finally, close the process handle as it's no longer needed
    CloseHandle(process_handle);

    // Free the memory region that's no longer being used
    VirtualFreeEx(process_handle, base_address, 0, MEM_RELEASE);

    return true;
}
