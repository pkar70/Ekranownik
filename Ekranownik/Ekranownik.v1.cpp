// Ekranownik.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "Winuser.h"

int _tmain(int argc, _TCHAR* argv[])
{DISPLAY_DEVICE dd;TCHAR*tch; DEVMODE dm;
  
	int i;
	dd.cb=sizeof(dd);
	dm.dmSize=sizeof(DEVMODE);
	for(i=0;i<5 && EnumDisplayDevices(NULL,i,&dd,0);i++)
	{
		printf("%S, %S (%x)\n",&dd.DeviceName,&dd.DeviceString,dd.StateFlags);
//		DISPLAY_DEVICE_ATTACHED_TO_DESKTOP = 1
//		DISPLAY_DEVICE_PRIMARY_DEVICE = 4

		if(EnumDisplaySettings((LPCWSTR)&dd.DeviceName,ENUM_CURRENT_SETTINGS,&dm))
		{
			printf("%d x %d, od %d x %d\n",dm.dmPelsWidth,dm.dmPelsHeight,dm.dmPosition.x,dm.dmPosition.y);
		}
	}


	if(argc==2 || argc==3)
	{int rv;
		
		ZeroMemory(&dm,sizeof(DEVMODE));
		dm.dmDriverExtra=0;
		dm.dmSize=sizeof(dm);

		if(argv[1][0]=='2')
		{
			dm.dmFields= DM_POSITION|DM_PELSWIDTH|DM_PELSHEIGHT;
			dm.dmPosition.x=1280;
			dm.dmPosition.y=0;

			dm.dmPelsHeight=768;
			dm.dmPelsWidth=1024;

			if(argc==3)
			{
				switch(argv[2][0])
				{
				case '1': break; // przyjmuje 1024x768
				case '8': 
							dm.dmPelsHeight=600;
							dm.dmPelsWidth=800;
							break;
				case '6': 
							dm.dmPelsHeight=480;
							dm.dmPelsWidth=640;
							break;
				default: 
					printf("Error in second parameter, using 1024x768\n");
				}
			}

			rv=ChangeDisplaySettingsEx(L"\\\\.\\DISPLAY2",&dm,NULL,CDS_UPDATEREGISTRY | CDS_NORESET ,NULL);
		}
		else if(argv[1][0]=='1')
		{
			dm.dmFields=DM_POSITION |DM_PELSWIDTH|DM_PELSHEIGHT;
			dm.dmPelsHeight=0;
			dm.dmPelsWidth=0;
			dm.dmPosition.x=1280;
			dm.dmPosition.y=0;

			rv=ChangeDisplaySettingsEx(L"\\\\.\\DISPLAY2",&dm,NULL,CDS_UPDATEREGISTRY | CDS_NORESET ,NULL);
		}
		else rv=0;	// do niczego nie wszedl, wiec raczej jest OK - bledne parametry, np. /? :)

		if(rv)
			printf("error changing display, retval=%d",rv);
		switch(rv)
		{
		case -6: printf(" (DISP_CHANGE_BADDUALVIEW = settings change was unsuccessful because the system is DualView capable)"); break;
		case -5: printf(" (DISP_CHANGE_BADPARAM = invalid parameter was passed in)"); break;
		case -4: printf(" (DISP_CHANGE_BADFLAGS = invalid set of flags was passed in )"); break;
		case -3: printf(" (DISP_CHANGE_NOTUPDATED = Unable to write settings to the registry )"); break;
		case -2: printf(" (DISP_CHANGE_BADMODE = graphics mode is not supported )"); break;
		case -1: printf(" (DISP_CHANGE_FAILED = display driver failed the specified graphics mode )"); break;
		case 1: printf(" (DISP_CHANGE_RESTART = computer must be restarted for the graphics mode to work)"); break;
		}

		printf("\n");

		// za http://www.gamedev.net/topic/512215-changeing-from-clone-to-extended-mode/
		rv=ChangeDisplaySettingsEx(NULL, 0,0,0,0);
		if(rv)
			printf("error changing display, retval=%d\n",rv);


		printf("\npo zmianach...\n\n");
		for(i=0;i<5 && EnumDisplayDevices(NULL,i,&dd,0);i++)
		{
			printf("%S, %S (%x)\n",&dd.DeviceName,&dd.DeviceString,dd.StateFlags);
			if(EnumDisplaySettings((LPCWSTR)&dd.DeviceName,ENUM_CURRENT_SETTINGS,&dm))
			{
				printf("%d x %d, od %d x %d\n",dm.dmPelsWidth,dm.dmPelsHeight,dm.dmPosition.x,dm.dmPosition.y);
			}
		}
	}


	return 0;
}
/*
\\.\DISPLAY1                    , Intel(R) G33/G31 Express Chipset Family (   5)
1280 x 1024

\\.\DISPLAY2                    , Intel(R) G33/G31 Express Chipset Family (   1)
1024 x 768

\\.\DISPLAYV1                   , RDPDD Chained DD (   8)
\\.\DISPLAYV2                   , RDP Encoder Mirror Driver (200008)

Po przelaczeniu na jedynie DVI zmiana: druga linijka ma flags=0.
*/
