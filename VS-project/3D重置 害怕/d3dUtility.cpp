#include "d3dUtility.h"

// vertex formats
const DWORD d3d::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


bool d3d::InitD3D(
    HINSTANCE hInstance,
    int width, int height,
    bool windowed,
    D3DDEVTYPE deviceType,
    IDirect3DDevice9** device)
{


    //
    // Create the main application window.
    //

    WNDCLASS wc;

    wc.style = CS_HREDRAW | CS_VREDRAW ;
    wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "Direct3D9App";

    if (!RegisterClass(&wc))
    {
        ::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
        return false;
    }

    HWND hwnd = 0;
    hwnd = ::CreateWindow("Direct3D9App", "Direct3D9App",
        WS_EX_TOPMOST,
        0, 0, width, height,
        0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/);

    if (!hwnd)
    {
        ::MessageBox(0, "CreateWindow() - FAILED", 0, 0);
        return false;
    }

    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    //
    // Init D3D: 
    //

    HRESULT hr = 0;

    // Step 1: Create the IDirect3D9 object.

    IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d9)
    {
        ::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
        return false;
    }

    // Step 2: Check for hardware vp.

    D3DCAPS9 caps;
    d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

    int vp = 0;
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // Step 3: Fill out the D3DPRESENT_PARAMETERS structure.

    D3DPRESENT_PARAMETERS d3dpp;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hwnd;
    d3dpp.Windowed = windowed;
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Step 4: Create the device.

    hr = d3d9->CreateDevice(
        D3DADAPTER_DEFAULT, // primary adapter
        deviceType,         // device type
        hwnd,               // window associated with device
        vp,                 // vertex processing
        &d3dpp,             // present parameters
        device);            // return created device

    if (FAILED(hr))
    {
        // try again using a 16-bit depth buffer
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

        hr = d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            deviceType,
            hwnd,
            vp,
            &d3dpp,
            device);

        if (FAILED(hr))
        {
            d3d9->Release(); // done with d3d9 object
            ::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
            return false;
        }
    }

    d3d9->Release(); // done with d3d9 object

    return true;
}

int d3d::EnterMsgLoop(bool(*ptr_display)(float timeDelta))
{
    MSG msg;
    ::ZeroMemory(&msg, sizeof(MSG));

    static float lastTime = (float)timeGetTime();

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            float currTime = (float)timeGetTime();
            float timeDelta = (currTime - lastTime)*0.001f;

            ptr_display(timeDelta);

            lastTime = currTime;
        }
    }
    return msg.wParam;
}

D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Ambient = *color * 0.4f;
    light.Diffuse = *color;
    light.Specular = *color * 0.6f;
    light.Direction = *direction;

    return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_POINT;
    light.Ambient = *color * 0.4f;
    light.Diffuse = *color;
    light.Specular = *color * 0.6f;
    light.Position = *position;
    light.Range = 1000.0f;
    light.Falloff = 1.0f;
    light.Attenuation0 = 1.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;

    return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));

    light.Type = D3DLIGHT_SPOT;
    light.Ambient = *color * 1.0f;
    light.Diffuse = *color;
    light.Specular = *color * 1.0f;
    light.Position = *position;
    light.Direction = *direction;
    light.Range = 1000.0f;
    light.Falloff = 1.0f;
    light.Attenuation0 = 1.0f;
    light.Attenuation1 = 0.0f;
    light.Attenuation2 = 0.0f;
    light.Theta = 0.0f;
    light.Phi = 0.5f;

    return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
    D3DMATERIAL9 mtrl;
    mtrl.Ambient = a;
    mtrl.Diffuse = d;
    mtrl.Specular = s;
    mtrl.Emissive = e;
    mtrl.Power = p;
    return mtrl;
}

d3d::BoundingBox::BoundingBox()
{
    // infinite small 
    _min.x = d3d::_INFINITY;
    _min.y = d3d::_INFINITY;
    _min.z = d3d::_INFINITY;

    _max.x = -d3d::_INFINITY;
    _max.y = -d3d::_INFINITY;
    _max.z = -d3d::_INFINITY;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p)
{
    if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
        p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
    {
        return true;
    }
    else
    {
        return false;
    }
}

d3d::BoundingSphere::BoundingSphere()
{
    _radius = 0.0f;
}

bool d3d::DrawBasicScene(IDirect3DDevice9* device, float scale)
{
    static IDirect3DVertexBuffer9* floor = 0;       // 存放地板的顶点数据
    static IDirect3DTexture9*      tex = 0;         // 地板的纹理
    static ID3DXMesh*              pillar = 0;      // 柱子

    static IDirect3DVertexBuffer9* Cube = 0;   
	static IDirect3DIndexBuffer9* IB = 0;
	static IDirect3DTexture9* CubeTex = 0;
	static IDirect3DTexture9* AfraidTex = 0;
	static Vertex* vertices = 0;

    HRESULT hr = 0;

    if (device == 0)
    {
        if (floor && tex && pillar)
        {
            // they already exist, destroy them
            d3d::Release<IDirect3DVertexBuffer9*>(floor);
            d3d::Release<IDirect3DTexture9*>(tex);
            d3d::Release<ID3DXMesh*>(pillar);
        }
    }
    else if (!floor && !tex && !pillar)
    {
        // they don't exist, create them
        device->CreateVertexBuffer(
            6 * sizeof(d3d::Vertex),
            0,
            d3d::Vertex::FVF,
            D3DPOOL_MANAGED,
            &floor,
            0);

        Vertex* v = 0;
        floor->Lock(0, 0, (void**)&v, 0);

		float length = 10.0f;
		float high = 1.1f;

        v[0] = Vertex(-length, -high, -length, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        v[1] = Vertex(-length, -high, length, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
        v[2] = Vertex(length, -high, length, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

        v[3] = Vertex(-length, -high, -length, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        v[4] = Vertex(length, -high, length, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        v[5] = Vertex(length, -high, -length, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

        floor->Unlock();

		device->CreateVertexBuffer(
			24 * sizeof(d3d::Vertex),
			0,
			d3d::Vertex::FVF,
			D3DPOOL_MANAGED,
			&Cube,
			0
		);

		Cube->Lock(0, 0, (void**)&vertices, 0);

		// 填充前面的顶点数据
		vertices[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		vertices[1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		vertices[2] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		vertices[3] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// 填充背面的顶点数据
		vertices[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		vertices[5] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		vertices[6] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		vertices[7] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		// 填充顶面的顶点数据
		vertices[8] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		vertices[9] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		vertices[10] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		vertices[11] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		// 填充底面的顶点数据
		vertices[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
		vertices[13] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
		vertices[14] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
		vertices[15] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);

		// 填充左面的顶点数据
		vertices[16] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		vertices[17] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		vertices[18] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		vertices[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// 填充右面的顶点数据
		vertices[20] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		vertices[21] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		vertices[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		vertices[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		Cube->Unlock();

		device->CreateIndexBuffer(
			36 * sizeof(WORD),
			0,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&IB,
			0
		);

		WORD* indices = 0;
		IB->Lock(0, 0, (void**)&indices, 0);

		// 填充前面索引数据
		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 0; indices[4] = 2; indices[5] = 3;

		// 填充后面索引数据
		indices[6] = 4; indices[7] = 5; indices[8] = 6;
		indices[9] = 4; indices[10] = 6; indices[11] = 7;

		// 填充顶面索引数据
		indices[12] = 8; indices[13] = 9; indices[14] = 10;
		indices[15] = 8; indices[16] = 10; indices[17] = 11;

		// 填充底面索引数据
		indices[18] = 12; indices[19] = 13; indices[20] = 14;
		indices[21] = 12; indices[22] = 14; indices[23] = 15;

		// 填充左面索引数据
		indices[24] = 16; indices[25] = 17; indices[26] = 18;
		indices[27] = 16; indices[28] = 18; indices[29] = 19;

		// 填充右面索引数据
		indices[30] = 20; indices[31] = 21; indices[32] = 22;
		indices[33] = 20; indices[34] = 22; indices[35] = 23;

		IB->Unlock();

        // D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);	// 柱子

        D3DXCreateTextureFromFile(
            device,
            "floor.bmp",
            &tex);
		D3DXCreateTextureFromFile(
			device,
			"haipa.bmp",
			&CubeTex
		);
		D3DXCreateTextureFromFile(
			device,
			"haipaAlpha1.bmp",
			&AfraidTex
		);

		device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// alpha从纹理图片中获取
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    }
    else
    {

		static float high = 0.0f;
		if (::GetAsyncKeyState('U') & 0x8000f)
			high += 0.0001f;
		if (::GetAsyncKeyState('J') & 0x8000f)
			high -= 0.0001f;

		static float angle0 = D3DX_PI * 0.25f;
		if (angle0 <= D3DX_PI * 2.26f)
			angle0 += 0.0001f;

		static float distance = 6.0f;
		
		if (distance > 1.0f)
		{
			if (angle0 > 1.75f * D3DX_PI && angle0 < 2.25f * D3DX_PI) 
			{	
				distance -= 0.0003f;
			}
			else if (angle0 > 2.25f * D3DX_PI)
			{
				distance -= 0.0006f;
			}
		}
		
		high = 3.0f * sinf((angle0 - 0.25f * D3DX_PI) / 2.0f);

		D3DXVECTOR3 positionView(distance * cosf(angle0), high, distance * sinf(angle0));
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &positionView, &target, &up);
		device->SetTransform(D3DTS_VIEW, &V);


        //
        // Pre-Render Setup
        //
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// 关闭背面拣选

		// 平行光
        D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
        D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
        D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);
		// 点光源
		D3DXVECTOR3 pointLightPos(0.0f, 0.0f, 0.0f);
		D3DLIGHT9 pointLight = d3d::InitPointLight(&pointLightPos, &col);
		// 聚光灯
		D3DXVECTOR3 spotLightPos(0.0f, 0.0f, 0.0f);	//-0.9f, -0.9f, -0.9f
		D3DXVECTOR3 spotLightDir(-1.0f, -1.0f, -1.0f);
		D3DXCOLOR spotLightColor(1.0f, 0.0f, 0.0f, 1.0f);
		D3DLIGHT9 spotLight = d3d::InitSpotLight(&spotLightPos, &spotLightDir, &spotLightColor);

        device->SetLight(0, &light);
		device->SetLight(1, &pointLight);
		device->SetLight(2, &spotLight);
		device->LightEnable(0, true);
		device->LightEnable(1, true);
		// device->LightEnable(2, true);
        device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
        device->SetRenderState(D3DRS_SPECULARENABLE, true);

        //
        // Render
        //

        D3DXMATRIX T, R, P, S;

        D3DXMatrixScaling(&S, scale, scale, scale);

        // used to rotate cylinders to be parallel with world's y-axis
        D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

        // draw floor
        D3DXMatrixIdentity(&T);
        T = T * S;
        device->SetTransform(D3DTS_WORLD, &T);
        device->SetMaterial(&d3d::WHITE_MTRL);
        device->SetTexture(0, tex);
        device->SetStreamSource(0, floor, 0, sizeof(Vertex));
        device->SetFVF(Vertex::FVF);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);


		// draw Cube
		
		// modify the vertex of the cube
		static float angle = 0.0f;
		if (angle > -D3DX_PI * 0.5f && angle0 > D3DX_PI)
		{
			angle -= 0.00003f;
		}

		// render the cube
		device->SetStreamSource(0, Cube, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->SetIndices(IB);
		device->SetTexture(0, CubeTex);

		// device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 12, 0, 6);
		// device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 12, 18, 6);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);	// 前面
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 18, 2);	// 下面
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 24, 2);	// 左面
		
		D3DXMATRIX trans1, rotate, trans2, finalMat;
		D3DXMatrixTranslation(&trans1, -1.0f, 1.0f, 0.0f);
		D3DXMatrixRotationZ(&rotate, angle);
		D3DXMatrixTranslation(&trans2, 1.0f, -1.0f, 0.0f);
		finalMat = trans1*rotate*trans2;
		device->SetTransform(D3DTS_WORLD, &finalMat);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 30, 2);	// 右面	

		D3DXMatrixTranslation(&trans1, 0.0f, 1.0f, -1.0f);
		D3DXMatrixRotationX(&rotate, -angle);
		D3DXMatrixTranslation(&trans2, 0.0f, -1.0f, 1.0f);
		finalMat = trans1*rotate*trans2;
		device->SetTransform(D3DTS_WORLD, &finalMat);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 6, 2);	// 后面

		D3DXVECTOR3 position(0.0f, -1.0f, 1.0f);
		position.x = 0.0f;
		position.y = -1.0f + 2.0f * cos(-angle);	// angle是负值
		position.z = 1.0f + 2.0f * sin(-angle);
		D3DXMatrixTranslation(&trans1, 0.0f, -1.0f, -1.0f);
		D3DXMatrixRotationX(&rotate, -1.8f * angle);
		D3DXMatrixTranslation(&trans2, position.x, position.y, position.z);
		finalMat = trans1*rotate*trans2;
		device->SetTransform(D3DTS_WORLD, &finalMat);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 12, 2);	// 上面

		// 害怕表情
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);		// 开启融合
		device->SetTexture(0, AfraidTex);
		D3DXMATRIX scaleMat;
		D3DXMatrixScaling(&scaleMat, 0.5f, 0.5f, 0.5f);
		D3DXMatrixTranslation(&trans1, 0.0f, 0.0f, 0.0f);
		D3DXMatrixRotationY(&rotate, -0.75f * D3DX_PI);
		D3DXMatrixTranslation(&trans2, -0.7f, -0.7f, -0.7f);
		finalMat = scaleMat * trans1 * rotate * trans2;
		device->SetTransform(D3DTS_WORLD, &finalMat);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		// draw pillars
        /*device->SetMaterial(&d3d::BLUE_MTRL);
        device->SetTexture(0, 0);
        for (int i = 0; i < 5; i++)
        {
            D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
            P = R * T * S;
            device->SetTransform(D3DTS_WORLD, &P);
            pillar->DrawSubset(0);

            D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
            P = R * T * S;
            device->SetTransform(D3DTS_WORLD, &P);
            pillar->DrawSubset(0);
        }*/
    }
    return true;
}

