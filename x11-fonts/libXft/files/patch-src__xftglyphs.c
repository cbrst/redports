--- src/xftglyphs.c	2009-10-16 23:27:08.000000000 +0200
+++ src/xftglyphs.c	2010-11-03 11:56:52.780577002 +0100
@@ -21,27 +21,18 @@
  */
 
 #include "xftint.h"
-#include <freetype/ftoutln.h>
 
 #if HAVE_FT_GLYPHSLOT_EMBOLDEN
 #include <freetype/ftsynth.h>
 #endif
 
-static const int    filters[3][3] = {
-    /* red */
-#if 0
-{    65538*4/7,65538*2/7,65538*1/7 },
-    /* green */
-{    65536*1/4, 65536*2/4, 65537*1/4 },
-    /* blue */
-{    65538*1/7,65538*2/7,65538*4/7 },
+#if FREETYPE_MAJOR*10000 + FREETYPE_MINOR*100 + FREETYPE_PATCH < 20202
+#  error  "FreeType 2.2.2 or later required to compile this version of libXft"
 #endif
-{    65538*9/13,65538*3/13,65538*1/13 },
-    /* green */
-{    65538*1/6, 65538*4/6, 65538*1/6 },
-    /* blue */
-{    65538*1/13,65538*3/13,65538*9/13 },
-};
+
+#include FT_OUTLINE_H
+#include FT_LCD_FILTER_H
+#include FT_SYNTHESIS_H
 
 /*
  * Validate the memory info for a font
@@ -69,6 +60,295 @@
 		font->glyph_memory, glyph_memory);
 }
 
+
+/* we sometimes need to convert the glyph bitmap in a FT_GlyphSlot
+ * into a different format. For example, we want to convert a
+ * FT_PIXEL_MODE_LCD or FT_PIXEL_MODE_LCD_V bitmap into a 32-bit
+ * ARGB or ABGR bitmap.
+ *
+ * this function prepares a target descriptor for this operation.
+ *
+ * input :: target bitmap descriptor. The function will set its
+ *          'width', 'rows' and 'pitch' fields, and only these
+ *
+ * slot  :: the glyph slot containing the source bitmap. this
+ *          function assumes that slot->format == FT_GLYPH_FORMAT_BITMAP
+ *
+ * mode  :: the requested final rendering mode. supported values are
+ *          MONO, NORMAL (i.e. gray), LCD and LCD_V
+ *
+ * the function returns the size in bytes of the corresponding buffer,
+ * it's up to the caller to allocate the corresponding memory block
+ * before calling _fill_xrender_bitmap
+ *
+ * it also returns -1 in case of error (e.g. incompatible arguments,
+ * like trying to convert a gray bitmap into a monochrome one)
+ */
+static int
+_compute_xrender_bitmap_size( FT_Bitmap*      target,
+                              FT_GlyphSlot    slot,
+                              FT_Render_Mode  mode )
+{
+    FT_Bitmap*  ftbit;
+    int         width, height, pitch;
+
+    if ( slot->format != FT_GLYPH_FORMAT_BITMAP )
+        return -1;
+
+    // compute the size of the final bitmap
+    ftbit  = &slot->bitmap;
+
+    width  = ftbit->width;
+    height = ftbit->rows;
+    pitch  = (width+3) & ~3;
+
+    switch ( ftbit->pixel_mode )
+    {
+    case FT_PIXEL_MODE_MONO:
+        if ( mode == FT_RENDER_MODE_MONO )
+        {
+            pitch = (((width+31) & ~31) >> 3);
+            break;
+        }
+        /* fall-through */
+
+    case FT_PIXEL_MODE_GRAY:
+        if ( mode == FT_RENDER_MODE_LCD   ||
+             mode == FT_RENDER_MODE_LCD_V )
+        {
+          /* each pixel is replicated into a 32-bit ARGB value */
+          pitch = width*4;
+        }
+        break;
+
+    case FT_PIXEL_MODE_LCD:
+        if ( mode != FT_RENDER_MODE_LCD )
+            return -1;
+
+        /* horz pixel triplets are packed into 32-bit ARGB values */
+        width   /= 3;
+        pitch    = width*4;
+        break;
+
+    case FT_PIXEL_MODE_LCD_V:
+        if ( mode != FT_RENDER_MODE_LCD_V )
+            return -1;
+
+        /* vert pixel triplets are packed into 32-bit ARGB values */
+        height  /= 3;
+        pitch    = width*4;
+        break;
+
+    default:  /* unsupported source format */
+        return -1;
+    }
+
+    target->width  = width;
+    target->rows   = height;
+    target->pitch  = pitch;
+    target->buffer = NULL;
+
+    return pitch * height;
+}
+
+/* this functions converts the glyph bitmap found in a FT_GlyphSlot
+ * into a different format (see _compute_xrender_bitmap_size)
+ *
+ * you should call this function after _compute_xrender_bitmap_size
+ *
+ * target :: target bitmap descriptor. Note that its 'buffer' pointer
+ *           must point to memory allocated by the caller
+ *
+ * slot   :: the glyph slot containing the source bitmap
+ *
+ * mode   :: the requested final rendering mode
+ *
+ * bgr    :: boolean, set if BGR or VBGR pixel ordering is needed
+ */
+static void
+_fill_xrender_bitmap( FT_Bitmap*      target,
+                      FT_GlyphSlot    slot,
+                      FT_Render_Mode  mode,
+                      int             bgr )
+{
+    FT_Bitmap*   ftbit = &slot->bitmap;
+
+    {
+        unsigned char*   srcLine   = ftbit->buffer;
+        unsigned char*   dstLine   = target->buffer;
+        int              src_pitch = ftbit->pitch;
+        int              width     = target->width;
+        int              height    = target->rows;
+        int              pitch     = target->pitch;
+        int              subpixel;
+        int              h;
+
+        subpixel = ( mode == FT_RENDER_MODE_LCD ||
+                     mode == FT_RENDER_MODE_LCD_V );
+
+        if ( src_pitch < 0 )
+          srcLine -= src_pitch*(ftbit->rows-1);
+
+        switch ( ftbit->pixel_mode )
+        {
+        case FT_PIXEL_MODE_MONO:
+            if ( subpixel )  /* convert mono to ARGB32 values */
+            {
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                {
+                    int  x;
+
+                    for ( x = 0; x < width; x++ )
+                    {
+                        if ( srcLine[(x >> 3)] & (0x80 >> (x & 7)) )
+                            ((unsigned int*)dstLine)[x] = 0xffffffffU;
+                    }
+                }
+            }
+            else if ( mode == FT_RENDER_MODE_NORMAL )  /* convert mono to 8-bit gray */
+            {
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                {
+                    int  x;
+
+                    for ( x = 0; x < width; x++ )
+                    {
+                        if ( srcLine[(x >> 3)] & (0x80 >> (x & 7)) )
+                            dstLine[x] = 0xff;
+                    }
+                }
+            }
+            else  /* copy mono to mono */
+            {
+                int  bytes = (width+7) >> 3;
+
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                    memcpy( dstLine, srcLine, bytes );
+            }
+            break;
+
+        case FT_PIXEL_MODE_GRAY:
+            if ( subpixel )  /* convert gray to ARGB32 values */
+            {
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                {
+                    int            x;
+                    unsigned int*  dst = (unsigned int*)dstLine;
+
+                    for ( x = 0; x < width; x++ )
+                    {
+                        unsigned int  pix = srcLine[x];
+
+                        pix |= (pix << 8);
+                        pix |= (pix << 16);
+
+                        dst[x] = pix;
+                    }
+                }
+            }
+            else  /* copy gray into gray */
+            {
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                    memcpy( dstLine, srcLine, width );
+            }
+            break;
+
+        case FT_PIXEL_MODE_LCD:
+            if ( !bgr )
+            {
+                /* convert horizontal RGB into ARGB32 */
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                {
+                    int            x;
+                    unsigned char* src = srcLine;
+                    unsigned int*  dst = (unsigned int*)dstLine;
+
+                    for ( x = 0; x < width; x++, src += 3 )
+                    {
+                        unsigned int  pix;
+
+                        pix = ((unsigned int)src[0] << 16) |
+                              ((unsigned int)src[1] <<  8) |
+                              ((unsigned int)src[2]      ) |
+                              ((unsigned int)src[1] << 24) ;
+
+                        dst[x] = pix;
+                    }
+                }
+            }
+            else
+            {
+                /* convert horizontal BGR into ARGB32 */
+                for ( h = height; h > 0; h--, srcLine += src_pitch, dstLine += pitch )
+                {
+                    int            x;
+                    unsigned char* src = srcLine;
+                    unsigned int*  dst = (unsigned int*)dstLine;
+
+                    for ( x = 0; x < width; x++, src += 3 )
+                    {
+                        unsigned int  pix;
+
+                        pix = ((unsigned int)src[2] << 16) |
+                              ((unsigned int)src[1] <<  8) |
+                              ((unsigned int)src[0]      ) |
+                              ((unsigned int)src[1] << 24) ;
+
+                        dst[x] = pix;
+                    }
+                }
+            }
+            break;
+
+        default:  /* FT_PIXEL_MODE_LCD_V */
+            /* convert vertical RGB into ARGB32 */
+            if ( !bgr )
+            {
+                for ( h = height; h > 0; h--, srcLine += 3*src_pitch, dstLine += pitch )
+                {
+                    int            x;
+                    unsigned char* src = srcLine;
+                    unsigned int*  dst = (unsigned int*)dstLine;
+
+                    for ( x = 0; x < width; x++, src += 1 )
+                    {
+                        unsigned int  pix;
+
+                        pix = ((unsigned int)src[0]           << 16) |
+                              ((unsigned int)src[src_pitch]   <<  8) |
+                              ((unsigned int)src[src_pitch*2]      ) |
+                              ((unsigned int)src[src_pitch]   << 24) ;
+
+                        dst[x] = pix;
+                    }
+                }
+            }
+            else
+            {
+                for ( h = height; h > 0; h--, srcLine += 3*src_pitch, dstLine += pitch )
+                {
+                    int            x;
+                    unsigned char* src = srcLine;
+                    unsigned int*  dst = (unsigned int*)dstLine;
+
+                    for ( x = 0; x < width; x++, src += 1 )
+                    {
+                        unsigned int  pix;
+
+                        pix = ((unsigned int)src[src_pitch*2] << 16) |
+                              ((unsigned int)src[src_pitch]   <<  8) |
+                              ((unsigned int)src[0]                ) |
+                              ((unsigned int)src[src_pitch]   << 24) ;
+
+                        dst[x] = pix;
+                    }
+                }
+            }
+        }
+    }
+}
+
+
 _X_EXPORT void
 XftFontLoadGlyphs (Display	    *dpy,
 		   XftFont	    *pub,
@@ -87,20 +367,14 @@
     unsigned char   *bufBitmap = bufLocal;
     int		    bufSize = sizeof (bufLocal);
     int		    size, pitch;
-    unsigned char   bufLocalRgba[4096];
-    unsigned char   *bufBitmapRgba = bufLocalRgba;
-    int		    bufSizeRgba = sizeof (bufLocalRgba);
-    int		    sizergba, pitchrgba, widthrgba;
     int		    width;
     int		    height;
     int		    left, right, top, bottom;
-    int		    hmul = 1;
-    int		    vmul = 1;
-    FT_Bitmap	    ftbit;
-    FT_Matrix	    matrix;
+    FT_Bitmap*	    ftbit;
+    FT_Bitmap       local;
     FT_Vector	    vector;
-    Bool	    subpixel = False;
     FT_Face	    face;
+    FT_Render_Mode  mode = FT_RENDER_MODE_MONO;
 
     if (!info)
 	return;
@@ -110,24 +384,19 @@
     if (!face)
 	return;
 
-    matrix.xx = matrix.yy = 0x10000L;
-    matrix.xy = matrix.yx = 0;
-
     if (font->info.antialias)
     {
 	switch (font->info.rgba) {
 	case FC_RGBA_RGB:
 	case FC_RGBA_BGR:
-	    matrix.xx *= 3;
-	    subpixel = True;
-	    hmul = 3;
+	    mode = FT_RENDER_MODE_LCD;
 	    break;
 	case FC_RGBA_VRGB:
 	case FC_RGBA_VBGR:
-	    matrix.yy *= 3;
-	    vmul = 3;
-	    subpixel = True;
+            mode = FT_RENDER_MODE_LCD_V;
 	    break;
+        default:
+            mode = FT_RENDER_MODE_NORMAL;
 	}
     }
 
@@ -148,7 +417,10 @@
 	if (xftg->glyph_memory)
 	    continue;
 	
+        FT_Library_SetLcdFilter( _XftFTlibrary, font->info.lcd_filter);
+
 	error = FT_Load_Glyph (face, glyphindex, font->info.load_flags);
+
 	if (error)
 	{
 	    /*
@@ -181,7 +453,7 @@
 	/*
 	 * Compute glyph metrics from FreeType information
 	 */
-	if(font->info.transform && glyphslot->format != ft_glyph_format_bitmap) 
+	if(font->info.transform && glyphslot->format != FT_GLYPH_FORMAT_BITMAP)
 	{
 	    /*
 	     * calculate the true width by transforming all four corners.
@@ -260,17 +532,14 @@
 	    }
 	}
 
-	if (font->info.antialias)
-	    pitch = (width * hmul + 3) & ~3;
-	else
-	    pitch = ((width + 31) & ~31) >> 3;
-
-	size = pitch * height * vmul;
+        if ( glyphslot->format != FT_GLYPH_FORMAT_BITMAP )
+        {
+            error = FT_Render_Glyph( face->glyph, mode );
+            if (error)
+                continue;
+        }
 
-	xftg->metrics.width = width;
-	xftg->metrics.height = height;
-	xftg->metrics.x = -TRUNC(left);
-	xftg->metrics.y = TRUNC(top);
+        FT_Library_SetLcdFilter( _XftFTlibrary, FT_LCD_FILTER_NONE );
 
 	if (font->info.spacing >= FC_MONO)
 	{
@@ -310,103 +579,13 @@
 	    xftg->metrics.yOff = -TRUNC(ROUND(glyphslot->advance.y));
 	}
 	
-	/*
-	 * If the glyph is relatively large (> 1% of server memory),
-	 * don't send it until necessary
-	 */
-	if (!need_bitmaps && size > info->max_glyph_memory / 100)
-	    continue;
 	
-	/*
-	 * Make sure there's enough buffer space for the glyph
-	 */
-	if (size > bufSize)
-	{
-	    if (bufBitmap != bufLocal)
-		free (bufBitmap);
-	    bufBitmap = (unsigned char *) malloc (size);
-	    if (!bufBitmap)
-		continue;
-	    bufSize = size;
-	}
-	memset (bufBitmap, 0, size);
-
-	/*
-	 * Rasterize into the local buffer
-	 */
-	switch (glyphslot->format) {
-	case ft_glyph_format_outline:
-	    ftbit.width      = width * hmul;
-	    ftbit.rows       = height * vmul;
-	    ftbit.pitch      = pitch;
-	    if (font->info.antialias)
-		ftbit.pixel_mode = ft_pixel_mode_grays;
-	    else
-		ftbit.pixel_mode = ft_pixel_mode_mono;
-	    
-	    ftbit.buffer     = bufBitmap;
-	    
-	    if (subpixel)
-		FT_Outline_Transform (&glyphslot->outline, &matrix);
+        // compute the size of the final bitmap
+        ftbit  = &glyphslot->bitmap;
 
-	    FT_Outline_Translate ( &glyphslot->outline, -left*hmul, -bottom*vmul );
-
-	    FT_Outline_Get_Bitmap( _XftFTlibrary, &glyphslot->outline, &ftbit );
-	    break;
-	case ft_glyph_format_bitmap:
-	    if (font->info.antialias)
-	    {
-		unsigned char	*srcLine, *dstLine;
-		int		height;
-		int		x;
-		int	    h, v;
-
-		srcLine = glyphslot->bitmap.buffer;
-		dstLine = bufBitmap;
-		height = glyphslot->bitmap.rows;
-		while (height--)
-		{
-		    for (x = 0; x < glyphslot->bitmap.width; x++)
-		    {
-			/* always MSB bitmaps */
-			unsigned char	a = ((srcLine[x >> 3] & (0x80 >> (x & 7))) ?
-					     0xff : 0x00);
-			if (subpixel)
-			{
-			    for (v = 0; v < vmul; v++)
-				for (h = 0; h < hmul; h++)
-				    dstLine[v * pitch + x*hmul + h] = a;
-			}
-			else
-			    dstLine[x] = a;
-		    }
-		    dstLine += pitch * vmul;
-		    srcLine += glyphslot->bitmap.pitch;
-		}
-	    }
-	    else
-	    {
-		unsigned char	*srcLine, *dstLine;
-		int		h, bytes;
-
-		srcLine = glyphslot->bitmap.buffer;
-		dstLine = bufBitmap;
-		h = glyphslot->bitmap.rows;
-		bytes = (glyphslot->bitmap.width + 7) >> 3;
-		while (h--)
-		{
-		    memcpy (dstLine, srcLine, bytes);
-		    dstLine += pitch;
-		    srcLine += glyphslot->bitmap.pitch;
-		}
-	    }
-	    break;
-	default:
-	    if (XftDebug() & XFT_DBG_GLYPH)
-		printf ("glyph %d is not in a usable format\n",
-			(int) glyphindex);
-	    continue;
-	}
+        width  = ftbit->width;
+        height = ftbit->rows;
+        pitch  = (width+3) & ~3;
 	
 	if (XftDebug() & XFT_DBG_GLYPH)
 	{
@@ -423,29 +602,72 @@
 		int		x, y;
 		unsigned char	*line;
 
-		line = bufBitmap;
-		for (y = 0; y < height * vmul; y++)
+                line = ftbit->buffer;
+
+                if (ftbit->pitch < 0)
+                  line -= ftbit->pitch*(height-1);
+
+                for (y = 0; y < height; y++)
 		{
 		    if (font->info.antialias) 
 		    {
-			static char    den[] = { " .:;=+*#" };
-			for (x = 0; x < pitch; x++)
+                        static const char    den[] = { " .:;=+*#" };
+                        for (x = 0; x < width; x++)
 			    printf ("%c", den[line[x] >> 5]);
 		    }
 		    else
 		    {
-			for (x = 0; x < pitch * 8; x++)
+                        for (x = 0; x < width * 8; x++)
 			{
 			    printf ("%c", line[x>>3] & (1 << (x & 7)) ? '#' : ' ');
 			}
 		    }
 		    printf ("|\n");
-		    line += pitch;
+                    line += ftbit->pitch;
 		}
 		printf ("\n");
 	    }
 	}
 
+        size = _compute_xrender_bitmap_size( &local, glyphslot, mode );
+        if ( size < 0 )
+            continue;
+
+        xftg->metrics.width  = local.width;
+	xftg->metrics.height = local.rows;
+	xftg->metrics.x      = - glyphslot->bitmap_left;
+	xftg->metrics.y      =   glyphslot->bitmap_top;
+	    
+	    /*
+	 * If the glyph is relatively large (> 1% of server memory),
+	 * don't send it until necessary
+	     */
+	if (!need_bitmaps && size > info->max_glyph_memory / 100)
+	    continue;
+
+	/*
+	 * Make sure there's enough buffer space for the glyph
+	 */
+	if (size > bufSize)
+	    {
+	    if (bufBitmap != bufLocal)
+		free (bufBitmap);
+	    bufBitmap = (unsigned char *) malloc (size);
+	    if (!bufBitmap)
+		    continue;
+	    bufSize = size;
+	    }
+	memset (bufBitmap, 0, size);
+
+        local.buffer = bufBitmap;
+	    
+        _fill_xrender_bitmap( &local, glyphslot, mode,
+                              (font->info.rgba == FC_RGBA_BGR  ||
+                               font->info.rgba == FC_RGBA_VBGR ) );
+	/*
+	 * Copy or convert into local buffer
+	 */
+
 	/*
 	 * Use the glyph index as the wire encoding; it
 	 * might be more efficient for some locales to map
@@ -455,121 +677,23 @@
 	 */
 	glyph = (Glyph) glyphindex;
 
-	if (subpixel)
-	{
-	    int		    x, y;
-	    unsigned char   *in_line, *out_line, *in;
-	    unsigned int    *out;
-	    unsigned int    red, green, blue;
-	    int		    rf, gf, bf;
-	    int		    s;
-	    int		    o, os;
-	    
-	    /*
-	     * Filter the glyph to soften the color fringes
-	     */
-	    widthrgba = width;
-	    pitchrgba = (widthrgba * 4 + 3) & ~3;
-	    sizergba = pitchrgba * height;
-
-	    os = 1;
-	    switch (font->info.rgba) {
-	    case FC_RGBA_VRGB:
-		os = pitch;
-	    case FC_RGBA_RGB:
-	    default:
-		rf = 0;
-		gf = 1;
-		bf = 2;
-		break;
-	    case FC_RGBA_VBGR:
-		os = pitch;
-	    case FC_RGBA_BGR:
-		bf = 0;
-		gf = 1;
-		rf = 2;
-		break;
-	    }
-	    if (sizergba > bufSizeRgba)
-	    {
-		if (bufBitmapRgba != bufLocalRgba)
-		    free (bufBitmapRgba);
-		bufBitmapRgba = (unsigned char *) malloc (sizergba);
-		if (!bufBitmapRgba)
-		    continue;
-		bufSizeRgba = sizergba;
-	    }
-	    memset (bufBitmapRgba, 0, sizergba);
-	    in_line = bufBitmap;
-	    out_line = bufBitmapRgba;
-	    for (y = 0; y < height; y++)
-	    {
-		in = in_line;
-		out = (unsigned int *) out_line;
-		in_line += pitch * vmul;
-		out_line += pitchrgba;
-		for (x = 0; x < width * hmul; x += hmul)
-		{
-		    red = green = blue = 0;
-		    o = 0;
-		    for (s = 0; s < 3; s++)
-		    {
-			red += filters[rf][s]*in[x+o];
-			green += filters[gf][s]*in[x+o];
-			blue += filters[bf][s]*in[x+o];
-			o += os;
-		    }
-		    red = red / 65536;
-		    green = green / 65536;
-		    blue = blue / 65536;
-		    *out++ = (green << 24) | (red << 16) | (green << 8) | blue;
-		}
-	    }
-	    
-	    xftg->glyph_memory = sizergba + sizeof (XftGlyph);
-	    if (font->format)
+	xftg->glyph_memory = size + sizeof (XftGlyph);
+ 	    if (font->format)
 	    {
 		if (!font->glyphset)
 		    font->glyphset = XRenderCreateGlyphSet (dpy, font->format);
-		if (ImageByteOrder (dpy) != XftNativeByteOrder ())
-		    XftSwapCARD32 ((CARD32 *) bufBitmapRgba, sizergba >> 2);
-		XRenderAddGlyphs (dpy, font->glyphset, &glyph,
-				  &xftg->metrics, 1, 
-				  (char *) bufBitmapRgba, sizergba);
-	    }
-	    else
-	    {
-		if (sizergba)
-		{
-		    xftg->bitmap = malloc (sizergba);
-		    if (xftg->bitmap)
-			memcpy (xftg->bitmap, bufBitmapRgba, sizergba);
-		}
-		else
-		    xftg->bitmap = NULL;
-	    }
-	}
-	else
-	{
-	    xftg->glyph_memory = size + sizeof (XftGlyph);
-	    if (font->format)
-	    {
-		/*
-		 * swap bit order around; FreeType is always MSBFirst
-		 */
-		if (!font->info.antialias)
+	    if ( mode == FT_RENDER_MODE_MONO )
 		{
+		  /* swap bits in each byte */
 		    if (BitmapBitOrder (dpy) != MSBFirst)
 		    {
-			unsigned char   *line;
-			unsigned char   c;
-			int		    i;
+		      unsigned char   *line = (unsigned char*)bufBitmap;
+		      int             i = size;
 
-			line = (unsigned char *) bufBitmap;
-			i = size;
 			while (i--)
 			{
-			    c = *line;
+			  int  c = *line;
+
 			    c = ((c << 1) & 0xaa) | ((c >> 1) & 0x55);
 			    c = ((c << 2) & 0xcc) | ((c >> 2) & 0x33);
 			    c = ((c << 4) & 0xf0) | ((c >> 4) & 0x0f);
@@ -577,8 +701,12 @@
 			}
 		    }
 		}
-		if (!font->glyphset)
-		    font->glyphset = XRenderCreateGlyphSet (dpy, font->format);
+            else if ( mode != FT_RENDER_MODE_NORMAL )
+            {
+                /* invert ARGB <=> BGRA */
+                if (ImageByteOrder (dpy) != XftNativeByteOrder ())
+                    XftSwapCARD32 ((CARD32 *) bufBitmap, size >> 2);
+            }
 		XRenderAddGlyphs (dpy, font->glyphset, &glyph,
 				  &xftg->metrics, 1, 
 				  (char *) bufBitmap, size);
@@ -594,7 +722,7 @@
 		else
 		    xftg->bitmap = NULL;
 	    }
-	}
+
 	font->glyph_memory += xftg->glyph_memory;
 	info->glyph_memory += xftg->glyph_memory;
 	if (XftDebug() & XFT_DBG_CACHE)
@@ -605,8 +733,6 @@
     }
     if (bufBitmap != bufLocal)
 	free (bufBitmap);
-    if (bufBitmapRgba != bufLocalRgba)
-	free (bufBitmapRgba);
     XftUnlockFace (&font->public);
 }
 
