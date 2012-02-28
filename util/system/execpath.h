#pragma once

class Stroka;
class TMappedFile;

const Stroka& GetExecPath();

// Opens the program's binary for reading, if it still exists. Throws an exception on failure.
TMappedFile* OpenExecFile();
