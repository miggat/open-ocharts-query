#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// Incluye las definiciones reales de wx
#include <wx/hashmap.h>
#include <wx/dynarray.h>
#include <wx/arrstr.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/log.h>      // wxLogMessage
#include <wx/utils.h>    // wxMilliSleep (o <wx/thread.h>)

#include "s52s57.h"

// ⚠️ NO volver a declarar wxIntegerHash, wxIntegerEqual, wxArrayInt, etc.
// Eso ya está en wxWidgets



// Forward decls para objetos S57
// using S57ObjVector = std::vector<S57Obj*>;
// using VE_ElementVector = std::vector<VE_Element*>;
// using VC_ElementVector = std::vector<VC_Element*>;
struct s57RegistrarMgr {
    std::string getFeatureAcronym(int) { return {}; }
    std::string getAttributeAcronym(int) { return {}; }
};

// Hash maps y arrays que usa Osenc
// using wxStringHashMap   = std::unordered_map<std::string, std::string>;
// using VectorHelperHash  = std::unordered_map<int, int>;
//using SENCFloatPtrArray = std::vector<float*>;
