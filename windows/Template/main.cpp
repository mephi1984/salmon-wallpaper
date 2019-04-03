#include "main.h"

#include "main_code.h"

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{

	int width = 720;
	int height = 1280;

	if (CreateEngine(width, height))
	{
		
		//MyApplication scope
		{
			TAndroidApplication Application;

			Application.OuterInit(width, height, width, height);

			MainLoop(&Application);
			
			Application.OuterDeinit();
		}

		DestroyEngine();
	}

	return 0;
}