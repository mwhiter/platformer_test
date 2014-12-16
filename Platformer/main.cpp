#include "PCH.h"
#include "base.h"
#include "core.h"

// Entry point of the program.
// CORE is declared extern in core.h (so it basically begins here)
int main(int argc, char** argv)
{
	CORE.GetScreenManager()->AddScreen(SCREEN_TYPE_MAIN_MENU);
	CORE.GameLoop();
	return 0;
}