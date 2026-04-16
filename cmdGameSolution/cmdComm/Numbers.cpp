#include "Numbers.h"
#include "ScreenBuffer.h"

int Number[10][5][3] =
{
	{ //0
		1,1,1,
		1,0,1,
		1,0,1,
		1,0,1,
		1,1,1,
	},
	{ //1
		1,1,0,
		0,1,0,
		0,1,0,
		0,1,0,
		1,1,1,
	},
	{ //2
		1,1,1,
		0,0,1,
		1,1,1,
		1,0,0,
		1,1,1,
	},
	{ //3
		1,1,1,
		0,0,1,
		0,1,1,
		0,0,1,
		1,1,1,
	},
	{ //4
		1,0,0,
		1,0,1,
		1,1,1,
		0,0,1,
		0,0,1,
	},
	{ //5
		1,1,1,
		1,0,0,
		1,1,1,
		0,0,1,
		1,1,1,
	},
	{ //6
		1,1,1,
		1,0,0,
		1,1,1,
		1,0,1,
		1,1,1,
	},
	{ //7
		1,1,1,
		1,0,1,
		0,0,1,
		0,0,1,
		0,0,1,
	},
	{ //8
		1,1,1,
		1,0,1,
		1,1,1,
		1,0,1,
		1,1,1,
	},
	{ //9
		1,1,1,
		1,0,1,
		1,1,1,
		0,0,1,
		1,1,1,
	},
};

void DrawNumber(int x, int y, int num, const wchar_t* ch, unsigned short color)
{
	if (num < 0 || num > 9) return;

	SetColor(color);

	for (int iy = 0; iy < 5; iy++)
	{
		for (int ix = 0; ix < 3; ix++)
		{
			if (Number[num][iy][ix] == 1)
			{
				ScreenPrint(x + ix, y + iy, ch);
			}
			else
			{
				ScreenPrint(x + ix, y + iy, L" ");
			}
		}
	}
}

