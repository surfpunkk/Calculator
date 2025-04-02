#ifndef CALC_APP_H
#define CALC_APP_H

#include "event_handlers.h"

class Calculator {
public:
    Calculator();
    int run(int argc, char *argv[]);

private:
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *header_bar;
    GtkWidget *undo_button;
    GtkWidget *theme_switch;
    std::vector<std::string> history;
    bool dark_theme = false;

    static void load_css(const char* css_path);
    void setup_header_bar();
    void undo_last_action();
    void toggle_theme() const;
    void setup_ui();
    static void on_button_clicked(GtkWidget *widget, gpointer data);
    static gboolean on_key_press(GtkWidget *widget, const GdkEventKey *event);
};

#endif //CALC_APP_H