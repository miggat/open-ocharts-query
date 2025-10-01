/******************************************************************************
 *
 *
 * Project:  OpenCPN
 * Purpose:  PlugIn Manager Object
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 *
 */

#ifndef _PLUGINMGR_H_
#define _PLUGINMGR_H_

#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/dynlib.h>
#include <wx/glcanvas.h>


#include "ocpn_plugin.h"
#include "ocpn_types.h"
#include "ocpndc.h"
//#include "chart1.h"                 // for MyFrame
//#include "chcanv.h"                 // for ViewPort
//#include "nmea.h"                   // for GenericPosDat

//For widgets...
#include "wx/hyperlink.h"
#include <wx/choice.h>
#include <wx/tglbtn.h>
#include <wx/bmpcbox.h>
#include "ColorTable.h"

//    Include wxJSON headers
//    We undefine MIN/MAX so avoid warning of redefinition coming from
//    json_defs.h
//    Definitions checked manually, and are identical
#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif



//    Assorted static helper routines


class PluginListPanel;
class PluginPanel;


//-----------------------------------------------------------------------------------------------------
//
//          The PlugIn Container Specification
//
//-----------------------------------------------------------------------------------------------------
class PlugInContainer
{
      public:
            PlugInContainer(){ m_pplugin = NULL;
                               m_bEnabled = false;
                               m_bInitState = false;
                               m_bToolboxPanel = false;
                               m_bitmap = NULL; 
            }

            opencpn_plugin    *m_pplugin;
            bool              m_bEnabled;
            bool              m_bInitState;
            bool              m_bToolboxPanel;
            int               m_cap_flag;             // PlugIn Capabilities descriptor
            wxString          m_plugin_file;          // The full file path
            destroy_t         *m_destroy_fn;
            wxDynamicLibrary  *m_plibrary;
            wxString          m_common_name;            // A common name string for the plugin
            wxString          m_short_description;
            wxString          m_long_description;
            int               m_api_version;
            int               m_version_major;
            int               m_version_minor;
            wxBitmap          *m_bitmap;

};

//    Declare an array of PlugIn Containers
WX_DEFINE_ARRAY_PTR(PlugInContainer *, ArrayOfPlugIns);

class PlugInMenuItemContainer
{
      public:
            wxMenuItem        *pmenu_item;
            opencpn_plugin    *m_pplugin;
            bool              b_viz;
            bool              b_grey;
            int               id;
};

//    Define an array of PlugIn MenuItem Containers
WX_DEFINE_ARRAY_PTR(PlugInMenuItemContainer *, ArrayOfPlugInMenuItems);


class PlugInToolbarToolContainer
{
      public:
            PlugInToolbarToolContainer();
            ~PlugInToolbarToolContainer();

            opencpn_plugin    *m_pplugin;
            int               id;
            wxString          label;
            wxBitmap          *bitmap_day;
            wxBitmap          *bitmap_dusk;
            wxBitmap          *bitmap_night;
            wxBitmap          *bmpDisabled;
            wxItemKind        kind;
            wxString          shortHelp;
            wxString          longHelp;
            wxObject          *clientData;
            int               position;
            bool              b_viz;
            bool              b_toggle;
            int               tool_sel;

};

//    Define an array of PlugIn ToolbarTool Containers
WX_DEFINE_ARRAY_PTR(PlugInToolbarToolContainer *, ArrayOfPlugInToolbarTools);



//-----------------------------------------------------------------------------------------------------
//
//          The PlugIn Manager Specification
//
//-----------------------------------------------------------------------------------------------------

class PlugInManager
{

public:
      PlugInManager();
      virtual ~PlugInManager();

      bool LoadAllPlugIns(wxString &plugin_dir,wxString spec=wxEmptyString);
      bool UnLoadAllPlugIns();
      bool DeactivateAllPlugIns();
      bool UpdatePlugIns();

      bool UpdateConfig();

      PlugInContainer *LoadPlugIn(wxString plugin_file);
      ArrayOfPlugIns *GetPlugInArray(){ return &plugin_array; }

      bool RenderAllCanvasOverlayPlugIns( ocpnDC &dc, const ViewPort &vp);
      bool RenderAllGLCanvasOverlayPlugIns( wxGLContext *pcontext, const ViewPort &vp);
      void SendCursorLatLonToAllPlugIns( double lat, double lon);
      void SendViewPortToRequestingPlugIns( ViewPort &vp );

//      void AddAllPlugInToolboxPanels( wxNotebook *pnotebook);
//      void CloseAllPlugInPanels( int );

      void SendMessageToAllPlugins(wxString &message_id, wxString &message_body);

      void SendResizeEventToAllPlugIns(int x, int y);
      void SetColorSchemeForAllPlugIns(ColorScheme cs);
      void NotifyAuiPlugIns(void);

      wxArrayString GetPlugInChartClassNameArray(void);

      wxString GetLastError();
 

      void DimeWindow(wxWindow *win);

private:
      bool DeactivatePlugIn(PlugInContainer *pic);
      wxBitmap *BuildDimmedToolBitmap(wxBitmap *pbmp_normal, unsigned char dim_ratio);
      bool UpDateChartDataTypes(void);
      bool CheckPluginCompatibility(wxString plugin_file);

  
      ArrayOfPlugIns    plugin_array;
      wxString          m_last_error_string;

      ArrayOfPlugInMenuItems        m_PlugInMenuItems;
      ArrayOfPlugInToolbarTools     m_PlugInToolbarTools;

      wxString          m_plugin_location;

      int               m_plugin_tool_id_next;
      int               m_plugin_menu_item_id_next;
      wxBitmap          m_cached_overlay_bm;
	  

 //     opencpn_plugin    *m_plugin_base;


};

WX_DEFINE_ARRAY_PTR(PluginPanel *, ArrayOfPluginPanel);

class PluginListPanel: public wxScrolledWindow
{
public:
      PluginListPanel( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, ArrayOfPlugIns *pPluginArray );
      ~PluginListPanel();

      void SelectPlugin( PluginPanel *pi );

private:
      ArrayOfPlugIns     *m_pPluginArray;
      ArrayOfPluginPanel  m_PluginItems;
      PluginPanel        *m_PluginSelected;
};

class PluginPanel: public wxPanel
{
public:
      PluginPanel( PluginListPanel *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, PlugInContainer *p_plugin );
      ~PluginPanel();

      void OnPluginSelected( wxMouseEvent &event );
      void SetSelected( bool selected );
      void OnPluginPreferences( wxCommandEvent& event );
      void OnPluginEnable( wxCommandEvent& event );
      void SetEnabled( bool enabled );

private:
      PluginListPanel *m_PluginListPanel;
      bool             m_bSelected;
      PlugInContainer *m_pPlugin;
      wxStaticText    *m_pName;
      wxStaticText    *m_pVersion;
      wxStaticText    *m_pDescription;
//      wxBoxSizer      *m_pButtons;
      wxFlexGridSizer      *m_pButtons;
      wxButton        *m_pButtonEnable;
      wxButton        *m_pButtonPreferences;
};

class PluginConfigBase{
public:
    wxString        s57BaseDir;
    wxString        configDir;
    wxString        exeDir;
    float           baseFontScale;
    float           soundingsFontScale;
    wxFileConfig    *configFile;
    int             depthUnits;
    PI_LUPname      symbolStyle;
    PI_LUPname      boundaryStyle;
    int             settingsSequence;
    //values that need to be updated via json
    bool            bShowS57Text;
    bool            bShowSoundg;
    bool            showLights;
    int             nDisplayCategory;
    bool            bShowLightDescription;
    bool            bShowS57ImportantTextOnly;
    bool            bShowAtonText;
    bool            showAnchorConditions;
    bool            bUseScamin;
    
    PluginConfigBase(){
        s57BaseDir=wxEmptyString;
        configDir=wxEmptyString;
        exeDir=wxEmptyString;
        baseFontScale=1;
        soundingsFontScale=1;
        configFile=NULL;
        depthUnits=1;
        symbolStyle=PI_PAPER_CHART;
        boundaryStyle=PI_PLAIN_BOUNDARIES;
        settingsSequence=1;
        bShowS57Text=true;
        bShowSoundg=true;
        showLights=true;
        nDisplayCategory=83; //Standard
        bShowLightDescription=true;
        bShowS57ImportantTextOnly=false;
        bShowAtonText=true;
        showAnchorConditions=true;
        bUseScamin=true;
        
    }
    
};

void setPluginBaseConfig(PluginConfigBase cfg,bool sendJson=false);


void setColorTable(ColorTable *table);

#endif            // _PLUGINMGR_H_

