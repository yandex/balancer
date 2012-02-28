#pragma once

enum EOptionValue {
    OV_DEFAULT,
    OV_FALSE,
    OV_TRUE,
};

inline bool CalcOptionValue(EOptionValue value, bool defaultValue) {
    switch (value) {
        case OV_DEFAULT:
            return defaultValue;
        case OV_FALSE:
            return false;
        case OV_TRUE:
            return true;
    }

    assert(false);

    return false;
}

inline EOptionValue OptionValue(bool value) {
    return value ? OV_TRUE : OV_FALSE;
}
