#pragma once
#include <gdiplus.h>
#include <string>

#pragma comment(lib, "gdiplus.lib")

class Draw {
public:
    explicit Draw(Gdiplus::Graphics* g) : g(g) {
        g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    }

    void Line(int x1, int y1, int x2, int y2, COLORREF color, int thickness = 1) {
        Gdiplus::Pen pen(ColorRefToColor(color), static_cast<Gdiplus::REAL>(thickness));
        g->DrawLine(&pen, x1, y1, x2, y2);
    }

    void Rect(int x, int y, int w, int h, COLORREF color, int thickness = 1) {
        Gdiplus::Pen pen(ColorRefToColor(color), static_cast<Gdiplus::REAL>(thickness));
        g->DrawRectangle(&pen, static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y),
            static_cast<Gdiplus::REAL>(w), static_cast<Gdiplus::REAL>(h));
    }

    void FillRect(int x, int y, int w, int h, COLORREF color) {
        Gdiplus::SolidBrush brush(ColorRefToColor(color));
        g->FillRectangle(&brush, static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y),
            static_cast<Gdiplus::REAL>(w), static_cast<Gdiplus::REAL>(h));
    }

    void Circle(int cx, int cy, int r, COLORREF color, int thickness = 1) {
        Gdiplus::Pen pen(ColorRefToColor(color), static_cast<Gdiplus::REAL>(thickness));
        g->DrawEllipse(&pen, cx - r, cy - r, r * 2, r * 2);
    }

    void FillCircle(int cx, int cy, int r, COLORREF color) {
        Gdiplus::SolidBrush brush(ColorRefToColor(color));
        g->FillEllipse(&brush, cx - r, cy - r, r * 2, r * 2);
    }

    void Text(int x, int y, const std::wstring& text, COLORREF color, int fontSize = 16, const wchar_t* fontName = L"Segoe UI") {
        Gdiplus::FontFamily fontFamily(fontName);
        Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush brush(ColorRefToColor(color));
        Gdiplus::PointF point(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y));
        g->DrawString(text.c_str(), static_cast<INT>(text.length()), &font, point, &brush);
    }

    int CalcTextWidth(const std::wstring& text, int fontSize = 16, const wchar_t* fontName = L"Segoe UI") {
        Gdiplus::FontFamily fontFamily(fontName);
        Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::RectF layoutRect(0, 0, 1000, 100);
        Gdiplus::RectF boundingBox;
        Gdiplus::SolidBrush brush(Gdiplus::Color::White);
        g->MeasureString(text.c_str(), static_cast<INT>(text.length()), &font, layoutRect, &boundingBox);
        return static_cast<int>(boundingBox.Width);
    }

private:
    Gdiplus::Graphics* g;

    Gdiplus::Color ColorRefToColor(COLORREF color) {
        return Gdiplus::Color(255, GetRValue(color), GetGValue(color), GetBValue(color));
    }
};
