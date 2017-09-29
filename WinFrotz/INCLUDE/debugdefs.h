#ifdef	_DEBUG

#define	TEXTOUT( hDC, X, Y, str, len ) {\
	if( !TextOut( hDC, X, Y,  str, len ) )\
		MessageBox( NULL, "Text output failed", "WinFrotz Debug", MB_ICONWARNING );\
}

#define RELEASEDC( hWnd, hDC ) {\
	if( !hWnd || !hDC )\
		MessageBox( NULL, "Attempt to ReleaseDC() with hWnd or hDC==NULL!", "WinFrotz Debug", MB_ICONWARNING );\
	else if( !ReleaseDC( hWnd, hDC ) )\
		MessageBox( NULL, "Could not release DC!", "WinFrotz Debug", MB_ICONWARNING);\
}

#define	ASSERT_NOT_NULL( val ) {\
	if( val == NULL )\
		MessageBox( NULL, "Assertion failed, value is non-null", "Winfrotz ASSERT", MB_ICONWARNING );\
}

#define BITBLT( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) {\
	if( !BitBlt( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) )\
		MessageBox( NULL, "BitBlt failed!", "WinFrotz Debug", MB_ICONWARNING ); \
}

#define PATBLT( destDC, destX, destY, width, height, flags) {\
	if( !PatBlt( destDC, destX, destY, width, height, flags))\
		MessageBox( NULL, "PatBlt failed!", "WinFrotz Debug", MB_ICONWARNING );\
}
#else

#define TEXTOUT( hDC, X, Y, str, len ) {TextOut( hDC, X, Y, str, len ); }

#define RELEASEDC( hWnd, hDC ) { ReleaseDC( hWnd, hDC ); }

#define BITBLT( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ) {\
	    BitBlt( destDC, destX, destY, width, height, srcDC, srcX, srcY , Flags ); }

#define PATBLT( destDC, destX, destY, width, height, flags) {\
		PatBlt( destDC, destX, destY, width, height, flags); }

#define ASSERT_NOT_NULL( val ) {}

#endif