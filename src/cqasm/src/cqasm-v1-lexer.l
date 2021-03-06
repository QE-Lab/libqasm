%option noinput nounput noyywrap 8bit nodefault
%option prefix="cqasm_v1"
%option yylineno
%option reentrant bison-bridge bison-locations

%{
    /**
     * \file
     * Lexer/tokenizer, generated by Flex.
     */
    #include "cqasm-v1-parser.hpp"

    //#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
    #define DEBUG(...)

    #define YY_USER_ACTION yylloc->last_column += yyleng;
    #define LOCATION_RESET \
        yylloc->first_column = 1; yylloc->first_line = 1; \
        yylloc->last_column = 1; yylloc->last_line = 1;
    #define LOCATION_LINE \
        yylloc->last_column = 1; yylloc->last_line += 1;
    #define LOCATION_OVERRIDE_LINE \
        yylloc->last_column = 1; yylloc->last_line = std::strtol(yylval->str + 12, nullptr, 10);
    #define LOCATION_STEP \
        yylloc->first_column = yylloc->last_column; \
        yylloc->first_line = yylloc->last_line;
    #define WITH_STR(TOKNAME) \
        DEBUG("Pushing %s token (%s) starting at %d:%d\n", #TOKNAME, yytext, yylloc->first_line, yylloc->first_column); \
        yylval->str = strdup(yytext); return TOKNAME
    #define WITHOUT_STR(TOKNAME) \
        DEBUG("Pushing %s token starting at %d:%d\n", #TOKNAME, yylloc->first_line, yylloc->first_column); \
        return TOKNAME
    #define OPERATOR \
        DEBUG("Pushing '%c' token starting at %d:%d\n", yytext[0], yylloc->first_line, yylloc->first_column); \
        return yytext[0]
    #define DELETE LOCATION_STEP
%}

%x MODE_MULTILINE_COMMENT
%x MODE_VERSION
%x MODE_STRING
%x MODE_JSON
%x MODE_JSON_STRING
%x MODE_UNEXPECTED_EOF

%%

%{
    LOCATION_STEP;
%}

<INITIAL>{


    /*=======================================================================*/
    /* WHITESPACE MANAGEMENT                                                 */
    /*=======================================================================*/

    /* Newline and single-line comment */
(\n|\r\n?)                                          LOCATION_LINE; WITHOUT_STR(NEWLINE);

    /* Semicolons function exactly the same way as newlines */
;                                                   WITHOUT_STR(NEWLINE);

    /* And then there's the opposite, the escaped newline */
\\(\n|\r\n?)                                        LOCATION_LINE; DELETE;

    /* We can also do a newline or an escaped newline that sets the line */
    /* number for the next line. That's useful primarily for generated */
    /* code, to make line numbers more sensible for the user. */
(@@NEXT_LINE=[0-9]+\n|\r\n?)                        LOCATION_OVERRIDE_LINE; WITHOUT_STR(NEWLINE);
(@@NEXT_LINE=[0-9]+\\\n|\r\n?)                      LOCATION_OVERRIDE_LINE; DELETE;


    /* Gobble up comments */
#.*                                                 DELETE;
"/*"                                                BEGIN(MODE_MULTILINE_COMMENT); DELETE;
}
<MODE_MULTILINE_COMMENT>{
"*/"                                                BEGIN(INITIAL); DELETE;
(\n|\r\n?)                                          LOCATION_LINE; DELETE;
.                                                   DELETE;
<<EOF>>                                             BEGIN(MODE_UNEXPECTED_EOF); WITHOUT_STR(END_OF_FILE);
}
<INITIAL>{

    /* Gobble up whitespace */
[ \t]+                                              DELETE;


    /*=======================================================================*/
    /* VERSION STATEMENT                                                     */
    /*=======================================================================*/

    /* This is treated separately to avoid conflicts with float literals */
(?i:version)                                        BEGIN(MODE_VERSION); WITHOUT_STR(VERSION);
}
<MODE_VERSION>{
(\n|\r\n?)                                          LOCATION_LINE; BEGIN(INITIAL); WITHOUT_STR(NEWLINE);
;                                                   BEGIN(INITIAL); WITHOUT_STR(NEWLINE);
[ \t]+                                              DELETE;
[0-9]+                                              WITH_STR(INT_LITERAL);
\.                                                  OPERATOR;
.                                                   WITHOUT_STR(BAD_CHARACTER);
}
<INITIAL>{


    /*=======================================================================*/
    /* KEYWORDS                                                              */
    /*=======================================================================*/

    /* Qubit allocation */
(?i:qubits)                                         WITHOUT_STR(QUBITS);

    /* Qubit register naming */
(?i:map)                                            WITHOUT_STR(MAP);

    /* Variable declaration */
(?i:var)                                            WITHOUT_STR(VAR);

    /* C-dash prefix for binary-controlled gates */
[cC]-/[a-zA-Z_]                                     WITHOUT_STR(CDASH);

    /* New-style binary-controlled gate syntax */
(?i:cond)                                           WITHOUT_STR(COND);

    /* cQASM 1.2 statements */
(?i:if)                                             WITHOUT_STR(IF);
(?i:else)                                           WITHOUT_STR(ELSE);
(?i:for)                                            WITHOUT_STR(FOR);
(?i:foreach)                                        WITHOUT_STR(FOREACH);
(?i:while)                                          WITHOUT_STR(WHILE);
(?i:repeat)                                         WITHOUT_STR(REPEAT);
(?i:until)                                          WITHOUT_STR(UNTIL);
(?i:continue)                                       WITHOUT_STR(CONTINUE);
(?i:break)                                          WITHOUT_STR(BREAK);

    /* cQASM 1.2 special instructions */
(?i:set)                                            WITHOUT_STR(SET);

    /* Absolute nonsense for compatibility purposes: the reset-averaging */
    /* operation has a - in it, which is not legal in an identifier for */
    /* obvious reasons when expressions come along. */
(?i:reset-averaging)                                WITH_STR(IDENTIFIER);


    /*=======================================================================*/
    /* NUMERIC LITERALS                                                      */
    /*=======================================================================*/

    /* Integer literals */
[0-9]+                                              WITH_STR(INT_LITERAL);

    /* Float literals */
[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?                    WITH_STR(FLOAT_LITERAL);

    /* Any digit followed by a letter not detected by the above is an invalid number */
[0-9][a-zA-Z0-9_]                                   WITHOUT_STR(BAD_NUMBER);


    /*=======================================================================*/
    /* STRING LITERALS                                                       */
    /*=======================================================================*/

["]                                                 BEGIN(MODE_STRING); WITHOUT_STR(STRING_OPEN);
}
<MODE_STRING>{
["]                                                 BEGIN(INITIAL); WITHOUT_STR(STRING_CLOSE);
[\\][tn'"\\]                                        WITH_STR(STRBUILD_ESCAPE);
[\\](\n|\r\n?)                                      LOCATION_LINE; DELETE;
(\n|\r\n?)                                          LOCATION_LINE; WITH_STR(STRBUILD_APPEND);
.                                                   WITH_STR(STRBUILD_APPEND);
<<EOF>>                                             BEGIN(MODE_UNEXPECTED_EOF); WITHOUT_STR(END_OF_FILE);
}
<INITIAL>{


    /*=======================================================================*/
    /* JSON LITERALS                                                         */
    /*=======================================================================*/

"{|"                                                BEGIN(MODE_JSON); WITHOUT_STR(JSON_OPEN);
}
<MODE_JSON>{
"|}"                                                BEGIN(INITIAL); WITHOUT_STR(JSON_CLOSE);
["]                                                 BEGIN(MODE_JSON_STRING); WITH_STR(STRBUILD_APPEND);
(\n|\r\n?)                                          LOCATION_LINE; WITH_STR(STRBUILD_APPEND);
.                                                   WITH_STR(STRBUILD_APPEND);
<<EOF>>                                             BEGIN(MODE_UNEXPECTED_EOF); WITHOUT_STR(END_OF_FILE);
}
<MODE_JSON_STRING>{
["]                                                 BEGIN(MODE_JSON); WITH_STR(STRBUILD_APPEND);
[\\].                                               WITH_STR(STRBUILD_APPEND);
(\n|\r\n?)                                          LOCATION_LINE; WITH_STR(STRBUILD_APPEND);
.                                                   WITH_STR(STRBUILD_APPEND);
<<EOF>>                                             BEGIN(MODE_UNEXPECTED_EOF); WITHOUT_STR(END_OF_FILE);
}
<INITIAL>{


    /*=======================================================================*/
    /* IDENTIFIERS                                                           */
    /*=======================================================================*/

[a-zA-Z_][a-zA-Z0-9_]*                              WITH_STR(IDENTIFIER);


    /*=======================================================================*/
    /* OPERATORS                                                             */
    /*=======================================================================*/

[()[\]{}|+\-*/=:,.@!~<>&|^?%]                       OPERATOR;
"**"                                                WITHOUT_STR(POWER);
"//"                                                WITHOUT_STR(INT_DIV);
"||"                                                WITHOUT_STR(LOGIC_OR);
"&&"                                                WITHOUT_STR(LOGIC_AND);
"^^"                                                WITHOUT_STR(LOGIC_XOR);
">="                                                WITHOUT_STR(CMP_GE);
"<="                                                WITHOUT_STR(CMP_LE);
"=="                                                WITHOUT_STR(CMP_EQ);
"!="                                                WITHOUT_STR(CMP_NE);
"<<"                                                WITHOUT_STR(SHL);
">>"                                                WITHOUT_STR(ARITH_SHR);
">>>"                                               WITHOUT_STR(LOGIC_SHR);
".."                                                WITHOUT_STR(ELLIPSIS);


    /*=======================================================================*/
    /* DEFAULT                                                               */
    /*=======================================================================*/

    /* Catch any other character as a single-character lexeme */
.                                                   WITHOUT_STR(BAD_CHARACTER);


}
<MODE_UNEXPECTED_EOF>{
(\n|\r\n?)                                          LOCATION_LINE; DELETE;
.                                                   DELETE;
}
