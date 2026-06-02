
#include <wx/wx.h>
#include <wx/image.h>
#include "FrameMain.h"

/****************************************************************************
 * MainApp
 ****************************************************************************/

class TiniLAApp: public wxApp {
public:
    bool OnInit();
};

IMPLEMENT_APP(TiniLAApp)

bool TiniLAApp::OnInit()
{
    wxInitAllImageHandlers();

    FrameMain* frame_main = new FrameMain( NULL);
    SetTopWindow(frame_main);
#ifdef __WXMSW__
    frame_main->SetIcon(wxICON(AppIcon));
#endif
    frame_main->Show();

    wxLogMessage(_("This is a log window"));

    return true;
}
