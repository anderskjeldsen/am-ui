#pragma once
#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
// ViewContextGraphics is a peer of LayerGraphics — it forwards every
// drawing primitive to a borrowed LayerGraphics. Implementation in
// ViewContextGraphics.c just routes through.
