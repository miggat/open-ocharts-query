#pragma once
#include <wx/string.h>
#include <map>

// S-52 colour record (lo que esperaba s52s57.h)
typedef struct _S52color {
  char colName[20];
  unsigned char R;
  unsigned char G;
  unsigned char B;
} S52color;

// Mapa simple de nombre -> S52color (evita WX_DECLARE... para no requerir GUI)
using colorHashMap = std::map<wxString, S52color>;
