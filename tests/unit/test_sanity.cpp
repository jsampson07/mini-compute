#include <cassert>
#include "log.h"

int main() {
    logutil::info("running unit sanity test");
    assert(1 + 1 == 2);
    return 0;
}
