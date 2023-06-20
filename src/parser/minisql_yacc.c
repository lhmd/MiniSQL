/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "minisql.y"

  #include <stdio.h>
  #include "parser/parser.h"

  extern char *yytext;
  extern int yylex(void);
  int yyerror(char* error);

#line 79 "./minisql_yacc.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_MINISQL_YACC_H_INCLUDED
# define YY_YY_MINISQL_YACC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CREATE = 258,
    DROP = 259,
    SELECT = 260,
    INSERT = 261,
    DELETE = 262,
    UPDATE = 263,
    TRXBEGIN = 264,
    TRXCOMMIT = 265,
    TRXROLLBACK = 266,
    QUIT = 267,
    EXECFILE = 268,
    SHOW = 269,
    USE = 270,
    USING = 271,
    DATABASE = 272,
    DATABASES = 273,
    TABLE = 274,
    TABLES = 275,
    INDEX = 276,
    INDEXES = 277,
    ON = 278,
    FROM = 279,
    WHERE = 280,
    INTO = 281,
    SET = 282,
    VALUES = 283,
    PRIMARY = 284,
    KEY = 285,
    UNIQUE = 286,
    CHAR = 287,
    INT = 288,
    FLOAT = 289,
    AND = 290,
    OR = 291,
    NOT = 292,
    IS = 293,
    FLAGNULL = 294,
    IDENTIFIER = 295,
    STRING = 296,
    NUMBER = 297,
    EQ = 298,
    NE = 299,
    LE = 300,
    GE = 301
  };
#endif
/* Tokens.  */
#define CREATE 258
#define DROP 259
#define SELECT 260
#define INSERT 261
#define DELETE 262
#define UPDATE 263
#define TRXBEGIN 264
#define TRXCOMMIT 265
#define TRXROLLBACK 266
#define QUIT 267
#define EXECFILE 268
#define SHOW 269
#define USE 270
#define USING 271
#define DATABASE 272
#define DATABASES 273
#define TABLE 274
#define TABLES 275
#define INDEX 276
#define INDEXES 277
#define ON 278
#define FROM 279
#define WHERE 280
#define INTO 281
#define SET 282
#define VALUES 283
#define PRIMARY 284
#define KEY 285
#define UNIQUE 286
#define CHAR 287
#define INT 288
#define FLOAT 289
#define AND 290
#define OR 291
#define NOT 292
#define IS 293
#define FLAGNULL 294
#define IDENTIFIER 295
#define STRING 296
#define NUMBER 297
#define EQ 298
#define NE 299
#define LE 300
#define GE 301

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 10 "minisql.y"

	pSyntaxNode syntax_node;

#line 227 "./minisql_yacc.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MINISQL_YACC_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  53
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   106

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  77
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  134

#define YYUNDEFTOK  2
#define YYMAXUTOK   301


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      48,    49,    51,     2,    50,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    47,
      52,     2,    53,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    35,    35,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    64,    71,    78,    84,    91,    97,   107,   111,
     117,   121,   124,   131,   136,   144,   147,   150,   157,   164,
     172,   186,   193,   199,   204,   215,   218,   225,   230,   236,
     239,   245,   253,   256,   259,   265,   268,   271,   274,   277,
     280,   283,   286,   292,   302,   306,   312,   316,   326,   333,
     348,   352,   358,   366,   372,   378,   384,   390
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CREATE", "DROP", "SELECT", "INSERT",
  "DELETE", "UPDATE", "TRXBEGIN", "TRXCOMMIT", "TRXROLLBACK", "QUIT",
  "EXECFILE", "SHOW", "USE", "USING", "DATABASE", "DATABASES", "TABLE",
  "TABLES", "INDEX", "INDEXES", "ON", "FROM", "WHERE", "INTO", "SET",
  "VALUES", "PRIMARY", "KEY", "UNIQUE", "CHAR", "INT", "FLOAT", "AND",
  "OR", "NOT", "IS", "FLAGNULL", "IDENTIFIER", "STRING", "NUMBER", "EQ",
  "NE", "LE", "GE", "';'", "'('", "')'", "','", "'*'", "'<'", "'>'",
  "$accept", "start", "sql", "sql_create_database", "sql_drop_database",
  "sql_show_databases", "sql_use_database", "sql_show_tables",
  "sql_create_table", "column_list", "column_definition_list",
  "column_definition", "column_type", "sql_drop_table", "sql_create_index",
  "sql_drop_index", "sql_show_indexes", "sql_select", "select_columns",
  "where_conditions", "connector", "where_condition", "column_value",
  "operator", "sql_insert", "column_values", "sql_delete", "sql_update",
  "update_values", "update_value", "sql_trx_begin", "sql_trx_commit",
  "sql_trx_rollback", "sql_quit", "sql_exec_file", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,    59,    40,    41,
      44,    42,    60,    62
};
# endif

#define YYPACT_NINF (-85)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      32,     2,     3,   -36,   -19,     8,    -7,   -85,   -85,   -85,
     -85,    10,     7,    12,    53,    11,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,   -85,   -85,    14,    15,    17,    19,    20,
      21,    13,   -85,   -85,    38,    24,    25,    39,   -85,   -85,
     -85,   -85,   -85,   -85,   -85,   -85,    22,    44,   -85,   -85,
     -85,    28,    29,    43,    47,    33,   -24,    34,   -85,    50,
      30,    36,    37,    52,    31,    49,    16,    35,    40,    41,
      36,   -11,   -35,   -22,   -85,   -11,    36,    33,    45,    46,
     -85,   -85,    51,   -85,   -24,    28,   -22,   -85,   -85,   -85,
      42,    48,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -11,   -85,   -85,    36,   -85,   -22,   -85,    28,    54,   -85,
     -85,    55,   -11,   -85,   -85,   -85,    56,    57,    67,   -85,
     -85,   -85,    58,   -85
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    73,    74,    75,
      76,     0,     0,     0,     0,     0,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     0,     0,     0,     0,     0,
       0,    29,    45,    46,     0,     0,     0,     0,    77,    24,
      26,    42,    25,     1,     2,    22,     0,     0,    23,    38,
      41,     0,     0,     0,    66,     0,     0,     0,    28,    43,
       0,     0,     0,    68,    71,     0,     0,     0,    31,     0,
       0,     0,     0,    67,    48,     0,     0,     0,     0,     0,
      35,    36,    34,    27,     0,     0,    44,    54,    52,    53,
      65,     0,    62,    61,    55,    56,    57,    58,    59,    60,
       0,    49,    50,     0,    72,    69,    70,     0,     0,    33,
      30,     0,     0,    63,    51,    47,     0,     0,    39,    64,
      32,    37,     0,    40
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -61,
      -9,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -74,
     -85,   -27,   -84,   -85,   -85,   -34,   -85,   -85,     0,   -85,
     -85,   -85,   -85,   -85,   -85
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    43,
      77,    78,    92,    22,    23,    24,    25,    26,    44,    83,
     113,    84,   100,   110,    27,   101,    28,    29,    73,    74,
      30,    31,    32,    33,    34
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      68,   114,   102,   103,    41,    75,    96,    45,   104,   105,
     106,   107,   115,   111,   112,    42,    76,   108,   109,    35,
      38,    36,    39,    37,    40,    49,   124,    50,    97,    51,
      98,    99,    46,    47,   121,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    89,    90,
      91,    48,    52,    53,    55,    56,   126,    57,    54,    58,
      59,    60,    62,    61,    63,    64,    65,    67,    41,    69,
      66,    70,    71,    72,    79,    80,    82,    86,    81,    88,
      85,    87,   119,   132,    93,   120,   125,   116,   129,    95,
      94,     0,   122,   117,   118,     0,   127,   123,   133,     0,
       0,     0,     0,     0,   128,   130,   131
};

static const yytype_int8 yycheck[] =
{
      61,    85,    37,    38,    40,    29,    80,    26,    43,    44,
      45,    46,    86,    35,    36,    51,    40,    52,    53,    17,
      17,    19,    19,    21,    21,    18,   110,    20,    39,    22,
      41,    42,    24,    40,    95,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    32,    33,
      34,    41,    40,     0,    40,    40,   117,    40,    47,    40,
      40,    40,    24,    50,    40,    40,    27,    23,    40,    40,
      48,    28,    25,    40,    40,    25,    40,    25,    48,    30,
      43,    50,    31,    16,    49,    94,   113,    87,   122,    48,
      50,    -1,    50,    48,    48,    -1,    42,    49,    40,    -1,
      -1,    -1,    -1,    -1,    49,    49,    49
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    55,    56,    57,    58,    59,    60,
      61,    62,    67,    68,    69,    70,    71,    78,    80,    81,
      84,    85,    86,    87,    88,    17,    19,    21,    17,    19,
      21,    40,    51,    63,    72,    26,    24,    40,    41,    18,
      20,    22,    40,     0,    47,    40,    40,    40,    40,    40,
      40,    50,    24,    40,    40,    27,    48,    23,    63,    40,
      28,    25,    40,    82,    83,    29,    40,    64,    65,    40,
      25,    48,    40,    73,    75,    43,    25,    50,    30,    32,
      33,    34,    66,    49,    50,    48,    73,    39,    41,    42,
      76,    79,    37,    38,    43,    44,    45,    46,    52,    53,
      77,    35,    36,    74,    76,    73,    82,    48,    48,    31,
      64,    63,    50,    49,    76,    75,    63,    42,    49,    79,
      49,    49,    16,    40
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    54,    55,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    57,    58,    59,    60,    61,    62,    63,    63,
      64,    64,    64,    65,    65,    66,    66,    66,    67,    68,
      68,    69,    70,    71,    71,    72,    72,    73,    73,    74,
      74,    75,    76,    76,    76,    77,    77,    77,    77,    77,
      77,    77,    77,    78,    79,    79,    80,    80,    81,    81,
      82,    82,    83,    84,    85,    86,    87,    88
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     2,     2,     2,     6,     3,     1,
       3,     1,     5,     3,     2,     1,     1,     4,     3,     8,
      10,     3,     2,     4,     6,     1,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     7,     3,     1,     3,     5,     4,     6,
       3,     1,     3,     1,     1,     1,     1,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 35 "minisql.y"
          {
    (yyval.syntax_node) = (yyvsp[-1].syntax_node);
    MinisqlParserSetRoot((yyval.syntax_node));
  }
#line 1502 "./minisql_yacc.c"
    break;

  case 3:
#line 42 "minisql.y"
                      { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1508 "./minisql_yacc.c"
    break;

  case 4:
#line 43 "minisql.y"
                      { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1514 "./minisql_yacc.c"
    break;

  case 5:
#line 44 "minisql.y"
                       { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1520 "./minisql_yacc.c"
    break;

  case 6:
#line 45 "minisql.y"
                     { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1526 "./minisql_yacc.c"
    break;

  case 7:
#line 46 "minisql.y"
                    { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1532 "./minisql_yacc.c"
    break;

  case 8:
#line 47 "minisql.y"
                     { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1538 "./minisql_yacc.c"
    break;

  case 9:
#line 48 "minisql.y"
                   { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1544 "./minisql_yacc.c"
    break;

  case 10:
#line 49 "minisql.y"
                     { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1550 "./minisql_yacc.c"
    break;

  case 11:
#line 50 "minisql.y"
                   { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1556 "./minisql_yacc.c"
    break;

  case 12:
#line 51 "minisql.y"
                     { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1562 "./minisql_yacc.c"
    break;

  case 13:
#line 52 "minisql.y"
               { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1568 "./minisql_yacc.c"
    break;

  case 14:
#line 53 "minisql.y"
               { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1574 "./minisql_yacc.c"
    break;

  case 15:
#line 54 "minisql.y"
               { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1580 "./minisql_yacc.c"
    break;

  case 16:
#line 55 "minisql.y"
               { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1586 "./minisql_yacc.c"
    break;

  case 17:
#line 56 "minisql.y"
                  { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1592 "./minisql_yacc.c"
    break;

  case 18:
#line 57 "minisql.y"
                   { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1598 "./minisql_yacc.c"
    break;

  case 19:
#line 58 "minisql.y"
                     { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1604 "./minisql_yacc.c"
    break;

  case 20:
#line 59 "minisql.y"
             { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1610 "./minisql_yacc.c"
    break;

  case 21:
#line 60 "minisql.y"
                  { (yyval.syntax_node) = (yyvsp[0].syntax_node); }
#line 1616 "./minisql_yacc.c"
    break;

  case 22:
#line 64 "minisql.y"
                             {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCreateDB, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1625 "./minisql_yacc.c"
    break;

  case 23:
#line 71 "minisql.y"
                           {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeDropDB, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1634 "./minisql_yacc.c"
    break;

  case 24:
#line 78 "minisql.y"
                 {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeShowDB, NULL);
  }
#line 1642 "./minisql_yacc.c"
    break;

  case 25:
#line 84 "minisql.y"
                 {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeUseDB, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1651 "./minisql_yacc.c"
    break;

  case 26:
#line 91 "minisql.y"
              {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeShowTables, NULL);
  }
#line 1659 "./minisql_yacc.c"
    break;

  case 27:
#line 97 "minisql.y"
                                                         {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCreateTable, NULL);
    pSyntaxNode list_node = CreateSyntaxNode(kNodeColumnDefinitionList, NULL);
    SyntaxNodeAddChildren(list_node, (yyvsp[-1].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-3].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), list_node);
  }
#line 1671 "./minisql_yacc.c"
    break;

  case 28:
#line 107 "minisql.y"
                             {
    (yyval.syntax_node) = (yyvsp[-2].syntax_node);
    SyntaxNodeAddSibling((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1680 "./minisql_yacc.c"
    break;

  case 29:
#line 111 "minisql.y"
               {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 1688 "./minisql_yacc.c"
    break;

  case 30:
#line 117 "minisql.y"
                                               {
    (yyval.syntax_node) = (yyvsp[-2].syntax_node);
    SyntaxNodeAddSibling((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1697 "./minisql_yacc.c"
    break;

  case 31:
#line 121 "minisql.y"
                      {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 1705 "./minisql_yacc.c"
    break;

  case 32:
#line 124 "minisql.y"
                                    {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnList, "primary keys");
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-1].syntax_node));
  }
#line 1714 "./minisql_yacc.c"
    break;

  case 33:
#line 131 "minisql.y"
                                {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnDefinition, "unique");
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-1].syntax_node));
  }
#line 1724 "./minisql_yacc.c"
    break;

  case 34:
#line 136 "minisql.y"
                           {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnDefinition, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-1].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1734 "./minisql_yacc.c"
    break;

  case 35:
#line 144 "minisql.y"
      {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnType, "int");
  }
#line 1742 "./minisql_yacc.c"
    break;

  case 36:
#line 147 "minisql.y"
          {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnType, "float");
  }
#line 1750 "./minisql_yacc.c"
    break;

  case 37:
#line 150 "minisql.y"
                        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnType, "char");
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-1].syntax_node));
  }
#line 1759 "./minisql_yacc.c"
    break;

  case 38:
#line 157 "minisql.y"
                        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeDropTable, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1768 "./minisql_yacc.c"
    break;

  case 39:
#line 164 "minisql.y"
                                                            {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCreateIndex, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-5].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-3].syntax_node));
    pSyntaxNode index_keys_node = CreateSyntaxNode(kNodeColumnList, "index keys");
    SyntaxNodeAddChildren(index_keys_node, (yyvsp[-1].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), index_keys_node);
  }
#line 1781 "./minisql_yacc.c"
    break;

  case 40:
#line 172 "minisql.y"
                                                                               {
      (yyval.syntax_node) = CreateSyntaxNode(kNodeCreateIndex, NULL);
      SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-7].syntax_node));
      SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-5].syntax_node));
      pSyntaxNode index_keys_node = CreateSyntaxNode(kNodeColumnList, "index keys");
      SyntaxNodeAddChildren(index_keys_node, (yyvsp[-3].syntax_node));
      SyntaxNodeAddChildren((yyval.syntax_node), index_keys_node);
      pSyntaxNode index_type_node = CreateSyntaxNode(kNodeIndexType, "index type");
      SyntaxNodeAddChildren(index_type_node, (yyvsp[0].syntax_node));
      SyntaxNodeAddChildren((yyval.syntax_node), index_type_node);
  }
#line 1797 "./minisql_yacc.c"
    break;

  case 41:
#line 186 "minisql.y"
                        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeDropIndex, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1806 "./minisql_yacc.c"
    break;

  case 42:
#line 193 "minisql.y"
               {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeShowIndexes, NULL);
  }
#line 1814 "./minisql_yacc.c"
    break;

  case 43:
#line 199 "minisql.y"
                                        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeSelect, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1824 "./minisql_yacc.c"
    break;

  case 44:
#line 204 "minisql.y"
                                                                 {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeSelect, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-4].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    pSyntaxNode condition_node = CreateSyntaxNode(kNodeConditions, NULL);
    SyntaxNodeAddChildren(condition_node, (yyvsp[0].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), condition_node);
  }
#line 1837 "./minisql_yacc.c"
    break;

  case 45:
#line 215 "minisql.y"
      {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeAllColumns, NULL);
  }
#line 1845 "./minisql_yacc.c"
    break;

  case 46:
#line 218 "minisql.y"
                {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeColumnList, "select columns");
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1854 "./minisql_yacc.c"
    break;

  case 47:
#line 225 "minisql.y"
                                              {
    (yyval.syntax_node) = (yyvsp[-1].syntax_node);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1864 "./minisql_yacc.c"
    break;

  case 48:
#line 230 "minisql.y"
                    {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 1872 "./minisql_yacc.c"
    break;

  case 49:
#line 236 "minisql.y"
      {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeConnector, "and");
  }
#line 1880 "./minisql_yacc.c"
    break;

  case 50:
#line 239 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeConnector, "or");
  }
#line 1888 "./minisql_yacc.c"
    break;

  case 51:
#line 245 "minisql.y"
                                   {
    (yyval.syntax_node) = (yyvsp[-1].syntax_node);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 1898 "./minisql_yacc.c"
    break;

  case 52:
#line 253 "minisql.y"
         {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 1906 "./minisql_yacc.c"
    break;

  case 53:
#line 256 "minisql.y"
           {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 1914 "./minisql_yacc.c"
    break;

  case 54:
#line 259 "minisql.y"
             {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeNull, NULL);
  }
#line 1922 "./minisql_yacc.c"
    break;

  case 55:
#line 265 "minisql.y"
     {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "=");
  }
#line 1930 "./minisql_yacc.c"
    break;

  case 56:
#line 268 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "<>");
  }
#line 1938 "./minisql_yacc.c"
    break;

  case 57:
#line 271 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "<=");
  }
#line 1946 "./minisql_yacc.c"
    break;

  case 58:
#line 274 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, ">=");
  }
#line 1954 "./minisql_yacc.c"
    break;

  case 59:
#line 277 "minisql.y"
        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "<");
  }
#line 1962 "./minisql_yacc.c"
    break;

  case 60:
#line 280 "minisql.y"
        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, ">");
  }
#line 1970 "./minisql_yacc.c"
    break;

  case 61:
#line 283 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "is");
  }
#line 1978 "./minisql_yacc.c"
    break;

  case 62:
#line 286 "minisql.y"
        {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeCompareOperator, "not");
  }
#line 1986 "./minisql_yacc.c"
    break;

  case 63:
#line 292 "minisql.y"
                                                      {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeInsert, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-4].syntax_node));
    pSyntaxNode col_val_node = CreateSyntaxNode(kNodeColumnValues, NULL);
    SyntaxNodeAddChildren(col_val_node, (yyvsp[-1].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), col_val_node);
  }
#line 1998 "./minisql_yacc.c"
    break;

  case 64:
#line 302 "minisql.y"
                                 {
    (yyval.syntax_node) = (yyvsp[-2].syntax_node);
    SyntaxNodeAddSibling((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 2007 "./minisql_yacc.c"
    break;

  case 65:
#line 306 "minisql.y"
                 {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 2015 "./minisql_yacc.c"
    break;

  case 66:
#line 312 "minisql.y"
                         {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeDelete, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 2024 "./minisql_yacc.c"
    break;

  case 67:
#line 316 "minisql.y"
                                                  {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeDelete, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    pSyntaxNode condition_node = CreateSyntaxNode(kNodeConditions, NULL);
    SyntaxNodeAddChildren(condition_node, (yyvsp[0].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), condition_node);
  }
#line 2036 "./minisql_yacc.c"
    break;

  case 68:
#line 326 "minisql.y"
                                      {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeUpdate, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    pSyntaxNode upd_values_node = CreateSyntaxNode(kNodeUpdateValues, NULL);
    SyntaxNodeAddChildren(upd_values_node, (yyvsp[0].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), upd_values_node);
  }
#line 2048 "./minisql_yacc.c"
    break;

  case 69:
#line 333 "minisql.y"
                                                               {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeUpdate, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-4].syntax_node));
    // update values
    pSyntaxNode upd_values_node = CreateSyntaxNode(kNodeUpdateValues, NULL);
    SyntaxNodeAddChildren(upd_values_node, (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), upd_values_node);
    // where conditions
    pSyntaxNode condition_node = CreateSyntaxNode(kNodeConditions, NULL);
    SyntaxNodeAddChildren(condition_node, (yyvsp[0].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), condition_node);
  }
#line 2065 "./minisql_yacc.c"
    break;

  case 70:
#line 348 "minisql.y"
                                 {
    (yyval.syntax_node) = (yyvsp[-2].syntax_node);
    SyntaxNodeAddSibling((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 2074 "./minisql_yacc.c"
    break;

  case 71:
#line 352 "minisql.y"
                 {
    (yyval.syntax_node) = (yyvsp[0].syntax_node);
  }
#line 2082 "./minisql_yacc.c"
    break;

  case 72:
#line 358 "minisql.y"
                             {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeUpdateValue, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[-2].syntax_node));
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 2092 "./minisql_yacc.c"
    break;

  case 73:
#line 366 "minisql.y"
           {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeTrxBegin, NULL);
  }
#line 2100 "./minisql_yacc.c"
    break;

  case 74:
#line 372 "minisql.y"
            {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeTrxCommit, NULL);
  }
#line 2108 "./minisql_yacc.c"
    break;

  case 75:
#line 378 "minisql.y"
              {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeTrxRollback, NULL);
  }
#line 2116 "./minisql_yacc.c"
    break;

  case 76:
#line 384 "minisql.y"
       {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeQuit, NULL);
  }
#line 2124 "./minisql_yacc.c"
    break;

  case 77:
#line 390 "minisql.y"
                  {
    (yyval.syntax_node) = CreateSyntaxNode(kNodeExecFile, NULL);
    SyntaxNodeAddChildren((yyval.syntax_node), (yyvsp[0].syntax_node));
  }
#line 2133 "./minisql_yacc.c"
    break;


#line 2137 "./minisql_yacc.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 396 "minisql.y"

int yyerror(char* error) {
	MinisqlParserSetError(error);
	return 0;
}
