#include "stdafx.h"
#include "Octopus.h"
#include "OctopusLCPolarizer.h"
#include "OctopusCameraDlg.h"
#include "OctopusObjectivePiezo.h"
#include "math.h"
#include "usbdrvd.h"

extern COctopusGlobals B;
extern COctopusPolarizerLC*     glob_m_pPolarizerLC;

COctopusPolarizerLC::COctopusPolarizerLC(CWnd* pParent)
	: CDialog(COctopusPolarizerLC::IDD, pParent)
{    

	B.position_degrees_T	 = 0.0;

	Initialized				 =  false;
	B.POL_loaded			 =  false;
	first_tick				 =   true;
	B.POL_scan_in_progress   =  false;
	current_angle_deg        =    0.0;
	save_pos_deg_T			 =   90.0;	
	stepsize_deg_T			 =    4.0;
	scan_start_deg_T		 =   90.0;
	offset_T				 =   -5.0;

	if( Create(COctopusPolarizerLC::IDD, pParent) ) 
		ShowWindow( SW_SHOW );

}

BOOL COctopusPolarizerLC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowPos(NULL, 9, 430, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	Initialize();

	return TRUE;
}

void COctopusPolarizerLC::OnTimer( UINT nIDEvent ) 
{
	if( nIDEvent == TIMER_POL_POS ) 
	{
		if( first_tick ) 
		{
			first_tick = false;
		}
		if( Initialized && !B.POL_scan_in_progress ) 
		{
			GetPosition();
			ShowPosition();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void COctopusPolarizerLC::DoDataExchange(CDataExchange* pDX) 
{
	CDialog::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_POL_REP_STEP,	    m_Step); // which step?
	DDX_Control( pDX, IDC_POL_REP_POS, 	    m_Pos);  // current position

	//size of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_SIZE, scan_step_size_deg);
	DDV_MinMaxDouble(pDX, scan_step_size_deg, 0.001, 90.0);

	//number of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_NUMBER, scan_number_of_steps);
	DDV_MinMaxUInt(pDX, scan_number_of_steps, 1, 1000);

	//size of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_SIZE_Z, scan_step_size_microns_z);
	DDV_MinMaxDouble(pDX, scan_step_size_microns_z, 0.001, 5.0);

	//number of steps
	DDX_Text(   pDX, IDC_POL_SCAN_STEP_NUMBER_Z, scan_number_of_steps_z);
	DDV_MinMaxUInt(pDX, scan_number_of_steps_z, 0, 200);

	DDX_Control(pDX, IDC_POL_ROT,  m_ctl_Rot_CheckBox);
	DDX_Control(pDX, IDC_POL_ROT2, m_ctl_Rot_CheckBox2);
	DDX_Control(pDX, IDC_POL_ROT3, m_ctl_Rot_CheckBox3);

}  

BEGIN_MESSAGE_MAP(COctopusPolarizerLC, CDialog)
	
	ON_BN_CLICKED(IDC_POL_CW,			      MoveCW)
	ON_BN_CLICKED(IDC_POL_CCW,			      MoveCCW)
	ON_BN_CLICKED(IDC_POL_SAVE,				  SavePositionSet)
	ON_BN_CLICKED(IDC_POL_SAVE_GOTO,		  SavePositionGoTo)

	ON_BN_CLICKED(IDC_POL_SCAN,		          ScanStartStop)
	ON_BN_CLICKED(IDC_POL_SCAN_SET_START,     ScanSetStart)
	ON_BN_CLICKED(IDC_POL_ZERO,			      Zero)

	ON_EN_KILLFOCUS(IDC_POL_SCAN,			    OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_SIZE,     OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_NUMBER,   OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_SIZE_Z,   OnKillfocusGeneral)
	ON_EN_KILLFOCUS(IDC_POL_SCAN_STEP_NUMBER_Z, OnKillfocusGeneral)
	
	ON_BN_CLICKED(IDC_POL_ROT,                   OnBnClickedRot)
	ON_BN_CLICKED(IDC_POL_ROT2,                  OnBnClickedRot2)
	ON_BN_CLICKED(IDC_POL_ROT3,                  OnBnClickedRot3)
	ON_WM_TIMER()

END_MESSAGE_MAP()

void COctopusPolarizerLC::OnKillfocusGeneral() { UpdateData( true ); }
COctopusPolarizerLC::~COctopusPolarizerLC() {  }

void COctopusPolarizerLC::MoveCW(  void ) { MoveRel( +1 * stepsize_deg_T ); }
void COctopusPolarizerLC::MoveCCW( void ) { MoveRel( -1 * stepsize_deg_T ); }

void COctopusPolarizerLC::MoveRel( double deg ) 
{ 
	double target = current_angle_deg + deg;

	if( target < 0.0 ) 
		target = 0.0;
	if( target >= 180.0 ) 
		target = 0.0;

	MoveTo( target );
	
	current_angle_deg = target;
}

void COctopusPolarizerLC::MoveTo( double deg ) 
{ 
	double target = 0.0;
	
	if( target <    0.0 ) target = 0.0;
	if( target >= 180.0 ) target = fmod(target, 180.0);
	
	float target_volt = DegToVolt( deg );

}

float COctopusPolarizerLC::DegToVolt( float deg ) 
{ 
	if( deg <    0.0) deg = 0.0;
	if( deg >= 180.0) deg = 0.0;
	
    double x       = deg;
	double voltage = 0.0;

	voltage = 1.17092                 + \
		     (0.0199711   *        x) - \
			 (3.15114e-4  * pow(x,2)) + \
			 (3.65746e-6  * pow(x,3)) - \
			 (1.99273e-8  * pow(x,4)) + \
			 (4.36914e-11 * pow(x,5));
	
	if ( voltage >= 3.22 ) voltage = 3.22;
	if ( voltage <   0.0 ) voltage =  0.0;
	
	return voltage;

}
void COctopusPolarizerLC::Initialize( void ) 
{
	/*
	Polarizer_T.SetHWSerialNum(83819652);
	Polarizer_T.StartCtrl();
	Polarizer_T.SetRotStageModes( 0, 3, 1 );
	Polarizer_T.MoveHome( 0, false );
	Polarizer_T.SetVelParams( 0, 5.0, 20.0, 20.0 );
	Polarizer_T.SetBLashDist( 0, 0.0 );
	MoveAE_T( scan_start_deg_T, true );
	*/
	//Polarizer_B.SetHWSerialNum(83823650);
	//Polarizer_B.StartCtrl();
	//Polarizer_B.SetRotStageModes( 0, 3, 1 );
	//Polarizer_B.MoveHome( 0, true );
	//Polarizer_B.SetVelParams( 0, 5.0, 20.0, 20.0 );
	//Polarizer_B.SetBLashDist( 0, 0.0 );
	//MoveAE_B( scan_start_deg_B, true );

#define  flagsandattrs  0x40000000

BYTE ver_cmd[] = {'v', 'e', 'r', ':', '?', '\n'};
BYTE status[64];

HANDLE dev1, pipe0, pipe1;

UINT devcnt, i;

GUID  theGUID;

theGUID.Data1 = 0xa22b5b8b;
theGUID.Data2 = 0xc670;
theGUID.Data3 = 0x4198;
theGUID.Data4[0] = 0x93;
theGUID.Data4[1] = 0x85;
theGUID.Data4[2] = 0xaa;
theGUID.Data4[3] = 0xba;
theGUID.Data4[4] = 0x9d;
theGUID.Data4[5] = 0xfc;
theGUID.Data4[6] = 0x7d;
theGUID.Data4[7] = 0x2b;

devcnt = USBDRVD_GetDevCount (&theGUID);

if (devcnt == 0)
   {
	cout<<"No Meadowlark Optics USB Devices Present."<<endl;
   }
else
	{
	/* open device and pipes */
	dev1  = USBDRVD_OpenDevice( 1, flagsandattrs, &theGUID);
	pipe0 = USBDRVD_PipeOpen(1, 0, flagsandattrs, &theGUID);
	pipe1 = USBDRVD_PipeOpen(1, 1, flagsandattrs, &theGUID);

	/* send ver:? command */
	USBDRVD_BulkWrite(dev1, 1, ver_cmd, sizeof(ver_cmd));

   /* read status response */
	USBDRVD_BulkRead(dev1, 0, status, sizeof(status));

   cout<<endl;

   /* output status until a <CR> is found */
   for (i = 0; status[i] != 0xd; i++)
   {
      cout<<status[i];
   }

   cout<<endl;

	/* close device and pipes */
	USBDRVD_PipeClose (pipe0);
	USBDRVD_PipeClose (pipe1);
	USBDRVD_CloseDevice (dev1);
	}




	Initialized  = true;
	B.POL_loaded = true;

	SetTimer( TIMER_POL_POS, 500, NULL ); //this is for position display
}

void COctopusPolarizerLC::OnBnClickedRot()
{
    int l_ChkBox = m_ctl_Rot_CheckBox.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Both = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Both = true;
		PolRot_Only_Pol = false; CheckDlgButton(IDC_POL_ROT2, BST_UNCHECKED);
		PolRot_Only_Z   = false; CheckDlgButton(IDC_POL_ROT3, BST_UNCHECKED);
	}
	UpdateData( false );
}

void COctopusPolarizerLC::OnBnClickedRot2()
{
    int l_ChkBox = m_ctl_Rot_CheckBox2.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Only_Pol = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Only_Pol = true;
		PolRot_Both     = false; CheckDlgButton(IDC_POL_ROT , BST_UNCHECKED);
		PolRot_Only_Z   = false; CheckDlgButton(IDC_POL_ROT3, BST_UNCHECKED);
	}
	UpdateData( false );
}

void COctopusPolarizerLC::OnBnClickedRot3()
{
	//Z only
    int l_ChkBox = m_ctl_Rot_CheckBox3.GetCheck();
	
	if (l_ChkBox == 0) 
		PolRot_Only_Z = false;
	else if (l_ChkBox == 1) 
	{
		PolRot_Only_Pol  = false; CheckDlgButton(IDC_POL_ROT2, BST_UNCHECKED);
		PolRot_Both      = false; CheckDlgButton(IDC_POL_ROT , BST_UNCHECKED);
		PolRot_Only_Z    = true;
	}
	UpdateData( false );
}

void COctopusPolarizerLC::GetPosition( void ) 
{ 
	float pos;
	//Polarizer_T.GetPosition( 0, (float *)&pos ); 
	//B.position_degrees_T = FrameConvert( pos - offset_T );

	//Polarizer_B.GetPosition( 0, (float *)&pos ); 
	//B.position_degrees_B = FrameConvert( pos - offset_B );;
}

float  COctopusPolarizerLC::FrameConvert( float deg )
{
	if ( deg > 359.999 )
		return (deg - 360.0);
	else if ( deg < 0.0 )
		return (360.0 + deg);
	else 
		return deg;
}

void COctopusPolarizerLC::Close( void ) 
{ 
	//Polarizer_T.StopCtrl();
	//Polarizer_B.StopCtrl();
	Initialized = false; 
}

void COctopusPolarizerLC::Zero( void ) 
{	
	MoveTo( 0.0 ); 
}

void COctopusPolarizerLC::SavePositionGoTo( void ) 
{	
	MoveTo( save_pos_deg_T );
}

void COctopusPolarizerLC::SavePositionSet( void ) 
{	
	GetPosition(); 
	save_pos_deg_T = B.position_degrees_T;
}

void COctopusPolarizerLC::ShowPosition( void )
{
	CString str;

	if( IsWindowVisible() ) 
	{
		str.Format(_T("T:%.1f Save:%.1f ScanStart:%.1f\n"), B.position_degrees_T, save_pos_deg_T, scan_start_deg_T);
		m_Pos.SetWindowText( str );
	}
}

void COctopusPolarizerLC::ShowStep( void )
{
	CString str;

	if( IsWindowVisible() ) 
	{
		str.Format(_T("Pol step: %d of %d\nZ-step: %d of %d"), scan_current_step, scan_number_of_steps, scan_current_step_z, scan_number_of_steps_z);
		m_Step.SetWindowText( str );
	}
}

BOOL COctopusPolarizerLC::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int id = LOWORD(wParam);     // Notification code
	if( id == 2 ) return FALSE;  // Trap ESC key
	if( id == 1 ) return FALSE;  // Trap RTN key
    return CDialog::OnCommand(wParam, lParam);
}

void COctopusPolarizerLC::ScanSetStart( void )
{
	GetPosition(); 
	scan_start_deg_T = B.position_degrees_T;
}

void COctopusPolarizerLC::ScanStartStop( void )
{
	if ( B.POL_scan_in_progress )
	{
		//stop scan
		ScanDone();
	}
	else
	{   
		//go to beginning, if we are going to rotate
		if ( PolRot_Only_Pol || PolRot_Both )
		{
			MoveTo( scan_start_deg_T );
		} 
		
		//see if camera is on
		if ( B.Camera_Thread_running ) 
		{
			AfxMessageBox( _T("Please stop the camera, and then try again.") );
			return;
		}

		scan_current_step       = 1;
		scan_current_step_z     = 1;
		
		B.POL_scan_in_progress  = true;

		GetPosition();
		ShowPosition();
		ShowStep();

		TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 100, NULL );
	}
}

void COctopusPolarizerLC::ScanStep( void )
{
	
	if ( glob_m_pCamera == NULL ) return;
	
	KillTimer( TIMER_Polarizer_Scan );

	if( PolRot_Only_Pol )
	{
		if( scan_current_step <= scan_number_of_steps )
		{
			//move the polarizers
			//where are we?
			GetPosition();
			ShowPosition();
			ShowStep();

			//take a triple of pictures	
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 

			float target = scan_start_deg_T + (scan_current_step * scan_step_size_deg);
			
			if ( target > 360.00 ) target = target - 360.0;
			
			MoveTo( target ); 
		
			scan_current_step++;
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );
		}
		else
		{
			ScanDone();
		}
	}
	else if( PolRot_Both )
	{
		if( scan_current_step <= scan_number_of_steps )
		{
			//move the polarizers
			//where are we?
			GetPosition();
			ShowPosition();
			ShowStep();

			//take a triple of pictures	
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 

			float target = scan_start_deg_T + (scan_current_step * scan_step_size_deg);
			
			if ( target > 360.00 ) target = target - 360.0;
			
			//MoveAE_T( target, true ); 
		
			scan_current_step++;
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );
		}
		else if ( scan_current_step_z <= scan_number_of_steps_z )
		{
			// move in Z
			// so that each scan starts with a fresh file
			// when rotating the polarizer
			glob_m_pCamera->FileClose();
			
			//go to beginning
			// MoveAE_T( scan_start_deg_T, true );
			
			scan_current_step = 1;
			
			if ( glob_m_pObjPiezo != NULL )
				glob_m_pObjPiezo->MoveRelZ( scan_step_size_microns_z );

			Sleep( 100 );
			scan_current_step_z++;
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );	
		}
		else
		{
			ScanDone();
		}
	}
	else if( PolRot_Only_Z )
	{
		if ( scan_current_step_z <= scan_number_of_steps_z )
		{
			//take a triple of pictures	
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 
			glob_m_pCamera->TakePicture(); 

			//and move
			if ( glob_m_pObjPiezo != NULL )
				glob_m_pObjPiezo->MoveRelZ( scan_step_size_microns_z );

			Sleep( 100 );
			scan_current_step_z++;
			TIMER_Polarizer_Scan = SetTimer( TIMER_POL_SCAN, 200, NULL );	
		}
		else
		{
			ScanDone();
		}
	}

}

void COctopusPolarizerLC::ScanDone( void )
{	
	//need to go back to beginning
	KillTimer( TIMER_Polarizer_Scan );
	
	if ( glob_m_pObjPiezo != NULL )
		glob_m_pObjPiezo->MoveRelZ( -1 * float(scan_number_of_steps_z) * scan_step_size_microns_z );

	B.POL_scan_in_progress = false;

	if ( PolRot_Only_Pol || PolRot_Both )
	{
		//MoveAE_T( scan_start_deg_T, true );
	}

	GetPosition();
	ShowPosition();
}
