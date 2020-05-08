#include <cstring>
#include <cstdlib>

bool is_debug()
{
    const char *env = getenv("DEBUG");
    return env != NULL && strcmp("true", env) == 0;
}
