#include "stdafx.h"
#include "Octopus.h"
#include "OctopusGrating.h"
#include "OctopusLog.h"

extern COctopusLog*    glob_m_pLog;

COctopusGrating::COctopusGrating(CWnd* pParent)
	: CDialog(COctopusGrating::IDD, pParent)
{    
	if( Create(COctopusGrating::IDD, pParent) ) 
		ShowWindow( SW_SHOW );

	c_stepsize_microns    = 1.0;
	c_middle_microns      = 200.0;
	c_target_microns      = 0.0;
	c_save_microns		  = 0.0;
	c_position_microns    = 0.0;
	
	first_tick          = true;
	initialized			= false;
	connected			= false;

	SetTimer( TIMER_GRAT, 100, NULL );
}

BOOL COctopusGrating::OnInitDialog()
{
     CDialog::OnInitDialog();
     SetWindowPos(NULL, 320, 775, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
     return TRUE;
}

COctopusGrating::~COctopusGrating() {  }

int COctopusGrating::Initialize( void ) 
{

	const char* piezo_string = "0111038443";
  
	ID = PI_ConnectUSB( piezo_string );

	if (ID < 0)
	{
		AfxMessageBox(_T("PI 601 stage not connected"));
	} 
	else
	{
		debug.Format(_T(" PI 601 stage connected "));
		
		if ( glob_m_pLog != NULL ) 
			glob_m_pLog->Write(debug);
		
		connected = true;
	}

   // Get the name of the connected axis 
   if ( !PI_qSAI(ID, axis, 16) )
   {
		AfxMessageBox(_T("PI 601 stage could not get name"));
   }

	/////////////////////////////////////////
	// close the servo loop (closed-loop). //
	/////////////////////////////////////////
	BOOL bFlags[1];

	// Switch on the Servo for all axes
	bFlags[0] = TRUE; // servo on for first axis in the string 'axes'.

	// call the SerVO mode command.
	if(!PI_SVO(ID, axis, bFlags))
	{
		debug.Format(_T(" PI 601 stage SVO failed "));
		if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
		initialized = false;
		PI_CloseConnection(ID);
		AfxMessageBox(_T("PI 601 stage SVO failed"));
		return 1;
	}

	debug.Format(_T(" COctopusGrating::Initialize( void ) "));
	if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);

	initialized = true;
	return 0;
	
}

void COctopusGrating::Close( void ) 
{ 
    KillTimer( TIMER_GRAT );
	Sleep(500);
	initialized = false; 

	if ( connected )
	{
		PI_CloseConnection( ID );
		connected = false;
	}

	debug.Format(_T(" PI 601 stage closed "));
	if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
}

void COctopusGrating::MoveRel( double dist )
{
	MoveTo( c_target_microns + dist );
}

void COctopusGrating::MoveTo( double pos )
{
	if( pos < 0.0 || pos > 400.0 ) return;

	if( connected == false) return;
	
	BOOL bIsMoving[1];
	bIsMoving[0] = TRUE;
	
	PI_IsMoving(ID, NULL, bIsMoving);

	if ( bIsMoving[0] == TRUE ) return; 

	double dPos[1];
	c_target_microns = pos;
	dPos[0] = c_target_microns;
			
	if(!PI_MOV(ID, axis, dPos))
	{
		debug.Format(_T(" PI 601 stage MoveTo failed"));
		if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
	}
	else
	{
		debug.Format(_T(" PI 601 stage MoveTo(%.3f) succeeded"), c_target_microns);
		if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
	}

	c_position_microns = c_target_microns; //this will get refreshed in a moment, via the timer		
}

void COctopusGrating::MoveFwd(  void ) { MoveRel( +c_stepsize_microns ); }
void COctopusGrating::MoveBack( void ) { MoveRel( -c_stepsize_microns ); }

void COctopusGrating::OnStepSize1() { m_Radio_S = 0; c_stepsize_microns = 0.005; UpdateData (false); };
void COctopusGrating::OnStepSize2() { m_Radio_S = 1; c_stepsize_microns = 0.050; UpdateData (false); };
void COctopusGrating::OnStepSize3() { m_Radio_S = 2; c_stepsize_microns = 0.500; UpdateData (false); };
void COctopusGrating::OnStepSize4() { m_Radio_S = 3; c_stepsize_microns = 1.000; UpdateData (false); };
void COctopusGrating::OnStepSize5() { m_Radio_S = 4; c_stepsize_microns = 5.000; UpdateData (false); };

void COctopusGrating::UpdatePosition( void ) 
{ 
	if( connected == false ) return;

	BOOL bIsMoving[1];
	bIsMoving[0] = TRUE;

	PI_IsMoving(ID, NULL, bIsMoving);

	if ( bIsMoving[0] == TRUE ) return; 

	double dPos[1];
	dPos[0] = 0.0; 

	PI_qPOS(ID, axis, dPos);

	c_position_microns = dPos[0];
}

void COctopusGrating::ShowPosition( void )
{
	CString str;

	if( IsWindowVisible() ) 
	{
	    str.Format(_T("X:%.3f\nSave:%.3f"), c_position_microns, c_save_microns);
		m_Pos.SetWindowText( str );
	}
}

void COctopusGrating::Center( void ) 
{	
	MoveTo( c_middle_microns );
}

void COctopusGrating::OnSave( void ) 
{	
	UpdatePosition(); 
	c_save_microns = c_position_microns;
	debug.Format(_T(" COctopusGrating::OnSave( void ) "));
	if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
}

void COctopusGrating::OnSaveGoTo( void ) 
{	
	MoveTo( c_save_microns );
}

void COctopusGrating::OnTimer( UINT nIDEvent ) 
{
	if( nIDEvent == TIMER_GRAT ) 
	{
		if( first_tick ) 
		{
			OnStepSize4();
			Initialize();
			Center();

			first_tick = false;
			
			debug.Format(_T(" OctopusGrating first_tick "));
			if ( glob_m_pLog != NULL ) glob_m_pLog->Write(debug);
		} 
		else 
		{
			UpdatePosition();
			ShowPosition();
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void COctopusGrating::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange( pDX );
	DDX_Radio(pDX,	 IDC_GRAT_SSIZE_1, m_Radio_S);
	DDX_Control(pDX, IDC_GRAT_POS,	   m_Pos);
}  

BEGIN_MESSAGE_MAP(COctopusGrating, CDialog)
	ON_BN_CLICKED(IDC_GRAT_FWD,	        MoveFwd)
	ON_BN_CLICKED(IDC_GRAT_BACK,        MoveBack)
	ON_BN_CLICKED(IDC_GRAT_SAVE,	    OnSave)
	ON_BN_CLICKED(IDC_GRAT_SAVE_GOTO,   OnSaveGoTo)
	ON_BN_CLICKED(IDC_GRAT_CENTER,		Center)
	ON_BN_CLICKED(IDC_GRAT_SSIZE_1,     OnStepSize1)
	ON_BN_CLICKED(IDC_GRAT_SSIZE_2,     OnStepSize2)
	ON_BN_CLICKED(IDC_GRAT_SSIZE_3,     OnStepSize3)
	ON_BN_CLICKED(IDC_GRAT_SSIZE_4,     OnStepSize4)
	ON_BN_CLICKED(IDC_GRAT_SSIZE_5,     OnStepSize5)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void COctopusGrating::OnKillfocusGeneral() { UpdateData( true ); }

BOOL COctopusGrating::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int id = LOWORD(wParam);     // Notification code
	if( id == 2 ) return FALSE;  // Trap ESC key
	if( id == 1 ) return FALSE;  // Trap RTN key
    return CDialog::OnCommand(wParam, lParam);
}