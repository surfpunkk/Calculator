#include <gtk/gtk.h>
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

    if (error != nullptr) {
        g_warning("Ошибка загрузки CSS: %s", error->message);
        g_error_free(error);
    }

    g_object_unref(provider);
}

void Calculator::setup_ui() {
    load_css();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Калькулятор");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(window), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), FALSE);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
    gtk_widget_set_size_request(entry, 280, 300);
    gtk_widget_set_name(entry, "entry-field");
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    const char *buttons[] = {
        "%", "√", "^", "C",
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ",", "=", "+"
    };
    for (int i = 0; i < 20; ++i) {
        GtkWidget *button = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(button, 50, 50);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4 + 1, 1, 1);
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