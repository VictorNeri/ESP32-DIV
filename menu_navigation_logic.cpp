#include "menu_navigation_logic.h"

namespace MenuNavigation {

MenuEvent hitTestMenu(int16_t x, int16_t y, size_t itemCount, size_t page) {
  if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return {};

  page = clampPage(page, itemCount);
  if (y < STATUS_HEIGHT) return {};
  if (y < ROW_TOP) {
    return x >= BACK_LEFT ? MenuEvent{MenuAction::Back, 0} : MenuEvent{};
  }

  if (y < FOOTER_TOP) {
    const size_t row = static_cast<size_t>((y - ROW_TOP) / ROW_HEIGHT);
    const size_t index = page * ROWS_PER_PAGE + row;
    return index < itemCount ? MenuEvent{MenuAction::Item, index} : MenuEvent{};
  }

  if (x < FOOTER_THIRD && page > 0) {
    return {MenuAction::PreviousPage, page - 1};
  }
  if (x >= FOOTER_THIRD * 2 && page + 1 < pageCount(itemCount)) {
    return {MenuAction::NextPage, page + 1};
  }
  return {};
}

MenuEvent ReleaseTracker::press(const MenuEvent& event) {
  if (!pressed_ && event.action != MenuAction::None) {
    pressed_ = true;
    pressedEvent_ = event;
  }
  return {};
}

void ReleaseTracker::update(const MenuEvent& event) {
  if (pressed_ && event != pressedEvent_) cancel();
}

MenuEvent ReleaseTracker::release(const MenuEvent& event) {
  if (!pressed_) return {};
  const MenuEvent original = pressedEvent_;
  pressed_ = false;
  pressedEvent_ = {};
  return event == original ? original : MenuEvent{};
}

void ReleaseTracker::cancel() {
  pressed_ = false;
  pressedEvent_ = {};
}

}  // namespace MenuNavigation
