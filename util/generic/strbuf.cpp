/*
 *  Created on: May 28, 2010
 *      Author: albert@
 *
 * $Id: strbuf.cpp 224164 2010-05-31 22:57:49Z albert $
 */

#include "strbuf.h"
#include <util/stream/output.h>

template <>
void Out<TStringBuf>(TOutputStream& os, const TStringBuf& obj) {
    os.Write(obj.data(), obj.length());
}
template <>
void Out<TWtringBuf>(TOutputStream& os, const TWtringBuf& obj) {
    WriteString(os, obj.data(), obj.length());
}
