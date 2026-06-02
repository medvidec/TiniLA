/*
 *
 *
 * 2009-06-10 - double buffering for SignalListPanel as well
 * 2009-06-09 - double buffering for WavePanel - OnEraseBackground is essential
 */
#include "FrameMain.h"
#include "WavePanel.h"
// trigger changer images
#include "./images/trig_off.xpm"
#include "./images/trig_01.xpm"
#include "./images/trig_10.xpm"

/****************************************************************************
 * WavePanel
 ****************************************************************************/
IMPLEMENT_DYNAMIC_CLASS(WavePanel, wxPanel)

WavePanel::WavePanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    m_lefttopY = 5;
    m_textshft = 25;
    m_signalspan = 25;
    m_lefttopX = m_signalspan - 20; // the picture is 20x20 pixels large

    m_num_of_signals = 32;
    m_zoom = 1;
    m_samples.Clear();

   m_pFrameMain = NULL;
   m_cursorX = 0;

    Connect( -1, wxEVT_LEFT_UP,(wxObjectEventFunction)& WavePanel::OnLeftUp);
    Connect( -1, wxEVT_PAINT,(wxObjectEventFunction)& WavePanel::OnPaint);
    Connect( -1, wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)& WavePanel::OnEraseBackground);
    Connect(-1, wxEVT_MOUSEWHEEL, (wxObjectEventFunction)& WavePanel::OnMouseWheel);

}

void WavePanel::SetNewOrigin( long int w, long int h )
{
    m_currentW = w;
    m_currentH = h;
    Refresh();
}

void WavePanel::OnLeftUp( wxMouseEvent &event )
{
   if(m_pFrameMain) {
      // Convert screen X position to sample index
      int screenX = event.GetX();
      long int sampleX = (long int)((m_currentW + screenX) / m_zoom + 0.5);
      
      //wxString bla; bla.Printf(_("WavePanel OnPaint: %d sample"),sampleX);
      //wxLogMessage( bla);
      m_pFrameMain->SetCursorPosition(sampleX);
   }

}

void WavePanel::OnPaint( wxPaintEvent &event )
{
   wxPaintDC dc(this) ;  // device context for painting
   PaintDblBuf( dc);
}

void WavePanel::FillData( float zoom, ArrayOfSamples arr)
{
   m_zoom = zoom;
   m_samples.Clear();
   WX_APPEND_ARRAY(m_samples, arr);

//    wxString bla;
//    bla.Printf("New data came %d samples", m_samples.Count());
//    (void)wxMessageBox( bla,_T("MiniLA"), wxICON_INFORMATION | wxOK );

   Refresh();
   //wxClientDC dc( this);
   //PaintDblBuf( dc);
}

void WavePanel::PaintDblBuf( wxDC & dc)
{
  // here will be the Double Buffer
  wxRect rec = GetClientRect();

  wxBitmap* bmpBlit = new wxBitmap(rec.width,rec.height, 32);
  wxMemoryDC* memDC= new wxMemoryDC();
  //clear the memdc with a certain background color
  memDC->SelectObject(*bmpBlit);
  //memDC->BeginDrawing();
  memDC->Clear();

  PaintAll( *memDC);

  //memDC->EndDrawing();

  dc.Blit(rec.x,rec.y,rec.width,rec.height,memDC,0,0,wxCOPY);

  delete bmpBlit;
  delete memDC;

}

void WavePanel::PaintAll( wxDC & dc)
{
   int i_cnt;
   int j_cnt;
    wxString bla;
    wxBrush tmp_brush;
    wxPen tmp_pen;
    int W,H;
    
    unsigned int samplesSize = m_samples.GetCount();
    wxRect rec = GetClientRect();
    W = rec.width;
    H = rec.height;

    m_recWidth = rec.width;

    dc.SetFont( wxFont( 10 , wxSWISS, wxNORMAL, wxNORMAL, false));
    dc.GetTextExtent(_T("D00 = x"),&m_textW,&m_textH);

    tmp_brush = dc.GetBrush();
    tmp_pen = dc.GetPen();

    // cerne pozadi na celoou plochu
    dc.SetPen(wxPen(wxColor(0,0,0),1,wxSOLID));
    dc.SetBrush(wxBrush(wxColor(0,0,0),wxSOLID));
    dc.DrawRectangle( 0, 0, W, H);

    // Signaly jsou oddeleny sedou tenkou carou
    for( i_cnt = 1; i_cnt <= m_num_of_signals; i_cnt++) {
       dc.SetPen(wxPen(wxColor(128,128,128),1,wxSOLID));
       //dc.SetBrush(wxBrush(wxColor(128,128,128),wxSOLID));
       dc.DrawLine( 0, i_cnt*m_signalspan-m_currentH+m_lefttopY+1, W, i_cnt*m_signalspan-m_currentH+m_lefttopY+1);
    }

    dc.SetBrush( tmp_brush);
    dc.SetPen( tmp_pen);
    dc.SetTextForeground( wxColor(0,255,0)); // green color

   // new function to paint samples
   unsigned long sample_act, sample_next;
   unsigned int sample_act_level, sample_next_level, k_cnt;
   
   dc.SetPen(wxPen(wxColor( 0, 255, 0),1,wxSOLID));
   for( i_cnt = 1; i_cnt <= m_num_of_signals; i_cnt++) {
      for( j_cnt = 0; j_cnt < W; j_cnt++) { // X index on the screen
	     sample_act = (unsigned long)((m_currentW + j_cnt)/m_zoom); // actual index in the samples array
		 if (sample_act >= samplesSize) { break; }
		 sample_next = (unsigned long)((m_currentW + j_cnt+1)/m_zoom); // next index into the array
		 if (sample_next >= samplesSize) { break; }
		 sample_act_level = (m_samples.Item( sample_act)) & (1<<(i_cnt-1)); //actual level
		 
		 // check if sample is changed during the period
		 sample_next_level = (m_samples.Item( sample_next)) & (1<<(i_cnt-1)); //actual level
		 for ( k_cnt = sample_act; k_cnt < sample_next; k_cnt++) {
			if ( sample_next_level != sample_act_level) {
			   break;
			}
		    sample_next_level = ((m_samples.Item( k_cnt)) & (1<<(i_cnt-1)));
		 }
		 if (sample_next_level != sample_act_level) {
		    // vertical line
			dc.DrawLine( j_cnt, i_cnt*m_signalspan-m_currentH+m_lefttopX-19, j_cnt, i_cnt*m_signalspan-m_currentH+m_lefttopX-3);
		 }
		 if ( 0 != sample_act_level) {
		    dc.DrawPoint( j_cnt, i_cnt*m_signalspan-m_currentH+m_lefttopX-19);
		 } else {
		    dc.DrawPoint( j_cnt, i_cnt*m_signalspan-m_currentH+m_lefttopX-3);
		 }
		 
	     
	  } // for ( j_cnt - screen width )
   } // for (i_cnt - num_of_signals)

    // Draw cursor line if visible
    if(1) {    //if(m_pFrameMain && m_pFrameMain->IsCursorVisible()) {
        long int cursorScreenX = (long int)(m_cursorX * m_zoom - m_currentW);
        
        if(cursorScreenX >= 0 && cursorScreenX < (long int)W) {
            dc.SetPen(wxPen(wxColor(255, 0, 0), 2, wxSOLID));  // Red cursor line
            dc.DrawLine(cursorScreenX, 0, cursorScreenX, H);
        }
    }
}

int WavePanel::ReportDCWidth( void)
{
    wxRect rec = GetClientRect();
	return rec.width;
}

void WavePanel::OnMouseWheel(wxMouseEvent& event)
{
    if(m_pFrameMain) {
        m_pFrameMain->OnPanelMouseWheel(event);
    }
}

/****************************************************************************
 * SignalListPanel
 ****************************************************************************/
IMPLEMENT_DYNAMIC_CLASS(SignalListPanel, wxPanel)

SignalListPanel::SignalListPanel(wxPanel* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel( parent, id, pos, size, wxTAB_TRAVERSAL)
{
    int i_cnt;
    //int W,H;
    wxClientDC dc(this);

    m_lefttopY = 5;
    m_textshft = 25;
    m_signalspan = 25;
    m_lefttopX = m_signalspan - 20; // the picture is 20x20 pixels large
    m_sampleCount = 0;

    PrepareDC( dc);
    //dc.SetFont( wxFont( 10 , wxSWISS, wxNORMAL, wxNORMAL, false));
    //dc.GetTextExtent(_T("D00 = x"),&W,&H);
    //SetSize(wxSize(2*m_lefttopX+m_textshft+W, -1));

    Connect( -1, wxEVT_LEFT_UP,(wxObjectEventFunction)& SignalListPanel::OnLeftUp);
    Connect( -1, wxEVT_PAINT,(wxObjectEventFunction)& SignalListPanel::OnPaint);
    Connect( -1, wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)& SignalListPanel::OnEraseBackground);

    m_num_of_signals = 32;

   // Prepare signal names
   for(i_cnt = 0; i_cnt < m_num_of_signals; i_cnt++) {
       m_signalNames[i_cnt].Printf(_T("Channel %02d"), i_cnt);
   }

    for( i_cnt =0; i_cnt < m_num_of_signals; i_cnt++) {
      m_memo[i_cnt] = 0;
    }
}

int SignalListPanel::GetVirtualHeight( void)
{
    return m_num_of_signals * m_signalspan + 2 * m_lefttopX;
}

void SignalListPanel::OnLeftUp( wxMouseEvent &event )
{
    int LogPosX, LogPosY;
    int setref = 0;
    int i_cnt;

    event.GetPosition(&LogPosX, &LogPosY);

    if(LogPosX<80) {
        // Right click - show rename dialog
        for(i_cnt = 0; i_cnt < m_num_of_signals; i_cnt++) {
            if((m_lefttopY + 20 + m_signalspan * i_cnt - m_currentH > LogPosY) &&
               (m_lefttopY + m_signalspan * i_cnt - m_currentH < LogPosY)) {
                
                wxTextEntryDialog dlg(NULL, 
                    wxString::Format(_T("Enter name for Channel %02d:"), i_cnt),
                    _T("Rename Signal"),
                    m_signalNames[i_cnt]);
                
                if(wxID_OK == dlg.ShowModal()) {
                    m_signalNames[i_cnt] = dlg.GetValue();
                    if(m_signalNames[i_cnt].Length() > 15) {
                        m_signalNames[i_cnt] = m_signalNames[i_cnt].Left(15);
                    }
                    Refresh();
                }
                return;
            }
        }
    }
    else {
        // Left click - toggle trigger state
        int col2_x = m_lefttopX + 80;

        for(i_cnt = 0; i_cnt < m_num_of_signals; i_cnt++) {
            int y_pos = m_lefttopY + m_signalspan * i_cnt - m_currentH;
            
            if((col2_x < LogPosX) &&
               (col2_x + 25 > LogPosX) &&
               (y_pos + 2 < LogPosY) &&
               (y_pos + 22 > LogPosY)) {

                if(0 == m_memo[i_cnt]) {
                    m_memo[i_cnt] = 1;  // No edge -> Rising
                } 
                else if(1 == m_memo[i_cnt]) {
                    m_memo[i_cnt] = 2;  // Rising -> Falling
                } 
                else {
                    m_memo[i_cnt] = 0;  // Falling -> No edge
                }
                setref = 1;
            }
        }
    }

    if(1 == setref) {
        Refresh();
    }
}

void SignalListPanel::OnPaint( wxPaintEvent &event )
{

    wxPaintDC dc(this);
    //PrepareDC( dc);

    PaintDblBuf( dc);
}

void SignalListPanel::PaintDblBuf( wxDC & dc)
{
  // here will be the Double Buffer
  wxRect rec = GetClientRect();
  
  wxBitmap* bmpBlit = new wxBitmap(rec.width,rec.height, 32);
  wxMemoryDC* memDC= new wxMemoryDC();
  //clear the memdc with a certain background color
  memDC->SelectObject(*bmpBlit);
  //memDC->BeginDrawing();
  memDC->Clear();

  PaintAll( *memDC);

  //memDC->EndDrawing();

  dc.Blit(rec.x,rec.y,rec.width,rec.height,memDC,0,0,wxCOPY);

  delete bmpBlit;
  delete memDC;

}


void SignalListPanel::PaintAll( wxDC & dc)
{
    int i_cnt, offset;
    wxString bla;
    wxBrush tmp_brush;
    wxPen tmp_pen;
    int W, H;
    GetSize(&W, &H);

    dc.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false));
    dc.GetTextExtent(_T("Channel 32"), &m_textW, &m_textH);

    tmp_brush = dc.GetBrush();
    tmp_pen = dc.GetPen();

    // Light grey background (same as HeaderPanel)
    dc.SetPen(wxPen(wxColour(236,233,216), 1, wxSOLID));
    dc.SetBrush(wxBrush(wxColour(236,233,216), wxSOLID));
    dc.DrawRectangle(0, 0, W, H);

    // Signals are separated by grey lines
    for(i_cnt = 1; i_cnt <= m_num_of_signals; i_cnt++) {
        dc.SetPen(wxPen(wxColor(128, 128, 128), 1, wxSOLID));
        dc.DrawLine(0, i_cnt * m_signalspan - m_currentH + m_lefttopY + 1, 
                    W, i_cnt * m_signalspan - m_currentH + m_lefttopY + 1);
    }

    // Signal names, trigger icons, and cursor value
    dc.SetBrush(tmp_brush);
    dc.SetPen(tmp_pen);
    dc.SetTextForeground(wxColor(0, 0, 0));  // Black text
    dc.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false));

    int col1_x = m_lefttopX;                    // Channel name column
    int col2_x = m_lefttopX + 80;              // Trigger icon column
    int col3_x = col2_x + 50;                   // Cursor value column
    int max_col_width = 80 - 5;   // Max width for channel name

    for(i_cnt = 0; i_cnt < m_num_of_signals; i_cnt++) {
        int y_pos = m_lefttopY + m_signalspan * i_cnt - m_currentH;

        // Column 1: Channel name
        wxString displayText = m_signalNames[i_cnt];
        int textWidth = 0;
        dc.GetTextExtent(displayText, &textWidth, NULL);
        
        while(textWidth > max_col_width) {
            // Add ellipsis and truncate
            displayText = displayText.Left(displayText.Len()-1);  // "Channel " is 8 chars, plus 2 more for number
            dc.GetTextExtent(displayText, &textWidth, NULL);
        }
        
        offset = (m_signalspan - m_textH) / 2;
        dc.DrawText(displayText, col1_x, y_pos + offset+1);

        // Column 2: Trigger icon (rising/falling/no edge)
        // Draw trigger box background
        //dc.SetPen(wxPen(wxColor(100, 100, 100), 1, wxSOLID));
        //dc.SetBrush(wxBrush(wxColor(220, 220, 220), wxSOLID));
        //dc.DrawRectangle(col2_x, y_pos, 20, 20);

        // Draw trigger state icon based on m_memo
        //dc.SetPen(wxPen(wxColor(0, 0, 0), 1, wxSOLID));
        //dc.SetBrush(wxBrush(wxColor(0, 0, 0), wxSOLID));
        
        int icon_x = col2_x;
        int icon_y = y_pos+offset;

        if(0 == m_memo[i_cnt]) {
            // No edge - draw circle
            dc.DrawBitmap(wxBitmap(trig_off_xpm), icon_x, icon_y, FALSE);
        } 
        else if(1 == m_memo[i_cnt]) {
            // Rising edge - draw up arrow
            dc.DrawBitmap(wxBitmap(trig_01_xpm), icon_x, icon_y, FALSE);
        } 
        else if(2 == m_memo[i_cnt]) {
            // Falling edge - draw down arrow
            dc.DrawBitmap(wxBitmap(trig_10_xpm), icon_x, icon_y, FALSE);
        }

        // Column 3: Cursor value (reserved for future use)
        dc.SetTextForeground(wxColor(255, 0, 0));  // Grey red text
        dc.SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false));
        if( 0 == m_sampleCount) {
          dc.DrawText(_T("--"), col3_x, y_pos + offset);
        } else {
          if(m_cursorData & (1<<(i_cnt))) {
             dc.DrawText(_T("1"), col3_x+4, y_pos + offset);
          } else {
             dc.DrawText(_T("0"), col3_x+4, y_pos + offset);
          }
        }
        dc.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false));
        dc.SetTextForeground(wxColor(0, 0, 0));
    }
}

void SignalListPanel::SetNewOrigin( long int w, long int h )
{
  m_currentW = w;
  if ( m_currentH != h) {
    m_currentH = h;
    Refresh();
  }
}

int SignalListPanel::GetTriggerMask( void)
{
   int mask = 0x01;
   int icnt;
   int retval = 0;

   // pokud je m_memo[i] nenulove, pak je maska
   for( icnt = 0; icnt < 16; icnt++) {
      if ( m_memo[icnt] != 0) {
         retval |= mask;
      }
      mask <<= 1;
   }

   return retval;
}

int SignalListPanel::GetTriggerLevel( void)
{
   int mask = 0x01;
   int icnt;
   int retval = 0;

   // pokud je m_memo[i] == 2, pak je hodnota v jednicce
   for( icnt = 0; icnt < 16; icnt++) {
      if ( m_memo[icnt] == 1) {
         retval |= mask;
      }
      mask <<= 1;
   }

   return retval;

}

/****************************************************************************
 * TimeScalePanel
 ****************************************************************************/
IMPLEMENT_DYNAMIC_CLASS(TimeScalePanel, wxPanel)

TimeScalePanel::TimeScalePanel( wxPanel* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel( parent, id, pos, size, wxTAB_TRAVERSAL)
{
    int W,H;
    wxClientDC dc(this);

    m_currentH = 0;
    m_currentW = 0;
    m_zoom = 1;
   m_pFrameMain = NULL;
   m_cursorX = 0;

    PrepareDC( dc);
    dc.GetTextExtent(_T("D00 = x"),&W,&H);
    SetSize(wxSize(256, H+15));
    SetBackgroundColour(wxColor(0,0,0));

    Connect( -1, wxEVT_LEFT_UP,(wxObjectEventFunction)& TimeScalePanel::OnLeftUp);
    Connect( -1, wxEVT_PAINT,(wxObjectEventFunction)& TimeScalePanel::OnPaint);
    //Connect( -1, wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)& TimeScalePanel::OnEraseBackground);
}

void TimeScalePanel::OnPaint( wxPaintEvent &event )
{
    int W,H, i_cnt;
    wxPaintDC dc(this);

    GetSize( &W, &H);

    PrepareDC( dc);

    dc.SetPen(wxPen(wxColor(255,255,255), 1, wxSOLID));
    dc.SetBrush(wxBrush(wxColor(0,0,0), wxSOLID));
    dc.SetTextForeground(wxColor(255,255,255));
    dc.SetFont( wxFont( 10 , wxSWISS, wxNORMAL, wxNORMAL, false));

    // kreslime pravitko
    dc.DrawLine( 0, 10, W, 10);

    // Small ticks every 20 pixels, large ticks every 100 pixels
    int SMALL_STEP = 20;
    int LARGE_STEP = 100;

    // Calculate time range visible on screen
    // At position m_currentW (left edge) to m_currentW + W (right edge)
    // Each pixel represents (1 / m_zoom) samples when m_zoom is pixels_per_sample
    
   double samples_per_pixel = 1.0 / m_zoom;
   double total_samples_visible = samples_per_pixel * W;    

    // Calculate time in nanoseconds for the visible span
    // To avoid overflow: (samples / m_zoom) * m_sampfreq_in_ns
    // But safer: samples * (m_sampfreq_in_ns / m_zoom) or use double
    double time_span_ns = 0;
    if(m_sampfreq_in_ns > 0) {
        time_span_ns = (double)total_samples_visible * m_sampfreq_in_ns / m_zoom;
    }

    // Determine optimal time interval for major ticks (LARGE_STEP pixels apart)
    // Time represented by LARGE_STEP pixels
    double samples_per_large_tick = samples_per_pixel * LARGE_STEP;
    double time_per_large_tick_ns = 0;
    if(m_sampfreq_in_ns > 0) {
        time_per_large_tick_ns = (double)samples_per_large_tick * m_sampfreq_in_ns / m_zoom;
    }

    // Determine best unit and round time interval to nice numbers (1, 2, 5, 10, ...)
    enum TimeUnit { UNIT_NS, UNIT_US, UNIT_MS, UNIT_S } displayUnit;
    double nice_interval = 0;

    if(m_sampfreq_in_ns == 0) {
        // No sampling frequency - just show sample numbers
        displayUnit = UNIT_NS;  // Dummy unit, we'll show samples instead
        nice_interval = 1.0;
    } else {
        // Auto-select unit based on time_per_large_tick_ns
        if(time_per_large_tick_ns >= 1000000000.0) {
            displayUnit = UNIT_S;
            nice_interval = time_per_large_tick_ns / 1000000000.0;
        } else if(time_per_large_tick_ns >= 1000000.0) {
            displayUnit = UNIT_MS;
            nice_interval = time_per_large_tick_ns / 1000000.0;
        } else if(time_per_large_tick_ns >= 1000.0) {
            displayUnit = UNIT_US;
            nice_interval = time_per_large_tick_ns / 1000.0;
        } else {
            displayUnit = UNIT_NS;
            nice_interval = time_per_large_tick_ns;
        }

        // Round to nice number (1, 2, 5, 10, 20, 50, 100, ...)
        if(nice_interval > 0) {
            double magnitude = pow(10.0, floor(log10(nice_interval)));
            double normalized = nice_interval / magnitude;
            
            if(normalized < 1.5) {
                nice_interval = 1.0 * magnitude;
            } else if(normalized < 3.0) {
                nice_interval = 2.0 * magnitude;
            } else if(normalized < 7.0) {
                nice_interval = 5.0 * magnitude;
            } else {
                nice_interval = 10.0 * magnitude;
            }
        }
    }

    // Convert nice_interval back to nanoseconds
    double nice_interval_ns = nice_interval;
    if(displayUnit == UNIT_US) {
        nice_interval_ns = nice_interval * 1000.0;
    } else if(displayUnit == UNIT_MS) {
        nice_interval_ns = nice_interval * 1000000.0;
    } else if(displayUnit == UNIT_S) {
        nice_interval_ns = nice_interval * 1000000000.0;
    }

    // Draw ticks and labels
    i_cnt = 0;
    while((i_cnt * SMALL_STEP - m_currentW) < W) {
        int screen_x = i_cnt * SMALL_STEP - m_currentW;
        
        if(screen_x >= 0) {
            if((i_cnt % 5) == 0) {
                // Large tick
                dc.DrawLine(screen_x, 5, screen_x, 15);

                // Calculate time/sample value at this position
                wxString label;
                if(m_sampfreq_in_ns == 0) {
                    // Show sample number
                    double sample_num = (m_currentW + screen_x) / m_zoom;
                    label = wxString::Format(_T("%.0f"), sample_num);
                } else {
                    // Calculate time in nanoseconds at this screen position
                    double screen_pos = m_currentW + screen_x;
                    double sample_pos = screen_pos / m_zoom;
                    double time_ns = sample_pos * m_sampfreq_in_ns;

                    // Format according to selected unit
                    switch(displayUnit) {
                        case UNIT_NS:
                            if(time_ns < 1e6) {
                                label = wxString::Format(_T("%.0f ns"), time_ns);
                            } else {
                                label = wxString::Format(_T("%.2e ns"), time_ns);
                            }
                            break;
                        case UNIT_US:
                            label = wxString::Format(_T("%.1f us"), time_ns / 1000.0);
                            break;
                        case UNIT_MS:
                            label = wxString::Format(_T("%.1f ms"), time_ns / 1000000.0);
                            break;
                        case UNIT_S:
                            label = wxString::Format(_T("%.3f s"), time_ns / 1000000000.0);
                            break;
                    }
                }
                
                dc.DrawText(label, screen_x - 15, 15);
            } else {
                // Small tick
                dc.DrawLine(screen_x, 8, screen_x, 10);
            }
        }
        i_cnt++;
    }
    // Paint cursor if visible
    if(1) {
        int cursorScreenX = (int)(1.0*m_cursorX * m_zoom - m_currentW);

        if(cursorScreenX >= 0 && cursorScreenX < W) {
            dc.SetPen(wxPen(wxColor(255, 0, 0), 2, wxSOLID));  // Red cursor line
            dc.DrawLine(cursorScreenX, 0, cursorScreenX, H);

            // Display cursor time below the ruler
            wxString cursorLabel;
            if(m_sampfreq_in_ns == 0) {
                cursorLabel = wxString::Format(_T("Sample: %.0f"), 1.0*m_cursorX);
            } else {
                double cursor_time_ns = 1.0*m_cursorX * m_sampfreq_in_ns;

                switch(displayUnit) {
                    case UNIT_NS:
                        if(cursor_time_ns < 1e6) {
                            cursorLabel = wxString::Format(_T("%.0f ns"), cursor_time_ns);
                        } else {
                            cursorLabel = wxString::Format(_T("%.2e ns"), cursor_time_ns);
                        }
                        break;
                    case UNIT_US:
                        cursorLabel = wxString::Format(_T("%.1f us"), cursor_time_ns / 1000.0);
                        break;
                    case UNIT_MS:
                        cursorLabel = wxString::Format(_T("%.1f ms"), cursor_time_ns / 1000000.0);
                        break;
                    case UNIT_S:
                        cursorLabel = wxString::Format(_T("%.3f s"), cursor_time_ns / 1000000000.0);
                        break;
                }
            }

            // Draw cursor label to the right of the cursor line
            dc.SetTextForeground(wxColor(255,0,0));
            if(cursorScreenX + 70 < W) {
                dc.DrawText(cursorLabel, cursorScreenX + 5, 27);
            } else {
                dc.DrawText(cursorLabel, cursorScreenX - 65, 27);
            }
        }
    }
    // paint cursor
/*    if(1) {
        //long int cursorSamplePos = m_pFrameMain->GetCursorPosition();
        long int cursorScreenX = (long int)(m_cursorX * m_zoom - m_currentW);
        
        if(cursorScreenX >= 0 && cursorScreenX < (long int)W) {
            dc.SetPen(wxPen(wxColor(255, 0, 0), 2, wxSOLID));  // Red cursor line
            dc.DrawLine(cursorScreenX, 0, cursorScreenX, H);
        }
    }
*/
}

void TimeScalePanel::OnLeftUp( wxMouseEvent &event )
{
   if(m_pFrameMain) {
      // Convert screen X position to sample index
      int screenX = event.GetX();
      long int sampleX = (long int)((m_currentW + screenX) / m_zoom + 0.5);
      
      m_pFrameMain->SetCursorPosition(sampleX);
   }

}

void TimeScalePanel::SetNewOrigin( long int w, long int h )
{
    m_currentH = h;
    if (m_currentW != w) {
      m_currentW = w;
      Refresh();
    }
}

/****************************************************************************
 * HeaderPanel
 ****************************************************************************/
IMPLEMENT_DYNAMIC_CLASS(HeaderPanel, wxPanel)

HeaderPanel::HeaderPanel(wxPanel* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    wxPanel( parent, id, pos, size, wxTAB_TRAVERSAL)
{

    wxClientDC dc(this);

    PrepareDC( dc);

    // Set minimum size based on text dimensions
    SetMinSize(wxSize(165, 40));
    SetBackgroundColour(wxColour(236,233,216));
}

void HeaderPanel::OnPaint( wxPaintEvent &event )
{

    int W,H;
    int winW,winH;
    wxPaintDC dc(this);

    GetSize(&winW, &winH);

   dc.SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false));
   dc.GetTextExtent(_T("Signal"),&W,&H);
   dc.DrawText(_T("Signal"), (85-W)/2, winH-H);
   dc.GetTextExtent(_T("Trigger"),&W,&H);
   dc.DrawText(_T("Trigger"), 85+(20-W)/2, winH-H);
   dc.GetTextExtent(_T("Value"),&W,&H);
   dc.DrawText(_T("Value"), 115+(50-W)/2, winH-H);
}

BEGIN_EVENT_TABLE(HeaderPanel, wxPanel)
    EVT_PAINT(HeaderPanel::OnPaint)
END_EVENT_TABLE()

/***************************************************************************/
