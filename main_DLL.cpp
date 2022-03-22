#include <windows.h>
#include "XPLMDefs.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMGraphics.h"
#include "XPLMDisplay.h"
#include <stdio.h>
#include <string.h>

static XPLMDataRef ptrue_airspeed;
static XPLMDataRef pENGN_thro_use;
static XPLMDataRef poverride_throttles;
static XPLMDataRef pphi;
//static XPLMDataRef poverride_joystick_roll;
static XPLMDataRef pyoke_roll_ratio;
static XPLMDataRef pyoke_pitch_ratio;
static XPLMDataRef pyoke_heading_ratio;

static XPLMDataRef pail1_def;
static XPLMDataRef pelv1_def;
static XPLMDataRef prudd_def;
static XPLMDataRef poverride_control_surfaces;
static XPLMDataRef plail1def;

static XPLMDataRef pw_roll;
static XPLMDataRef pw_pitch;
static XPLMDataRef pw_heading;

static XPLMDataRef ptheta;
static XPLMDataRef ppsi;

static XPLMDataRef pg_nrml;
static XPLMDataRef pg_side;

static XPLMDataRef py_agl;
static XPLMDataRef p_vh_ind_float;

static XPLMDataRef pvertical_velocity;
static XPLMDataRef pframe_rate_period;

XPLMHotKeyID	gHotKeyF1 = NULL;
void	MyHotKeyCallbackF1(void* inRefcon);
char F1_on = 0;

XPLMHotKeyID	gHotKeyF2 = NULL;
void	MyHotKeyCallbackF2(void* inRefcon);
char F2_on = 0;

XPLMHotKeyID	gHotKeyF3 = NULL;
void	MyHotKeyCallbackF3(void* inRefcon);
char F3_on = 0;

XPLMHotKeyID	gHotKeyF4 = NULL;
void	MyHotKeyCallbackF4(void* inRefcon);
char F4_on = 0;

XPLMHotKeyID	gHotKeyF5 = NULL;
void	MyHotKeyCallbackF5(void* inRefcon);
char F5_on = 0;



int override_throttles;
float ENGN_thro_use[8];
float true_airspeed;
float V_ref;
float roll;
float pitch;
int override_joystick_roll;
int override_control_surfaces;
float yoke_roll_ratio;
float yoke_pitch_ratio;
float yoke_heading_ratio;
float pitch_ref;
float heading;
float heading_ref;

float roll_ref;
float ail1_def[56];
float elv1_def[56];
float rudd_def[56];


float w_roll;
float w_pitch;
float w_heading;

float g_nrml;
float g_side;

float y_agl;

float vertical_velocity;
float frame_rate_period;


BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

int DrawCallback1(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{

	int final_x = 130;
	int final_y = 750;

	// Now we have something in screen coordinates, which we can then draw a label on.

	XPLMDrawTranslucentDarkBox(final_x - 5, final_y + 10, final_x + 220, final_y - 80 * 3);

	float colWHT[] = { 1.0, 1.0, 1.0 };
	float colYellow[] = { 1.0, 1.0, 0.0 };
	char str[100];
	int space1 = 20;

	sprintf_s(str, "true_airspeed = %4.2f m/s", true_airspeed);
	XPLMDrawString(colWHT, final_x, final_y, str, NULL, xplmFont_Basic);

	sprintf_s(str, "V_ref = %4.2f m/s", V_ref);
	XPLMDrawString(colYellow, final_x, final_y - space1, str, NULL, xplmFont_Basic);

	sprintf_s(str, "roll = %4.2f deg", roll);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 2, str, NULL, xplmFont_Basic);

	sprintf_s(str, "roll_ref = %4.2f deg", roll_ref);
	XPLMDrawString(colYellow, final_x, final_y - space1 * 3, str, NULL, xplmFont_Basic);

	sprintf_s(str, "y_agl = %4.2f m", y_agl);
	XPLMDrawString(colYellow, final_x, final_y - space1 * 4, str, NULL, xplmFont_Basic);

	sprintf_s(str, "Auto_throttle = %d ", F1_on);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 5, str, NULL, xplmFont_Basic);

	sprintf_s(str, "Auto_roll = %d ", F2_on);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 6, str, NULL, xplmFont_Basic);

	sprintf_s(str, "Auto_pitch = %d ", F3_on);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 7, str, NULL, xplmFont_Basic);

	sprintf_s(str, "Auto_height = %d ", F4_on);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 8, str, NULL, xplmFont_Basic);

	sprintf_s(str, "Auto_vertical = %d ", F5_on);
	XPLMDrawString(colWHT, final_x, final_y - space1 * 9, str, NULL, xplmFont_Basic);

	return -1;
}

PLUGIN_API int XPluginStart(
	char* outName,
	char* outSig,
	char* outDesc)
{
	strcpy_s(outName, 100, "TestDLLPlugin1");
	strcpy_s(outSig, 100, "xpsdk.examples.TestDLLPlugin1");
	strcpy_s(outDesc, 100, "An example of linking DLL.");

	XPLMRegisterDrawCallback(DrawCallback1, xplm_Phase_Window, 0, NULL);

	gHotKeyF1 = XPLMRegisterHotKey(XPLM_VK_F1, xplm_ShiftFlag | xplm_DownFlag,
		"Auto throttle",
		MyHotKeyCallbackF1,
		NULL);

	gHotKeyF2 = XPLMRegisterHotKey(XPLM_VK_F2, xplm_ShiftFlag | xplm_DownFlag,
		"Auto roll",
		MyHotKeyCallbackF2,
		NULL);

	gHotKeyF3 = XPLMRegisterHotKey(XPLM_VK_F3, xplm_ShiftFlag | xplm_DownFlag,
		"Auto pitch",
		MyHotKeyCallbackF3,
		NULL);

	gHotKeyF4 = XPLMRegisterHotKey(XPLM_VK_F4, xplm_ShiftFlag | xplm_DownFlag,
		"Auto height",
		MyHotKeyCallbackF4,
		NULL);
	
	gHotKeyF5 = XPLMRegisterHotKey(XPLM_VK_F5, xplm_ShiftFlag | xplm_DownFlag,
		"Auto vertical",
		MyHotKeyCallbackF5,
		NULL);


	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	int i = 1;
}

PLUGIN_API void XPluginDisable(void)
{
	__asm nop
}

float boundaries1(float x, float lim_down, float lim_up)
{
	if (x > lim_up)
		x = lim_up;
	else if (x < lim_down)
		x = lim_down;
	return x;
}

// this flightloop callback will be called every frame to update the targets
float floop_cb(float elapsed1, float elapsed2, int ctr, void* refcon)
{
	true_airspeed = XPLMGetDataf(ptrue_airspeed);

	char autothrottle = F1_on;

	//Автомат тяги
	if (autothrottle)
	{
		XPLMSetDatai(poverride_throttles, 1);
		float k_V = 0.5;
		V_ref = 60;
		ENGN_thro_use[0] = k_V * (V_ref - true_airspeed);

		ENGN_thro_use[0] = boundaries1(ENGN_thro_use[0], 0, 1);

		XPLMSetDatavf(pENGN_thro_use, ENGN_thro_use, 0, 8);
	}
	else
		XPLMSetDatai(poverride_throttles, 0);

	// 	//Крен автопилот
	// 	roll = XPLMGetDataf(pphi);
	// 
	// 	char autoroll = 0;
	// 	if (autoroll)
	// 	{
	// 		XPLMSetDatai(poverride_joystick_roll, 1);
	// 		float k_roll = 0.1;
	// 		roll_ref = -10;
	// 		yoke_roll_ratio = k_roll*(roll_ref - roll);
	// 		XPLMSetDataf(pyoke_roll_ratio, yoke_roll_ratio);
	// 	}
	// 	else
	// 		XPLMSetDatai(poverride_joystick_roll, 0);

	yoke_roll_ratio = XPLMGetDataf(pyoke_roll_ratio);
	yoke_pitch_ratio = XPLMGetDataf(pyoke_pitch_ratio);
	yoke_heading_ratio = XPLMGetDataf(pyoke_heading_ratio);

	// Auto pitch

	if (1)
	{
		static char flag_roll = 0;
		static char mode_roll = 0;

		if (F2_on)
		{
			if (!flag_roll)
			{
				if ((roll > -7) && (roll < 7))
					mode_roll = 2;
				else
					mode_roll = 1;

			}
			flag_roll = 1;
		}
		else
		{
			flag_roll = 0;
			mode_roll = 0;
		}


		//Surfaces!
		XPLMSetDatai(poverride_control_surfaces, 1);

		//Ailerons
		roll = XPLMGetDataf(pphi);
		w_roll = XPLMGetDataf(pw_roll);
		float k_w_roll = 0.6;
		float k_roll = 0.1;
		float d_roll = roll_ref - roll;

		switch (mode_roll)
		{
		case 0:
		{
			ail1_def[8] = 10 * yoke_roll_ratio + k_w_roll * w_roll;
			ail1_def[9] = -10 * yoke_roll_ratio + k_w_roll * w_roll;
			ail1_def[10] = 10 * yoke_roll_ratio + k_w_roll * w_roll;
			ail1_def[11] = -10 * yoke_roll_ratio + k_w_roll * w_roll;
			// 		ail1_def[16] = 80 * yoke_roll_ratio;
			// 		ail1_def[17] = -80 * yoke_roll_ratio;
			for (int i = 0; i < 56; i++)
			{
				ail1_def[i] = boundaries1(ail1_def[i], -30, 30);
			}
			 
			XPLMSetDatavf(pail1_def, ail1_def, 0, 56);
			roll_ref = roll;
		}
		break;
		case 1:
		{
			ail1_def[8] = k_roll*d_roll + k_w_roll * w_roll;
			ail1_def[9] = -1 * k_roll * d_roll + k_w_roll * w_roll;
			ail1_def[10] = k_roll * d_roll + k_w_roll * w_roll;
			ail1_def[11] = -1 * k_roll * d_roll + k_w_roll * w_roll;
			// 		ail1_def[16] = 80 * yoke_roll_ratio;
			// 		ail1_def[17] = -80 * yoke_roll_ratio;
			for (int i = 0; i < 56; i++)
			{
				ail1_def[i] = boundaries1(ail1_def[i], -30, 30);
			}
			XPLMSetDatavf(pail1_def, ail1_def, 0, 56);
		}
		break;
		case 2:
		{
			
			float k_heading = 1;
			heading_ref = 70;
			heading = XPLMGetDataf(ppsi);
			float d_heading = (heading_ref - heading);
			roll_ref = 0.9 * d_heading;
			roll_ref = boundaries1(roll_ref, -30, 30);

			ail1_def[8] = k_roll*d_roll + k_w_roll * w_roll;
			ail1_def[9] = -1* k_roll * d_roll + k_w_roll * w_roll;
			ail1_def[10] = k_roll*d_roll + k_w_roll * w_roll;
			ail1_def[11] = -1 * k_roll * d_roll + k_w_roll * w_roll;
			// 		ail1_def[16] = 80 * yoke_roll_ratio;
			// 		ail1_def[17] = -80 * yoke_roll_ratio;
			for (int i = 0; i < 56; i++)
			{
				ail1_def[i] = boundaries1(ail1_def[i], -30, 30);
			}
			XPLMSetDatavf(pail1_def, ail1_def, 0, 56);
		}
		break;
		}


		//Elevators

		pitch = XPLMGetDataf(ptheta);
		w_pitch = XPLMGetDataf(pw_pitch);
		float k_pitch = 0.8;
		float k_w_pitch = 0.5;
		float k_g_nrml = 3;

		if (F3_on)
		{
			
			float k_w_pitch = 0.5;
			float d_pitch = pitch_ref - pitch;
			elv1_def[16] = -10 * d_pitch + k_w_pitch * w_pitch;
			elv1_def[17] = -10 * d_pitch + k_w_pitch * w_pitch;
		}
		else
		{
			elv1_def[16] = -10 * yoke_pitch_ratio + k_w_pitch * w_pitch + k_g_nrml * (g_nrml - 1);
			elv1_def[17] = -10 * yoke_pitch_ratio + k_w_pitch * w_pitch + k_g_nrml * (g_nrml - 1);
			for (int i = 0; i < 56; i++)
			{
				elv1_def[i] = boundaries1(elv1_def[i], -20, 20);
			}
			XPLMSetDatavf(pelv1_def, elv1_def, 0, 56);
			pitch_ref = pitch;
		}

		y_agl = XPLMGetDataf(py_agl);

		
		if (F4_on)
		{

			float k_agl = 2;
			float y_agl_ref = 200;
			float pitch_ref = k_agl * (y_agl_ref - y_agl);
			pitch_ref = boundaries1(pitch_ref, -30, 20);
			elv1_def[16] = -4 * (pitch_ref - pitch) + k_w_pitch * w_pitch;
			elv1_def[17] = -4 * (pitch_ref - pitch) + k_w_pitch * w_pitch;

			for (int i = 0; i < 56; i++)
			{
				elv1_def[i] = boundaries1(elv1_def[i], -20, 20);
			}
			XPLMSetDatavf(pelv1_def, elv1_def, 0, 56);
		}

		if (F5_on) {
			float vertical_velocity_ref = 2;
			float k_vy = 4;
			vertical_velocity_ref = boundaries1(vertical_velocity_ref, -4, 4);
			float i_v = 0;
			i_v += (vertical_velocity_ref - vertical_velocity) * frame_rate_period;
			i_v = boundaries1(i_v, 10, 10);
			float pitch_ref = k_vy * (vertical_velocity_ref - vertical_velocity) * i_v + 2.63;
			pitch_ref = boundaries1(pitch_ref, -10, 10);
			/*
			float d_pitch = pitch_ref - pitch;
			d_pitch = boundaries1(d_pitch, -3, 3);
			float i_pitch = 0;
			i_pitch += d_pitch * frame_rate_period;
			i_pitch = boundaries1(i_pitch, -2, 2);
			*/
			elv1_def[16] = -4 * (pitch_ref - pitch) + k_w_pitch * w_pitch;
			elv1_def[17] = -4 * (pitch_ref - pitch) + k_w_pitch * w_pitch;

			for (int i = 0; i < 56; i++)
			{
				elv1_def[i] = boundaries1(elv1_def[i], -20, 20);
			}
			XPLMSetDatavf(pelv1_def, elv1_def, 0, 56);
			
		}

		//Rudders
		w_heading = XPLMGetDataf(pw_heading);
		g_side = XPLMGetDataf(pg_side);
		float k_w_heading = 0.1;
		float k_g_side = 4;
		rudd_def[18] = +20 * yoke_heading_ratio + k_w_heading * w_heading + k_g_side * g_side;
		rudd_def[19] = +20 * yoke_heading_ratio + k_w_heading * w_heading + k_g_side * g_side;
		for (int i = 0; i < 56; i++)
		{
			rudd_def[i] = boundaries1(rudd_def[i], -30, 30);
		}
		XPLMSetDatavf(prudd_def, rudd_def, 0, 56);
	}
	return -1;


}

PLUGIN_API int  XPluginEnable(void)
{
	ptrue_airspeed = XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
	pENGN_thro_use = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");
	poverride_throttles = XPLMFindDataRef("sim/operation/override/override_throttles");
	pphi = XPLMFindDataRef("sim/flightmodel/position/phi");
	//poverride_joystick_roll = XPLMFindDataRef("sim/operation/override/override_joystick_roll");
	ptheta = XPLMFindDataRef("sim/flightmodel/position/theta");

	ppsi = XPLMFindDataRef("sim/flightmodel/position/psi");

	pyoke_roll_ratio = XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
	pyoke_pitch_ratio = XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
	pyoke_heading_ratio = XPLMFindDataRef("sim/joystick/yoke_heading_ratio");

	pail1_def = XPLMFindDataRef("sim/flightmodel/controls/ail1_def");
	pelv1_def = XPLMFindDataRef("sim/flightmodel/controls/elv1_def");
	prudd_def = XPLMFindDataRef("sim/flightmodel/controls/rudd_def");
	poverride_control_surfaces = XPLMFindDataRef("sim/operation/override/override_control_surfaces");

	pw_roll = XPLMFindDataRef("sim/flightmodel/position/P");
	pw_pitch = XPLMFindDataRef("sim/flightmodel/position/Q");
	pw_heading = XPLMFindDataRef("sim/flightmodel/position/R");

	pg_nrml = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
	pg_side = XPLMFindDataRef("sim/flightmodel/forces/g_side");

	py_agl = XPLMFindDataRef("sim/flightmodel/position/y_agl");
	p_vh_ind_float = XPLMFindDataRef("sim/flightmodel/position/vh_ind_float");

	pvertical_velocity = XPLMFindDataRef("sim/cockpit/autopilot/vertical_velocity");

	pframe_rate_period = XPLMFindDataRef("sim/operation/misc/frame_rate_period");

	XPLMRegisterFlightLoopCallback(floop_cb, 1, NULL);
	return 1;
}
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void* inParam)
{
	int i = 1;
}

void	MyHotKeyCallbackF1(void* inRefcon)
{
	F1_on = !F1_on;
}

void	MyHotKeyCallbackF2(void* inRefcon)
{
	F2_on = !F2_on;
}

void	MyHotKeyCallbackF3(void* inRefcon)
{
	F3_on = !F3_on;
}

void	MyHotKeyCallbackF4(void* inRefcon)
{
	F4_on = !F4_on;
}

void	MyHotKeyCallbackF5(void* inRefcon)
{
	F5_on = !F5_on;
}
