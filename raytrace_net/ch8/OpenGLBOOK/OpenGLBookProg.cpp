/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing This Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>
#include <gl\glaux.h>
#include <stdio.h>
#include <math.h>
#include <vcl.h>

USELIB("glaux.lib");

#define BOOK 1

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;
int     loop;
GLuint  texture[7];

GLfloat xspeed,yspeed;

GLfloat ambientLight[]={0.1,0.1,0.1,1.0};
GLfloat diffuseLight[]={0.7,0.7,0.7,1.0};
GLfloat lightPos[]={0.0, 0.0, 5.0,  1.0};

GLfloat points[45][45][3];
int wiggle_count = 0;
GLfloat hold;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

AUX_RGBImageRec *LoadBMP(char *Filename)
{
        FILE *File=NULL;
        if (!Filename)
        {
                return NULL;
        }
        File=fopen(Filename,"r");
        if (File)
        {
            fclose(File);
            return auxDIBImageLoad(Filename);
        }
    return NULL;
}
int LoadTextures()
{
    int Status = FALSE;
    AUX_RGBImageRec *TextureImage[7];
    memset(TextureImage, 0, sizeof(void *)*7);
    if ( (TextureImage[0]=LoadBMP("openglfront.bmp")) &&
         (TextureImage[1]=LoadBMP("openglback.bmp")) &&
         (TextureImage[2]=LoadBMP("opengltyl.bmp")) &&
         (TextureImage[3]=LoadBMP("openglprzod.bmp")) &&
         (TextureImage[4]=LoadBMP("openglgora.bmp")) &&
         (TextureImage[5]=LoadBMP("opengldol.bmp")) &&
         (TextureImage[6]=LoadBMP("gluttlo.bmp")))
    {
        Status=TRUE;

        glGenTextures(7, &texture[0]);                                  // Create The Texture
        for(loop=0;loop<7;loop++)
        {
            glBindTexture(GL_TEXTURE_2D, texture[loop]);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3,TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
        }
        for(loop=0;loop<7;loop++)
        {
            if (TextureImage[loop])                                                    // If Texture Exists
            {
                if (TextureImage[loop]->data)                                      // If Texture Image Exists
                    free(TextureImage[loop]->data);                            // Free The Texture Image Memory
            free(TextureImage[loop]);                                          // Free The Image Structure
            }
        }
    }
    return Status;
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height==0)
	{
		height=1;
	}
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
GLvoid BuildBook()
{
glNewList(BOOK,GL_COMPILE);

    glScalef(0.6,0.6,0.6);
// Cover front
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glScalef(1,1.1,1);
    glBegin(GL_QUADS);
        glNormal3f(0,0,-1);
        glTexCoord2f(0.0,0.0);  glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1.0,0.0);  glVertex3f(1.0, -1.0, 0.0);
        glTexCoord2f(1.0,1.0);  glVertex3f(1.0, 1.0, 0.0);
        glTexCoord2f(0.0,1.0);  glVertex3f(-1.0, 1.0, 0.0);
    glEnd();
// Cover back
    glNormal3f(0,0,1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);  glVertex3f(-1.0, -1.0, 0.4);
        glTexCoord2f(1.0,0.0);  glVertex3f(1.0, -1.0, 0.4);
        glTexCoord2f(1.0,1.0);  glVertex3f(1.0, 1.0, 0.4);
        glTexCoord2f(0.0,1.0);  glVertex3f(-1.0, 1.0, 0.4);
    glEnd();
//  'Back'
    glRotatef(-90,0,1,0);
    glTranslatef(0.4,0,1);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);  glVertex3f(-0.4, -1.0, 0.0);
        glTexCoord2f(1.0,0.0);  glVertex3f(0.0, -1.0, 0.0);
        glTexCoord2f(1.0,1.0);  glVertex3f(0.0, 1.0, 0.0);
        glTexCoord2f(0.0,1.0);  glVertex3f(-0.4, 1.0, 0.0);
    glEnd();
//  'Front'
    glTranslatef(-0.4,0,-2);
    glRotatef(180,1,0,0);
    glRotatef(180,0,0,1);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);  glVertex3f(-0.4, -1.0, 0.0);
        glTexCoord2f(1.0,0.0);  glVertex3f(0.0, -1.0, 0.0);
        glTexCoord2f(1.0,1.0);  glVertex3f(0.0, 1.0, 0.0);
        glTexCoord2f(0.0,1.0);  glVertex3f(-0.4, 1.0, 0.0);
    glEnd();
//  'Top'
    glRotatef(-90,1,0,0);
    glTranslatef(0,0,1.0);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);  glVertex3f(-0.4, 0.0, 0.0);
        glTexCoord2f(1.0,0.0);  glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0,1.0);  glVertex3f(0.0, 2.0, 0.0);
        glTexCoord2f(0.0,1.0);  glVertex3f(-0.4, 2.0, 0.0);
    glEnd();
//  'Bottom'
    glRotatef(180,0,1,0);
    glTranslatef(0,0,2);
    glTranslatef(0.4,0,0);

    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);  glVertex3f(-0.4, 0.0, 0.0);
        glTexCoord2f(1.0,0.0);  glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0,1.0);  glVertex3f(0.0, 2.0, 0.0);
        glTexCoord2f(0.0,1.0);  glVertex3f(-0.4, 2.0, 0.0);
    glEnd();

glEndList();
}
int InitGL(GLvoid)
{
    if(!LoadTextures())
        return FALSE;

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    BuildBook();
    
//  BuildBackground
    for(int x=0;x<45;x++)
    {
        for(int y=0;y<45;y++)
        {
            points[x][y][0]=float((x/5.0f)-4.5f);
            points[x][y][1]=float((y/5.0f)-4.5f);
            points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
        }
    }
	return TRUE;
}
GLint DrawGLScene(GLvoid)							
{
    int x,y;
    float float_x, float_y, float_xb, float_yb;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0,0,-3);
    glRotatef(xspeed,0,1,0);
    glRotatef(yspeed,1,0,0);
    glRotatef(180,0,1,0);
    glCallList(BOOK);

    glLoadIdentity();
    glTranslatef(0,0,-6);

    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glBegin(GL_QUADS);
        for(x=0;x<44;x++)
        {
            for(y=0;y<44;y++)
            {
                float_x = float(x)/44.0f;
                float_y = float(y)/44.0f;
                float_xb= float(x+1)/44.0f;
                float_yb= float(y+1)/44.0f;

                glTexCoord2f( float_x, float_y);
                glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2]);

                glTexCoord2f( float_x, float_yb);
                glVertex3f(points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);

                glTexCoord2f( float_xb, float_yb);
                glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2]);

                glTexCoord2f( float_xb, float_y);
                glVertex3f(points[x+1][y][0], points[x+1][y][1], points[x+1][y][2]);
            }
        }
    glEnd();

    if( wiggle_count == 2)
    {
        for(y=0;y<45;y++)
        {
            hold = points[0][y][2];
            for(x=0;x<45;x++)
            {
                points[x][y][2] = points[x+1][y][2];
            }
            points[44][y][2] = hold;
        }
        wiggle_count = 0;
    }
    wiggle_count++;
    glFlush();

	return TRUE;										// Everything Went OK
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done=TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					DrawGLScene();					// Draw The Scene
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
                    if(keys[VK_UP])
                        yspeed+=1.0;
                    if(keys[VK_DOWN])
                        yspeed-=1.0f;
                    if(keys[VK_LEFT])
                        xspeed+=1.0;
                    if(keys[VK_RIGHT])
                        xspeed-=1.0f;
				}
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
