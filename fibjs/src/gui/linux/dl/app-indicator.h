/*
An object to represent the application as an application indicator
in the system panel.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>
    Cody Russell <cody.russell@canonical.com>

This program is free software: you can redistribute it and/or modify it
under the terms of either or both of the following licenses:

1) the GNU Lesser General Public License version 3, as published by the
   Free Software Foundation; and/or
2) the GNU Lesser General Public License version 2.1, as published by
   the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranties of
MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the applicable version of the GNU Lesser General Public
License for more details.

You should have received a copy of both the GNU Lesser General Public
License version 3 and version 2.1 along with this program.  If not, see
<http://www.gnu.org/licenses/>
*/

#ifndef __APP_INDICATOR_H__
#define __APP_INDICATOR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * APP_INDICATOR_TYPE:
 *
 * Get the #GType for a #AppIndicator.
 */
/**
 * APP_INDICATOR:
 * @obj: The object to convert
 *
 * Safely convert a #GObject into an #AppIndicator.
 */
/**
 * APP_INDICATOR_CLASS:
 * @klass: #GObjectClass based class to convert.
 *
 * Safely convert a #GObjectClass into a #AppIndicatorClass.
 */
/**
 * IS_APP_INDICATOR:
 * @obj: An #GObject to check
 *
 * Checks to see if @obj is in the object hierarchy of #AppIndicator.
 */
/**
 * IS_APP_INDICATOR_CLASS:
 * @klass: An #GObjectClass to check
 *
 * Checks to see if @klass is in the object class hierarchy of #AppIndicatorClass.
 */
/**
 * APP_INDICATOR_GET_CLASS:
 * @obj: A #GObject in the class hierarchy of #AppIndicator.
 *
 * Gets a pointer to the #AppIndicatorClass for the object @obj.
 */

#define APP_INDICATOR_TYPE            (app_indicator_get_type ())
#define APP_INDICATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), APP_INDICATOR_TYPE, AppIndicator))
#define APP_INDICATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), APP_INDICATOR_TYPE, AppIndicatorClass))
#define IS_APP_INDICATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), APP_INDICATOR_TYPE))
#define IS_APP_INDICATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), APP_INDICATOR_TYPE))
#define APP_INDICATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), APP_INDICATOR_TYPE, AppIndicatorClass))

/**
 * APP_INDICATOR_SIGNAL_NEW_ICON:
 *
 * String identifier for the #AppIndicator::new-icon signal.
 */
/**
 * APP_INDICATOR_SIGNAL_NEW_ATTENTION_ICON:
 *
 * String identifier for the #AppIndicator::new-attention-icon signal.
 */
/**
 * APP_INDICATOR_SIGNAL_NEW_STATUS:
 *
 * String identifier for the #AppIndicator::new-status signal.
 */
/**
 * APP_INDICATOR_SIGNAL_NEW_LABEL:
 *
 * String identifier for the #AppIndicator::new-label signal.
 */
/**
 * APP_INDICATOR_SIGNAL_CONNECTION_CHANGED:
 *
 * String identifier for the #AppIndicator::connection-changed signal.
 */
/**
 * APP_INDICATOR_SIGNAL_NEW_ICON_THEME_PATH:
 *
 * String identifier for the #AppIndicator::new-icon-theme-path signal.
 */
/**
 * APP_INDICATOR_SIGNAL_SCROLL_EVENT:
 *
 * String identifier for the #AppIndicator::scroll-event signal.
 */
#define APP_INDICATOR_SIGNAL_NEW_ICON            "new-icon"
#define APP_INDICATOR_SIGNAL_NEW_ATTENTION_ICON  "new-attention-icon"
#define APP_INDICATOR_SIGNAL_NEW_STATUS          "new-status"
#define APP_INDICATOR_SIGNAL_NEW_LABEL           "new-label"
#define APP_INDICATOR_SIGNAL_CONNECTION_CHANGED  "connection-changed"
#define APP_INDICATOR_SIGNAL_NEW_ICON_THEME_PATH "new-icon-theme-path"
#define APP_INDICATOR_SIGNAL_SCROLL_EVENT        "scroll-event"

/**
 * AppIndicatorCategory:
 * @APP_INDICATOR_CATEGORY_APPLICATION_STATUS: The indicator is used to display the status of the application.
 * @APP_INDICATOR_CATEGORY_COMMUNICATIONS: The application is used for communication with other people.
 * @APP_INDICATOR_CATEGORY_SYSTEM_SERVICES: A system indicator relating to something in the user's system.
 * @APP_INDICATOR_CATEGORY_HARDWARE: An indicator relating to the user's hardware.
 * @APP_INDICATOR_CATEGORY_OTHER: Something not defined in this enum, please don't use unless you really need it.
 * 
 * The category provides grouping for the indicators so that
 * users can find indicators that are similar together.
 */
typedef enum { /*< prefix=APP_INDICATOR_CATEGORY >*/
	APP_INDICATOR_CATEGORY_APPLICATION_STATUS, /*< nick=ApplicationStatus >*/
	APP_INDICATOR_CATEGORY_COMMUNICATIONS, /*< nick=Communications >*/
	APP_INDICATOR_CATEGORY_SYSTEM_SERVICES, /*< nick=SystemServices >*/
	APP_INDICATOR_CATEGORY_HARDWARE, /*< nick=Hardware >*/
	APP_INDICATOR_CATEGORY_OTHER /*< nick=Other >*/
} AppIndicatorCategory;

/**
 * AppIndicatorStatus:
 * @APP_INDICATOR_STATUS_PASSIVE: The indicator should not be shown to the user.
 * @APP_INDICATOR_STATUS_ACTIVE: The indicator should be shown in it's default state.
 * @APP_INDICATOR_STATUS_ATTENTION: The indicator should show it's attention icon.
 *
 * These are the states that the indicator can be on in
 * the user's panel.  The indicator by default starts
 * in the state @APP_INDICATOR_STATUS_PASSIVE and can be
 * shown by setting it to @APP_INDICATOR_STATUS_ACTIVE.
 */
typedef enum { /*< prefix=APP_INDICATOR_STATUS >*/
	APP_INDICATOR_STATUS_PASSIVE, /*< nick=Passive >*/
	APP_INDICATOR_STATUS_ACTIVE, /*< nick=Active >*/
	APP_INDICATOR_STATUS_ATTENTION /*< nick=NeedsAttention >*/
} AppIndicatorStatus;

typedef struct _AppIndicator        AppIndicator;
typedef struct _AppIndicatorClass   AppIndicatorClass;
typedef struct _AppIndicatorPrivate AppIndicatorPrivate;

/**
 * AppIndicatorClass:
 * @parent_class: Mia familia
 * @new_icon: Slot for #AppIndicator::new-icon.
 * @new_attention_icon: Slot for #AppIndicator::new-attention-icon.
 * @new_status: Slot for #AppIndicator::new-status.
 * @new_icon_theme_path: Slot for #AppIndicator::new-icon-theme-path
 * @new_label: Slot for #AppIndicator::new-label.
 * @connection_changed: Slot for #AppIndicator::connection-changed.
 * @scroll_event: Slot for #AppIndicator::scroll-event
 * @app_indicator_reserved_ats: Reserved for future use.
 * @fallback: Function that gets called to make a #GtkStatusIcon when
 *            there is no Application Indicator area available.
 * @unfallback: The function that gets called if an Application
 *              Indicator area appears after the fallback has been created.
 * @app_indicator_reserved_1: Reserved for future use.
 * @app_indicator_reserved_2: Reserved for future use.
 * @app_indicator_reserved_3: Reserved for future use.
 * @app_indicator_reserved_4: Reserved for future use.
 * @app_indicator_reserved_5: Reserved for future use.
 * @app_indicator_reserved_6: Reserved for future use.
 * 
 * The signals and external functions that make up the #AppIndicator
 * class object.
 */
struct _AppIndicatorClass {
	/* Parent */
	GObjectClass parent_class;

	/* DBus Signals */
	void (* new_icon)               (AppIndicator      *indicator,
	                                 gpointer            user_data);
	void (* new_attention_icon)     (AppIndicator      *indicator,
	                                 gpointer            user_data);
	void (* new_status)             (AppIndicator      *indicator,
	                                 const gchar       *status,
	                                 gpointer            user_data);
	void (* new_icon_theme_path)    (AppIndicator      *indicator,
	                                 const gchar       *icon_theme_path,
	                                 gpointer            user_data);
	void (* new_label)              (AppIndicator       *indicator,
	                                 const gchar        *label,
	                                 const gchar        *guide,
	                                 gpointer            user_data);

	/* Local Signals */
	void (* connection_changed)     (AppIndicator * indicator,
	                                 gboolean          connected,
	                                 gpointer          user_data);

	void (* scroll_event)           (AppIndicator * indicator,
	                                 gint                  delta,
	                                 GdkScrollDirection direction,
	                                 gpointer          user_data);

	void (*app_indicator_reserved_ats)(void);

	/* Overridable Functions */
	GtkStatusIcon * (*fallback)     (AppIndicator * indicator);
	void (*unfallback)              (AppIndicator * indicator,
	                                 GtkStatusIcon * status_icon);

	/* Reserved */
	void (*app_indicator_reserved_1)(void);
	void (*app_indicator_reserved_2)(void);
	void (*app_indicator_reserved_3)(void);
	void (*app_indicator_reserved_4)(void);
	void (*app_indicator_reserved_5)(void);
	void (*app_indicator_reserved_6)(void);
};

/**
 * AppIndicator:
 * 
 * A application indicator represents the values that are needed to show a
 * unique status in the panel for an application.  In general, applications
 * should try to fit in the other indicators that are available on the
 * panel before using this.  But, sometimes it is necissary.
 *
 *  Private fields
 * @parent: Parent object.
 * @priv: Internal data.
 */
struct _AppIndicator {
	GObject parent;

	/*< Private >*/
	AppIndicatorPrivate *priv;
};

/* GObject Stuff */
GType                           app_indicator_get_type           (void) G_GNUC_CONST;

AppIndicator                   *app_indicator_new                (const gchar          *id,
                                                                  const gchar          *icon_name,
                                                                  AppIndicatorCategory  category);
AppIndicator                   *app_indicator_new_with_path      (const gchar          *id,
                                                                  const gchar          *icon_name,
                                                                  AppIndicatorCategory  category,
                                                                  const gchar          *icon_theme_path);

/* Set properties */
void                            app_indicator_set_status         (AppIndicator       *self,
                                                                  AppIndicatorStatus  status);
void                            app_indicator_set_attention_icon (AppIndicator       *self,
                                                                  const gchar        *icon_name);
void                            app_indicator_set_attention_icon_full (AppIndicator       *self,
                                                                  const gchar        *icon_name,
                                                                  const gchar        *icon_desc);
void                            app_indicator_set_menu           (AppIndicator       *self,
                                                                  GtkMenu            *menu);
void                            app_indicator_set_icon           (AppIndicator       *self,
                                                                  const gchar        *icon_name);
void                            app_indicator_set_icon_full      (AppIndicator       *self,
                                                                  const gchar        *icon_name,
                                                                  const gchar        *icon_desc);
void                            app_indicator_set_label          (AppIndicator       *self,
                                                                  const gchar        *label,
                                                                  const gchar        *guide);
void                            app_indicator_set_icon_theme_path(AppIndicator       *self,
                                                                  const gchar        *icon_theme_path);
void                            app_indicator_set_ordering_index (AppIndicator       *self,
                                                                  guint32             ordering_index);
void                            app_indicator_set_secondary_activate_target (AppIndicator *self,
                                                                             GtkWidget    *menuitem);
void                            app_indicator_set_title          (AppIndicator       *self,
                                                                  const gchar        *title);

/* Get properties */
const gchar *                   app_indicator_get_id                   (AppIndicator *self);
AppIndicatorCategory            app_indicator_get_category             (AppIndicator *self);
AppIndicatorStatus              app_indicator_get_status               (AppIndicator *self);
const gchar *                   app_indicator_get_icon                 (AppIndicator *self);
const gchar *                   app_indicator_get_icon_desc            (AppIndicator *self);
const gchar *                   app_indicator_get_icon_theme_path      (AppIndicator *self);
const gchar *                   app_indicator_get_attention_icon       (AppIndicator *self);
const gchar *                   app_indicator_get_attention_icon_desc  (AppIndicator *self);
const gchar *                   app_indicator_get_title                (AppIndicator *self);

GtkMenu *                       app_indicator_get_menu                 (AppIndicator *self);
const gchar *                   app_indicator_get_label                (AppIndicator *self);
const gchar *                   app_indicator_get_label_guide          (AppIndicator *self);
guint32                         app_indicator_get_ordering_index       (AppIndicator *self);
GtkWidget *                     app_indicator_get_secondary_activate_target (AppIndicator *self);

/* Helpers */
void                            app_indicator_build_menu_from_desktop (AppIndicator * self,
                                                                  const gchar * desktop_file,
                                                                  const gchar * desktop_profile);

G_END_DECLS

/**
 * SECTION:app-indicator
 * @short_description: An object to put application information
 *                     into the panel.
 * @stability: Unstable
 * @include: libappindicator/app-indicator.h
 *
 * An application indicator is a way for an application to put
 * a menu into the panel on the user's screen.  This allows the
 * user to interact with the application even though it might
 * not be visible to the user at the time.  In most cases this
 * is not a good solution as there are other ways to inform the
 * user.  It should only be use if persistence is a desired
 * feature for the user (not for your marketing purpose of
 * having your logo in the panel).
 */

#endif
