#include <chrono>
#include <thread>

#include "load_library.hpp"

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cout << "Usage: injector.exe --process <PROCESS> --dll <DLL_PATH>"
                  << std::endl;
        return 1;
    }

    std::cout << "LoadLibraryA injector (proof of concept)" << std::endl;
    std::cout << "by traeok (trae@trae.is)" << std::endl;

    std::string dll_name{argv[4]}, process_name{argv[2]};

    if (dll_name.find(".dll") == std::string::npos) {
        std::cout << "Invalid DLL specified. Check the file name and try again."
                  << std::endl;
        std::cout << "Expected format: <DLL_NAME>.dll" << std::endl;
        return 1;
    }

    if (process_name.find(".exe") == std::string::npos) {
        std::cout << "Invalid process specified. Check the process name and "
                     "try again."
                  << std::endl;
        std::cout << "Expected format: <PROCESS_NAME>.exe" << std::endl;
        return 1;
    }

    const auto injection_result =
        load_library_injection(process_name, dll_name);
    std::cout << (injection_result ? "Injected library into target process."
                                   : "Injection was unsuccessful.")
              << std::endl;

    std::cout << "Exiting in 3 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return int(!injection_result);
}
