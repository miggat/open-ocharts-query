#include "mygeom.h"

// --- Implementaciones mínimas ---

PolyTessGeo::~PolyTessGeo() {
    // Libera PolyTriGroups
    m_polytri_groups.clear();
}
