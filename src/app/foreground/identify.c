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

#include "identify.h"

// atspi role to control type mappings
static const ControlType role_to_type[ATSPI_ROLE_COUNT] = {
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_INVALID
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ACCELERATOR_LABEL
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ALERT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ANIMATION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ARROW
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CALENDAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CANVAS
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_CHECK_BOX
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_CHECK_MENU_ITEM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_COLOR_CHOOSER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_COLUMN_HEADER
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_COMBO_BOX
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_DATE_EDITOR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DESKTOP_ICON
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DESKTOP_FRAME
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_DIAL
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DIALOG
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DIRECTORY_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DRAWING_AREA
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FILE_CHOOSER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FILLER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FOCUS_TRAVERSABLE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FONT_CHOOSER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FRAME
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_GLASS_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_HTML_CONTAINER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ICON
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_IMAGE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_INTERNAL_FRAME
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LABEL
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LAYERED_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LIST
    CONTROL_TYPE_ONLY_ACTION, // ATSPI_ROLE_LIST_ITEM
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_MENU
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MENU_BAR
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_MENU_ITEM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_OPTION_PANE
    CONTROL_TYPE_PAGE_TAB,    // ATSPI_ROLE_PAGE_TAB
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_PAGE_TAB_LIST
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_PANEL
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_PASSWORD_TEXT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_POPUP_MENU
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_PROGRESS_BAR
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_PUSH_BUTTON
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_RADIO_BUTTON
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_RADIO_MENU_ITEM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ROOT_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ROW_HEADER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SCROLL_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SCROLL_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SEPARATOR
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_SLIDER
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_SPIN_BUTTON
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SPLIT_PANE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_STATUS_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TABLE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TABLE_CELL
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TABLE_COLUMN_HEADER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TABLE_ROW_HEADER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TEAROFF_MENU_ITEM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TERMINAL
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_TEXT
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_TOGGLE_BUTTON
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TOOL_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TOOL_TIP
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TREE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TREE_TABLE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_UNKNOWN
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_VIEWPORT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_WINDOW
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_EXTENDED
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_HEADER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FOOTER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_PARAGRAPH
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_RULER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_APPLICATION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_AUTOCOMPLETE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_EDITBAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_EMBEDDED
    CONTROL_TYPE_FOCUS,       // ATSPI_ROLE_ENTRY
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CHART
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CAPTION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_FRAME
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_HEADING
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_PAGE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SECTION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_REDUNDANT_OBJECT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FORM
    CONTROL_TYPE_PRESS,       // ATSPI_ROLE_LINK
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_INPUT_METHOD_WINDOW
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TABLE_ROW
    CONTROL_TYPE_ONLY_ACTION, // ATSPI_ROLE_TREE_ITEM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_SPREADSHEET
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_PRESENTATION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_TEXT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_WEB
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DOCUMENT_EMAIL
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_COMMENT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LIST_BOX
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_GROUPING
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_IMAGE_MAP
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_NOTIFICATION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_INFO_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LEVEL_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TITLE_BAR
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_BLOCK_QUOTE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_AUDIO
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_VIDEO
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DEFINITION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_ARTICLE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LANDMARK
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LOG
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MARQUEE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MATH
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_RATING
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_TIMER
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_STATIC
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MATH_FRACTION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MATH_ROOT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SUBSCRIPT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SUPERSCRIPT
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DESCRIPTION_LIST
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DESCRIPTION_TERM
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_DESCRIPTION_VALUE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_FOOTNOTE
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CONTENT_DELETION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_CONTENT_INSERTION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_MARK
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_SUGGESTION
    CONTROL_TYPE_NONE,        // ATSPI_ROLE_LAST_DEFINED
};

// from an accessible find the control type
ControlType identify_control(AtspiAccessible *accessible)
{
    if (!accessible)
        return CONTROL_TYPE_NONE;

    // get control type from role
    AtspiRole role = atspi_accessible_get_role(accessible, NULL);
    ControlType control_type = role_to_type[role];

    // check specific exceptions
    switch (control_type)
    {
    case CONTROL_TYPE_FOCUS:
        // todo: check if already has focus
        break;

    case CONTROL_TYPE_PAGE_TAB:
        // todo: check if already active (what about closing in shifted state)
        break;

    case CONTROL_TYPE_ONLY_ACTION:
        // must have action interface
        AtspiAction *action = atspi_accessible_get_action_iface(accessible);
        if (!action)
        {
            control_type = CONTROL_TYPE_NONE;
            break;
        }

        // todo: check action name against common phrases, like "click" and "press"

        // must have at least one action
        if (atspi_action_get_n_actions(action, NULL) < 1)
            control_type = CONTROL_TYPE_NONE;

        g_object_unref(action);
        break;

    default:
        break;
    }

    // return
    return control_type;
}