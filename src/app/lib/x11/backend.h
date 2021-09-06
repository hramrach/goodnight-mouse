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

#ifndef D581DDBD_8977_4761_88A1_CC3A73C94115
#define D581DDBD_8977_4761_88A1_CC3A73C94115

#if USE_X11

#include <glib.h>
#include <X11/Xlib.h>

typedef struct BackendX11
{
    Display *display;
    GSource *source;
} BackendX11;

BackendX11 *backend_x11_new();
void backend_x11_destroy(BackendX11 *backend);
// todo: allow to subscribe to event types
// keyboard will subscribe to focus events directly, getting the window id

Display *backend_x11_get_display(BackendX11 *backend);

#endif /* USE_X11 */

#endif /* D581DDBD_8977_4761_88A1_CC3A73C94115 */
