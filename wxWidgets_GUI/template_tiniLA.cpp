///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "WavePanel.h"

#include "template_tiniLA.h"

#include "images/open.xpm"
#include "images/print.xpm"
#include "images/save.xpm"
#include "images/start.xpm"
#include "images/zoomin.xpm"
#include "images/zoomout.xpm"
#include "images/zoom11.xpm"
#include "images/zoomf.xpm"

///////////////////////////////////////////////////////////////////////////

tiniLAFrame::tiniLAFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	// Create splitter window
	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );

	m_panel1 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableRow( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_headpanel = new HeaderPanel (m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer1->Add( m_headpanel, 1, wxALL|wxEXPAND, 0 );
	
	m_timeScalePanel = new TimeScalePanel( m_panel1, ID_PANEL_TIMESCALE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer1->Add( m_timeScalePanel, 1, wxALL|wxEXPAND, 0 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_signalPanel = new SignalListPanel( m_panel1, ID_PANEL_SIGNALLIST, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer1->Add( m_signalPanel, 1, wxALL|wxEXPAND, 0 );
	
	m_wavePanel = new WavePanel( m_panel1, ID_PANEL_WAVE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer1->Add( m_wavePanel, 1, wxEXPAND | wxALL, 0 );
	
	m_scrollV = new wxScrollBar( m_panel1, ID_SCROLL_VERT, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
	fgSizer1->Add( m_scrollV, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 1 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_scrollH = new wxScrollBar( m_panel1, ID_SCROLL_HORIZ, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
	fgSizer1->Add( m_scrollH, 0, wxALL|wxEXPAND, 0 );
		
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 0 );
	
	m_panel1->SetSizer( fgSizer1 );
	m_panel1->Layout();
	fgSizer1->Fit( m_panel1 );
   /* Begin of copilot */
	// BOTTOM PANE - Log panel
	m_panelLog = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* logSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* logLabel = new wxStaticText( m_panelLog, wxID_ANY, wxT("Debug Log:"), wxDefaultPosition, wxDefaultSize, 0 );
	logSizer->Add( logLabel, 0, wxALL|wxEXPAND, 3 );
	
	m_logTextCtrl = new wxTextCtrl( m_panelLog, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	logSizer->Add( m_logTextCtrl, 1, wxALL|wxEXPAND, 3 );
	
	m_panelLog->SetSizer( logSizer );
	m_panelLog->Layout();
	logSizer->Fit( m_panelLog );
	
	// Split the splitter
	m_splitter->SplitHorizontally( m_panel1, m_panelLog, 450 );
	m_splitter->SetSashGravity( 1 );  // log panel stays the same size
	m_splitter->SetMinimumPaneSize( 50 );
	
	bSizer1->Add( m_splitter, 1, wxEXPAND, 0 );
   // Redirect wxLogMessage to text control
   wxLog::SetActiveTarget(new wxLogTextCtrl(m_logTextCtrl));   

	/* End of copilot */
	this->SetSizer( bSizer1 );
	this->Layout();
	m_toolBar2 = this->CreateToolBar( wxTB_HORIZONTAL & ~wxTB_NO_TOOLTIPS, wxID_ANY );
	m_toolBar2->SetToolBitmapSize( wxSize( 20,20 ) );
	m_toolBar2->AddTool( wxID_OPEN, wxT("Load"), wxBitmap( open_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Load the OLS file"), wxEmptyString );
	m_toolBar2->AddTool( wxID_SAVE, wxT("Save"), wxBitmap( save_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Save the OLS file"), wxEmptyString );
	m_toolBar2->AddSeparator();
	m_toolBar2->AddTool( wxID_PRINT, wxT("Print"), wxBitmap( print_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Print the screen"), wxEmptyString );
	m_toolBar2->AddSeparator();
	m_staticText1 = new wxStaticText( m_toolBar2, wxID_ANY, wxT("Time base:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_toolBar2->AddControl( m_staticText1 );
	m_comboFreq = new wxComboBox( m_toolBar2, ID_SAMPFREQCOMBO, wxT("External clock"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboFreq->Append( wxT("50 ns - 20MHz") );
	m_comboFreq->Append( wxT("100 ns - 10MHz") );
	m_comboFreq->Append( wxT("200 ns - 5MHz") );
	m_comboFreq->Append( wxT("500 ns - 2MHz") );
	m_comboFreq->Append( wxT("1 us - 1MHz") );
	m_comboFreq->Append( wxT("2 us - 500kHz") );
	m_comboFreq->Append( wxT("5 us - 200kHz") );
	m_comboFreq->Append( wxT("10 us - 100kHz") );
	m_comboFreq->Append( wxT("20 us - 50kHz") );
	m_comboFreq->Append( wxT("50 us - 20kHz") );
	m_comboFreq->Append( wxT("100 us - 10kHz") );
	m_comboFreq->Append( wxT("200 us - 5kHz") );
	m_comboFreq->Append( wxT("500 us - 2kHz") );
	m_comboFreq->Append( wxT("1 ms - 1kHz") );
	m_comboFreq->Append( wxT("10 ms - 500Hz") );
	m_comboFreq->Append( wxT("External clock") );
	m_toolBar2->AddControl( m_comboFreq );
	m_toolBar2->AddSeparator();
	m_toolBar2->AddTool( ID_TOOL_STARTSTOP, wxT("tool"), wxBitmap( start_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Start"), wxEmptyString );
	m_toolBar2->AddSeparator();
	m_toolBar2->AddTool( ID_TOOL_ZOOMIN, wxT("tool"), wxBitmap( zoomin_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Zoom in"), wxEmptyString );
	m_toolBar2->AddTool( ID_TOOL_ZOOMOUT, wxT("tool"), wxBitmap( zoomout_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Zoom out"), wxEmptyString );
	m_toolBar2->AddTool( ID_TOOL_ZOOM11, wxT("tool"), wxBitmap( zoom11_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Zoom max"), wxEmptyString );
	m_toolBar2->AddTool( ID_TOOL_ZOOMF, wxT("tool"), wxBitmap( zoomf_xpm ), wxNullBitmap, wxITEM_NORMAL, wxT("Zoom full"), wxEmptyString );
	m_toolBar2->Realize();
	
	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	m_menu1->Append(wxID_OPEN, wxT("&Open data\tAlt+O"), wxT(""), wxITEM_NORMAL);
   m_menu1->Append(wxID_SAVE, wxT("&Save data\tAlt+S"), wxT(""), wxITEM_NORMAL);
   m_menu1->Append(wxID_SAVEAS, wxT("Save data &As"), wxT(""), wxITEM_NORMAL);
   m_menu1->AppendSeparator();
   m_menu1->Append(wxID_EXIT, wxString( wxT("Close") ) + wxT('\t') + wxT("Alt+X"), wxEmptyString, wxITEM_NORMAL );
	m_menubar1->Append( m_menu1, wxT("File") );
	
	m_menu2 = new wxMenu();
	wxMenuItem* menuItem2;
	menuItem2 = new wxMenuItem( m_menu2, ID_ANLZRPORT, wxString( wxT("Port") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( menuItem2 );
	
	m_menubar1->Append( m_menu2, wxT("Config") );
	
	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem3 );
	
	m_menubar1->Append( m_menu3, wxT("Help") );
	
	this->SetMenuBar( m_menubar1 );
	
	
	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( tiniLAFrame::OnSize ) );
	m_scrollV->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( tiniLAFrame::OnSetFocus ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( tiniLAFrame::OnSetFocus ), NULL, this );
	this->Connect( wxID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFileOpen ) );
	this->Connect( wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFileSave ) );
	this->Connect( wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnFileOpen ) );
	this->Connect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnFileSave ) );
	this->Connect( wxID_PRINT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFilePrint ) );
	m_comboFreq->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( tiniLAFrame::OnSampFreqChanged ), NULL, this );
	this->Connect( ID_TOOL_STARTSTOP, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnToolStartStop ) );
	this->Connect( ID_TOOL_ZOOMIN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomIn ) );
	this->Connect( ID_TOOL_ZOOMOUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomOut ) );
	this->Connect( ID_TOOL_ZOOM11, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoom11 ) );
	this->Connect( ID_TOOL_ZOOMF, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomFull ) );
	this->Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnExit ) );
	this->Connect( menuItem2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnConfigApp ) );
	this->Connect( m_menuItem3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnHelpAbout ) );
}

tiniLAFrame::~tiniLAFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( tiniLAFrame::OnSize ) );
	m_scrollV->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( tiniLAFrame::OnVertScroll ), NULL, this );
	m_scrollV->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( tiniLAFrame::OnSetFocus ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( tiniLAFrame::OnHorizScroll ), NULL, this );
	m_scrollH->Disconnect( wxEVT_SET_FOCUS, wxFocusEventHandler( tiniLAFrame::OnSetFocus ), NULL, this );
	this->Disconnect( wxID_OPEN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFileOpen ) );
	this->Disconnect( wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFileSave ) );
	this->Disconnect( wxID_PRINT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnFilePrint ) );
	m_comboFreq->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( tiniLAFrame::OnSampFreqChanged ), NULL, this );
	this->Disconnect( ID_TOOL_STARTSTOP, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnToolStartStop ) );
	this->Disconnect( ID_TOOL_ZOOMIN, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomIn ) );
	this->Disconnect( ID_TOOL_ZOOMOUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomOut ) );
	this->Disconnect( ID_TOOL_ZOOM11, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoom11 ) );
	this->Disconnect( ID_TOOL_ZOOMF, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( tiniLAFrame::OnZoomFull ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnExit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnConfigApp ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( tiniLAFrame::OnHelpAbout ) );
}

DialogConfigApp::DialogConfigApp( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Set Hardware Port") ), wxVERTICAL );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxT("/dev/ttyACM0"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_textCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( sbSizer2, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer2->Add( 10, 10, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button1, 0, wxALL, 5 );
	
	
	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_button2, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, 0, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
	bSizer2->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogConfigApp::OnButtonOKClick ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogConfigApp::OnButtonCancelClick ), NULL, this );
}

DialogConfigApp::~DialogConfigApp()
{
	// Disconnect Events
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogConfigApp::OnButtonOKClick ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DialogConfigApp::OnButtonCancelClick ), NULL, this );
}
