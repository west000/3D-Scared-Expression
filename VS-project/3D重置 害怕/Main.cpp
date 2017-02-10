#include "d3dUtility.h"
#include "camera.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 640;
const int Height = 480;

Camera TheCamera(Camera::LANDOBJECT);

//
// Framework functions
//
bool Setup()
{
    //
    // Setup a basic scene.  The scene will be created the
    // first time this function is called.
    //

    d3d::DrawBasicScene(Device, 0.0f);

	// TheCamera.setPosition(new D3DXVECTOR3(0.0f, 0.0f, -10.0f));

    //
    // Set projection matrix.
    //

    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,
        D3DX_PI * 0.4f, // 90 - degree
        (float)Width / (float)Height,
        1.0f,
        1000.0f);
    Device->SetTransform(D3DTS_PROJECTION, &proj);

    return true;
}

void Cleanup()
{
    // pass 0 for the first parameter to instruct cleanup.
    d3d::DrawBasicScene(0, 0.0f);
}

bool Display(float timeDelta)
{
    if (Device)
    {
        //
        // Update: Update the camera.
        //

		/*float movespeed = 8.0f;

        if (::GetAsyncKeyState('W') & 0x8000f)
            TheCamera.walk(movespeed * timeDelta);

        if (::GetAsyncKeyState('S') & 0x8000f)
            TheCamera.walk(-movespeed * timeDelta);

        if (::GetAsyncKeyState('A') & 0x8000f)
            TheCamera.strafe(-movespeed * timeDelta);

        if (::GetAsyncKeyState('D') & 0x8000f)
            TheCamera.strafe(movespeed * timeDelta);

        if (::GetAsyncKeyState('R') & 0x8000f)
            TheCamera.fly(movespeed * timeDelta);

        if (::GetAsyncKeyState('F') & 0x8000f)
            TheCamera.fly(-movespeed * timeDelta);

        if (::GetAsyncKeyState(VK_UP) & 0x8000f)
            TheCamera.pitch(1.0f * timeDelta);

        if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
            TheCamera.pitch(-1.0f * timeDelta);

        if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
            TheCamera.yaw(-1.0f * timeDelta);

        if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
            TheCamera.yaw(1.0f * timeDelta);

        if (::GetAsyncKeyState('N') & 0x8000f)
            TheCamera.roll(1.0f * timeDelta);

        if (::GetAsyncKeyState('M') & 0x8000f)
            TheCamera.roll(-1.0f * timeDelta);



        // Update the view matrix representing the cameras 
        // new position/orientation.
		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);*/

        //
        // Render
        //

        Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
        Device->BeginScene();

        d3d::DrawBasicScene(Device, 1.0f);

        Device->EndScene();
        Device->Present(0, 0, 0, 0);
    }
    return true;
}

// 测试移动窗口，阻塞消息循环
bool ButtonDown = false;
POINT LastMousePos;
POINT CurMousePos;
RECT CurWinddowRect;

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
	case WM_SYSCOMMAND:
	{
		switch (wParam & 0xFFF0)
		{
			case SC_MOVE:
			{
				if (ButtonDown == false)
				{
					ButtonDown = true;
					::GetCursorPos(&LastMousePos);
					::GetWindowRect(hwnd, &CurWinddowRect);
				}
				return 0;
			}
			break;
		}
		break;
	}

	case WM_NCLBUTTONUP:
		ButtonDown = false;
		break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            ::DestroyWindow(hwnd);

        break;
    }

	if (ButtonDown)
	{
		::GetCursorPos(&CurMousePos);
		const int DeltaX = CurMousePos.x - LastMousePos.x;
		const int DeltaY = CurMousePos.y - LastMousePos.y;
		if (DeltaX < -1 || DeltaX >1 || DeltaY < -1 || DeltaY > 1)
		{
			CurWinddowRect.left += DeltaX;
			CurWinddowRect.top += DeltaY;
			LastMousePos = CurMousePos;
			::SetWindowPos(hwnd, HWND_NOTOPMOST, CurWinddowRect.left, CurWinddowRect.top, CurWinddowRect.right, CurWinddowRect.bottom, SWP_NOSIZE);
		}
	}

    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
    HINSTANCE prevInstance,
    PSTR cmdLine,
    int showCmd)
{
    if (!d3d::InitD3D(hinstance,
        Width, Height, true, D3DDEVTYPE_HAL, &Device))
    {
        ::MessageBox(0, "InitD3D() - FAILED", 0, 0);
        return 0;
    }

    if (!Setup())
    {
        ::MessageBox(0, "Setup() - FAILED", 0, 0);
        return 0;
    }

    d3d::EnterMsgLoop(Display);

    Cleanup();

    Device->Release();

    return 0;
}
