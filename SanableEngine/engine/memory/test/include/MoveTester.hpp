#pragma once

//This will have alignment 4 and size 12. Useful not just for checking that moves worked, but for detecting alignment shearing as well...
struct MoveTester
{
	int a = 1;
	int b = 2;
	int c = 3;
};
