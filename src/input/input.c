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

#include <stdio.h>

#include "../utils/timeout.h"
#include "../utils/modifiers.h"

#define KEYBOARD_EVENTS ((1 << ATSPI_KEY_PRESSED_EVENT) | (1 << ATSPI_KEY_RELEASED_EVENT))
#define KEYBOARD_SYNC_TYPE (ATSPI_KEYLISTENER_SYNCHRONOUS | ATSPI_KEYLISTENER_CANCONSUME)

#define MOUSE_EVENTS ((1 << ATSPI_BUTTON_PRESSED_EVENT) | (1 << ATSPI_BUTTON_RELEASED_EVENT))

#define SUBSCRIPTIONS_MATCH 0
#define SUBSCRIPTIONS_NOT_MATCH -1

typedef struct Subscriber
{
    InputCallback callback;
    gpointer user_data;
    gboolean match_all;
    InputEvent event;
} Subscriber;

void subscribe(Input *input, Subscriber subscription);
void subscriber_free(gpointer subscriber);
gint subscriber_matches_subscription(gconstpointer subscriber_ptr, gconstpointer subscription_ptr);
gint subscriber_matches_event(gconstpointer subscriber_ptr, gconstpointer event_ptr);
gint subscriber_matches_callback(gconstpointer subscriber_ptr, gconstpointer callback_ptr);
void register_listeners(Input *input);
void deregister_listeners(Input *input);
gboolean event_callback(AtspiDeviceEvent *event, gpointer input_ptr);

Input *input_new()
{
    Input *input = g_malloc(sizeof(Input));

    // initialize subscriber linked list
    input->subscribers = NULL;

    // register listeners
    register_listeners(input);

    return input;
}

void input_destroy(Input *input)
{
    // free subscriber lists
    g_slist_free_full(input->subscribers, subscriber_free);

    // register listeners
    deregister_listeners(input);

    g_free(input);
}

void input_subscribe(Input *input, InputEvent event, InputCallback callback, gpointer user_data)
{
    Subscriber subscription = {
        .match_all = FALSE,
        .event = event,
        .callback = callback,
        .user_data = user_data,
    };

    subscribe(input, subscription);
}

void input_subscribe_all(Input *input, InputCallback callback, gpointer user_data)
{
    Subscriber subscription = {
        .match_all = TRUE,
        .callback = callback,
        .user_data = user_data,
    };

    subscribe(input, subscription);
}

void subscribe(Input *input, Subscriber subscription)
{
    if (g_slist_find_custom(input->subscribers, &subscription, subscriber_matches_subscription))
        return;

    Subscriber *subscriber = g_malloc(sizeof(Subscriber));
    *subscriber = subscription;

    input->subscribers = g_slist_prepend(input->subscribers, subscriber);
}

void input_unsubscribe(Input *input, InputCallback callback)
{
    GSList *subscriber;
    while ((subscriber = g_slist_find_custom(input->subscribers, callback, subscriber_matches_callback)))
    {
        subscriber_free(subscriber->data);
        input->subscribers = g_slist_remove_all(input->subscribers, subscriber->data);
    }
}

void subscriber_free(gpointer subscriber)
{
    g_free(subscriber);
}

gint subscriber_matches_subscription(gconstpointer subscriber_ptr, gconstpointer subscription_ptr)
{
    Subscriber *subscriber = (Subscriber *)subscriber_ptr;
    Subscriber *subscription = (Subscriber *)subscription_ptr;

    // compare callback
    if (subscriber_matches_callback(subscriber, subscription->callback) == SUBSCRIPTIONS_NOT_MATCH)
        return SUBSCRIPTIONS_NOT_MATCH;

    // compare with event
    return subscriber_matches_event(subscriber, &subscription->event);
}

gint subscriber_matches_event(gconstpointer subscriber_ptr, gconstpointer event_ptr)
{
    Subscriber *subscriber = (Subscriber *)subscriber_ptr;
    InputEvent *event = (InputEvent *)event_ptr;

    // check if should match all
    if (subscriber->match_all)
        return SUBSCRIPTIONS_MATCH;

    // compare with event
    if ((subscriber->event.type != event->type) ||
        (subscriber->event.id != event->id) ||
        (map_modifiers(subscriber->event.modifiers) != map_modifiers(event->modifiers)))
        return SUBSCRIPTIONS_NOT_MATCH;

    return SUBSCRIPTIONS_MATCH;
}

gint subscriber_matches_callback(gconstpointer subscriber_ptr, gconstpointer callback_ptr)
{
    Subscriber *subscriber = (Subscriber *)subscriber_ptr;
    InputCallback callback = (InputCallback)callback_ptr;

    // compare callback
    if (subscriber->callback != callback)
        return SUBSCRIPTIONS_NOT_MATCH;

    return SUBSCRIPTIONS_MATCH;
}

void register_listeners(Input *input)
{
    // disable timeout to prevent long blocking on pending key events
    timeout_disable();

    // register keyboard listener
    input->keyboard_listener = atspi_device_listener_new(event_callback, input, NULL);
    for (gint modifier_mask = 0; modifier_mask <= 0xFF; modifier_mask++)
        atspi_register_keystroke_listener(input->keyboard_listener,
                                          NULL,
                                          modifier_mask,
                                          KEYBOARD_EVENTS,
                                          KEYBOARD_SYNC_TYPE,
                                          NULL);

    // register mouse listener
    input->mouse_listener = atspi_device_listener_new(event_callback, input, NULL);
    atspi_register_device_event_listener(input->mouse_listener, MOUSE_EVENTS, NULL, NULL);

    // renable timeout
    timeout_enable();
}

void deregister_listeners(Input *input)
{
    // disable timeout to prevent long blocking on pending key events
    timeout_disable();

    // deregister keyboard listener
    for (gint modifier_mask = 0; modifier_mask < 256; modifier_mask++)
        atspi_deregister_keystroke_listener(input->keyboard_listener,
                                            NULL,
                                            modifier_mask,
                                            KEYBOARD_EVENTS,
                                            NULL);
    g_object_unref(input->keyboard_listener);

    // deregister mouse listener
    atspi_deregister_device_event_listener(input->mouse_listener, NULL, NULL);
    g_object_unref(input->mouse_listener);

    // renable timeout
    timeout_enable();
}

gboolean event_callback(AtspiDeviceEvent *atspi_event, gpointer input_ptr)
{
    // extract event
    InputEvent event = {
        .type = atspi_event->type,
        .id = atspi_event->id,
        .modifiers = map_modifiers(atspi_event->modifiers),
    };
    // free given event
    g_boxed_free(ATSPI_TYPE_DEVICE_EVENT, atspi_event);

    // retrieve input
    Input *input = (Input *)input_ptr;

    // find matching subscribers
    InputEventAction consumption = INPUT_RELAY_EVENT;
    for (GSList *subscriber_list = input->subscribers;
         subscriber_list != NULL;
         subscriber_list = subscriber_list->next)
    {
        Subscriber *subscriber = (Subscriber *)subscriber_list->data;

        if (subscriber_matches_event(subscriber, &event) == SUBSCRIPTIONS_NOT_MATCH)
            continue;

        if (subscriber->callback(event, subscriber->user_data) == INPUT_CONSUME_EVENT)
            consumption = INPUT_CONSUME_EVENT;
    }

    return consumption;
}
