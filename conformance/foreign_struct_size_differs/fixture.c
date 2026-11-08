/* The library conformance/foreign_struct_size_differs calls: check.sh builds it next to the program. */
#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

typedef struct { int x; int y; } POINT_PAIR;
EXPORTED void MovePoint(POINT_PAIR* point) { point->x = point->x + 10; point->y = point->y + 20; }
