#include "stdafx.h"
#include "d3dscene.h"


// global declarations
IDirect3D9* d3d;    // the pointer to our Direct3D interface
IDirect3DDevice9* d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer; // vertex buffer

CUSTOMVERTEX tri[] =
{
	{ 2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
	{ 0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
	{ -2.5f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
};

void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D

	// create a device class using this information and information from the d3dpp stuct
	d3d->lpVtbl->CreateDevice(d3d, D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	// create the vertex and store the pointer into v_buffer, which is created globally
	d3ddev->lpVtbl->CreateVertexBuffer(d3ddev, 3 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	d3ddev->lpVtbl->SetRenderState(d3ddev, D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
	d3ddev->lpVtbl->SetRenderState(d3ddev, D3DRS_CULLMODE, D3DCULL_NONE);
	
	VOID* pVoid;    // the void pointer

	v_buffer->lpVtbl->Lock(v_buffer, 0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
	memcpy(pVoid, tri, sizeof(tri));    // copy the vertices to the locked buffer
	v_buffer->lpVtbl->Unlock(v_buffer);    // unlock the vertex buffer
}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->lpVtbl->Clear(d3ddev, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

	d3ddev->lpVtbl->BeginScene(d3ddev);    // begins the 3D scene

	// do 3D rendering on the back buffer here
	d3ddev->lpVtbl->SetFVF(d3ddev, CUSTOMFVF);

	// SET UP THE PIPELINE

	D3DMATRIX matRotateY;    // a matrix to store the rotation information

	static float index = 0.0f; index += 0.05f;    // an ever-increasing float value

	// build a matrix to rotate the model based on the increasing float value
	D3DXMatrixRotationY(&matRotateY, index);

	// tell Direct3D about our matrix
	d3ddev->lpVtbl->SetTransform(d3ddev, D3DTS_WORLD, &matRotateY);

	D3DMATRIX matView;    // the view transform matrix
	D3DXVECTOR3 v1 = { 0.0f, 0.0f, 10.0f };
	D3DXVECTOR3 v2 = { 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3 v3 = { 0.0f, 1.0f, 0.0f };
	D3DXMatrixLookAtLH(&matView,
		&v1,    // the camera position
		&v2,    // the look-at position
		&v3);    // the up direction

	d3ddev->lpVtbl->SetTransform(d3ddev, D3DTS_VIEW, &matView);    // set the view transform to matView

	D3DMATRIX matProjection;     // the projection transform matrix

	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),    // the horizontal field of view
		(FLOAT)640 / (FLOAT)480, // aspect ratio
		1.0f,    // the near view-plane
		100.0f);    // the far view-plane

	d3ddev->lpVtbl->SetTransform(d3ddev, D3DTS_PROJECTION, &matProjection);    // set the projection


	d3ddev->lpVtbl->SetStreamSource(d3ddev, 0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->lpVtbl->DrawPrimitive(d3ddev, D3DPT_TRIANGLELIST, 0, 1);

	d3ddev->lpVtbl->EndScene(d3ddev);    // ends the 3D scene

	d3ddev->lpVtbl->Present(d3ddev, NULL, NULL, NULL, NULL);    // displays the created frame
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	v_buffer->lpVtbl->Release(v_buffer);    // close and release the vertex buffer
	d3ddev->lpVtbl->Release(d3ddev);    // close and release the 3D device
	d3d->lpVtbl->Release(d3d);    // close and release Direct3D
}
