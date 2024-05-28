#define S_FUNCTION_NAME  MyDrawSystem
#define S_FUNCTION_LEVEL 2

#include <math.h>

#include "simstruc.h"
#include "matrix.h"



// Here begins the GL part of the block
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

real_T realFromMxArray(const mxArray * p)
{
	return * ( (real_T *) (mxGetData(p)) );
}

struct SWindowWorkInfo
{
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
};

typedef struct SWindowWorkInfo WindowWorkInfo;


LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
					WPARAM wParam, LPARAM lParam);


void EnableOpenGL (HWND, HDC *, HGLRC *);
void DisableOpenGL (HWND, HDC, HGLRC);


void createWindow (WindowWorkInfo * pWWI)
{
    // PreVars
        HINSTANCE hInstance = 0;
        HINSTANCE hPrevInstance = 0;
        LPSTR lpCmdLine = "";
        int iCmdShow = 1;
    // Vars
        WNDCLASS wc;
        HWND hWnd;
        HDC hDC;
        HGLRC hRC;

    // Register window class
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor (NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "GLSample";
		RegisterClass (&wc);
    // Create main window
		hWnd = CreateWindow (
		  "GLSample", "OpenGL Window",
		  WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		  0, 0, 512, 512,
		  NULL, NULL, hInstance, NULL);
    // Enable OpenGL for the window
		EnableOpenGL (hWnd, &hDC, &hRC);

    pWWI -> hwnd  = hWnd;
    pWWI -> hdc   = hDC;
    pWWI -> hglrc = hRC;
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;
    case WM_DESTROY:
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;
    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}

void EnableOpenGL (HWND hWnd, HDC * hDC, HGLRC * hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    *hDC = GetDC (hWnd);

    ZeroMemory(&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );
}

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

void drawCircle(double radius, double x, double y) {
    double t = .0;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(x, y);
        for(t = 0; t <= 2 * 3.1415; t += 2 * 3.1415 / 100.0) {
            glVertex2d(x+radius*cos(t),y+radius*sin(t));
        }
        glEnd();
}

void drawLine(GLint x, GLint y, GLint t){
     double a=512/12/10;
     double b=512/3;
     glColor3ub(215,81,30);
    //may increase theta n times for more observability
	glPushMatrix();
	glTranslatef(x, y, 0);
    glPushMatrix();
    glRotated(t*1, 0, 0, 1.0);
	glTranslatef(-x, -y, 0);
    glBegin(GL_QUADS);
                          glVertex2f(x-a/2,y-b/50);
                          glVertex2f(x+a/2,y-b/50);
                          glVertex2f(x+a/2,y+0.98*b);
                          glVertex2f(x-a/2,y+0.98*b);
    glEnd();
    glColor3ub(255,255,255);
    drawCircle(1.5*a,x,y+0.98*b);
    glPopMatrix();
    glPopMatrix();
}
void drawTriangle(GLint x, GLint y){
	double a=512/20;
	double k=0.57735;
	glColor3ub(200,103,6);
     glBegin(GL_TRIANGLES);
		glVertex2f(x, y+k*a);
		glVertex2f(x-a/2,y-a*k/2);
		glVertex2f(x+a/2,y-a*k/2);
     glEnd();
}

void drawGL(SimStruct * S) {
    real_T a;
    double ad;
    real_T **pp;

    // Get parameters
    {
    }

    pp = (real_T**)ssGetInputPortRealSignalPtrs(S, 0);
    a = *(pp[0]);
    ad = -a;

    glViewport(0,0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,512,0,512,-1.0,1.0);
    }
    glClearColor(0,0,0,1); //background
	drawLine(512/2,512/4,ad);
	drawTriangle(512/2,512/4);
	glFlush();
}




// ######################################################################################################################################################
// ######################################################################################################################################################
// ######################################################################################################################################################
// Here begins the nonGL part of the block

#define MDL_CHECK_PARAMETERS
void mdlCheckParameters(SimStruct * S) {
	if (ssGetSFcnParamsCount(S) != 0) {
		ssSetErrorStatus(S, "Must have 0 parameters");
		return;
	}
}

#define MDL_INITIALIZE_SIZES
void mdlInitializeSizes(SimStruct *S)
{
    int i = 0;
    ssSetNumSFcnParams(S, 0);
    mdlCheckParameters(S);
    if (ssGetErrorStatus(S) != NULL)
        return;

    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }
	// DWork
		ssSetNumDWork(S, 1);
        for(i = 0; i < 1; i++) {
		// 1
			ssSetDWorkWidth(S, i, 1);
			ssSetDWorkDataType(S, i, SS_UINT32); // pointer
			ssSetDWorkUsageType(S, i, SS_DWORK_USED_AS_DWORK);
			ssSetDWorkComplexSignal(S, i, COMPLEX_NO); // "No complex. Real."
        }

	// States
		ssSetNumContStates( S, 0 );
		ssSetNumDiscStates( S, 0 );

	// Inputs
		if (! ssSetNumInputPorts( S, 1 ) )
			return;

        for(i = 0; i < 1; i++)
		{
			DECL_AND_INIT_DIMSINFO(di);
			int_T dims[1];
			dims[0] = 1;
			di.numDims = 1;
			di.dims = dims;
			di.width = 1;
			ssSetInputPortDimensionInfo(S, i ,&di);
		}
    for(i = 0; i < 1; i++) {
        ssSetInputPortDirectFeedThrough(S, i, 1);
    }
	// Outputs
		if(!ssSetNumOutputPorts(S, 0))
			return;
	ssSetNumSampleTimes(S, 1); // "How many frequences we have"
}

//#define MDL_INITIALIZE_SAMPLE_TIMES
void mdlInitializeSampleTimes(SimStruct *S)
{
	// Continuous block. (CONT_SAM_TIME, 0.0)
		ssSetSampleTime(S, 0 /* sample time index */ , CONTINUOUS_SAMPLE_TIME );
		ssSetOffsetTime(S, 0 /* sample time index */ , 0.0 );


}

#define MDL_START
void mdlStart(SimStruct * S)
{
    WindowWorkInfo ** ppWWI;
	ppWWI = (WindowWorkInfo **) ssGetDWork(S,0);
    *ppWWI = (WindowWorkInfo *) malloc(sizeof(WindowWorkInfo));
    createWindow( *ppWWI );
}

#define MDL_OUTPUTS // So, the MatLab will know it exists and will call it
void mdlOutputs(SimStruct *S, int_T tid /* task id */ )
{
	//return;
	{
	// Vars
		WindowWorkInfo * pWWI;
		MSG msg;
		int i;
		//real_T * pReal;
	// Get the pWWI
		pWWI = * (   (WindowWorkInfo **) ssGetDWork(S,0)   );
		//pWWI = thePWWI;
	//if (pWWI != thePWWI)
	//		for (i = 0; i + 1 > i; ++i)
	//			;

	// Process the messages
		while ( PeekMessage(&msg, pWWI -> hwnd, 0, 0, PM_REMOVE) )
		{
			TranslateMessage (&msg);
			DispatchMessage  (&msg);
		}
	// Draw the scene
		// Draw
			wglMakeCurrent( pWWI -> hdc, pWWI -> hglrc );
			drawGL(S);
		// Wait until the time
			// Have decided to not to do it. Simulation will be as fast
			// as the PC can perform it.
		// Swap the buffers
			SwapBuffers (pWWI -> hdc);
	}
}

void mdlDerivatives(SimStruct * S)
{
}

void mdlTerminate(SimStruct *S)
{
    WindowWorkInfo * pWWI = * (   (WindowWorkInfo **) ssGetDWork(S,0)   );

    // Shutdown OpenGL
		DisableOpenGL (pWWI -> hwnd, pWWI -> hdc, pWWI -> hglrc);

    // Destroy the window
		DestroyWindow (pWWI -> hwnd);

	free(pWWI);
}


#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
