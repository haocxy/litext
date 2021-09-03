#if defined(WIN32) && (!defined(NOTESHARP_WINDOWS_CONSOLE))
#define NOTESHARP_ENTRY_WINDOWS_GUI
#endif

#if defined(NOTESHARP_ENTRY_WINDOWS_GUI)
#include <Windows.h>
#endif

int loader_entry(int argc, char *argv[]);

#if defined(NOTESHARP_ENTRY_WINDOWS_GUI)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return loader_entry(__argc, __argv);
}
#else
int main(int argc, char *argv[])
{
    return loader_entry(argc, argv);
}
#endif
