#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "calc_app.h"

Calculator::Calculator() : window(nullptr), grid(nullptr), entry(nullptr) {}

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    const char *css_path = "../styles.css";
    GError *error = nullptr;
    gtk_css_provider_load_from_path(provider, css_path, &error);

    g_object_unref(provider);
}

void Calculator::setup_ui() {
    load_css();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Калькулятор");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(window), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), FALSE);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
    gtk_widget_set_size_request(entry, 400, 200);
    gtk_widget_set_name(entry, "entry-field");
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 5, 1);

    g_signal_connect(entry, "key-press-event", G_CALLBACK(on_key_press), nullptr);

    const char *buttons[] = {
        "(", ")", "^", "C", "⌫",
        "7", "8", "9", "/", "%",
        "4", "5", "6", "*", "√",
        "1", "2", "3", "-", "log",
        "0", ",", "=", "+", "Pi"
    };
    for (int i = 0; i < 25; ++i) {
        GtkWidget *button = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(button, 50, 50);
        if (buttons[i] == "=") gtk_widget_set_name(button, "button_equal");
        if (buttons [i] == "⌫") gtk_widget_set_name(button, "button_del");
        gtk_grid_attach(GTK_GRID(grid), button, i % 5, i / 5 + 1, 1, 1);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
    }
    gtk_widget_show_all(window);
}
int Calculator::run(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    setup_ui();
    gtk_main();
    return 0;
}

void Calculator::on_button_clicked(GtkWidget *widget, gpointer data) {
    const char *label = gtk_button_get_label(GTK_BUTTON(widget));
    GtkWidget *entry = GTK_WIDGET(data);

    EventHandlers::handle_input(entry, label);
}

gboolean Calculator::on_key_press(GtkWidget *widget, const GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        EventHandlers::handle_input(widget, "=");
        return TRUE;
    }
    return FALSE;
}