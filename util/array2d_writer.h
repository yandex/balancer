#pragma once

#include "ffb.h"
#include "fput.h"

#include <util/system/oldfile.h>
#include <util/system/defaults.h>
#include <util/generic/stroka.h>
#include <util/generic/yexception.h>

// Интерфейс записи на диск двумерного массива в файле (см. "array2d.h")
// Предполагается, что запись выполняется в один проход, т.е заранее количество строк массива неизвестно
template<class I, class T>
class TFile2DArrayWriter {

private:
    // Реузультирующий файл, в который пишутся заголовок и смещения начал строк
    // (первая половина 2D-массива в файле)
    Stroka ResultFileName;
    FILE* ResultFile;
    // Файл, в который пишутся данные (вторая половина 2D-массива в файле),
    // и который приписывается к ResultFile по Finish()
    Stroka DataFileName;
    FILE* DataFile;
    ui32 CurrentLineNumber; // Номер текущей строки (с 0)
    ui32 CurrentOffset; // Смещение следующего элемента (количетсво записанных элементов)

public:
    TFile2DArrayWriter(const char* filename, int offset = 0);
    ~TFile2DArrayWriter();

    // Запись данных
    // Начать новую строку массива (после создания объекта первая строка массива уже начата)
    void NewLine();
    // Записать следующий элемент в текущую строку массива
    void Write(const T& t);

    // Сохранить файл (без этого вызова файл сохранён не будет)
    // В случае неудачи удалить временный DataFile, ошибка проглатывается
    void Finish();
    // После Finish вызывать функции записи в файл нельзя
};

// ----------------- реализация --------------------

// Записи структурки в FILE. В случае неудачи кидается исключение
template<class T>
inline void FPutEx(FILE* file, const T& a)
{
    if (!fput(file, a)) {
        ythrow yexception() << "Can't write to file: " << LastSystemErrorText();
    }
}

template<class I, class T>
TFile2DArrayWriter<I, T>::TFile2DArrayWriter(const char* filename, int offset)
  : ResultFileName(filename)
  , ResultFile(fopen_or_die(filename, "wb"))
  , DataFileName(ResultFileName + ".temp") // Временный файл создаётся рядом с целевым
  , DataFile(fopen_or_die(~DataFileName, "wb"))
  , CurrentLineNumber(0)
  , CurrentOffset(offset)
{
    // Прыгаем через заголовок (номер версии и количество строк) в ResultFile,
    // Число строк будем считать в CurrentLineNumber
    fseek(ResultFile, sizeof(ui32) + sizeof(ui32), SEEK_CUR);
    FPutEx(ResultFile, CurrentOffset);
    // Теперь оба файла готовы к записи
}

template<class I, class T>
TFile2DArrayWriter<I, T>::~TFile2DArrayWriter()
{
    // Если хотя бы один файл не закрыт, значит не было Finish и нужно удалить файлы
    if (ResultFile || DataFile) {
        NFs::Remove(~ResultFileName);
        NFs::Remove(~DataFileName);
    }
}

template<class I, class T>
void TFile2DArrayWriter<I, T>::NewLine()
{
    FPutEx(ResultFile, CurrentOffset);
    CurrentLineNumber++;
}

template<class I, class T>
void TFile2DArrayWriter<I, T>::Write(const T& t)
{
    FPutEx(DataFile, t);
    CurrentOffset++;
}

template<class I, class T>
void TFile2DArrayWriter<I, T>::Finish()
{
    // Запишем размер элемента и количество строк в начало ResultFile
    fseek(ResultFile, 0, SEEK_SET);
    const ui32 elemSize = sizeof(T);
    FPutEx(ResultFile, elemSize);
    const ui32 lineCount = CurrentLineNumber + 1;
    FPutEx(ResultFile, lineCount);

    // Закрываем файлы
    if (fclose(ResultFile) == 0)
        ResultFile = NULL;
    if (fclose(DataFile) == 0)
        DataFile = NULL;

    // Допишем в конец ResultFile содержимое DataFile
    TOldOsFile::Cat(~ResultFileName, ~DataFileName);

    // Удаляем временный
    NFs::Remove(~DataFileName);
}
