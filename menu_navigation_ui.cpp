#include "menu_navigation_ui.h"

#include "shared.h"
#include "utils.h"

#include <TFT_eSPI.h>
#include <algorithm>

extern TFT_eSPI tft;

namespace UnifiedMenu {
namespace {

void drawFooter(size_t page, size_t itemCount) {
  using namespace MenuNavigation;
  const size_t pages = pageCount(itemCount);
  page = clampPage(page, itemCount);
  tft.fillRect(0, FOOTER_TOP, SCREEN_WIDTH, SCREEN_HEIGHT - FOOTER_TOP, TFT_BLACK);
  tft.drawFastHLine(0, FOOTER_TOP, SCREEN_WIDTH, UI_GUNMETAL);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(page > 0 ? UI_CYAN : UI_GUNMETAL, TFT_BLACK);
  tft.setCursor(15, FOOTER_TOP + 12);
  tft.print("PREV");
  tft.setTextColor(UI_GUNMETAL, TFT_BLACK);
  tft.setCursor(101, FOOTER_TOP + 12);
  tft.printf("%u/%u", static_cast<unsigned>(page + 1), static_cast<unsigned>(pages));
  tft.setTextColor(page + 1 < pages ? UI_CYAN : UI_GUNMETAL, TFT_BLACK);
  tft.setCursor(190, FOOTER_TOP + 12);
  tft.print("NEXT");
}

}  // namespace

void drawHeader(const char* title, bool showBack) {
  using namespace MenuNavigation;
  tft.fillRect(0, HEADER_TOP, SCREEN_WIDTH, HEADER_HEIGHT, TFT_BLACK);
  tft.drawFastHLine(0, HEADER_TOP, SCREEN_WIDTH, UI_CYAN);
  tft.drawFastHLine(0, ROW_TOP - 1, SCREEN_WIDTH, UI_AMBER);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(UI_CYAN, TFT_BLACK);
  tft.setCursor(6, HEADER_TOP + 12);
  tft.printf("%.19s", title != nullptr ? title : "Menu");
  if (showBack) {
    tft.drawRoundRect(BACK_LEFT + 2, HEADER_TOP + 3,
                      SCREEN_WIDTH - BACK_LEFT - 4, HEADER_HEIGHT - 7, 4, UI_CYAN);
    tft.setTextColor(UI_AMBER, TFT_BLACK);
    tft.setCursor(BACK_LEFT + 8, HEADER_TOP + 12);
    tft.print("< BACK");
  }
}

void drawMenu(const char* title, const Item* items, size_t itemCount, size_t page,
              bool showBack, size_t selectedIndex) {
  using namespace MenuNavigation;
  page = clampPage(page, itemCount);
  tft.fillRect(0, STATUS_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - STATUS_HEIGHT, TFT_BLACK);
  drawHeader(title, showBack);

  const size_t start = pageStart(page, itemCount);
  const size_t end = std::min(itemCount, start + ROWS_PER_PAGE);
  tft.setTextFont(2);
  tft.setTextSize(1);
  for (size_t index = start; index < end; ++index) {
    const int16_t row = static_cast<int16_t>(index - start);
    const int16_t y = ROW_TOP + row * ROW_HEIGHT;
    const bool selected = index == selectedIndex;
    const bool enabled = items[index].enabled;
    const uint16_t color = !enabled ? UI_GUNMETAL : selected ? UI_AMBER : UI_CYAN;
    if (selected && enabled) tft.fillRoundRect(4, y + 2, 232, ROW_HEIGHT - 4, 4, DARK_GRAY);
    tft.drawRoundRect(4, y + 2, 232, ROW_HEIGHT - 4, 4, color);
    int16_t textX = 12;
    if (items[index].icon != nullptr) {
      tft.drawBitmap(12, y + 14, items[index].icon, 16, 16, color);
      textX = 36;
    }
    tft.setTextColor(color, selected ? DARK_GRAY : TFT_BLACK);
    tft.setCursor(textX, y + 14);
    tft.printf("%.27s", items[index].label != nullptr ? items[index].label : "");
  }
  drawFooter(page, itemCount);
  drawStatusBar(readBatteryVoltage(), true);
}

}  // namespace UnifiedMenu
