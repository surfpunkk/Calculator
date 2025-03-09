#include "calc_app.h"

int main(const int argc, char *argv[]) {
    auto app = Calculator();
    return app.run(argc, argv);
}
