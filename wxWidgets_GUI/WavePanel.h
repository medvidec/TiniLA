#include <wx/wx.h>
#include <wx/image.h>
#include <wx/dynarray.h>


#ifndef _WAVEPANEL_H
#define _WAVEPANEL_H

#define MAXNUMOFSIG 64

#include "arraydef.h"

class FrameMain;

/****************************************************************************
 * TimeScalePanel
 ****************************************************************************/
class TimeScalePanel: public wxPanel {
public:

    TimeScalePanel(wxPanel* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
  TimeScalePanel() {};

    void OnLeftUp( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );
    //void OnEraseBackground( wxEraseEvent& event ) { };

    void SetNewOrigin( long int w, long int h );
    void SetScaleLength( long int w, long int h );
    void SetZoom( float aF) { m_zoom = aF; Refresh();} ;
	void SetSamplePeriod( long int aF) { m_sampfreq_in_ns = aF;Refresh();} ;
   void SetCursorX( int x) { m_cursorX = x; }
   void SetFrameMain(FrameMain* frame) { m_pFrameMain = frame; }

protected:

    long int m_currentW, m_currentH;
    long int m_datalength;
	long int m_sampfreq_in_ns;
   long int m_cursorX;
   FrameMain* m_pFrameMain;  // Pointer to parent frame
   
    float m_zoom;

    DECLARE_DYNAMIC_CLASS(TimeScalePanel)
};


/****************************************************************************
 * SignalListPanel
 ****************************************************************************/
class SignalListPanel: public wxPanel {
public:
    SignalListPanel(wxPanel* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
    SignalListPanel() {};

    void OnLeftUp( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground( wxEraseEvent& event ) { };
    void PaintDblBuf( wxDC & dc);
    void PaintAll( wxDC & dc);

    void SetNewOrigin( long int w, long int h );
    int GetVirtualHeight( void);

    int GetTriggerMask( void);
    int GetTriggerLevel( void);
    
    void SetData( int d) { m_cursorData = d; }
    void SetSampleCount( int d) { m_sampleCount = d; }
    
    wxString GetSignalName( int i) { return m_signalNames[i]; }
    void SetSignalName( int i, wxString n) { m_signalNames[i] = n; }

protected:

    int m_memo[ 32];
    int m_num_of_signals;
    int m_textW, m_textH;
    int m_lefttopX, m_lefttopY, m_textshft, m_signalspan;
    long int m_currentW, m_currentH;
    
    int m_cursorData;
    int m_sampleCount;

    DECLARE_DYNAMIC_CLASS(SignalListPanel)

private:
    wxString m_signalNames[32];  // Store custom signal names
};


/****************************************************************************
 * WavePanel
 ****************************************************************************/
class WavePanel: public wxPanel {
public:
    WavePanel(wxWindow* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
    WavePanel() {};

    void OnEraseBackground( wxEraseEvent& event ) { };
    void OnLeftUp( wxMouseEvent &event );
    void OnPaint( wxPaintEvent &event );
    void PaintDblBuf( wxDC & dc);
    void PaintAll( wxDC & dc);

    void SetNewOrigin( long int w, long int h );
    void SetNumOfSignals( int num) { m_num_of_signals = num; };
    int GetRecWidth( void) { return m_recWidth; }

    void FillData( float zoom, ArrayOfSamples arr);
    void SetZoom( float aF) { m_zoom = aF; Refresh();} ;

    int GetVirtualWidth( void) { return (int)(m_zoom*m_samples.Count()); };
	int ReportDCWidth( void);

   void SetFrameMain(FrameMain* frame) { m_pFrameMain = frame; }
   void SetCursorX( int x) { m_cursorX = x; }

private:
   void OnMouseWheel(wxMouseEvent& event);
   FrameMain* m_pFrameMain;  // Pointer to parent frame
    long int m_cursorX;
     
protected:

    //int m_memo[ MAXNUMOFSIG][MAXNUMOFSAMPLES];
    int m_num_of_signals;
    int m_textW, m_textH;
    int m_lefttopX, m_lefttopY, m_textshft, m_signalspan;
    long int m_currentW, m_currentH;
    int m_recWidth;
    float m_zoom;
    ArrayOfSamples m_samples;

    DECLARE_DYNAMIC_CLASS(WavePanel)
 };

/****************************************************************************
 * HeaderPanel
 ****************************************************************************/
class HeaderPanel: public wxPanel {
public:

   HeaderPanel(wxPanel* parent, int id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);
   HeaderPanel() {};

   void OnPaint( wxPaintEvent &event );

protected:

   DECLARE_DYNAMIC_CLASS(HeaderPanel)
private:
   DECLARE_EVENT_TABLE()

};

#endif // TOPLEVELPANEL_H
