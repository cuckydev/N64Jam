#include <nusys.h>
#include "input.h"

//Controller state
static NUContData cont_data[NU_CONT_MAXCONTROLLERS];
static u8 cont_pattern;

//Input state
u16 input_down, input_press;

//Input interface
void InitInput()
{
	//Initialize controllers
	cont_pattern = nuContInit();
	
	//Initialize key state
	input_down = 0;
	input_press = 0;
}

void GetInput()
{
	//Update controllers
	nuContDataGetExAll(cont_data);
	
	//Update keys
	if (cont_pattern & 1)
	{
		//Get next key state
		u16 next_input = 0;
		
		//D-Pad
		if (cont_data[0].button & L_JPAD)
			next_input |= INPUT_LEFT;
		if (cont_data[0].button & U_JPAD)
			next_input |= INPUT_UP;
		if (cont_data[0].button & R_JPAD)
			next_input |= INPUT_RIGHT;
		if (cont_data[0].button & D_JPAD)
			next_input |= INPUT_DOWN;
		
		if ((next_input & (INPUT_LEFT | INPUT_RIGHT)) == (INPUT_LEFT | INPUT_RIGHT))
			next_input &= ~(INPUT_LEFT | INPUT_RIGHT);
		if ((next_input & (INPUT_UP | INPUT_DOWN)) == (INPUT_UP | INPUT_DOWN))
			next_input &= ~(INPUT_UP | INPUT_DOWN);
		
		//Face buttons
		if (cont_data[0].button & A_BUTTON)
			next_input |= INPUT_A;
		if (cont_data[0].button & B_BUTTON)
			next_input |= INPUT_B;
		if (cont_data[0].button & START_BUTTON)
			next_input |= INPUT_START;
		
		//Triggers
		if (cont_data[0].button & L_TRIG)
			next_input |= INPUT_L;
		if (cont_data[0].button & R_TRIG)
			next_input |= INPUT_R;
		if (cont_data[0].button & Z_TRIG)
			next_input |= INPUT_Z;
		
		//Set key state
		input_press = (input_down ^ next_input) & next_input;
		input_down = next_input;
	}
	else
	{
		//Controller is unplugged
		input_down = INPUT_NOCON;
		input_press = 0;
	}
}
