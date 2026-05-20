# am-ui TODO

## Mouse event routing — switch to subscriber model

Today every mouse move event is broadcast through the whole view
tree: `ViewGroup.onMouseEvent`, `Panel.onMouseEvent`, and
`ScrollView.onMouseEvent` each forward move events to their
children even when the cursor is outside their own bounds, so
descendants can clear hover / drag state. That works but it costs
O(view-tree-size) per cursor pixel, and every container has to
remember to be a polite forwarder. We've already hit two bugs that
came from a container forgetting (`[...]` glyph stayed lit when the
cursor moved sidebar → editor; scrollbar thumb stayed accent-blue
on the same path).

**Replace with a Window-level subscriber list:**

- `Window.subscribeMouseEvent(view: View)` / `unsubscribeMouseEvent(view: View)`.
- A view subscribes when it needs *all* mouse events regardless of
  position — that's anything tracking hover or holding a drag latch
  (hover-enabled Labels, ScrollView thumbs, CliView thumb,
  TextEditor thumb, Buttons with `hoverBorderEnabled` or
  `pressTranslateY`).
- The Window's `onMouseEvent` dispatch becomes:
  - **down / up / wheel** — walk the tree, first consumer wins. Same
    as today (z-ordering + "first claim absorbs" semantics matter
    for clicks).
  - **move** — fan out to subscribers with window-absolute coords;
    the tree walk stops at views whose `contains(x, y)` is false.
- Containers (ViewGroup, Panel, ScrollView) drop their move-event
  forwarding exemptions. Move is *only* forwarded into a child when
  the cursor is inside the child's bounds; otherwise it's not the
  container's job.

**Mechanics to design:**

- `View.absoluteX() / absoluteY()` so subscribers can convert the
  Window-absolute coords they receive into local hit-tests. Walks
  parents — needs a `parent` back-pointer on View, set in
  `addChild` and cleared in `removeChild` / `removeChildren`.
- Subscribe / unsubscribe lifecycle on `onAttach` / `onDetach`. A
  view that needs the events only when a state flag is on (e.g.
  `Label.hoverEnabled`) should toggle subscription on flag change.
- `visible == false` views shouldn't fire — gate inside the
  dispatcher.
- Iteration safety — snapshot the subscriber list at the top of
  dispatch so callbacks that re-enter the subscribe API don't
  corrupt the iteration.

**Why we're parking it:**

The above is a few-hundred-line rework across Window, View, every
container, and every hover/drag consumer. Doing it before the
popup-menu / LayeredPane feature means popups ride on the new model
from day one — but the current broadcast model also works for
popups (each open popup just subscribes to outside-clicks). Decision
deferred until the popup machinery is in place and we know the
exact shape we need.
