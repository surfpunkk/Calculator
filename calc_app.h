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
    void setup_ui();
    static void on_button_clicked(GtkWidget *widget, gpointer data);
    static gboolean on_key_press(GtkWidget *widget, const GdkEventKey *event);
};

#endif //CALC_APP_H
