#if !defined(FROM_QUICKLZ_BUILD)
    #undef qlz_decompress
    #undef qlz_compress
    #undef qlz_size_decompressed
    #undef qlz_size_compressed
    #undef qlz_get_setting
    #undef qlz_table
    #undef qlz_compress_core
    #undef qlz_decompress_core
    #undef hash_func
    #undef fast_read
    #undef fast_write

    #undef COMPRESSION_LEVEL
    #undef STREAMING_MODE
    #undef QLZ_YVERSION
    #undef QLZ_COMPRESSION_LEVEL
    #undef QLZ_STREAMING_BUFFER
#endif
