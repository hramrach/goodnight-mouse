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

#include "config.h"

// use https://developer.gnome.org/glib/stable/glib-Key-value-file-parser.html for config
// use getopt.h for arguments

static const Config DEFAULT_CONFIG = {
    .app = {
        .foreground = {},
        .background = {
            .trigger_id = GDK_KEY_v,
            .trigger_modifiers = GDK_SUPER_MASK,
        },
    },

    .run_once_now = FALSE,
};

Config config_parse(int argc, char **argv)
{
    Config config = DEFAULT_CONFIG;

    return config;
}