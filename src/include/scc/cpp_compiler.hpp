#if defined(__clang__)
    #define SCC_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define SCC_GCC
#elif defined(_MSC_VER)
    #define SCC_MSVC
#endif

// Borrowed from DOCTEST ..
// not tested, but should work :)

#ifdef SCC_CLANG 
    #define SCC_PRAGMA_TO_STR(x) _Pragma(#x)
    #define SCC_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
    #define SCC_CLANG_SUPPRESS_WARNING(w) SCC_PRAGMA_TO_STR(clang diagnostic ignored w)
    #define SCC_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
    #define SCC_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)                                                \
    SCC_CLANG_SUPPRESS_WARNING_PUSH SCC_CLANG_SUPPRESS_WARNING(w)
#else // SCC_CLANG
    #define SCC_CLANG_SUPPRESS_WARNING_PUSH
    #define SCC_CLANG_SUPPRESS_WARNING(w)
    #define SCC_CLANG_SUPPRESS_WARNING_POP
    #define SCC_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // SCC_CLANG

#ifdef SCC_GCC
    #define SCC_PRAGMA_TO_STR(x) _Pragma(#x)
    #define SCC_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define SCC_GCC_SUPPRESS_WARNING(w) SCC_PRAGMA_TO_STR(GCC diagnostic ignored w)
    #define SCC_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
    #define SCC_GCC_SUPPRESS_WARNING_WITH_PUSH(w)                                                  \
        SCC_GCC_SUPPRESS_WARNING_PUSH SCC_GCC_SUPPRESS_WARNING(w)
#else // SCC_GCC
    #define SCC_GCC_SUPPRESS_WARNING_PUSH
    #define SCC_GCC_SUPPRESS_WARNING(w)
    #define SCC_GCC_SUPPRESS_WARNING_POP
    #define SCC_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // SCC_GCC

#ifdef SCC_MSVC
    #define SCC_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
    #define SCC_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
    #define SCC_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
    #define SCC_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)                                                 \
        SCC_MSVC_SUPPRESS_WARNING_PUSH SCC_MSVC_SUPPRESS_WARNING(w)
#else // SCC_MSVC
    #define SCC_MSVC_SUPPRESS_WARNING_PUSH
    #define SCC_MSVC_SUPPRESS_WARNING(w)
    #define SCC_MSVC_SUPPRESS_WARNING_POP
    #define SCC_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif

#define SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON                                                \
    SCC_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wsign-compare")                                       \
    SCC_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsign-compare")                                         \
    SCC_MSVC_SUPPRESS_WARNING_WITH_PUSH(4388)

// pop ignore sign comparsion warning
#define SCC_SUPPRESS_WARNING_POP                                                 \
    SCC_CLANG_SUPPRESS_WARNING_POP SCC_GCC_SUPPRESS_WARNING_POP SCC_MSVC_SUPPRESS_WARNING_POP


