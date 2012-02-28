#include <util/string/util.h>

bool CheckExceptionMessage(const char* msg, Stroka& err) {
    static const char* badMsg[] = {
        "Операция успешно завершена",
        "The operation completed successfully",
        "No error"
    };

    err.clear();

    if (msg == 0) {
        err = "Error message is null";
        return false;
    }

    if (IsSpace(msg)) {
        err = "Error message is empty";
        return false;
    }

    for (int i = 0; i < (int)ARRAY_SIZE(badMsg); ++i) {
        if (strstr(msg, badMsg[i]) != 0) {
            err = "Invalid error message: " + Stroka(msg);
            return false;
        }
    }

    return true;
}
