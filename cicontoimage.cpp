#include "cicontoimage.h"

CIconToImage::CIconToImage()
{


    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    GetEncoderClsid(L"image/png", &g_pngClsid);



    GdiplusShutdown(gdiplusToken);

}
CIconToImage::~CIconToImage()
{

}


int CIconToImage::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if (size == 0)
            return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for (UINT j = 0; j < num; ++j)
        {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
}

//void CIconToImage::saveFileIconAsBMP(HICON hIcon, const wstring &BMPFile)
//{
//        HBITMAP hBitmap3;
//        HDC hDC;
//        int iBits;
//        WORD wBitCount;
//        DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
//        BITMAP Bitmap0;
//        BITMAPFILEHEADER bmfHdr;
//        BITMAPINFOHEADER bi;
//        LPBITMAPINFOHEADER lpbi;
//        HANDLE fh, hDib, hPal, hOldPal2 = NULL;
//        hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
//        iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
//        DeleteDC(hDC);
//        if (iBits <= 1)
//            wBitCount = 1;
//        else if (iBits <= 4)
//            wBitCount = 4;
//        else if (iBits <= 8)
//            wBitCount = 8;
//        else
//            wBitCount = 24;
//        GetObject(hBitmap3, sizeof(Bitmap0), (LPSTR)&Bitmap0);
//        bi.biSize = sizeof(BITMAPINFOHEADER);
//        bi.biWidth = Bitmap0.bmWidth;
//        bi.biHeight = -Bitmap0.bmHeight;
//        bi.biPlanes = 1;
//        bi.biBitCount = wBitCount;
//        bi.biCompression = BI_RGB;
//        bi.biSizeImage = 0;
//        bi.biXPelsPerMeter = 0;
//        bi.biYPelsPerMeter = 0;
//        bi.biClrImportant = 0;
//        bi.biClrUsed = 256;
//        dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
//            * Bitmap0.bmHeight;
//        hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
//        lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
//        *lpbi = bi;

//        hPal = GetStockObject(DEFAULT_PALETTE);
//        if (hPal)
//        {
//            hDC = GetDC(NULL);
//            hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
//            RealizePalette(hDC);
//        }


//        GetDIBits(hDC, hBitmap3, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
//            + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

//        if (hOldPal2)
//        {
//            SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
//            RealizePalette(hDC);
//            ReleaseDC(NULL, hDC);
//        }

//        fh = CreateFile(BMPFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
//            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

//        if (fh == INVALID_HANDLE_VALUE)
//        {
//            cerr << "[!]Cannot Create file!" << endl;
//            //return FALSE;
//        }
//        bmfHdr.bfType = 0x4D42; // "BM"
//        dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
//        bmfHdr.bfSize = dwDIBSize;
//        bmfHdr.bfReserved1 = 0;
//        bmfHdr.bfReserved2 = 0;
//        bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

//        WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

//        WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
//        GlobalUnlock(hDib);
//        GlobalFree(hDib);
//        CloseHandle(fh);

//        cout << "[+]Bitmap file Saved!" << endl;

//}

BITMAP_AND_BYTES CIconToImage::createAlphaChannelBitmapFromIcon(HICON hIcon)
{
    // Get the icon info
        ICONINFO iconInfo = { 0 };
        GetIconInfo(hIcon, &iconInfo);

        // Get the screen DC
        HDC dc = GetDC(NULL);

        // Get icon size info
        BITMAP bm = { 0 };
        GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

        // Set up BITMAPINFO
        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = bm.bmWidth;
        bmi.bmiHeader.biHeight = -bm.bmHeight;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        // Extract the color bitmap
        int nBits = bm.bmWidth * bm.bmHeight;
        int32_t* colorBits = new int32_t[nBits];
        GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

        // Check whether the color bitmap has an alpha channel.
        // (On my Windows 7, all file icons I tried have an alpha channel.)
        BOOL hasAlpha = FALSE;
        for (int i = 0; i < nBits; i++) {
            if ((colorBits[i] & 0xff000000) != 0) {
                hasAlpha = TRUE;
                break;
            }
        }

        // If no alpha values available, apply the mask bitmap
        if (!hasAlpha) {
            // Extract the mask bitmap
            int32_t* maskBits = new int32_t[nBits];
            GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
            // Copy the mask alphas into the color bits
            for (int i = 0; i < nBits; i++) {
                if (maskBits[i] == 0) {
                    colorBits[i] |= 0xff000000;
                }
            }
            delete[] maskBits;
        }

        // Release DC and GDI bitmaps
        ReleaseDC(NULL, dc);
        ::DeleteObject(iconInfo.hbmColor);
        ::DeleteObject(iconInfo.hbmMask);

        // Create GDI+ Bitmap
        Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, (BYTE*)colorBits);
        BITMAP_AND_BYTES ret = { bmp, colorBits };

        return ret;
}

void CIconToImage::saveFileIconAsPng(HICON hIcon, const wstring &pngFile)
{
    BITMAP_AND_BYTES bbs = createAlphaChannelBitmapFromIcon(hIcon);

        IStream* fstrm = NULL;
        SHCreateStreamOnFile(pngFile.c_str(), STGM_WRITE | STGM_CREATE, &fstrm);
        bbs.bmp->Save(fstrm, &g_pngClsid, NULL);
        fstrm->Release();

        delete bbs.bmp;
        delete[] bbs.bytes;
        DestroyIcon(hIcon);
}
