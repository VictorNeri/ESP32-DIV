#ifndef MENU_NAVIGATION_UI_H
#define MENU_NAVIGATION_UI_H

#include "menu_navigation_logic.h"

#include <cstddef>
#include <cstdint>

namespace UnifiedMenu {

struct Item {
  const char* label;
  const unsigned char* icon;
  bool enabled;
};

void drawHeader(const char* title, bool showBack);
void drawMenu(const char* title, const Item* items, size_t itemCount, size_t page,
              bool showBack, size_t selectedIndex = SIZE_MAX);

}  // namespace UnifiedMenu

#endif  // MENU_NAVIGATION_UI_H
