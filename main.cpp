#include "overlay.h"
#include "draw.h"

int main() {
  using namespace Overlay;
  if (!InitOverlay(L"Overlay", L"OverlayClass"))
    return -1;

  while (PumpMessages()) {
    Gdiplus::Graphics* g = BeginRender();
    Draw draw(g);

    draw.Rect(10, 10, 100, 100, RGB(255, 0, 0), 1);

    EndRender(g);
  }

  CloseOverlay();
  return 0;
}
