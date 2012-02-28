%%{
    machine ParseHttpRequest;

    action endOfHeaders {
        cur->AddPartTo(p + 1, pe, *Unparsed);
        MoveTo(&lst, Unparsed);

        return true;
    }

    action startOfRequest {
        curTok = &Request->Request_;
    }

    action startOfKey {
        curHdr = Request->NewHeader();
        curTok = &curHdr->Key;
    }

    action startOfValue {
        curTok = &curHdr->Value;
        curHdr = 0;
    }

    action endOfToken {
        cur->AddPartTo(sp, p, *curTok);
        curTok = 0;
    }

    action nextToken {
        sp = p;
    }

    char = any -- [\r\n];
    lend = ('\r'? '\n') %nextToken;
    request = (char+) >startOfRequest %endOfToken;
    hdelim = [ \t:];
    hchar = char -- hdelim;
    hkey = (hchar+) >startOfKey %endOfToken;
    hvalue = (hchar char*) >startOfValue %endOfToken;
    header = hkey ((hdelim+) %nextToken) hvalue?;

    main := request lend (header lend)* ((lend) @endOfHeaders);
}%%

#if defined(MACHINE_DATA)
#undef MACHINE_DATA
%%{
    write data;
}%%
#endif

#if defined(MACHINE_INIT)
#undef MACHINE_INIT
%%{
    write init;
}%%
#endif

#if defined(MACHINE_EXEC)
#undef MACHINE_EXEC
%%{
    write exec;
}%%
#endif
