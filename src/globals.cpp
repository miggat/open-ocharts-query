#include "ocharts/S57ClassRegistrar.h"

// nivel de depuración global que usa Osenc.cpp (0 = silencioso)
int g_debugLevel = 0;

// puntero global al registrador S-57 (algunos módulos lo usan como global)
S57ClassRegistrar* pi_poRegistrar = nullptr;
