#include "log.h"
#include "element.h"

TLogElement::TLogElement(TLog* parent)
    : Parent_(parent)
    , Priority_(Parent_->DefaultPriority())
{
    Reset();
}

TLogElement::~TLogElement() throw () {
    try {
        Finish();
    } catch (...) {
    }
}

void TLogElement::DoFlush() {
    const size_t filled = Filled();

    if (filled) {
        Parent_->Write(Priority_, Data(), filled);
        Reset();
    }
}
