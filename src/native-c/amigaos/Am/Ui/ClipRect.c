// ClipRect was a class with native lifecycle hooks (init/release/mark) that
// allocated an AmigaOS Region from its fields. It is now a struct (value type)
// — no lifecycle hooks are emitted, so this file is intentionally empty.
//
// The AmigaOS Region is now built ad-hoc inside LayerGraphics' / ViewContextGraphics'
// apply_clip_rect / setClipRect helpers, which read the struct fields directly.
