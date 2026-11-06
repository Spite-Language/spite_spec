#define EXPORTED __declspec(dllexport)
typedef void Nothing;
int Other(int value);
EXPORTED Nothing __cdecl QuietForget(int value);
