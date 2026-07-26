#ifndef MENU_NAVIGATION_LOGIC_H
#define MENU_NAVIGATION_LOGIC_H

#include <cstddef>
#include <cstdint>

namespace MenuNavigation {

constexpr int16_t SCREEN_WIDTH = 240;
constexpr int16_t SCREEN_HEIGHT = 320;
constexpr int16_t STATUS_HEIGHT = 20;
constexpr int16_t HEADER_TOP = STATUS_HEIGHT;
constexpr int16_t HEADER_HEIGHT = 40;
constexpr int16_t BACK_LEFT = 176;
constexpr int16_t ROW_TOP = HEADER_TOP + HEADER_HEIGHT;
constexpr int16_t ROW_HEIGHT = 44;
constexpr size_t ROWS_PER_PAGE = 5;
constexpr int16_t FOOTER_TOP = ROW_TOP + ROW_HEIGHT * ROWS_PER_PAGE;
constexpr int16_t FOOTER_THIRD = SCREEN_WIDTH / 3;

enum class MenuAction : uint8_t {
  None,
  Back,
  Item,
  PreviousPage,
  NextPage,
};

struct MenuEvent {
  MenuAction action{MenuAction::None};
  size_t index{0};

  constexpr bool operator==(const MenuEvent& other) const {
    return action == other.action && index == other.index;
  }
  constexpr bool operator!=(const MenuEvent& other) const { return !(*this == other); }
};

constexpr size_t pageCount(size_t itemCount) {
  return itemCount == 0 ? 1 : (itemCount + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
}

constexpr size_t clampPage(size_t page, size_t itemCount) {
  const size_t pages = pageCount(itemCount);
  return page < pages ? page : pages - 1;
}

constexpr size_t pageStart(size_t page, size_t itemCount) {
  return clampPage(page, itemCount) * ROWS_PER_PAGE;
}

MenuEvent hitTestMenu(int16_t x, int16_t y, size_t itemCount, size_t page);

class ReleaseTracker {
 public:
  MenuEvent press(const MenuEvent& event);
  void update(const MenuEvent& event);
  MenuEvent release(const MenuEvent& event);
  void cancel();

 private:
  bool pressed_{false};
  MenuEvent pressedEvent_{};
};

}  // namespace MenuNavigation

#endif  // MENU_NAVIGATION_LOGIC_H
