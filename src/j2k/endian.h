// Is __LITTLE_ENDIAN__ set
#ifdef __LITTLE_ENDIAN__
// Is __BIG_ENDIAN__ also set? (error)
#ifdef __BIG_ENDIAN__
#error Both __LITTLE_ENDIAN__ and __BIG_ENDIAN__ are defined
#endif

// Otherwise, is __BIG_ENDIAN__ set?
#elif __BIG_ENDIAN__

// Otherwise, is __BYTE_ORDER__ set?
#elif __BYTE_ORDER__
// get endian from __BYTE_ORDER__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __BIG_ENDIAN__
#else
// Not sure what __BYTE_ORDER__ is... raise error
#error Need to compile with either -D__LITTLE_ENDIAN__ or -D__BIG_ENDIAN__
#endif

// None of these are set... raise error
#else
#error Need to compile with either -D__LITTLE_ENDIAN__ or -D__BIG_ENDIAN__

#endif

// At this point, unless error was raised, either __LITTLE_ENDIAN__
//   or __BIG_ENDIAN__ should be set (and only one of them).
