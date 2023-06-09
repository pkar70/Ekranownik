// Ekranownik.cpp : Defines the entry point for the console application.
//

// 2021.11.18: wyszukiwanie monitora z najwyższą rozdzielczością jakby nie było dobrze poukładane (bo tak jest na nowym desktop)
// 2015.04.25: data pliku ekranownik.cpp przy wprowadzaniu zmiany

#include "stdafx.h"
#include "windows.h"
#include "Winuser.h"

DISPLAY_DEVICE oDD[5];	// 2021.11.18 - pełna tabelka odczytana, a nie w petli z gubieniem danych
DEVMODE oDM[5];
int giDisplays = 0;
int giMaxDisplay = 0;
int giOtherDisplay = 0;

void InitTabelki()
{
	ZeroMemory(&oDD,sizeof(DISPLAY_DEVICE));


	for(int i=0;i<5 && EnumDisplayDevices(NULL,i,&oDD[i],0);i++)
	{
		ZeroMemory(&oDD[i],sizeof(DISPLAY_DEVICE));
		oDD[i].cb=sizeof(DISPLAY_DEVICE);
		ZeroMemory(&oDM[i],sizeof(DEVMODE));
		oDM[i].dmSize=sizeof(DEVMODE);
	}
}

int WczytajTabelkeMonitorow()
{
	int iCnt = 0;

	for(int i=0;i<5 && EnumDisplayDevices(NULL,i,&oDD[i],0);i++)
	{
		if(EnumDisplaySettings((LPCWSTR)&oDD[i].DeviceName,ENUM_CURRENT_SETTINGS,&oDM[i]))
		{
			iCnt++;
		}
		else
		{
			oDM[i].dmSize = 0;	// sygnalizacja bledu
			oDD[i].cb = 0;
		}
	}

	if(iCnt < 1)
	{
		printf("error: nie znalazlem monitorów?");
		_exit(1);
	}

	return iCnt;
}

int ZnajdzPodstawowyMonitor(int iDisplays)
{int iMaxSize = -1;
 int iPtrMax = -1;

	for(int i=0;i<5 && oDD[i].cb ;i++)
	{
		int iScreenPixels = oDM[i].dmPelsWidth * oDM[i].dmPelsHeight;
		if(iMaxSize < iScreenPixels)
		{
			iMaxSize = iScreenPixels;
			iPtrMax = i;
		}
	}
 
	if(iPtrMax < 0)
	{
		printf("error: nie znalazlem podstawowego monitora?");
		_exit(2);
	}

	return iPtrMax;
}

int ZnajdzDrugiMonitor(int iDisplays, int iPodstawowy)
{
	for(int i=0;i<5 && oDD[i].cb ;i++)
	{
		if(i != iPodstawowy) return i;
	}
 
	printf("error: nie znalazlem innego monitora?");
	//_exit(4);

}

void PrintTabelkeMonitorow(int iDisplays, int iPodstawowy, int iOtherDisplay)
{
for(int i=0;i<5 && oDD[i].cb ;i++)
	{
		printf("%S, %S (%x = ",&oDD[i].DeviceName,&oDD[i].DeviceString,oDD[i].StateFlags);
		if(oDD[i].StateFlags & DISPLAY_DEVICE_ACTIVE) printf("active ");
		if(oDD[i].StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) printf("mirr ");
		if(oDD[i].StateFlags & DISPLAY_DEVICE_MODESPRUNED) printf("pruned ");
		if(oDD[i].StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) printf("primary ");	//		DISPLAY_DEVICE_PRIMARY_DEVICE = 4
		if(oDD[i].StateFlags & DISPLAY_DEVICE_REMOVABLE) printf("removable ");
		if(oDD[i].StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) printf("vga ");
//		DISPLAY_DEVICE_ATTACHED_TO_DESKTOP = 1

		if(i == iPodstawowy) printf(" ==> uznaje za podstawowy");
		if(i == iOtherDisplay) printf(" ==> uznaje za filmowy");

		printf("\n%d x %d, od %d x %d\n",oDM[i].dmPelsWidth,oDM[i].dmPelsHeight,oDM[i].dmPosition.x,oDM[i].dmPosition.y);
	}

}

DEVMODE PrzygotujDevMode(int argc, _TCHAR* argv[], int iPolozenieX)
{static DEVMODE dm;
	ZeroMemory(&dm,sizeof(DEVMODE));
	dm.dmDriverExtra=0;
	dm.dmSize=sizeof(dm);

	dm.dmFields= DM_POSITION|DM_PELSWIDTH|DM_PELSHEIGHT;
	dm.dmPosition.x=iPolozenieX;
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

	return dm;
}

DEVMODE PrzygotujDevModeWylaczajacy(int iPolozenieX)
{static DEVMODE dm;
	ZeroMemory(&dm,sizeof(DEVMODE));
	dm.dmDriverExtra=0;
	dm.dmSize=sizeof(dm);

	dm.dmFields=DM_POSITION |DM_PELSWIDTH|DM_PELSHEIGHT;
	dm.dmPelsHeight=0;
	dm.dmPelsWidth=0;
	dm.dmPosition.x=iPolozenieX;
	dm.dmPosition.y=0;


	return dm;
}


int _tmain(int argc, _TCHAR* argv[])
{//DISPLAY_DEVICE dd;TCHAR*tch; DEVMODE dm;DISPLAY_DEVICE oMaxDD;DEVMODE oMaxDM;
 
	InitTabelki();
	giDisplays = WczytajTabelkeMonitorow();
	giMaxDisplay = ZnajdzPodstawowyMonitor(giDisplays);
	giOtherDisplay = ZnajdzDrugiMonitor(giDisplays,giMaxDisplay);
	PrintTabelkeMonitorow(giDisplays,giMaxDisplay,giOtherDisplay);

	if(argc != 2 && argc != 3)	return 0; // mieliśmy tylko pokazać

	DEVMODE dm;

	switch(argv[1][0])
	{
	case '2':
		dm = PrzygotujDevMode(argc,argv,giMaxDisplay);
		break;
	case '1':
		dm = PrzygotujDevModeWylaczajacy(giMaxDisplay);
		break;
	default:
		printf("error: bledny parametr?");
		_exit(3);
	}

	LONG rv=ChangeDisplaySettingsEx(oDD[giOtherDisplay].DeviceName,&dm,NULL,CDS_UPDATEREGISTRY | CDS_NORESET ,NULL);
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

	InitTabelki();
	giDisplays = WczytajTabelkeMonitorow();
	giMaxDisplay = ZnajdzPodstawowyMonitor(giDisplays);
	giOtherDisplay = ZnajdzDrugiMonitor(giDisplays,giMaxDisplay);
	PrintTabelkeMonitorow(giDisplays,giMaxDisplay,giOtherDisplay);


	return 0;
}


// wersja sprzed migracji:
/*
\\.\DISPLAY1                    , Intel(R) G33/G31 Express Chipset Family (   5)
1280 x 1024

\\.\DISPLAY2                    , Intel(R) G33/G31 Express Chipset Family (   1)
1024 x 768

\\.\DISPLAYV1                   , RDPDD Chained DD (   8)
\\.\DISPLAYV2                   , RDP Encoder Mirror Driver (200008)

Po przelaczeniu na jedynie DVI zmiana: druga linijka ma flags=0.
*/
// wersja po migracji 2021:
/*
\\.\DISPLAY1, Intel(R) UHD Graphics 750 (8000001)
1024 x 768, od 1280 x 0
\\.\DISPLAY2, Intel(R) UHD Graphics 750 (8000005)
1280 x 1024, od 0 x 0
\\.\DISPLAY3, Intel(R) UHD Graphics 750 (0)
*/
