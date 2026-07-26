#include "../menu_navigation_logic.h"

#include <cassert>
#include <cstddef>

int main() {
  using namespace MenuNavigation;

  static_assert(SCREEN_WIDTH == 240);
  static_assert(SCREEN_HEIGHT == 320);
  static_assert(STATUS_HEIGHT == 20);
  static_assert(HEADER_TOP == 20);
  static_assert(ROWS_PER_PAGE == 5);
  static_assert(ROW_HEIGHT >= 44);
  static_assert(HEADER_HEIGHT >= 40);
  static_assert(ROW_TOP == 60);
  static_assert(FOOTER_TOP == 280);

  assert(pageCount(0) == 1);
  assert(pageCount(5) == 1);
  assert(pageCount(6) == 2);
  assert(pageCount(10) == 2);
  assert(clampPage(4, 6) == 1);

  MenuEvent event = hitTestMenu(200, 40, 10, 0);
  assert(event.action == MenuAction::Back);

  event = hitTestMenu(20, 62, 10, 0);
  assert(event.action == MenuAction::Item && event.index == 0);
  event = hitTestMenu(20, 279, 10, 0);
  assert(event.action == MenuAction::Item && event.index == 4);
  event = hitTestMenu(20, 62, 10, 1);
  assert(event.action == MenuAction::Item && event.index == 5);

  event = hitTestMenu(20, 300, 10, 0);
  assert(event.action == MenuAction::None);
  event = hitTestMenu(220, 300, 10, 0);
  assert(event.action == MenuAction::NextPage);
  event = hitTestMenu(20, 300, 10, 1);
  assert(event.action == MenuAction::PreviousPage);
  event = hitTestMenu(220, 300, 10, 1);
  assert(event.action == MenuAction::None);
  event = hitTestMenu(120, 300, 10, 0);
  assert(event.action == MenuAction::None);

  // Exact shared-region boundaries.
  assert(hitTestMenu(200, 19, 10, 0).action == MenuAction::None);
  assert(hitTestMenu(200, 20, 10, 0).action == MenuAction::Back);
  assert(hitTestMenu(200, 59, 10, 0).action == MenuAction::Back);
  assert((hitTestMenu(20, 60, 10, 0) == MenuEvent{MenuAction::Item, 0}));
  assert((hitTestMenu(20, 279, 10, 0) == MenuEvent{MenuAction::Item, 4}));
  assert(hitTestMenu(79, 300, 10, 1).action == MenuAction::PreviousPage);
  assert(hitTestMenu(80, 300, 10, 1).action == MenuAction::None);
  assert(hitTestMenu(159, 300, 10, 0).action == MenuAction::None);
  assert(hitTestMenu(160, 300, 10, 0).action == MenuAction::NextPage);

  // Empty rows on a partial final page and invalid coordinates are inert.
  assert((hitTestMenu(20, 60, 6, 1) == MenuEvent{MenuAction::Item, 5}));
  assert(hitTestMenu(20, 104, 6, 1).action == MenuAction::None);
  assert(hitTestMenu(-1, 60, 10, 0).action == MenuAction::None);
  assert(hitTestMenu(240, 60, 10, 0).action == MenuAction::None);

  ReleaseTracker tracker;
  const MenuEvent first = hitTestMenu(20, 62, 10, 0);
  assert(tracker.press(first).action == MenuAction::None);
  assert(tracker.press(first).action == MenuAction::None);
  assert(tracker.release(first) == first);
  assert(tracker.release(first).action == MenuAction::None);

  tracker.press(first);
  const MenuEvent different = hitTestMenu(20, 110, 10, 0);
  assert(tracker.release(different).action == MenuAction::None);

  tracker.press(first);
  tracker.update(MenuEvent{});
  assert(tracker.release(first).action == MenuAction::None);

  tracker.press(first);
  tracker.cancel();
  assert(tracker.release(first).action == MenuAction::None);

  return 0;
}
