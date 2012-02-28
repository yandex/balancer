CFLAGS(-I${ARCADIA_BUILD_ROOT}/contrib/libs/pire/pire)

MACRO (BUILDWITH_PIRE_INLINE srcfile dstfile)
TOOLDIR_EX(library/pire/inline __pire_inline_)
ADD_CUSTOM_COMMAND(
   OUTPUT "${dstfile}"
   COMMAND ${__pire_inline_} -o "${dstfile}" "${srcfile}"
   MAIN_DEPENDENCY "${srcfile}"
   DEPENDS "${srcfile}" pire_inline
   COMMENT "Embedding regexps into ${srcfile}"
   WORKING_DIRECTORY ${CURDIR}
   ${ARGN}
)
SRCS("${dstfile}")
ENDMACRO (BUILDWITH_PIRE_INLINE)
