
//**************************************************************************
//**
//** r_draw.c : Heretic 2 : Raven Software, Corp.
//**
//** $RCSfile: r_draw.c,v $
//** $Revision: 1.5 $
//** $Date: 2008-06-22 16:20:45 $
//** $Author: sezero $
//**
//**************************************************************************

#include "h2stdinc.h"
#include "h2def.h"
#include "r_local.h"

#ifdef RENDER3D
#include "ogl_def.h"
#endif

/*

All drawing to the view buffer is accomplished in this file.  The other refresh
files only know about ccordinates, not the architecture of the frame buffer.

*/

int	viewwidth, scaledviewwidth, viewheight, viewwindowx, viewwindowy;
byte	*ylookup[MAXHEIGHT];
int	columnofs[MAXWIDTH];
//byte	translations[3][256];	// color tables for different players
byte	*tinttable;		// used for translucent sprites


#ifndef RENDER3D

/*
==================
=
= R_DrawColumn
=
= Source is the top of the column to scale
=
==================
*/

lighttable_t	*dc_colormap;
int			dc_x;
int			dc_yl;
int			dc_yh;
fixed_t		dc_iscale;
fixed_t		dc_texturemid;
byte		*dc_source;		// first pixel in a column (possibly virtual)

//int		dccount;		// just for profiling


//#ifndef __i386
//#ifndef __m68k
void R_DrawColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x]; 

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = dc_colormap[dc_source[(frac>>FRACBITS) & 127]];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}
//#endif	// __m68k
//#endif	// __i386

void R_DrawColumnLow (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
//	dccount++;
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = dc_colormap[dc_source[(frac>>FRACBITS) & 127]];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}

/*
#define FUZZTABLE		50
#define FUZZOFF		(SCREENWIDTH)
static int	fuzzoffset[FUZZTABLE] =
{
	FUZZOFF,-FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
	FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
	FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,
	FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
	FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,
	FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,
	FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF
};
static int	fuzzpos = 0;
*/

void R_DrawFuzzColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight - 1)
		dc_yh = viewheight - 2;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

// OLD FUZZY INVISO SPRITE STUFF
/*
	do
	{
		*dest = colormaps[6*256 + dest[fuzzoffset[fuzzpos]]];
		if (++fuzzpos == FUZZTABLE)
			fuzzpos = 0;
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
*/
	do
	{
		*dest = tinttable[*dest + (dc_colormap[dc_source[(frac>>FRACBITS) & 127]]<<8)];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}


//============================================================================
//
// R_DrawAltFuzzColumn
//
//============================================================================

void R_DrawAltFuzzColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight - 1)
		dc_yh = viewheight - 2;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = tinttable[((*dest)<<8) + dc_colormap[dc_source[(frac>>FRACBITS) & 127]]];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}

/*
========================
=
= R_DrawTranslatedColumn
=
========================
*/

byte	*dc_translation;
byte	*translationtables;

void R_DrawTranslatedColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}

//============================================================================
//
// R_DrawTranslatedFuzzColumn
//
//============================================================================

void R_DrawTranslatedFuzzColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = tinttable[((*dest)<<8) + dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]]];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}

//============================================================================
//
// R_DrawTranslatedAltFuzzColumn
//
//============================================================================

/*
void R_DrawTranslatedAltFuzzColumn (void)
{
	int		count;
	byte		*dest;
	fixed_t		frac, fracstep;

	count = dc_yh - dc_yl;
	if (count < 0)
		return;

#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl - centery)*fracstep;

	do
	{
		*dest = tinttable[*dest + (dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]]<<8)];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}
*/

//--------------------------------------------------------------------------
//
// PROC R_InitTranslationTables
//
//--------------------------------------------------------------------------

/* version 1.0 wad has 9 lumps: trantbl0 ... trantbl8 */
#if 0
static byte transtable10[256 * 3 * (MAXPLAYERS_10 - 1)] =
{
#  include "transtb10.h"
};
#endif
/* version 1.1 wad has 21 lumps: trantbl0 .. trantbl9 
			    and  trantbla .. trantblk */
#if (MAXPLAYERS == MAXPLAYERS_11)
static byte transtable11[256 * 3 * (MAXPLAYERS_11 - 1)] =
{
#  include "transtb11.h"
};
#define TRANTBL11_OFS	(256 * 3 * (MAXPLAYERS_10 - 1))
#define TRANTBL11_CNT	(256 * 3 * (MAXPLAYERS_11 - MAXPLAYERS_10))
#endif

void R_InitTranslationTables (void)
{
	int		i;
	byte	*transLump;

	// Load tint table
	tinttable = W_CacheLumpName("TINTTAB", PU_STATIC);

	// Allocate translation tables
	translationtables = Z_Malloc(256 * 3 * (MAXPLAYERS - 1) + 255, PU_STATIC, 0);
	translationtables = (byte *)(((int)translationtables + 255) & ~255);

	for (i = 0; i < 3 * (MAXPLAYERS - 1); i++)
	{
#if (MAXPLAYERS == MAXPLAYERS_11)
		if (oldwad_10 && i == 3 * (MAXPLAYERS_10 - 1))
		{
			/* HACK !! ---  old 1.0 wad doesn't have TRANTBL9
			 *		to TRANTBLK. Let's just copy from
			 *		the extracted v1.1 data.	*/
			memcpy (translationtables + TRANTBL11_OFS,
				transtable11 + TRANTBL11_OFS, TRANTBL11_CNT);
			break;
		}
#endif	/* 8-players */
		transLump = W_CacheLumpNum(W_GetNumForName("trantbl0") + i, PU_STATIC);
		memcpy(translationtables + i*256, transLump, 256);
		Z_Free(transLump);
	}
}

/*
================
=
= R_DrawSpan
=
================
*/

int			ds_y;
int			ds_x1;
int			ds_x2;
lighttable_t	*ds_colormap;
fixed_t			ds_xfrac;
fixed_t			ds_yfrac;
fixed_t			ds_xstep;
fixed_t			ds_ystep;
byte			*ds_source;		// start of a 64*64 tile image

//int			dscount;		// just for profiling


//#ifndef __i386
//#ifndef __m68k
void R_DrawSpan (void)
{
	fixed_t		xfrac, yfrac;
	byte		*dest;
	int		count, spot;

#ifdef RANGECHECK
	if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= SCREENWIDTH || (unsigned)ds_y > SCREENHEIGHT)
		I_Error ("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
//	dscount++;
#endif

	xfrac = ds_xfrac;
	yfrac = ds_yfrac;

	dest = ylookup[ds_y] + columnofs[ds_x1];
	count = ds_x2 - ds_x1;
	do
	{
		spot = ((yfrac>>(16-6)) & (63*64)) + ((xfrac>>16) & 63);
		*dest++ = ds_colormap[ds_source[spot]];
		xfrac += ds_xstep;
		yfrac += ds_ystep;
	} while (count--);
}
//#endif
//#endif

void R_DrawSpanLow (void)
{
	fixed_t		xfrac, yfrac;
	byte		*dest;
	int		count, spot;

#ifdef RANGECHECK
	if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= SCREENWIDTH || (unsigned)ds_y > SCREENHEIGHT)
		I_Error ("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
//	dscount++;
#endif

	xfrac = ds_xfrac;
	yfrac = ds_yfrac;

	dest = ylookup[ds_y] + columnofs[ds_x1];
	count = ds_x2 - ds_x1;
	do
	{
		spot = ((yfrac>>(16-6)) & (63*64)) + ((xfrac>>16) & 63);
		*dest++ = ds_colormap[ds_source[spot]];
		xfrac += ds_xstep;
		yfrac += ds_ystep;
	} while (count--);
}

#endif	/* !RENDER3D */


/*
================
=
= R_InitBuffer
=
=================
*/

void R_InitBuffer (int width, int height)
{
	int		i;

	viewwindowx = (SCREENWIDTH - width) >> 1;
	for (i = 0; i < width; i++)
		columnofs[i] = viewwindowx + i;
	if (width == SCREENWIDTH)
		viewwindowy = 0;
	else
		viewwindowy = (SCREENHEIGHT - SBARHEIGHT - height) >> 1;
	for (i = 0; i < height; i++)
		ylookup[i] = screen + (i + viewwindowy)*SCREENWIDTH;
}


/*
==================
=
= R_DrawViewBorder
=
= Draws the border around the view for different size windows
==================
*/

boolean		BorderNeedRefresh;

void R_DrawViewBorder (void)
{
#ifdef RENDER3D
	int lump;

#if 0
	if (scaledviewwidth == SCREENWIDTH)
		return;
#else
	if ((scaledviewwidth == 320 && sbarscale == 20) ||
		(sbarscale != 20 && viewheight == 200))
		return;
#endif

	// View background.
	OGL_SetColorAndAlpha(1, 1, 1, 1);
	OGL_SetFlat(W_GetNumForName("F_022") - firstflat);

#if 1
	OGL_DrawCutRectTiled(0, 0, 320, 200 - SBARHEIGHT, 64, 64,
			viewwindowx - 4, viewwindowy - 4, viewwidth + 8, viewheight + 8);
#else
	//OGL_DrawRectTiled(0, 0, SCREENWIDTH, SCREENHEIGHT-SBARHEIGHT, 64, 64);
	OGL_DrawCutRectTiled(0, 0, 320, 200 - (sbarscale == 20 ? SBARHEIGHT : 0)
			/**sbarscale/20*/, 64, 64,
			viewwindowx - 4, viewwindowy - 4, viewwidth + 8, viewheight + 8);
#endif

	// The border top.
	OGL_SetPatch(lump = W_GetNumForName("bordt"));
	OGL_DrawRectTiled(viewwindowx, viewwindowy - 4, viewwidth,
			  lumptexsizes[lump].h, 16, lumptexsizes[lump].h);
	// Border bottom.
	OGL_SetPatch(lump = W_GetNumForName("bordb"));
	OGL_DrawRectTiled(viewwindowx, viewwindowy + viewheight, viewwidth,
			  lumptexsizes[lump].h, 16, lumptexsizes[lump].h);
	// Left view border.
	OGL_SetPatch(lump = W_GetNumForName("bordl"));
	OGL_DrawRectTiled(viewwindowx - 4, viewwindowy, lumptexsizes[lump].w,
			  viewheight, lumptexsizes[lump].w, 16);
	// Right view border.
	OGL_SetPatch(lump=W_GetNumForName("bordr"));
	OGL_DrawRectTiled(viewwindowx + viewwidth, viewwindowy,
			  lumptexsizes[lump].w, viewheight, lumptexsizes[lump].w, 16);

	OGL_DrawPatch(viewwindowx - 4, viewwindowy - 4, W_GetNumForName("bordtl"));
	OGL_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, W_GetNumForName("bordtr"));
	OGL_DrawPatch(viewwindowx + viewwidth, viewwindowy + viewheight, W_GetNumForName("bordbr"));
	OGL_DrawPatch(viewwindowx - 4, viewwindowy + viewheight, W_GetNumForName("bordbl"));

#else

	byte	*src, *dest;
	int		x, y;

	if (scaledviewwidth == SCREENWIDTH)
		return;

	src = W_CacheLumpName("F_022", PU_CACHE);
	dest = screen;

	for (y = 0 ; y < SCREENHEIGHT - SBARHEIGHT; y++)
	{
		for (x = 0; x < SCREENWIDTH/64; x++)
		{
			memcpy (dest, src + ((y & 63)<<6), 64);
			dest += 64;
		}
		if (SCREENWIDTH & 63)
		{
			memcpy (dest, src + ((y & 63)<<6), SCREENWIDTH & 63);
			dest += (SCREENWIDTH & 63);
		}
	}
	for (x = viewwindowx; x < viewwindowx + viewwidth; x += 16)
	{
		V_DrawPatch(x, viewwindowy - 4, W_CacheLumpName("bordt", PU_CACHE));
		V_DrawPatch(x, viewwindowy + viewheight, W_CacheLumpName("bordb", PU_CACHE));
	}
	for (y = viewwindowy; y < viewwindowy + viewheight; y += 16)
	{
		V_DrawPatch(viewwindowx - 4, y, W_CacheLumpName("bordl", PU_CACHE));
		V_DrawPatch(viewwindowx+viewwidth, y, W_CacheLumpName("bordr", PU_CACHE));
	}
	V_DrawPatch(viewwindowx - 4, viewwindowy - 4, W_CacheLumpName("bordtl", PU_CACHE));
	V_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, W_CacheLumpName("bordtr", PU_CACHE));
	V_DrawPatch(viewwindowx + viewwidth, viewwindowy + viewheight, W_CacheLumpName("bordbr", PU_CACHE));
	V_DrawPatch(viewwindowx - 4, viewwindowy + viewheight, W_CacheLumpName("bordbl", PU_CACHE));
#endif
}

/*
==================
=
= R_DrawTopBorder
=
= Draws the top border around the view for different size windows
==================
*/

boolean		BorderTopRefresh;

void R_DrawTopBorder (void)
{
#ifdef RENDER3D
	if (scaledviewwidth == SCREENWIDTH)
		return;

	OGL_SetColorAndAlpha(1, 1, 1, 1);
	OGL_SetFlat(W_GetNumForName("F_022") - firstflat);

	OGL_DrawRectTiled(0, 0, 320, 64, 64, 64);
	if (viewwindowy < 65)
	{
		int	lump;
		OGL_SetPatch(lump = W_GetNumForName("bordt"));
		OGL_DrawRectTiled(viewwindowx, viewwindowy - 4, viewwidth,
				  lumptexsizes[lump].h, 16, lumptexsizes[lump].h);

		OGL_DrawPatch(viewwindowx - 4, viewwindowy, W_GetNumForName("bordl"));
		OGL_DrawPatch(viewwindowx + viewwidth, viewwindowy, W_GetNumForName("bordr"));
		OGL_DrawPatch(viewwindowx - 4, viewwindowy + 16, W_GetNumForName("bordl"));
		OGL_DrawPatch(viewwindowx + viewwidth, viewwindowy + 16, W_GetNumForName("bordr"));

		OGL_DrawPatch(viewwindowx - 4, viewwindowy - 4, W_GetNumForName("bordtl"));
		OGL_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, W_GetNumForName("bordtr"));
	}

#else

	byte	*src, *dest;
	int		x, y;

	if (scaledviewwidth == SCREENWIDTH)
		return;

	src = W_CacheLumpName("F_022", PU_CACHE);
	dest = screen;

	for (y = 0; y < 34; y++)
	{
		for (x = 0; x < SCREENWIDTH/64; x++)
		{
			memcpy (dest, src + ((y & 63)<<6), 64);
			dest += 64;
		}
		if (SCREENWIDTH & 63)
		{
			memcpy (dest, src + ((y & 63)<<6), SCREENWIDTH & 63);
			dest += (SCREENWIDTH & 63);
		}
	}
	if (viewwindowy < 35)
	{
		for (x = viewwindowx; x < viewwindowx + viewwidth; x += 16)
		{
			V_DrawPatch(x, viewwindowy - 4, W_CacheLumpName("bordt", PU_CACHE));
		}
		V_DrawPatch(viewwindowx-4, viewwindowy, W_CacheLumpName("bordl", PU_CACHE));
		V_DrawPatch(viewwindowx + viewwidth, viewwindowy, W_CacheLumpName("bordr", PU_CACHE));
		V_DrawPatch(viewwindowx - 4, viewwindowy + 16, W_CacheLumpName("bordl", PU_CACHE));
		V_DrawPatch(viewwindowx + viewwidth, viewwindowy + 16, W_CacheLumpName("bordr", PU_CACHE));

		V_DrawPatch(viewwindowx - 4, viewwindowy - 4, W_CacheLumpName("bordtl", PU_CACHE));
		V_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, W_CacheLumpName("bordtr", PU_CACHE));
	}
#endif
}

