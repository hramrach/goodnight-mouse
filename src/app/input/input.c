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

#include "input.h"

#include "subscriber.h"
#include "timeout.h"
#include "modifiers.h"

#define KEYBOARD_EVENTS (INPUT_KEY_PRESSED | INPUT_KEY_RELEASED)
#define KEYBOARD_SYNC_TYPE (ATSPI_KEYLISTENER_SYNCHRONOUS | ATSPI_KEYLISTENER_CANCONSUME)

#define MOUSE_EVENTS (INPUT_BUTTON_PRESSED | INPUT_BUTTON_RELEASED)

static gboolean event_callback(AtspiDeviceEvent *event, gpointer input_ptr);

Input *input_new()
{
    Input *input = g_new(Input, 1);

    // initialize subscriber list
    input->subscribers = NULL;

    // create listeners
    input->listening = FALSE;
    input->keyboard_listener = atspi_device_listener_new(event_callback, input, NULL);
    input->mouse_listener = atspi_device_listener_new(event_callback, input, NULL);

    // register listeners
    input_start(input);

    return input;
}

void input_destroy(Input *input)
{
    // deregister listeners
    input_stop(input);

    // free listeners
    g_object_unref(input->keyboard_listener);
    g_object_unref(input->mouse_listener);

    // free subscriber lists
    g_slist_free_full(input->subscribers, (GDestroyNotify)subscriber_free);

    g_free(input);
}

void input_subscribe(Input *input, InputEvent event, InputCallback callback, gpointer data)
{
    Subscriber *subscriber = subscriber_new(event, callback, data);

    input->subscribers = g_slist_prepend(input->subscribers, subscriber);
}

void input_unsubscribe(Input *input, InputCallback callback)
{
    GSList *subscriber_node;
    while ((subscriber_node = g_slist_find_custom(input->subscribers, callback, subscriber_compare_callback)))
    {
        subscriber_free(subscriber_node->data);
        input->subscribers = g_slist_remove_all(input->subscribers, subscriber_node->data);
    }
}

guint input_modifiers(Input *input)
{
    GdkKeymap *keymap = gdk_keymap_get_for_display(gdk_display_get_default());
    guint modifiers = gdk_keymap_get_modifier_state(keymap);
    return modifiers;
}

void input_start(Input *input)
{
    // check state
    if (input->listening)
        return;
    input->listening = TRUE;

    // disable timeout to prevent long blocking on pending key events
    timeout_disable();

    // register keyboard listener
    for (gint modifier_mask = 0; modifier_mask <= 0xFF; modifier_mask++)
        atspi_register_keystroke_listener(input->keyboard_listener,
                                          NULL,
                                          modifier_mask,
                                          KEYBOARD_EVENTS,
                                          KEYBOARD_SYNC_TYPE,
                                          NULL);

    // register mouse listener
    atspi_register_device_event_listener(input->mouse_listener, MOUSE_EVENTS, NULL, NULL);

    // renable timeout
    timeout_enable();
}

void input_stop(Input *input)
{
    // check state
    if (!input->listening)
        return;
    input->listening = FALSE;

    // disable timeout to prevent long blocking on pending key events
    timeout_disable();

    // deregister keyboard listener
    for (gint modifier_mask = 0; modifier_mask < 0xFF; modifier_mask++)
        atspi_deregister_keystroke_listener(input->keyboard_listener,
                                            NULL,
                                            modifier_mask,
                                            KEYBOARD_EVENTS,
                                            NULL);

    // deregister mouse listener
    atspi_deregister_device_event_listener(input->mouse_listener, NULL, NULL);

    // renable timeout
    timeout_enable();
}

static gboolean event_callback(AtspiDeviceEvent *atspi_event, gpointer input_ptr)
{
    // extract event
    InputEvent event = {
        .type = (1 << atspi_event->type),
        .id = atspi_event->id,
        .modifiers = modifiers_map(atspi_event->modifiers),
    };
    // free given event
    g_boxed_free(ATSPI_TYPE_DEVICE_EVENT, atspi_event);

    // retrieve input
    Input *input = (Input *)input_ptr;

    // find matching subscribers
    InputResponse consumption = INPUT_RELAY_EVENT;
    for (GSList *subscriber_node = input->subscribers;
         subscriber_node != NULL;
         subscriber_node = subscriber_node->next)
    {
        Subscriber *subscriber = (Subscriber *)subscriber_node->data;

        if (subscriber_compare_event(subscriber, event))
            continue;

        if (subscriber_call(subscriber, event) == INPUT_CONSUME_EVENT)
            consumption = INPUT_CONSUME_EVENT;
    }

    return consumption;
}
