#pragma once

#include "base.h"

#include <util/generic/ptr.h>
#include <util/system/pipe.h>

/// @addtogroup Streams_Pipes
/// @{

/// Pipe к процессу.
/// @details Запускает fork системного процесса и открывает к нему канал (pipe).
/// @param[in] command - системная команда.
/// @param[in] mode - режим передачи данных через pipe: 'r' - чтение, 'w' - запись.
class TPipeBase {
    protected:
        TPipeBase(const char* command, const char* mode);
        virtual ~TPipeBase() throw ();

    protected:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// Pipe к процессу. Чтение данных.
/// @details Запускает fork процесса и открывает канал для чтения
/// данных из потока вывода команды.
/// @n = TPipeBase (const char* command, 'r').
class TPipeInput: virtual public TPipeBase, public TInputStream {
    public:
        TPipeInput(const char* command);

    private:
        virtual size_t DoRead(void* buf, size_t len);
};

/// Pipe к процессу. Запись данных.
/// @details Запускает fork процесса и открывает pipe для записи данных в поток ввода команды.
/// @n = TPipeBase (const char* command, 'w').
class TPipeOutput: virtual public TPipeBase, public TOutputStream {
    public:
        TPipeOutput(const char* command);
        void Close();

    private:
        virtual void DoWrite(const void* buf, size_t len);
};

/// Двунаправленный pipe к процессу (только FreeBSD)
/// @details @n = TPipeBase (const char* command, 'r+').
class TPipeStream: public TPipeInput, public TPipeOutput {
    public:
        TPipeStream(const char* command);
};

/// Pipe системного уровня
/// @details базовая функциональность реализована через класс TPipeHandle
/// @param[in] fd - файловый дескриптор уже открытого пайпа
class TPipedBase {
    protected:
        TPipedBase(PIPEHANDLE fd);
        virtual ~TPipedBase() throw();

    protected:
        TPipeHandle Handle_;
};

/// Pipe системного уровня. Чтение данных
class TPipedInput: public TPipedBase, public TInputStream {
    public:
        TPipedInput(PIPEHANDLE fd);
        virtual ~TPipedInput() throw();

    private:
        virtual size_t DoRead(void* buf, size_t len);
};

/// Pipe системного уровня. Запись данных
class TPipedOutput: public TPipedBase, public TOutputStream {
    public:
        TPipedOutput(PIPEHANDLE fd);
        virtual ~TPipedOutput() throw();

    private:
        virtual void DoWrite(const void* buf, size_t len);
};

/// @}
