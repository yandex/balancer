#pragma once

#include "base.h"
#include <util/generic/buffer.h>

/// @addtogroup Streams
/// @{

ui64 TransferData(TInputStream* in, TOutputStream* out, void* buf, size_t sz);
ui64 TransferData(TInputStream* in, TOutputStream* out, TBuffer&);
ui64 TransferData(TInputStream* in, TOutputStream* out, size_t sz);
ui64 TransferData(TInputStream* in, TOutputStream* out);

/// @}
