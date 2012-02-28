#include "quicklz.h"

const TQuickLZMethods* LzqTable(unsigned ver, unsigned level, unsigned buf) {
    if (ver > 1 || level > 3 || buf > 2) {
        return 0;
    }

    return GetLzqTable(ver, level, buf);
}
