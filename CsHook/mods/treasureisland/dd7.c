#include "stdafx.h"
#include <ddraw.h>
#include <stdio.h>

LPPALETTEENTRY _makePalette_8(int col, int count) {
	LPPALETTEENTRY rv;
	rv = malloc(sizeof(PALETTEENTRY)*count);
	for (int i = 0; i < count; i++) {
		rv[i].peFlags = 0;
		rv[i].peRed = (col >> 16) & 0xFF;
		rv[i].peGreen = (col >> 8) & 0xFF;
		rv[i].peBlue = col & 0xFF;
	}
	return rv;
}

void setPalette(int surfID, int col) {
	LPDIRECTDRAWSURFACE7 surf = CS_imgSurfaces[surfID];
	if (surf) {
		LPDIRECTDRAWPALETTE palette;
		INT32 paletteCaps;
		HRESULT result = surf->lpVtbl->GetPalette(surf, &palette);
		switch (result) {
		case DD_OK:
			palette->lpVtbl->GetCaps(palette, &paletteCaps);
			if (paletteCaps & DDPCAPS_8BIT) {
				LPPALETTEENTRY entries = _makePalette_8(col, 255);
				palette->lpVtbl->SetEntries(palette, 0, 1, 255, entries);
				free(entries);
			}
			break;
		case DDERR_INVALIDOBJECT:
			printf("invalid object");
			break;
		case DDERR_INVALIDPARAMS:
			printf("invalid params");
			break;
		case DDERR_NOPALETTEATTACHED:
			printf("No palette");
			break;
		case DDERR_GENERIC:
			printf("generic");
			break;
		case DDERR_UNSUPPORTED:
			printf("unsupported");
			break;
		}
	}
}
