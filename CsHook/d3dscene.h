
// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory

typedef struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;    // from the D3DFVF_XYZRHW flag
	DWORD color;    // from the D3DFVF_DIFFUSE flag
} CUSTOMVERTEX;

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
