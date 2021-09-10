/**
 * Copyright (C) 2021 ryan
 *
 * This file is part of Goodnight Mouse.
 *
 * Goodnight Mouse is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Goodnight Mouse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Goodnight Mouse.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef D4B246E4_2214_427B_B7DF_D273ED9A7D6B
#define D4B246E4_2214_427B_B7DF_D273ED9A7D6B

#if USE_X11

#include "x11.h"

#include <X11/extensions/XInput2.h>
//#include <X11/XKBlib.h>

// backend for keyboard modifiers using x11
typedef struct BackendX11Modifiers
{
    BackendX11 *backend;

    Display *display;
    Window root_window;

    int device_id;

    //XkbDescPtr virt_mods_map;
} BackendX11Modifiers;

BackendX11Modifiers *backend_x11_modifiers_new(BackendX11 *backend);
void backend_x11_modifiers_destroy(BackendX11Modifiers *modifiers);
guint backend_x11_modifiers_get(BackendX11Modifiers *modifiers);
guint backend_x11_modifiers_map(BackendX11Modifiers *modifiers, guint mods);

#endif /* USE_X11 */

#endif /* D4B246E4_2214_427B_B7DF_D273ED9A7D6B */
