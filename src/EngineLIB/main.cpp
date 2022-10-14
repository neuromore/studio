
#ifdef WIN32
#include <Windows.h>
BOOL WINAPI DllMain(
   HINSTANCE hinstDLL,  // handle to DLL module
   DWORD fdwReason,     // reason for calling function
   LPVOID lpvReserved)  // reserved
{
   return TRUE;
}
#else
int main(int argc, char* argv[])
{
   return 0;
}
#endif
