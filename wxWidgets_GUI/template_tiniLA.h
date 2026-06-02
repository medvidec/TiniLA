///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __template_tiniLA__
#define __template_tiniLA__

class SignalListPanel;
class TimeScalePanel;
class WavePanel;
class HeaderPanel;

#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PANEL_TIMESCALE 1000
#define ID_PANEL_SIGNALLIST 1001
#define ID_PANEL_WAVE 1002
#define ID_SCROLL_VERT 1003
#define ID_SCROLL_HORIZ 1004
#define ID_SAMPFREQCOMBO 1005
#define ID_TOOL_STARTSTOP 1006
#define ID_TOOL_ZOOMF 1007
#define ID_TOOL_ZOOMIN 1008
#define ID_TOOL_ZOOMOUT 1009
#define ID_ANLZRPORT 1010
#define ID_TOOL_ZOOM11 1011

///////////////////////////////////////////////////////////////////////////////
/// Class tiniLAFrame
///////////////////////////////////////////////////////////////////////////////
class tiniLAFrame : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		
		TimeScalePanel* m_timeScalePanel;
		
		SignalListPanel* m_signalPanel;
		WavePanel* m_wavePanel;
      HeaderPanel* m_headpanel;
		wxScrollBar* m_scrollV;
		wxScrollBar* m_scrollH;
		wxComboBox* m_comboFreq;
		
		wxToolBar* m_toolBar2;
		wxStaticText* m_staticText1;
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu2;
		wxMenu* m_menu3;
		
      wxSplitterWindow* m_splitter;      // Main sash window container
      wxPanel* m_panelLog;             // Log panel
      wxTextCtrl* m_logTextCtrl;       // Log text control
    
		// Virtual event handlers, overide them in your derived class
		virtual void OnSize( wxSizeEvent& event ){ event.Skip(); }
		virtual void OnVertScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnSetFocus( wxFocusEvent& event ){ event.Skip(); }
		virtual void OnHorizScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFilePrint( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSampFreqChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToolStartStop( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnZoomIn( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnZoomOut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnZoom11( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnZoomFull( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnConfigApp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnHelpAbout( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		tiniLAFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("TiniLA Logic Analyzer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
		~tiniLAFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DialogConfigApp
///////////////////////////////////////////////////////////////////////////////
class DialogConfigApp : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl1;
		
		wxButton* m_button1;
		
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonOKClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancelClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DialogConfigApp( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Config Port"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DialogConfigApp();
	
};

#endif //__template_tiniLA__
