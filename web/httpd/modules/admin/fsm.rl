%%{
    machine Fsm;

    action reopenLog {
        Control->ReopenLog();
        lst.Push(NewChunk(STRINGBUF("<ok>reopen log complete</ok>")));
    }

    action shutDown {
        Control->ShutDown();
    }

    action stats {
        Control->PrintStats(buf);
        lst.Push(NewChunk(TStringBuf(~buf.Buffer(), +buf.Buffer())));
    }

    action version {
        lst.Push(NewChunk(Version_));
    }

    action showListen {
        Control->GetListenAddrs(buf);
        lst.Push(NewChunk(TStringBuf(~buf.Buffer(), +buf.Buffer())));
    }

    action showPorts {
        Control->GetListenPorts(buf);
        lst.Push(NewChunk(TStringBuf(~buf.Buffer(), +buf.Buffer())));
    }

    main := any* 'action=' (('reopenlog' @reopenLog) | ('shutdown' @shutDown) | ('stats' @stats) | ('version' @version) | ('show_listen' @showListen) | ('show_ports' @showPorts)) any*;
}%%

#if defined(MACHINE_DATA)
%%{
    write data;
}%%
#endif

#if defined(MACHINE_CODE)
int cs;

%%{
    write init;
}%%

const TChunkList& req = descr.Request->Request();

for (TChunkList::TConstIterator it = req.Begin(); it != req.End(); ++it) {
    const char* p = it->Data();
    const char* pe = p + it->Length();

    %%{
        write exec;
    }%%
}
#endif

#undef MACHINE_DATA
#undef MACHINE_CODE
