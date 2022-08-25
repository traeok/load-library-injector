# load-library-injector
A simple LoadLibraryA injector for Windows (proof of concept)

No anti-detection measures are implemented in this project. Using this injector comes at your own risk; most protected processes will detect and actively prevent this method of injection.

Usage: `.\injector.exe --process <PROCESS_NAME>.exe --dll <DLL_NAME>.dll`

1. Load the target process first, and ensure it is running
2. Run the injector as admin, with the desired arguments
3. Done!

- Works on all versions of Windows (x86 and x64)
  - **NOTE:** This should go without saying, but you cannot mix and match x64 and x86 (e.g., both the target process and DLL need to be compiled for the same architecture)
