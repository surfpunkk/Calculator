#include "calc_app.h"

int main(int argc, char *argv[]) {
    auto app = Calculator();
    return app.run(argc, argv);
}
