#include <wx/wx.h>
#include <wx/image.h>

#include "FrameMain.h"
#include "WavePanel.h"
#include "DlgConfigApp.h"
#include "ftd2xx_if.h"

#include "wx/msgdlg.h"
#include <wx/ffile.h>
#include <wx/textfile.h>

#include "images/start.xpm"
#include "images/stop.xpm"
#include "datebuilt.h"

#define EVT_DATA_ARRIVED 7777
#define EVT_TIMER_ID 7778
#define ID_STATUS_GAUGE 7779

#ifdef _WINDOWS
#include <windows.h>
#endif

FrameMain::FrameMain( wxWindow* parent )
:
  tiniLAFrame( parent ), m_timer(this, EVT_TIMER_ID)
{
   
   // Set frame reference for panels
   m_wavePanel->SetFrameMain(this);
   m_timeScalePanel->SetFrameMain(this);

   int vhei = m_signalPanel->GetVirtualHeight();
   int hei = m_signalPanel->GetSize().GetHeight();
   m_scrollV->SetScrollbar(0, hei, vhei, hei);

   int wwid = m_wavePanel->GetVirtualWidth();
   int wid = m_wavePanel->GetSize().GetWidth();
   m_scrollH->SetScrollbar(0, wid, wwid, wid);

   m_zoom = 1;
   m_deviceString = _T("To be removed");
   m_deviceOpen = false;
   this->Connect( EVT_DATA_ARRIVED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FrameMain::OnThreadDataArrived ) );

   /* timer for HW comm */
   m_statusBar = new MyStatusBar(this);
   SetStatusBar( m_statusBar);

   this->Connect( EVT_TIMER_ID, wxEVT_TIMER, wxTimerEventHandler(FrameMain::OnTimerEvent));

   m_timer.Start(1000);
   m_statusBar->SetGauge( 0,100);

   m_comboFreq->SetSelection(4);
	m_samplingfreq = 1000000;
   m_timeScalePanel->SetSamplePeriod( 1000000000/m_samplingfreq);

      m_startpressed = false;      
      m_stoppressed = false;
//      wxString bla; bla.Printf(_("Sampling period: %d ns"),1000000000/m_samplingfreq);
//      wxLogMessage( bla);
}

FrameMain::~FrameMain( void )
{
   d2xx_close();
   m_timer.Stop();
   this->Disconnect( EVT_DATA_ARRIVED, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FrameMain::OnThreadDataArrived ) );

   this->Disconnect( EVT_TIMER_ID, wxEVT_TIMER, wxTimerEventHandler(FrameMain::OnTimerEvent));
}

void FrameMain::OnSize( wxSizeEvent& event )
{
   event.Skip();
   
   SetScrollSize();
   SetZoom();
}

void FrameMain::OnVertScroll( wxScrollEvent& event )
{
   SetZoom();
}

void FrameMain::OnHorizScroll( wxScrollEvent& event )
{
   SetZoom();
}

void FrameMain::OnToolStartStop( wxCommandEvent& event )
{
   // get the current frequency setting
   switch( m_comboFreq->GetCurrentSelection() ) {
      	case 0 :
      		m_samplingfreq = 20000000;
      		break;
      	case 1 :
      		m_samplingfreq = 10000000;
      		break;
      	case 2 :
      		m_samplingfreq = 5000000;
      		break;
      	case 3 :
      		m_samplingfreq = 2000000;
      		break;
      	case 4 :
      		m_samplingfreq = 1000000;
      		break;
      	case 5 :
      		m_samplingfreq = 500000;
      		break;
      	case 6 :
      		m_samplingfreq = 200000;
      		break;
      	case 7 :
      		m_samplingfreq = 100000;
      		break;
      	case 8 :
      		m_samplingfreq = 50000;
      		break;
      	case 9 :
      		m_samplingfreq = 20000;
      		break;
      	case 10:
      		m_samplingfreq = 10000;
      		break;
      	case 11:
      		m_samplingfreq = 5000;
      		break;
      	case 12:
      		m_samplingfreq = 2000;
      		break;        
        default:
      		m_samplingfreq = 0;
		break;
   }


   // change the Toolbar icon
   wxToolBar* tbar = GetToolBar();
   
   wxString txt = tbar->GetToolShortHelp(ID_TOOL_STARTSTOP);
   
   if ( wxT("Start") == txt) {
      tbar->SetToolNormalBitmap(ID_TOOL_STARTSTOP, wxBitmap(stop_xpm));
      tbar->SetToolShortHelp(ID_TOOL_STARTSTOP, wxT("Stop"));
      m_startpressed = true;
      m_stoppressed = false;
      tbar->Refresh();
   } else {
      tbar->SetToolNormalBitmap(ID_TOOL_STARTSTOP, wxBitmap(start_xpm));
      tbar->SetToolShortHelp(ID_TOOL_STARTSTOP, wxT("Start"));
      m_startpressed = false;      
      m_stoppressed = true;
      tbar->Refresh();
   }
   
}

void FrameMain::SetNewData(ArrayOfSamples arr)
{
  m_samples.Empty();
  WX_APPEND_ARRAY(m_samples, arr);
}

void FrameMain::OnThreadDataArrived( wxCommandEvent& event )
{
   wxToolBar* tbar = GetToolBar();

   tbar->SetToolNormalBitmap(ID_TOOL_STARTSTOP, wxBitmap(start_xpm));
   tbar->SetToolShortHelp(ID_TOOL_STARTSTOP, wxT("Start"));
   tbar->Refresh();
   
   m_startpressed = false;
   m_stoppressed = false;
   
   // set new time scale
     //TimeScalePanel *pan = (TimeScalePanel*) FindWindow( ID_PANEL_TIMESCALE);
	 if (0 != m_samplingfreq) {
	    m_timeScalePanel->SetSamplePeriod( 1000000000/m_samplingfreq);
	 } else {
	    m_timeScalePanel->SetSamplePeriod( 0);
	 }


   //WavePanel *panw = (WavePanel*) FindWindow( ID_PANEL_WAVE);
   //wxScrollBar * scrbh = (wxScrollBar *) FindWindow( ID_SCROLL_HORIZ);
   m_wavePanel->FillData( m_zoom, m_samples);
   int wwid = m_wavePanel->GetVirtualWidth();
   int wid = m_wavePanel->GetSize().GetWidth();
   m_scrollH->SetScrollbar( 0,wid, wwid, wid);
   m_signalPanel->SetSampleCount(m_samples.Count());

   wxString bla; bla.Printf(_("%ld samples"),m_samples.Count());
   wxLogMessage( _("New Data Arrived - ") + bla);
}

void FrameMain::UnPressStop( void)
{

   m_stoppressed = false;

}

void FrameMain::OnZoomIn( wxCommandEvent& event )
{
   if ( m_samples.Count() != 0) {
      m_zoom *= 2;
      if (m_zoom > 20) { // maximum is 20 points per sample; more does not make sense
         m_zoom = 20;
      }

      m_wavePanel->SetZoom( m_zoom);
      m_timeScalePanel->SetZoom( m_zoom);
   
      SetScrollSize();
      SetZoom();
   }
}

void FrameMain::OnZoomOut( wxCommandEvent& event )
{
   if ( m_samples.Count() != 0) {
      int recWidth = m_wavePanel->GetRecWidth();
      double minzoom = 1.0*recWidth/m_samples.Count();

      m_zoom *= 0.5;
      if (m_zoom < minzoom) {
		  m_zoom = minzoom;
	   }

      m_wavePanel->SetZoom( m_zoom);
      m_timeScalePanel->SetZoom( m_zoom);
   
      SetScrollSize();
      SetZoom();
   }
}

void FrameMain::OnZoom11( wxCommandEvent& event )
{

   if ( m_samples.Count() != 0) {

      m_zoom = 1;
      m_wavePanel->SetZoom( m_zoom);
      m_timeScalePanel->SetZoom( m_zoom);
   
      SetScrollSize();
      SetZoom();
   }
}

void FrameMain::OnZoomFull( wxCommandEvent& event )
{
   if ( m_samples.Count() != 0) {

      int recWidth = m_wavePanel->GetRecWidth();
      
      m_zoom = 1.0*recWidth/m_samples.Count();
      m_wavePanel->SetZoom( m_zoom);
      m_timeScalePanel->SetZoom( m_zoom);
   
      SetScrollSize();
      SetZoom();
   }

}

void FrameMain::SetZoom( void)
{

   int thpos = m_scrollV->GetThumbPosition();
   int thposh = m_scrollH->GetThumbPosition();
   m_signalPanel->SetNewOrigin( thposh, thpos);
   m_wavePanel->SetNewOrigin( thposh, thpos);
   m_timeScalePanel->SetNewOrigin( thposh, thpos);

}

void FrameMain::OnSampFreqChanged( wxCommandEvent& event )
{
   //wxComboBox *cbox = (wxComboBox *) FindWindow(ID_SAMPFREQCOMBO);

   // we can not do it here but only when START is pressed
   //m_samplingfreq = cbox->GetCurrentSelection(); 
}

void FrameMain::SetScrollSize( void)
{

   int vhei = m_signalPanel->GetVirtualHeight();
   int hei = m_signalPanel->GetSize().GetHeight();
   int thpos = m_scrollV->GetThumbPosition();
   m_scrollV->SetScrollbar( thpos, hei, vhei, hei);

   long int wwid = m_wavePanel->GetVirtualWidth();
   long int wid = m_wavePanel->GetSize().GetWidth();
   double thposh = (double) m_scrollH->GetThumbPosition();
   
   if ( 0 != m_scrollH->GetRange()) {
      thposh = (1.0*m_scrollH->GetThumbPosition()+wid/2)*wwid/m_scrollH->GetRange()-wid/2;
   }
   m_scrollH->SetScrollbar( (int)thposh,wid, wwid, wid);
}

void FrameMain::OnExit( wxCommandEvent& event )
{
   // TODO: Implement OnExit
   Close();
}

/* File format is SUMP/OLS (Open Logic Sniffer) */
/* More OLSs to test https://github.com/njanik/ols-parser/tree/master/ols */
void FrameMain::OnFileOpen( wxCommandEvent& event )
{
   ArrayOfSamples arr;
   wxString fname;
   wxTextFile filedat;
   int samplingfreqfromfile = 0;
   wxString str;


   wxFileDialog dlg(this, _T("Choose a file"), _T(""), _T(""), _T("*.ols"), wxFD_OPEN , wxDefaultPosition, wxDefaultSize, _T("filedlg"));

   if( wxID_OK == dlg.ShowModal()) {
      // get the file and open it
      fname = dlg.GetPath();
      if( true == filedat.Open( fname)) {
         // successfully opened file - get the header; it begins with ";"
         unsigned long samplecntr = 0xfffffff;
         unsigned long lastsample = 0;
         arr.Empty();
         for ( str = filedat.GetFirstLine(); !filedat.Eof(); str = filedat.GetNextLine() )
         {
             // try to parse the line
             if(str.Left(1) == _T(";")) {
                // comments here, parse TODO
                if ( 0 == str.Find(_T(";Rate:"))) {      // get rate
                  unsigned long ilng;
                  fname = str.Mid(6);
                  fname.ToULong(&ilng,10);
                  samplingfreqfromfile = (unsigned int) ilng;
                }
                if ( 0 == str.Find(_T(";Channels:"))) {
                  // we assume 32 channels, so no action here yet
                }
                if (( 0 == str.Find(_T(";Channel "))) && (_T("name:")==str.Mid(12,5))) { // find channel names
                  unsigned long t1; str.Mid(9,2).ToULong(&t1, 10);
                  if ((t1 >= 0) && (t1 < 33)) {
                     wxLogMessage ( _T("File open: channel number: ") + str.Mid(9,2) + _(" renamed to ") + str.Mid(18));
                     m_signalPanel->SetSignalName( (int)t1, str.Mid(18));
                  } else {
                     wxLogMessage ( _T("File open: wrong channel number: ") + str.Mid(9,2));
                  }
                }
             } else {
                int atpos;
                unsigned long actsample;
                unsigned long actcntr;
                // find @
                if (wxNOT_FOUND != (atpos = (str.Find(_T("@"))))) {
                   if (( true == str.BeforeFirst('@').ToULong(&actsample,16)) &&
                       ( true == str.AfterFirst('@').ToULong(&actcntr,10))) {
                          while (samplecntr<actcntr) {
                             arr.Add((unsigned int)lastsample);
                             samplecntr++;
                          }
                          arr.Add((unsigned int)actsample);
                          lastsample = actsample;
                          samplecntr = actcntr+1;
                       }
           			 } else {
                      wxString bla; bla.Printf(_("%d"), filedat.GetCurrentLine());
                      wxLogMessage( _("OLS import - unknown line ") + bla);
                   }

             }
			 
         }
         // file has to end with blank line

	     if ( 0 != samplingfreqfromfile) {
         m_signalPanel->SetSampleCount( arr.Count());
	        m_wavePanel->FillData( m_zoom, arr);
	        SetNewData( arr);
	        m_samplingfreq = samplingfreqfromfile;
    	     m_timeScalePanel->SetSamplePeriod( 1000000000/m_samplingfreq);
            int wwid = m_wavePanel->GetVirtualWidth();
            int wid = m_wavePanel->GetSize().GetWidth();
            m_scrollH->SetScrollbar( 0,wid, wwid, wid);
            SetCursorPosition(0);
	     } else {
	        wxMessageBox( _T("Bad or missing sample rate info"),_T("File Open Error"), wxICON_ERROR|wxOK);
         }
      } else {
	    wxMessageBox( wxT("Could not open selected file"),_T("File Open Error"), wxICON_ERROR|wxOK);  
      }
   }
}

/* File format is SUMP/OLS (Open Logic Sniffer) */
void FrameMain::OnFileSave( wxCommandEvent& event )
{
   wxString fname;
   wxFFile filedat;
   unsigned int icnt;
   unsigned int lastSample = 0xdeadbeef;
   
   wxFileDialog dlg(this, _T("Choose a file"), _T(""), _T(""), _T("*.ols"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT , wxDefaultPosition, wxDefaultSize, _T("filedlg"));

   if( wxID_OK == dlg.ShowModal()) {
     fname = dlg.GetPath();
     if( true == filedat.Open( fname, _T("w"))) {
	    // ok, now store the header
		fname.Printf(wxT(";Rate: %d\n"), m_samplingfreq);
		filedat.Write(fname); // store sample rate
		filedat.Write(wxT(";Channels: 32\n")); // store number of channels
      filedat.Write(wxT(";Compressed: true\n")); // store number of channels
      // store the channel names if they differ from default
      for ( icnt = 0; icnt < 32; icnt++) {
         wxString chname; chname.Printf(_T("Channel %02d"), icnt);
         if (chname != m_signalPanel->GetSignalName( icnt)) {
            wxString bla; bla.Printf(_T(";Channel %02d name: "), icnt);
            filedat.Write(bla + m_signalPanel->GetSignalName( icnt)+ _T("\n"));
         }
      }

		// store the main data - format is <sample value>@<sample number> [HEX,DEC]
		for ( icnt = 0; icnt < m_samples.GetCount(); icnt++) {
         if ((lastSample != m_samples.Item(icnt)) || (icnt == 0) || (icnt == m_samples.GetCount()-1)) { // adding compression
            fname.Printf(wxT("%08x@%d\n"), m_samples.Item(icnt), icnt); // samples start with 0
            filedat.Write(fname);
            lastSample = m_samples.Item(icnt);
         }
		}
		filedat.Close();
	 } else {
	    wxMessageBox( wxT("Could not open selected file"),_T("File Open Error"), wxICON_ERROR|wxOK);
	 }
   }
}

void FrameMain::OnFilePrint( wxCommandEvent& event )
{

   (void)wxMessageBox( _T("OnFilePrint() not yet ready"),_T("TiniLA"), wxICON_INFORMATION | wxOK );
}

void FrameMain::OnConfigApp( wxCommandEvent& event )
{
   DlgConfigApp dlg(this);

   if (!m_deviceString.IsEmpty()) {
      dlg.SetDevice( m_deviceString);
   }
   if (wxID_OK == dlg.ShowModal()) {
      m_deviceString = dlg.GetDevice();

      // here we need to restart the thread
   }

}

void FrameMain::OnHelpAbout( wxCommandEvent& event )
{

 (void)wxMessageBox( _T("Tini Logic Analyzer GUI\nVaclavPe@seznam.cz\nbuild date:")+datebuilt ,_T("About Application"), wxICON_INFORMATION | wxOK );
}

void FrameMain::GetTrigger( unsigned char *trigmask, unsigned char *trigval)
{
   if (m_signalPanel != NULL) {
      *trigmask = m_signalPanel->GetTriggerMask();
      *trigval = m_signalPanel->GetTriggerLevel();
   } else {
      *trigmask = 0;
   }
}
#define NUM_OF_SINGLE_READ (100)
void FrameMain::OnTimerEvent( wxTimerEvent& event)
{
  wxStatusBar * statbar = this->GetStatusBar();
  unsigned char ftdata;
  wxString bla;
  unsigned char sampfreq, trigmask, triglevel;
  unsigned long hw_cntr_top;
  unsigned long data32b = 0;
  unsigned long icnt, jcnt;
  unsigned char ftchardata[NUM_OF_SINGLE_READ];
  int dete;

  if (0 != (dete = d2xx_detect_tinila())) {
     if (false == m_deviceOpen) {
       if ( 0 != d2xx_open()) {
         wxString bla;
         unsigned char ftdata;
         d2xx_readreg( 0x01, &ftdata); //cteni status registru
         bla.Printf(_("TiniLA connected - firmware v.%d"), ftdata&0x7);
         m_statusBar->SetStatusText( bla, 0);
         d2xx_writereg ( 0x0, 0x40);
         m_deviceOpen = true;
         wxLogMessage( _("TiniLA connected"));
       } else {
         m_statusBar->SetStatusText( _("TiniLA not detected"), 0);
       }
     }
  } else {
     if (true == m_deviceOpen) {
        d2xx_close();
        wxLogMessage( _("TiniLA disconnected"));
     }
     m_deviceOpen = false;
     m_statusBar->SetStatusText( _("TiniLA not detected"), 0);
  }
       //bla.Printf(_("Timer: dete %d, open %d"), dete , m_deviceOpen);
       //wxLogMessage( bla);

  if (true == m_deviceOpen) {
     // cteni status registru a zobrazovani statusu
     d2xx_readreg( 0x01, &ftdata); //cteni status registru

     if (0x20 == (ftdata & 0x20)) { statbar->SetStatusText( _("HW in RESET"), 1); }
     else if (0x80 == (ftdata & 0x80)) { statbar->SetStatusText( _("Capture end"), 1); }
     else if (0x00 == (ftdata & 0x40)) { statbar->SetStatusText( _("Analyzer ready"), 1); }
     else if (0x50 == (ftdata & 0x50)) { statbar->SetStatusText( _("Capturing the data"), 1); }
     else if (0x40 == (ftdata & 0x40)) { statbar->SetStatusText( _("Waiting for trigger"), 1); }

     // vykonne funkce
     if(this->StartPressed()) {
       // send new capture request
       this->UnPressStart();
       m_statusBar->SetGauge( 0, 100);
       sampfreq = m_comboFreq->GetCurrentSelection();//this->GetSamplingFreq();
       this->GetTrigger( &trigmask, &triglevel);
       // reset analyzatoru
       d2xx_writereg ( 0x0, 0x40);
       d2xx_writereg ( 0x0, 0x00);
       // nastaveni casove zakladny a vypnuti ext. triggeru
       d2xx_writereg ( 0x3, sampfreq & 0x0f);
       // nastaveni trigger masky - pouze 5 bitu
       d2xx_writereg ( 0x2, trigmask);
       // nastaveni trigger urovne
       d2xx_writereg ( 0x1, triglevel);
       Sleep(50);
       // start
       d2xx_writereg ( 0x0, 0x40);
       d2xx_writereg ( 0x0, 0x80);
       bla.Printf(_("Writing RUN - samplefreq = 0x%02X, mask = 0x%02X, level = 0x%02X"), sampfreq &0x0f, trigmask, triglevel);
       wxLogMessage( bla);
      
     }

     // kdyz dobehne capture nebo kdyz se zmackne STOP, tak cti data a uloz
     if ((0x80 == (ftdata & 0x80))||(this->StopPressed())) {
       wxLogMessage( _("TiniLA stopped"));
       this->UnPressStop();
       m_samples.Clear();
       // cti data
       if (this->StopPressed()) { // tohle je kvuli debuggingu, vypada to,z e se m_timer nezastavuje...
         wxLogMessage( _("TiniLA stopped by press"));
       }
       m_timer.Stop();

       // stop & autoincrement
       d2xx_writereg ( 0x0, 0x08);
       //precti TOP adresu
       d2xx_readreg( 0x01, &ftdata); //cteni bitu 0
       hw_cntr_top = 0x01 & ( ftdata >> 3);
       d2xx_readreg( 0x02, &ftdata); //cteni bitu 8-1
       hw_cntr_top |= (0xff & ftdata) << 1;
       d2xx_readreg( 0x03, &ftdata); //cteni bitu 16-9
       hw_cntr_top |= (0xff & ftdata) << 9;
       bla.Printf(_("Counter: 0x%05X\n"), hw_cntr_top);
       wxLogMessage(bla);
       // reset sramcntr
       d2xx_writereg ( 0x0, 0x20);
       // autoincrement
       d2xx_writereg ( 0x0, 0x10);
       //hw_cntr_top = 10000;
       wxLogMessage( _("Getting data start"));
       // precti data
       for(icnt = 0; icnt < hw_cntr_top; icnt = icnt + NUM_OF_SINGLE_READ/4) {
         d2xx_readreg_multi( 0x0, ftchardata, NUM_OF_SINGLE_READ);
         for( jcnt=0; jcnt < NUM_OF_SINGLE_READ; jcnt++) {
            data32b >>= 8;
            data32b |= (ftchardata[jcnt] <<24);
            if ((jcnt%4) == 3) {
               m_samples.Add( data32b);
               if ( 0 == ((icnt+jcnt/4)%(hw_cntr_top/20))) {
                  m_statusBar->SetGauge( icnt, hw_cntr_top);
               }
            }
         }
         wxYield();
         if (m_stoppressed == true) {
            m_stoppressed = false;
            break;
         }
       }
       // konec cteni dat

       m_statusBar->SetGauge( 100, 100);
       wxLogMessage( _("Getting data end"));
       d2xx_writereg ( 0x0, 0x40);
       wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, EVT_DATA_ARRIVED);
       wxPostEvent( this, event);
       m_timer.Start(1000);
       wxLogMessage( _("TiniLA stopped end"));

     }
  }
}

void FrameMain::OnPanelMouseWheel(wxMouseEvent& event)
{
    int rotation = event.GetWheelRotation();
    int delta = event.GetWheelDelta();
    int scroll_amount = rotation / delta;  // Usually +1 or -1

    if(event.ControlDown()) {  // Ctrl + Wheel: Zoom in/out
       if(scroll_amount > 0) {
           wxCommandEvent zoomEvent(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_ZOOMIN);
           OnZoomIn(zoomEvent);
       } else {
           wxCommandEvent zoomEvent(wxEVT_COMMAND_TOOL_CLICKED, ID_TOOL_ZOOMOUT);
           OnZoomOut(zoomEvent);
       }
    }
    else if(event.ShiftDown()) {  // Shift + Wheel: Scroll horizontally
       int scroll_pixels = scroll_amount * 10;
       long int new_pos = m_scrollH->GetThumbPosition() - scroll_pixels;
       
       if(new_pos < 0) new_pos = 0;
       if(new_pos > m_scrollH->GetRange()) new_pos = m_scrollH->GetRange();
       m_scrollH->SetThumbPosition(new_pos);
       wxScrollEvent scrollEvent(wxEVT_SCROLL_THUMBTRACK, ID_SCROLL_HORIZ);
       scrollEvent.SetPosition(new_pos);
       OnHorizScroll(scrollEvent);
    }
    else { // No modifier: Scroll vertically
       int scroll_pixels = scroll_amount * 10;
       long int new_pos = m_scrollV->GetThumbPosition() - scroll_pixels;
       
       if(new_pos < 0) new_pos = 0;
       if(new_pos > m_scrollV->GetRange()) new_pos = m_scrollV->GetRange();
       m_scrollV->SetThumbPosition(new_pos);
       wxScrollEvent scrollEvent(wxEVT_SCROLL_THUMBTRACK, ID_SCROLL_VERT);
       scrollEvent.SetPosition(new_pos);
       OnVertScroll(scrollEvent);
    }

    event.Skip();
}

void FrameMain::SetCursorPosition(long int samplePos)
{
    if(samplePos >= (long int)m_samples.Count()) {
       samplePos = (long int)m_samples.Count()-1;
    }
    if(samplePos >= 0 && samplePos < (long int)m_samples.Count()) {
        m_cursorX = samplePos;
        m_cursorVisible = true;
        
      // Set cursors
      m_wavePanel->SetCursorX( samplePos) ;
      m_timeScalePanel->SetCursorX( samplePos) ;
      // set data to show
      m_signalPanel->SetData(m_samples.Item(samplePos));

        // Refresh all panels
        m_wavePanel->Refresh();
        m_timeScalePanel->Refresh();
        m_signalPanel->Refresh();
    }
}

//////////////////////////
// status bar
//////////////////////////

MyStatusBar::MyStatusBar(wxWindow *parent)
           : wxStatusBar(parent, wxID_ANY)
{
    wxRect tempRect;
     SetFieldsCount( 4);

     m_statusGauge = new wxGauge( (wxWindow*)this, -1, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);

    GetFieldRect(3, tempRect);
    m_statusGauge->SetSize(tempRect);
}

MyStatusBar::~MyStatusBar()
{
}

void MyStatusBar::OnSize(wxSizeEvent& event)
{
    wxRect tempRect;

    GetFieldRect(3, tempRect);
    m_statusGauge->SetSize(tempRect);

    event.Skip();
}

void MyStatusBar::SetGauge( unsigned int pos, unsigned int max)
{
  m_statusGauge->SetRange( max);
  m_statusGauge->SetValue( pos);
}

BEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
    EVT_SIZE(MyStatusBar::OnSize)
END_EVENT_TABLE()
