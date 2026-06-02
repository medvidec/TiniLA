#include "DlgConfigApp.h"

DlgConfigApp::DlgConfigApp( wxWindow* parent )
:
DialogConfigApp( parent )
{

}

void DlgConfigApp::OnButtonOKClick( wxCommandEvent& event )
{
   
	// TODO: Implement OnButtonOKClick
   event.Skip();
}

void DlgConfigApp::OnButtonCancelClick( wxCommandEvent& event )
{
	// TODO: Implement OnButtonCancelClick
   event.Skip();
}

void DlgConfigApp::SetDevice( wxString str)
{

}
wxString DlgConfigApp::GetDevice( void)
{

   return( _T("/dev/ttyACM0"));
}
