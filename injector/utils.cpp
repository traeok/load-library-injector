#include "utils.hpp"

#include <Windows.h>
#include <TlHelp32.h>

#include <string>

namespace utils {
    int get_process_id_by_name(const std::string& process) {
        PROCESSENTRY32 _target_entry{};
        _target_entry.dwSize = sizeof PROCESSENTRY32;

        auto process_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (process_snapshot == INVALID_HANDLE_VALUE) return -1;

        const auto is_desired_process =
            [&process, &process_snapshot](const char* entry_name) {
                const auto is_equal = !process.compare(entry_name);
                if (is_equal) CloseHandle(process_snapshot);

                return is_equal;
            };

        Process32First(process_snapshot, &_target_entry);
        if (is_desired_process(_target_entry.szExeFile))
            return _target_entry.th32ProcessID;

        while (Process32Next(process_snapshot, &_target_entry)) {
            if (is_desired_process(_target_entry.szExeFile))
                return _target_entry.th32ProcessID;
        }

        CloseHandle(process_snapshot);
        return -1;
    }
}  // namespace utils
