/**
 * Copyright (C) 2021 Ryan Britton
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

#include "execution.h"

static gboolean press_using_atspi_action(Control *control);
static gboolean press_using_atspi_keyboard(Control *control);
static gboolean press_using_atspi_mouse(Control *control);

static gboolean press_using_gtk_keyboard(Control *control);
static gboolean press_using_gtk_mouse(Control *control);

static gboolean focus_using_atspi(Control *control);

void control_execution_press(Control *control)
{
    g_debug("foreground: control_execution_press: Attempting");

    if (press_using_atspi_action(control))
        return;
    if (press_using_atspi_keyboard(control))
        return;
    if (press_using_atspi_mouse(control))
        return;

    g_warning("foreground: control_execution_press: Failed");
}

void control_execution_focus(Control *control)
{
    g_debug("foreground: control_execution_focus: Attempting");

    if (press_using_atspi_mouse(control))
        return;

    g_warning("foreground: control_execution_focus: Failed");
}

static gboolean press_using_atspi_action(Control *control)
{
    g_debug("foreground: press_using_atspi_action: Attempting");

    // get action
    AtspiAction *action = atspi_accessible_get_action_iface(control->accessible);
    if (!action)
        return FALSE;

    // make sure there is an action
    gint num_actions = atspi_action_get_n_actions(action, NULL);
    if (num_actions < 1)
        return FALSE;

    // do the first action
    atspi_action_do_action(action, 0, NULL);
    g_object_unref(action);
    return TRUE;
}

static gboolean press_using_atspi_keyboard(Control *control)
{
    g_debug("foreground: press_using_atspi_keyboard: Attempting");

    // grab focus
    if (!focus_using_atspi(control))
        return FALSE;

    // send return key
    gboolean success = atspi_generate_keyboard_event(GDK_KEY_Return, NULL, ATSPI_KEY_SYM, NULL);
    return success;
}

static gboolean press_using_atspi_mouse(Control *control)
{
    g_debug("foreground: press_using_atspi_mouse: Attempting");

    // record the current mouse position
    GdkSeat *seat = gdk_display_get_default_seat(gdk_display_get_default());
    GdkDevice *mouse = gdk_seat_get_pointer(seat);
    GdkWindow *window = gdk_display_get_default_group(gdk_display_get_default());
    gint original_x, original_y;
    gdk_window_get_device_position(window, mouse, &original_x, &original_y, NULL);

    // get position of the center of the accessible
    AtspiComponent *component = atspi_accessible_get_component_iface(control->accessible);
    if (!component)
        return FALSE;
    AtspiRect *bounds = atspi_component_get_extents(component, ATSPI_COORD_TYPE_SCREEN, NULL);
    gint accessible_x = bounds->x + bounds->width / 2;
    gint accessible_y = bounds->y + bounds->height / 2;
    g_object_unref(component);
    g_free(bounds);

    // move and click the mouse
    if (!atspi_generate_mouse_event(accessible_x, accessible_y, "b1c", NULL))
        return FALSE;

    // move the mouse back to original position
    if (!atspi_generate_mouse_event(original_x, original_y, "abs", NULL))
        return FALSE;

    // success
    return TRUE;
}

static gboolean press_using_gtk_keyboard(Control *control)
{
    g_debug("foreground: press_using_gtk_keyboard: Attempting");

    g_warning("foreground: press_using_gtk_keyboard: Not implemented");

    // todo: try to implement with https://developer.gnome.org/gdk3/stable/GdkDevice.html

    return FALSE;
}

static gboolean press_using_gtk_mouse(Control *control)
{
    g_debug("foreground: press_using_gtk_mouse: Attempting");

    g_warning("foreground: press_using_gtk_mouse: Not implemented");

    // todo: try to implement with https://developer.gnome.org/gdk3/stable/GdkDevice.html

    return FALSE;
}

static gboolean focus_using_atspi(Control *control)
{
    g_debug("foreground: focus_using_atspi: Attempting");

    // get component
    AtspiComponent *component = atspi_accessible_get_component_iface(control->accessible);
    if (!component)
        return FALSE;

    // grab focus
    gboolean success = atspi_component_grab_focus(component, NULL);
    g_object_unref(component);
    return success;
}