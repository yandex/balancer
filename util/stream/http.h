#pragma once

#include "ios.h"

#include <util/system/defaults.h>
#include <util/generic/ptr.h>
#include <util/generic/stroka.h>
#include <util/generic/strbuf.h>
#include <util/generic/vector.h>
#include <util/generic/yexception.h>

class TSocket;

struct THttpException: public yexception {
};

struct THttpParseException: public THttpException {
};

/// @addtogroup Streams_HTTP
/// @{

/// Объект, содержащий информацию о HTTP-заголовке.
class THttpInputHeader {
    public:
        /// @param[in] header - строка вида 'параметр: значение'.
        THttpInputHeader(const Stroka& header);
        /// @param[in] name - имя параметра.
        /// @param[in] value - значение параметра.
        THttpInputHeader(const Stroka& name, const Stroka& value);
        ~THttpInputHeader() throw ();

        /// Возвращает имя параметра.
        inline const Stroka& Name() const throw () {
            return Name_;
        }

        /// Возвращает значение параметра.
        inline const Stroka& Value() const throw () {
            return Value_;
        }

        /// Записывает заголовок вида "имя параметра: значение\r\n" в поток.
        void OutTo(TOutputStream* stream) const;

        /// Возвращает строку "имя параметра: значение".
        inline Stroka ToString() const {
            return Name_ + ": " + Value_;
        }

    private:
        Stroka Name_;
        Stroka Value_;
};

/// Контейнер для хранения HTTP-заголовков
class THttpHeaders {
        typedef yvector<THttpInputHeader> THeaders;
    public:
        typedef THeaders::const_iterator TConstIterator;

        THttpHeaders();
        /// Добавляет каждую строку из потока в контейнер, считая ее правильным заголовком.
        THttpHeaders(TInputStream* stream);
        ~THttpHeaders() throw ();

        /// Стандартный итератор.
        inline TConstIterator Begin() const throw () {
            return Headers_.begin();
        }

        /// Стандартный итератор.
        inline TConstIterator End() const throw () {
            return Headers_.end();
        }

        /// Возвращает количество заголовков в контейнере.
        inline size_t Count() const throw () {
            return Headers_.size();
        }

        /// Проверяет, содержит ли контейнер хотя бы один заголовок.
        inline bool Empty() const throw () {
            return Headers_.empty();
        }

        /// Добавляет заголовок в контейнер.
        void AddHeader(const THttpInputHeader& header);

        /// Добавляет заголовок в контейнер, если тот не содержит заголовка
        /// c таким же параметром. В противном случае, заменяет существующий
        /// заголовок на новый.
        void AddOrReplaceHeader(const THttpInputHeader& header);

        /// Записывает все заголовки контейнера в поток.
        /// @details Каждый заголовк записывается в виде "имя параметра: значение\r\n".
        void OutTo(TOutputStream* stream) const;

        /// Обменивает наборы заголовков двух контейнеров.
        void Swap(THttpHeaders& headers) throw() {
            Headers_.swap(headers.Headers_);
        }

    private:
        THeaders Headers_;
};

/// Чтение ответа HTTP-сервера.
class THttpInput: public TInputStream {
    public:
        THttpInput(TInputStream* slave);
        virtual ~THttpInput() throw ();

        /*
         * parsed http headers
         */
        /// Возвращает контейнер с заголовками ответа HTTP-сервера.
        const THttpHeaders& Headers() const throw ();

        /*
         * first line - response or request
         */
        /// Возвращает первую строку ответа HTTP-сервера.
        /// @details Первая строка HTTP-сервера - строка состояния,
        /// содержащая три поля: версию HTTP, код состояния и описание.
        const Stroka& FirstLine() const throw ();

        /*
         * connection can be keep-alive
         */
        /// Проверяет, не завершено ли соединение с сервером.
        /// @details Транзакция считается завершенной, если не передан заголовок
        /// "Connection: Keep Alive".
        bool IsKeepAlive() const throw ();

        /*
         * output data can be encoded
         */
        /// Проверяет, поддерживается ли данный тип кодирования содержимого
        /// ответа HTTP-сервера.
        bool AcceptEncoding(const Stroka& coding) const;

        /// Пытается определить наилучший тип кодирования ответа HTTP-сервера.
        /// @details Если ответ сервера говорит о том, что поддерживаются
        /// любые типы кодирования, выбирается gzip. В противном случае
        /// из списка типов кодирования выбирается лучший из поддерживаемых сервером.
        Stroka BestCompressionScheme() const;

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// Передача запроса HTTP-серверу.
class THttpOutput: public TOutputStream {
    public:
        THttpOutput(TOutputStream* slave);
        THttpOutput(TOutputStream* slave, THttpInput* request);
        virtual ~THttpOutput() throw ();

        /*
         * sent http headers
         */
        /// Возвращает контейнер с заголовками запроса к HTTP-серверу.
        const THttpHeaders& SentHeaders() const throw ();

        /// Устанавливает режим, при котором сервер выдает ответ в упакованном виде.
        void EnableCompression(bool enable);
        void EnableCompression(const char** schemas, size_t count);

        /// Устанавливает режим, при котором соединение с сервером не завершается
        /// после окончания транзакции.
        void EnableKeepAlive(bool enable);

        /// Проверяет, производится ли выдача ответов в упакованном виде.
        bool IsCompressionEnabled() const throw ();

        /// Проверяет, не завершается ли соединение с сервером после окончания транзакции.
        bool IsKeepAliveEnabled() const throw ();

        /*
         * is this connection can be really keep-alive
         */
        /// Проверяет, можно ли установить режим, при котором соединение с сервером
        /// не завершается после окончания транзакции.
        bool CanBeKeepAlive() const throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);
        virtual void DoFlush();
        virtual void DoFinish();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// Потоки ввода/вывода для получения запросов и отправки ответов HTTP-сервера.
class THttpServerConn {
    public:
        THttpServerConn(const TSocket& s);
        ~THttpServerConn() throw ();

        THttpInput* Input() throw ();
        THttpOutput* Output() throw ();

        inline const THttpInput* Input() const throw () {
            return const_cast<THttpServerConn*>(this)->Input();
        }

        inline const THttpOutput* Output() const throw () {
            return const_cast<THttpServerConn*>(this)->Output();
        }

        /// Проверяет, можно ли установить режим, при котором соединение с сервером
        /// не завершается после окончания транзакции.
        inline bool CanBeKeepAlive() const throw () {
            return Output()->CanBeKeepAlive();
        }

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};

/// Возвращает код состояния из ответа сервера.
unsigned ParseHttpRetCode(const TStringBuf& ret);

/// Отправляет HTTP-серверу запрос с минимумом необходимых заголовков.
void SendMinimalHttpRequest(
      TSocket& s
    , const TStringBuf& host
    , const TStringBuf& request
    , const TStringBuf& agent = "YandexSomething/1.0"
    , const TStringBuf& from = "webadmin@yandex.ru"
);

/// @}
