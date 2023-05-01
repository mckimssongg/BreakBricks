#include <windows.h>
#include <winuser.h>

/* Crearemos un juego de breakbricks*/

/* Definimos una estructura para la bola */
struct Bola {
	int x, y;
	int dx, dy;
	int ancho, alto;
	int velocidad;
};

/* Definimos una estructura para la paleta */
struct Paleta {
	int x, y;
	int ancho, alto;
	int velocidad;
};

/* Definimos una estructura para la ventana */
struct Ventana {
	int ancho, alto;
};


/* Definimos una estructura para el ladrillo */
struct Ladrillo {
    int x, y;
    int ancho, alto;
    bool roto;
};

/* Definimos una estructura para el juego */
#define NUM_LADRILLOS 60

struct Juego {
    Bola bola;
    Paleta paleta;
    Ladrillo ladrillos[NUM_LADRILLOS];
    Ventana ventana;
};


/* Definimos una estructura para el juego */
Juego juego;

/* Definimos una estructura para la ventana */
#define IDC_BUTTON1 1001
/*
 * Window field offsets for GetWindowLong()
 */
#define GWL_WNDPROC         (-4)

#ifdef _WIN64

#undef GWL_WNDPROC
#undef GWL_HINSTANCE
#undef GWL_HWNDPARENT
#undef GWL_USERDATA

#endif /* _WIN64 */

#define GWLP_WNDPROC        (-4)

#define GWL_HINSTANCE (-6)

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	/* Handle the message */
	switch(Message) {
		 
		/* Upon creation, we get a message for that */
		case WM_CREATE: {
			/* Get the size of the client area */
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			juego.ventana.ancho = clientRect.right - clientRect.left;
			juego.ventana.alto = clientRect.bottom - clientRect.top;
			
			/* Inicializamos la bola */
			juego.bola.x = juego.ventana.ancho / 2;
			juego.bola.y = juego.ventana.alto / 2;
			juego.bola.ancho = 10;
			juego.bola.alto = 10;
			juego.bola.dx = 1;
			juego.bola.dy = 1;
			juego.bola.velocidad = 5;
			
			/* Inicializamos la paleta */
			juego.paleta.ancho = 100;
			juego.paleta.alto = 10;
			juego.paleta.x = juego.ventana.ancho / 2 - juego.paleta.ancho / 2;
			juego.paleta.y = juego.ventana.alto - juego.paleta.alto;
			juego.paleta.velocidad = 10;
			
			for (int i = 0; i < NUM_LADRILLOS; i++) {
				juego.ladrillos[i].ancho = 50;
				juego.ladrillos[i].alto = 20;
				juego.ladrillos[i].x = (i % 12) * juego.ladrillos[i].ancho + juego.ladrillos[i].ancho;
				juego.ladrillos[i].y = (i / 12) * juego.ladrillos[i].alto + juego.ladrillos[i].alto * 3;
				juego.ladrillos[i].roto = false;
			}

			/* Inicializamos el timer */
			SetTimer(hwnd, 1, 1000 / 30, NULL);
			break;
		}

		/* This is called many times a second and should be used for game logic */
		case WM_TIMER: {
			/* Mueve la bola */
			juego.bola.x += juego.bola.dx * juego.bola.velocidad;
			juego.bola.y += juego.bola.dy * juego.bola.velocidad;
			
			/* Revisa si la bola choca con la ventana */
			if(juego.bola.x < 0 || juego.bola.x + juego.bola.ancho > juego.ventana.ancho) {
				juego.bola.dx *= -1;
			}
			if(juego.bola.y < 0 || juego.bola.y + juego.bola.alto > juego.ventana.alto) {
				juego.bola.dy *= -1;
			}
			
			/* Revisa si la bola choca con la paleta */
			if(juego.bola.x + juego.bola.ancho >= juego.paleta.x && juego.bola.x <= juego.paleta.x + juego.paleta.ancho &&
			   juego.bola.y + juego.bola.alto >= juego.paleta.y && juego.bola.y <= juego.paleta.y + juego.paleta.alto) {
				juego.bola.dy *= -1;
			}
			
			for (int i = 0; i < NUM_LADRILLOS; i++) {
				if (!juego.ladrillos[i].roto) {
					if (juego.bola.x + juego.bola.ancho >= juego.ladrillos[i].x && juego.bola.x <= juego.ladrillos[i].x + juego.ladrillos[i].ancho &&
						juego.bola.y + juego.bola.alto >= juego.ladrillos[i].y && juego.bola.y <= juego.ladrillos[i].y + juego.ladrillos[i].alto) {
						juego.bola.dy *= -1;
						juego.ladrillos[i].roto = true;
					}
				}
			}

			/* Repinta la ventana */
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		
		/* If the window is resized, we should take note of the new client area */
		case WM_SIZE: {
			juego.ventana.ancho = LOWORD(lParam);
			juego.ventana.alto = HIWORD(lParam);
			break;
		}
		
		/* If the window is being painted, we should draw stuff */
		case WM_PAINT: {
			/* Get a device context to paint with */
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			
			/* Fill the background with white */
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
			FillRect(hdc, &clientRect, hBrush);
			
			/* Dibuja la paleta */
			Rectangle(hdc, juego.paleta.x, juego.paleta.y, juego.paleta.x + juego.paleta.ancho, juego.paleta.y + juego.paleta.alto);

			/* Dibuja la bola */
			Ellipse(hdc, juego.bola.x, juego.bola.y, juego.bola.x + juego.bola.ancho, juego.bola.y + juego.bola.alto);
			
			for (int i = 0; i < NUM_LADRILLOS; i++) {
				if (!juego.ladrillos[i].roto) {
					Rectangle(hdc, juego.ladrillos[i].x, juego.ladrillos[i].y, juego.ladrillos[i].x + juego.ladrillos[i].ancho, juego.ladrillos[i].y + juego.ladrillos[i].alto);
				} else {
					HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
					RECT rect = {juego.ladrillos[i].x, juego.ladrillos[i].y, juego.ladrillos[i].x + juego.ladrillos[i].ancho, juego.ladrillos[i].y + juego.ladrillos[i].alto};
					FillRect(hdc, &rect, hBrush);
					DeleteObject(hBrush);
				}
			}

			/* End painting */
			EndPaint(hwnd, &ps);
			break;
		}

		/* If a key is pressed, we should react to it */
		case WM_KEYDOWN: {
			/* If the left arrow key is pressed */
			if(wParam == VK_LEFT) {
				juego.paleta.x -= juego.paleta.velocidad;
				if(juego.paleta.x < 0) {
					juego.paleta.x = 0;
				}
			}
			
			/* If the right arrow key is pressed */
			if(wParam == VK_RIGHT) {
				juego.paleta.x += juego.paleta.velocidad;
				if(juego.paleta.x + juego.paleta.ancho > juego.ventana.ancho) {
					juego.paleta.x = juego.ventana.ancho - juego.paleta.ancho;
				}
			}

			/* Repinta la ventana */
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}

		/* Si se presiona el boton */
		case WM_COMMAND: {
			switch(LOWORD(wParam))
			{
				case IDC_BUTTON1:
					/* Inicia el timer */
					SetTimer(hwnd, 1, 10, NULL);
					break;
			}
			break;
		}

		/* If the window is being destroyed, we should clean up */
		case WM_DESTROY: {
			/* Kill the application, this sends a WM_QUIT message */
			PostQuitMessage(0);
			break;
		}

		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Caption",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}
