#include "calc_app.h"

Calculator::Calculator() : window(nullptr), grid(nullptr), entry(nullptr), header_bar(nullptr), undo_button(),
theme_switch() {}

void Calculator::load_css(const char* css_path) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    static GtkCssProvider *old_provider = nullptr;
    if (old_provider) {
        gtk_style_context_remove_provider_for_screen(
            screen,
            GTK_STYLE_PROVIDER(old_provider)
        );
        g_object_unref(old_provider);
    }
    old_provider = provider;

    GError *error = nullptr;
    if (!gtk_css_provider_load_from_path(provider, css_path, &error)) {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_error_free(error);
    }

    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void Calculator::setup_header_bar() {
    header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "Калькулятор");
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    undo_button = gtk_button_new_with_label("↩");
    g_signal_connect_swapped(undo_button, "clicked", G_CALLBACK(+[](Calculator* self) {
        self->undo_last_action();
    }), this);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), undo_button);

    theme_switch = gtk_switch_new();
    g_signal_connect(theme_switch, "state-set", G_CALLBACK(+[](GtkSwitch*, const gboolean state, Calculator* self) {
        self->dark_theme = state;
        self->toggle_theme();
        return FALSE;
    }), this);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), theme_switch);
}

void Calculator::undo_last_action() {
    if (!history.empty()) {
        history.pop_back();
        if (!history.empty()) {
            gtk_entry_set_text(GTK_ENTRY(entry), history.back().c_str());
        } else {
            gtk_entry_set_text(GTK_ENTRY(entry), "");
        }
    }
}

void Calculator::toggle_theme() const {
    GdkScreen *screen = gdk_screen_get_default();
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_style_context_remove_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider)
    );
    g_object_unref(provider);

    if (dark_theme) {
        load_css("../styles/dark_styles.css");
    } else {
        load_css("../styles/styles.css");
    }
    gtk_widget_queue_draw(window);
}

void Calculator::setup_ui() {

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(window), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), FALSE);
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "../styles/icon.png", nullptr);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    setup_header_bar();
    load_css("../styles/styles.css");

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
    gtk_widget_set_size_request(entry, 400, 200);
    gtk_widget_set_name(entry, "entry-field");
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 5, 1);

    g_signal_connect(entry, "key-press-event", G_CALLBACK(on_key_press), nullptr);

    const char *buttons[] = {
        "(", ")", "𝑥ⁿ", "C", "⌫",
        "7", "8", "9", "÷", "%",
        "4", "5", "6", "×", "√",
        "1", "2", "3", "-", "𝑥!",
        "0", ",", "=", "+", "π"
    };
    for (int i = 0; i < 25; ++i) {
        GtkWidget *button = gtk_button_new_with_label(buttons[i]);
        gtk_widget_set_size_request(button, 50, 50);
        if (buttons[i] == "=") gtk_widget_set_name(button, "button_equal");
        else if (buttons [i] == "⌫") gtk_widget_set_name(button, "button_del");
        else if (buttons [i] == "𝑥ⁿ") gtk_widget_set_name(button, "xn");
        else gtk_widget_set_name(button, "calc_buttons");
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

gboolean Calculator::on_key_press(GtkWidget *widget, const GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        EventHandlers::handle_input(widget, "=");
        gtk_editable_set_position(GTK_EDITABLE(widget), -1);
        return TRUE;
    } if (event->keyval == GDK_KEY_period || event->keyval == GDK_KEY_KP_Decimal) {
        EventHandlers::handle_input(widget, ",");
        gtk_editable_set_position(GTK_EDITABLE(widget), -1);
        return TRUE;
    }
    return FALSE;
}