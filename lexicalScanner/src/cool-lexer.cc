/*
 *  The lexical analyzer for COOL.
 *  Author: Ziang Wan
 *  Date: 09/11/2018
 *
 *  12-hour challenge. FUCK!!!!
 *  MY CODE FOLLOWS MY STYLE.
 */

#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>
#include <strings.h>
#include <ctype.h>      // isalpha(), isdigit()
#include <stdio.h>

// Max size of string constants
#define MAX_STR_CONST 1025

// white space value def
#define CHAR_BLANK          32
#define CHAR_NEWLINE        10
#define CHAR_FORMFEED       12
#define CHAR_CARRAIGERET    13
#define CHAR_TAB            9
#define CHAR_VERTICALTAB    11
#define CHAR_BACKSPACE      8


// flag, unused
extern int verbose_flag;
int yy_flex_debug;


// the current line number
extern int curr_lineno;
// the struct to store semantic value
extern YYSTYPE cool_yylval;


// the stringtables
extern IdTable idtable;
extern IntTable inttable;
extern StrTable stringtable;

/* File IO */

extern FILE *fin; 
char next;
bool nread;


// returns the next character without moving the file pointer
char look_next(){
    if(!nread){
        next = fgetc(fin);
        nread = true;
    }
    return next;
}


// returns the next character and moves the file pointer ahead
char get_next(){
    char cur = look_next();
    if(cur != EOF){
        next = fgetc(fin);
    }else{
        nread = false;
    }
    return cur;
}


/* Character Checks */


bool char_is_white_space(char a){
    switch(a){
        case CHAR_BLANK:
        case CHAR_NEWLINE:
        case CHAR_FORMFEED:
        case CHAR_CARRAIGERET:
        case CHAR_TAB:
        case CHAR_VERTICALTAB:
            return true;
        default:
            return false;
    }
}


bool char_is_valid_identifier(char a){
    if( isalpha(a) || isdigit(a) || a == '_'){
        return true;
    }else{
        return false;
    }
}


/* Manual Lexical Scanning - White Space and EOF */


// unfortunate hack - for unescaped char in string
int before_scan_line_num_additon = 0;

void lex_scan_skip_white_space(){
    // unfortunate hack
    curr_lineno += before_scan_line_num_additon;
    before_scan_line_num_additon = 0;

    while( char_is_white_space(look_next()) ){
        // check line number
        char this_char = get_next();
        if(this_char == CHAR_NEWLINE){
            curr_lineno += 1;
        }
    }
}


int lex_scan_end_of_file(){
    char next_char = look_next();
    if(next_char == EOF){
        get_next();
        return true;
    }else{
        return false;
    }
}


/* Scanning - Integer */


int lex_scan_integer(){
    // read in the next integer, start from a digit and end with a digit
    char cache[MAX_STR_CONST];
    int cache_pos = 0;

    while( isdigit(look_next()) ){
        cache[cache_pos] = get_next();
        cache_pos += 1;
    }
    cache[cache_pos] = '\0';

    // store the value as a string in cool_yylval
    cool_yylval.symbol = stringtable.add_string(cache);
    return INT_CONST;
}


/* Scanning - Identifier and Keyword */


bool lex_scan_check_bool_const(char *s){    
    // the length of string s is gauranteed to be greater than 1
    // convert s to lower case except for the first one
    char *to_lower_ptr = s + 1;
    while( *to_lower_ptr ){
        *to_lower_ptr = tolower( *to_lower_ptr );
        to_lower_ptr += 1;
    }

    if( strcmp(s, "true") == 0 ){
        cool_yylval.boolean = true;
        return true;
    }else if( strcmp(s, "false") == 0 ){
        cool_yylval.boolean = false;
        return true;
    }else{
        // not a bool const
        return false;
    }
}


int lex_scan_check_keyword(char *s){
    // only need to conver the first letter to lower case
    s[0] = tolower(s[0]);

    // unfortunate, naive strcmps, should use a map here
    if( strcmp(s, "class") == 0 ){
        return CLASS;
    }else if( strcmp(s, "else") == 0 ){
        return ELSE;
    }else if( strcmp(s, "fi") == 0 ){
        return FI;
    }else if( strcmp(s, "if") == 0 ){
        return IF;
    }else if( strcmp(s, "in") == 0 ){
        return IN;
    }else if( strcmp(s, "inherits") == 0 ){
        return INHERITS;
    }else if( strcmp(s, "isvoid") == 0 ){
        return ISVOID;
    }else if( strcmp(s, "let") == 0 ){
        return LET;
    }else if( strcmp(s, "loop") == 0 ){
        return LOOP;
    }else if( strcmp(s, "pool") == 0 ){
        return POOL;
    }else if( strcmp(s, "then") == 0 ){
        return THEN;
    }else if( strcmp(s, "while") == 0 ){
        return WHILE;
    }else if( strcmp(s, "case") == 0 ){
        return CASE;
    }else if( strcmp(s, "esac") == 0 ){
        return ESAC;
    }else if( strcmp(s, "new") == 0 ){
        return NEW;
    }else if( strcmp(s, "of") == 0 ){
        return OF;
    }else if( strcmp(s, "not") == 0 ){
        return NOT;
    }else{
        return -1;
    }
}


int lex_scan_identifier_and_keyword(){
    // read in the next identifier, start with valid and end with invalid
    char cache[MAX_STR_CONST];
    int cache_pos = 0;

    while( char_is_valid_identifier(look_next()) ){
        cache[cache_pos] = get_next();
        cache_pos += 1;
    }
    cache[cache_pos] = '\0';

    // make a copy for keyword checks
    char *copy_cache = new char[strlen(cache) + 1];
    strcpy(copy_cache, cache);

    bool possible_bool_const = lex_scan_check_bool_const(copy_cache);
    if( possible_bool_const ){
        delete[] copy_cache;
        return BOOL_CONST;
    }

    int possible_keyword = lex_scan_check_keyword(copy_cache);
    if( possible_keyword > 0 ){
        delete[] copy_cache;
        return possible_keyword;
    }

    delete[] copy_cache;

    // it is an identifier, store the value as a string in cool_yylval
    cool_yylval.symbol = stringtable.add_string(cache);
    return isupper(cache[0]) ? TYPEID : OBJECTID;
}


/* Scanning - Symbols, strings, comments and unknown token error */


bool lexical_scan_single_line_comment(){
    // skip this line till the end
    char this_char = get_next();
    while( this_char != CHAR_NEWLINE && this_char != EOF ){
        this_char = get_next();
    }
    
    // newline character is consumed
    curr_lineno += 1;

    // succeed
    return true;
}


bool lexical_scan_multi_line_comment(){
    char this_char = get_next();
    while( this_char != '*' || look_next() != ')' ){
        if( this_char == CHAR_NEWLINE ){
            curr_lineno += 1;
        }else if( this_char == '(' && look_next() == '*' ){
            // nested comments
            get_next();
            int subcomment_complete = lexical_scan_multi_line_comment();
            if( !subcomment_complete ){
                return false;
            }
        }else if( this_char == EOF ){
            // failure
            cool_yylval.error_msg = "EOF in comment";
            return false;
        }
        this_char = get_next();
    }
    // consume the ')'
    get_next();

    // succeed
    return true;
}


int lexical_scan_string(){
    // the string cache
    char cache[MAX_STR_CONST];
    int cache_pos = 0;

    // report the first error, with unescaped newline overwriting precedence
    bool error_so_far = false;

    char this_char = get_next();
    while( this_char != '"' ){
        if(this_char == '\\'){
            // escape
            char escaped_char = get_next();
            switch(escaped_char){
                case 'b':
                    cache[cache_pos] = CHAR_BACKSPACE;
                    cache_pos += 1;
                    break;
                case 't':
                    cache[cache_pos] = CHAR_TAB;
                    cache_pos += 1;
                    break;
                case 'n':
                    cache[cache_pos] = CHAR_NEWLINE;
                    cache_pos += 1;
                    break;
                case 'f':
                    cache[cache_pos] = CHAR_FORMFEED;
                    cache_pos += 1;
                    break;
                case '\n':
                    cache[cache_pos] = escaped_char;
                    cache_pos += 1;
                    curr_lineno += 1;
                    break;
                default:
                    cache[cache_pos] = escaped_char;
                    cache_pos += 1; 
            }
        }else if(this_char == CHAR_NEWLINE){
            // if non-escaped new line character, immediate termination and report
            cool_yylval.error_msg = "Unterminated string constant";
            // unfortunate hack to postpone incrementing curr_line by 1
            before_scan_line_num_additon = 1;
            return ERROR;
        }else if(this_char == '\0'){
            // null character
            if( !error_so_far ){
                cool_yylval.error_msg = "Null in a string";
                error_so_far = true;
            }
        }else if(this_char == EOF){
            // eof
            if( !error_so_far ){
                cool_yylval.error_msg = "EOF in string constant";
                error_so_far = true;
            }
            return ERROR;
        }else{
            // regular char
            cache[cache_pos] = this_char;
            cache_pos += 1; 
        }

        // cache overflow
        if(cache_pos >= MAX_STR_CONST){
            if( !error_so_far ){
                cool_yylval.error_msg = "String constant too long";
                error_so_far = true;
            }
            // reuse this invalid cache
            cache_pos = 0;
        }

        // keep moving
        this_char = get_next();
    }

    if( error_so_far ){
        return ERROR;
    }else{
        // string scanning succeed
        cache[cache_pos] = '\0';
        cool_yylval.symbol = stringtable.add_string(cache);
        return STR_CONST;
    }
}


// for continuation
int cool_yylex();

int lex_scan_symbol_string_comment(){
    char next_char = get_next();
    char next_next_char = look_next();

    switch(next_char){
        // simple cases
        case '+':
        case '/':
        case '.':
        case '~':
        case ',':
        case ';':
        case ':':
        case ')':
        case '@':
        case '{':
        case '}':
            return next_char;

        // complicated cases
        case '*':
            if(next_next_char == ')'){
                get_next();
                cool_yylval.error_msg = "Unmatched *)";
                return ERROR;
            }else{
                return next_char;
            }
        case '-':
            if(next_next_char == '-'){
                get_next();
                bool comment_complete = lexical_scan_single_line_comment();
                if( !comment_complete ){
                    return ERROR;
                }else{
                    // do a new scan
                    return cool_yylex();
                }
            }else{
                return next_char;
            }
        case '=':
            if(next_next_char == '>'){
                get_next();
                return DARROW;
            }else{
                return next_char;
            }
        case '<':
            if(next_next_char == '-'){
                get_next();
                return ASSIGN;
            }else if(next_next_char == '='){
                get_next();
                return LE;
            }else{
                return next_char;
            }
        case '(':
            if(next_next_char == '*'){
                get_next();
                bool comment_complete = lexical_scan_multi_line_comment();
                if( !comment_complete ){
                    return ERROR;
                }else{
                    // do a new scan
                    return cool_yylex();
                }
            }else{
                return next_char;
            }
        case '"':
            return lexical_scan_string();

        // unknown tokens
        default:
            char symbol_str[2];
            symbol_str[0] = next_char;
            symbol_str[1] = '\0';
            cool_yylval.error_msg = stringtable.add_string(symbol_str)->get_string();
            return ERROR;
    }
}


/* main */


// Returns the next token
int cool_yylex(){

    lex_scan_skip_white_space();

    if( lex_scan_end_of_file() ){
        return 0;
    }

    // dispatch based on the next symbol
    char next_char = look_next();

    if( isdigit(next_char) ){
        return lex_scan_integer();
    }else if( isalpha(next_char) ){
        return lex_scan_identifier_and_keyword();
    }else{
        return lex_scan_symbol_string_comment();
    }

}
