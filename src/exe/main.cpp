#ifdef WIN32
#include <Windows.h>
#endif

int entry(int argc, char *argv[]);

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return entry(__argc, __argv);
}
#else
int main(int argc, char *argv[])
{
    return entry(argc, argv);
}
#endif
