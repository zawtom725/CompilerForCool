/* 
Date: 09/24/2018
Author: Ziang Wan

MY CODE FOLLOWS MY STYLE.
14 HR PARSER CHALLENGE. AGAIN!!!!! FUCK!!!!.
*/

#include "cool-parse.h"
#include "cool-tree.h"
#include "stringtab.h"
#include "utilities.h"
#include <iostream>


// usefull macros
#define CHECK_NEXT_IS(s) if(lookNextToken() == s){consumeNextToken()}else{return handle_error();}


/************************************************************************/
/* Definitions and declarations */

extern char *curr_filename;

void yyerror(); /*  defined below; called for each parse error */
void printerr(const char *s); /*  called for errors in addition to parse errors */

extern int VERBOSE_ERRORS;
int cool_yydebug;

Program ast_root;      /* the result of the parse  */
Classes parse_results; /* for use in semantic analysis */
int omerrs = 0;        /* number of errors in lexing and parsing */
bool errorstate = false;

int next_token;
bool isfirst = true;

extern YYSTYPE cool_yylval;
extern int curr_lineno;

int lookNextToken();     /*  returns the next token */
void consumeNextToken(); /*  consumes the next token */

extern int cool_yylex();

// tables
extern IdTable idtable;
extern IntTable inttable;
extern StrTable stringtable;


/************************************************************************/
/* Implementations of utility functions */

void yyerror(){
	cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " << "syntax error at or near ";
	print_cool_token(lookNextToken());
	cerr << endl;
	omerrs++;

	if (omerrs > 20) {
		fprintf(stdout, "More than 20 errors\n");
		exit(1);
	}
}

void printerr(const char *s){
	if (VERBOSE_ERRORS){
		cerr << "line " << curr_lineno << s << endl;
	}
}

int lookNextToken(){
	if (isfirst){
		consumeNextToken();
		isfirst = false;
	}
	return next_token;
}

void consumeNextToken() {
	next_token = cool_yylex();
}


/************************************************************************/
/* Declarations of recursive descent parser functions */

// ADD YOUR DECLARATIONS HERE
YYSTYPE cool_program();
YYSTYPE cool_class();

// features & formals parsing
YYSTYPE cool_features();
YYSTYPE cool_feature();
YYSTYPE cool_formals();

// expressions
YYSTYPE cool_expression();
// predecence encoding, from highest to lowest, right-associative to avoid left recursion
// Actually, I treat the assign statement as a single factor to be detected at last
YYSTYPE cool_expression_p0(); // single factor, control flow and assign
YYSTYPE cool_expression_p1(); // . and @, because they appear together
YYSTYPE cool_expression_p2(); // ~
YYSTYPE cool_expression_p3(); // isvoid
YYSTYPE cool_expression_p4(); // *, /
YYSTYPE cool_expression_p5(); // +, -
YYSTYPE cool_expression_p6(); // <=, <, =
YYSTYPE cool_expression_p7(); // not
// special case let, if encountered, read all the way til the end.
YYSTYPE cool_expression_let();
// auxiliary
YYSTYPE cool_expressions_parenthesis();

// SIMPLE FUNCTION - You will need to modify this (or remove it) to handle


/**************************************************************************/
/* Error Handling and Reporting Unit */

// errors robustly
YYSTYPE handle_error();

/* You need to modify this function! */
YYSTYPE handle_error(){
	yyerror();
	errorstate = true;
	YYSTYPE retval;
	retval.error_msg = "Parser error";
	return retval;
}


/************************************************************************/
/* The main parse function */


int cool_yyparse(){
	YYSTYPE pr = cool_program();
	if (!errorstate){
		ast_root = pr.program;
		return 0;
	}else{
		return 1;
	}
}


/* program & class parsing: Given */


YYSTYPE cool_program() {
	/* Elements for program in AST */
	YYSTYPE retval;
	Classes classes = nil_Classes();

	/* program := [class;]+ */
	do {
		YYSTYPE cl = cool_class();
		if (!errorstate) {
			classes = append_Classes(classes, single_Classes(cl.class_));
		} else {
			return cl;
		}

		if (lookNextToken() == ';') {
			consumeNextToken();
		} else {
			return handle_error();
		}
	} while (lookNextToken() != 0 && lookNextToken() != ERROR);

	// a program is a list of classes
	parse_results = classes;
	retval.program = program(classes);
	return retval;
}


YYSTYPE cool_class() {
	/* Elements for class in AST */
	YYSTYPE retval;
	Symbol name;
	Symbol parent;
	Features features;
	Symbol filename;

	/* class := CLASS TYPEID [INHERITS TYPEID] { [features;]* } */
	if (lookNextToken() == CLASS) {
		consumeNextToken();
	} else {
		return handle_error();
	}

	if (lookNextToken() == TYPEID) {
		name = cool_yylval.symbol;
		consumeNextToken();
	} else {
		return handle_error();
	}

	// check inheritance	
	if (lookNextToken() == INHERITS) {
		// has inheritance
		consumeNextToken();
		if (lookNextToken() == TYPEID) {
			parent = cool_yylval.symbol;
			consumeNextToken();
		} else {
			return handle_error();
		}
	} else {
		// no inheritance, automatically inherit Object
		parent = idtable.add_string("Object");
	}

	// parse features
	if (lookNextToken() == '{') {
		consumeNextToken();
	} else {
		return handle_error();
	}

	YYSTYPE ft = cool_features();
	if (!errorstate){
		features = ft.features;
	} else {
		return ft;
	}

	if (lookNextToken() == '}') {
		consumeNextToken();
	} else {
		return handle_error();
	}

	// get the file name
	filename = stringtable.add_string(curr_filename);
	retval.class_ = class_(name, parent, features, filename);
	return retval;
}


/* feature parsing */


YYSTYPE cool_features() {
	/* Function to parse multiple features in a row */
	Features features = nil_Features();

	/* features := (feature;)* */
	while(lookNextToken() != '}'){
		YYSTYPE ft = cool_feature();
		if (!errorstate){
			Features parsed_feature = single_Features(ft.feature);
			features = append_Features(features, parsed_feature);
		}else{
			return ft;
		}
		// feature has to end with a ;
		if (lookNextToken() == ';') {
			consumeNextToken();
		} else {
			return handle_error();
		}
	}

	// return the list of features
	YYSTYPE retval;
	retval.features = features;
	return retval;
}



YYSTYPE cool_feature(){
	/* function to parse a single feature */
	YYSTYPE retval;
	Symbol symbol1;
	Symbol symbol2;
	Formals formals;
	Expression expression;

	/* feature := ID(formal,*) : TYPE{expr} 
		feature := ID : TYPE [<- expr]
	*/
	// feature name
	if(lookNextToken() == OBJECTID){
		symbol1 = cool_yylval.symbol;
		consumeNextToken();
	}else{
		return handle_error();
	}

	// two different types of features based on the next token
	if(lookNextToken() == '('){
		// methods (a, b, c..)

		consumeNextToken();
		// formals
		YYSTYPE fmls = cool_formals();
		if(!errorstate){
			formals = fmls.formals;
		}else{
			return fmls;
		}
		// ending paranthens
		if(lookNextToken() == ')'){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// type dec semicolon
		if(lookNextToken() == ':'){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// type id
		if(lookNextToken() == TYPEID){
			symbol2 = cool_yylval.symbol;
			consumeNextToken();
		}else{
			return handle_error();
		}
		// opening curly bracket
		if(lookNextToken() == '{'){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// expression
		YYSTYPE expr = cool_expression();
		if(!errorstate){
			expression = expr.expression;
		}else{
			return expr;
		}
		// construct the method feature and return
		retval.feature = method(symbol1, formals, symbol2, expression);
		// ending curly bracket
		if(lookNextToken() == '}'){
			consumeNextToken();
		}else{
			return handle_error();
		}

		return retval;
	}else if(lookNextToken() == ':'){
		// attribute

		consumeNextToken();
		// type id
		if(lookNextToken() == TYPEID){
			symbol2 = cool_yylval.symbol;
			consumeNextToken();
		}else{
			return handle_error();
		}
		// optional initialization
		if(lookNextToken() == ASSIGN){
			consumeNextToken();
			YYSTYPE expr = cool_expression();
			if(!errorstate){
				expression = expr.expression;
			}else{
				return handle_error();
			}
		}else{
			expression = no_expr();
		}

		// construct the attribute feature and return
		retval.feature = attr(symbol1, symbol2, expression);
		return retval;
	}else{
		// symbol error
		return handle_error();
	}
}


YYSTYPE cool_formals(){
	/* parsing a list of formals, don't split into cool_formal cuz its short */
	Formals formals = nil_Formals();

	/* formal := ID : TYPE */
	while(lookNextToken() != ')'){
		// one formal
		Symbol oid;
		Symbol tid;

		// objectid
		if(lookNextToken() == OBJECTID){
			oid = cool_yylval.symbol;
			consumeNextToken();
		}else{
			return handle_error();
		}
		// semicolon
		if(lookNextToken() == ':'){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// type id
		if(lookNextToken() == TYPEID){
			tid = cool_yylval.symbol;
			consumeNextToken();
		}else{
			return handle_error();
		}
		// optional comma
		if(lookNextToken() == ','){
			consumeNextToken();
		}

		// add formals
		Formals parsed_formal = single_Formals(formal(oid, tid));
		formals = append_Formals(formals, parsed_formal);
	}

	// finalize
	YYSTYPE retval;
	retval.formals = formals;
	return retval;
}


/* Expressions, the hard part */


YYSTYPE cool_expression(){
	// whether it is let or other
	if(lookNextToken() == LET){
		// let special case, read till end
		consumeNextToken();
		return cool_expression_let();
	}else{
		// follow the precedence
		return cool_expression_p7();
	}
}


YYSTYPE cool_expression_p7(){
	/* not statement */
	int not_consumed = 0;
	// chained not
	while(lookNextToken() == NOT){
		consumeNextToken();
		not_consumed += 1;
	}

	// subexpression
	YYSTYPE sub_exp = cool_expression_p6();
	if(!errorstate){
		// notting
		Expression expression = sub_exp.expression;
		while(not_consumed > 0){
			expression = comp(expression);
			not_consumed -= 1;
		}
		// finalize and return
		YYSTYPE retval;
		retval.expression = expression;
		return retval;
	}else{
		return sub_exp;
	}
}


YYSTYPE cool_expression_p6(){
	/* predecence level for <=, <, = */
	// accumulated expression
	Expression expression;

	// firstly read the sub-preced exp
	YYSTYPE p5_exp1 = cool_expression_p5();
	if(!errorstate){
		expression = p5_exp1.expression;
	}else{
		return p5_exp1;
	}

	// then consumes operators
	while(lookNextToken() == LE || lookNextToken() == '<' || lookNextToken() == '='){
		// the second sub-preced exp
		Expression e2;

		if(lookNextToken() == LE){
			/* expr := expr <= expr */
			consumeNextToken();
			YYSTYPE p5_exp2 = cool_expression_p5();
			if(!errorstate){
				e2 = p5_exp2.expression;
			}else{
				return p5_exp2;
			}
			// accumulate
			expression = leq(expression, e2);
		}else if(lookNextToken() == '<'){
			/* expr := expr < expr */
			consumeNextToken();
			YYSTYPE p5_exp2 = cool_expression_p5();
			if(!errorstate){
				e2 = p5_exp2.expression;
			}else{
				return p5_exp2;
			}
			// accumulate
			expression = lt(expression, e2);
		}else{
			/* expr := expr = expr <- higher precedence */
			consumeNextToken();
			YYSTYPE p5_exp2 = cool_expression();
			if(!errorstate){
				e2 = p5_exp2.expression;
			}else{
				return p5_exp2;
			}
			// accumulate
			expression = eq(expression, e2);
		}
	}

	// finalize and return
	YYSTYPE retval;
	retval.expression = expression;
	return retval;
}


YYSTYPE cool_expression_p5(){
	/* the + and - statements */
	// accumulated expression
	Expression expression;

	// firstly read the sub-preced exp
	YYSTYPE p4_exp1 = cool_expression_p4();
	if(!errorstate){
		expression = p4_exp1.expression;
	}else{
		return p4_exp1;
	}

	// then consumes operators
	while(lookNextToken() == '+' || lookNextToken() == '-'){
		// the second sub-preced exp
		Expression e2;

		if(lookNextToken() == '+'){
			/* expr := expr + expr */
			consumeNextToken();
			YYSTYPE p4_exp2 = cool_expression_p4();
			if(!errorstate){
				e2 = p4_exp2.expression;
			}else{
				return p4_exp2;
			}
			// accumulate
			expression = plus(expression, e2);
		}else{
			/* expr := expr - expr */
			consumeNextToken();
			YYSTYPE p4_exp2 = cool_expression_p4();
			if(!errorstate){
				e2 = p4_exp2.expression;
			}else{
				return p4_exp2;
			}
			// accumulate
			expression = sub(expression, e2);
		}
	}

	// finalize and return
	YYSTYPE retval;
	retval.expression = expression;
	return retval;
}


YYSTYPE cool_expression_p4(){
	/* for * and / */
	// accumulated expression
	Expression expression;

	// firstly read the sub-preced exp
	YYSTYPE p3_exp1 = cool_expression_p3();
	if(!errorstate){
		expression = p3_exp1.expression;
	}else{
		return p3_exp1;
	}

	// then consumes operators
	while(lookNextToken() == '*' || lookNextToken() == '/'){
		// the second sub-preced exp
		Expression e2;

		if(lookNextToken() == '*'){
			/* expr := expr * expr */
			consumeNextToken();
			YYSTYPE p3_exp2 = cool_expression_p3();
			if(!errorstate){
				e2 = p3_exp2.expression;
			}else{
				return p3_exp2;
			}
			// accumulate
			expression = mul(expression, e2);
		}else{
			/* expr := expr / expr */
			consumeNextToken();
			YYSTYPE p3_exp2 = cool_expression_p3();
			if(!errorstate){
				e2 = p3_exp2.expression;
			}else{
				return p3_exp2;
			}
			// accumulate
			expression = divide(expression, e2);
		}
	}

	// finalize and return
	YYSTYPE retval;
	retval.expression = expression;
	return retval;
}


YYSTYPE cool_expression_p3(){
	/* for isvoid */
	int isvoid_consumed = 0;
	// chained not
	while(lookNextToken() == ISVOID){
		consumeNextToken();
		isvoid_consumed += 1;
	}

	// subexpression
	YYSTYPE sub_exp = cool_expression_p2();
	if(!errorstate){
		// notting
		Expression expression = sub_exp.expression;
		while(isvoid_consumed > 0){
			expression = isvoid(expression);
			isvoid_consumed -= 1;
		}
		// finalize and return
		YYSTYPE retval;
		retval.expression = expression;
		return retval;
	}else{
		return sub_exp;
	}
}


YYSTYPE cool_expression_p2(){
	/* for neg ~ statement */
	int neg_consumed = 0;
	// chained not
	while(lookNextToken() == '~'){
		consumeNextToken();
		neg_consumed += 1;
	}

	// subexpression
	YYSTYPE sub_exp = cool_expression_p1();
	if(!errorstate){
		// notting
		Expression expression = sub_exp.expression;
		while(neg_consumed > 0){
			expression = neg(expression);
			neg_consumed -= 1;
		}
		// finalize and return
		YYSTYPE retval;
		retval.expression = expression;
		return retval;
	}else{
		return sub_exp;
	}
}


YYSTYPE cool_expression_p1(){
	/* for method dispath */
	Expression expression;
	YYSTYPE sub_exp = cool_expression_p0();
	if(!errorstate){
		expression = sub_exp.expression;
	}else{
		return sub_exp;
	}

	// chained . operator possible
	while(lookNextToken() == '@' || lookNextToken() == '.'){
		// whether is static dispatch or not
		Symbol method_id;
		Symbol par_ref;
		bool is_static = false;

		// optional parent ref
		if(lookNextToken() == '@'){
			/* expr := expr@TYPE.ID(expr,*) */
			is_static = true;
			consumeNextToken();

			// the parent type id reference
			if(lookNextToken() == TYPEID){
				par_ref = cool_yylval.symbol;
				consumeNextToken();
			}else{
				return handle_error();
			}
		}

		// a type binding must be followed by a invocation
		if(lookNextToken() == '.'){
			consumeNextToken();
		}else{
			return handle_error();
		}

		/* expr := expr.ID(expr,*) */
		// read the method symbol
		if(lookNextToken() == OBJECTID){
			method_id = cool_yylval.symbol;
			consumeNextToken();
		}else{
			return handle_error();
		}
		// finally read in multiple expressions as input and return
		YYSTYPE multi_exprs = cool_expressions_parenthesis();
		if(!errorstate){
			// chained invocation
			if(is_static){
				expression = static_dispatch(expression, par_ref, method_id, multi_exprs.expressions);
			}else{
				expression = dispatch(expression, method_id, multi_exprs.expressions);
			}
			// consume )
			if(lookNextToken() == ')'){
				consumeNextToken();
			}else{
				return handle_error();
			}
		}else{
			return multi_exprs;
		}
	}

	// finalize and return
	YYSTYPE retval;
	retval.expression = expression;
	return retval;
}


YYSTYPE cool_expression_p0(){
	/* the one with control follow */
	YYSTYPE retval;

	// control flow except let
	if(lookNextToken() == IF){
		/* expr := if expr then expr else expr fi */
		consumeNextToken();
		Expression e1;
		Expression e2;
		Expression e3;

		// the first expression
		YYSTYPE e1_retval = cool_expression();
		if(!errorstate){
			e1 = e1_retval.expression;
		}else{
			return e1_retval;
		}
		// consumes then
		if(lookNextToken() == THEN){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// the second expression
		YYSTYPE e2_retval = cool_expression();
		if(!errorstate){
			e2 = e2_retval.expression;
		}else{
			return e2_retval;
		}
		// consumes else
		if(lookNextToken() == ELSE){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// the third expression
		YYSTYPE e3_retval = cool_expression();
		if(!errorstate){
			e3 = e3_retval.expression;
		}else{
			return e3_retval;
		}

		// finalize before consume the line breaker
		retval.expression = cond(e1, e2, e3);

		// consumes the final fi
		if(lookNextToken() == FI){
			consumeNextToken();
		}else{
			return handle_error();
		}

		return retval;
	}else if(lookNextToken() == WHILE){
		/* expr := while expr loop expr pool */
		consumeNextToken();
		Expression e1;
		Expression e2;

		// the first expression
		YYSTYPE e1_retval = cool_expression();
		if(!errorstate){
			e1 = e1_retval.expression;
		}else{
			return e1_retval;
		}
		// consumes the loop
		if(lookNextToken() == LOOP){
			consumeNextToken();
		}else{
			return handle_error();
		}
		// second expression
		YYSTYPE e2_retval = cool_expression();
		if(!errorstate){
			e2 = e2_retval.expression;
		}else{
			return e2_retval;
		}

		// finalize and return
		retval.expression = loop(e1, e2);

		// consumes pool
		if(lookNextToken() == POOL){
			consumeNextToken();
		}else{
			return handle_error();
		}

		return retval; 
	}else if(lookNextToken() == CASE){
		/* expr := case expr of [ID:TYPE => expr;]+ esac */
		Expression case_expression;
		Cases case_cases = nil_Cases();
		consumeNextToken();

		// get the case expression
		YYSTYPE case_exp = cool_expression();
		if(!errorstate){
			case_expression = case_exp.expression;
		}else{
			return case_exp;
		}

		// consume of
		if(lookNextToken() == OF){
			consumeNextToken();
		}else{
			return handle_error();
		}

		// get cases
		do{
			// case ID:TYPE => expr;
			Symbol case_of_id;
			Symbol case_of_type;
			Expression case_of_expression;

			// read the object id, type id
			if(lookNextToken() == OBJECTID){
				case_of_id = cool_yylval.symbol;
				consumeNextToken();
			}else{
				return handle_error();
			}
			if(lookNextToken() == ':'){
				consumeNextToken();
			}else{
				return handle_error();
			}
			if(lookNextToken() == TYPEID){
				case_of_type = cool_yylval.symbol;
				consumeNextToken();
			}else{
				return handle_error();
			}
			if(lookNextToken() == DARROW){
				consumeNextToken();
			}else{
				return handle_error();
			}
			// case exp
			YYSTYPE case_of_exp = cool_expression();
			if(!errorstate){
				case_of_expression = case_of_exp.expression;
			}else{
				return case_of_exp;
			}
			// finalize and append to case
			case_cases = append_Cases(case_cases, single_Cases(branch(case_of_id, case_of_type, case_of_expression)));
			// final semicolon
			if(lookNextToken() == ';'){
				consumeNextToken();
			}else{
				return handle_error();
			}
		}while(lookNextToken() != ESAC);

		// finalize and store
		YYSTYPE retval;
		retval.expression = typcase(case_expression, case_cases);

		// consume ESAC
		consumeNextToken();

		return retval;
	}else if(lookNextToken() == '{'){
		/* expr := {expr,+} */
		consumeNextToken();
		Expressions expressions = nil_Expressions();

		do{
			// the expression
			YYSTYPE block_sub = cool_expression();
			if(!errorstate){
				expressions = append_Expressions(expressions, single_Expressions(block_sub.expression));
			}else{
				return block_sub;
			}
			// consumes the required semicolon
			if(lookNextToken() == ';'){
				consumeNextToken();
			}else{
				return handle_error();
			}
		}while(lookNextToken() != '}');
		// consume the final }
		retval.expression = block(expressions);
		consumeNextToken();
		
		return retval;
	}else if(lookNextToken() == '('){
		/* expr := (expr) */
		consumeNextToken();
		Expression e;

		// sub exp
		YYSTYPE paren_sub = cool_expression();
		if(!errorstate){
			e = paren_sub.expression;
		}else{
			return paren_sub;
		}
		// finalize
		retval.expression = e;
		// consume the closing parenthesis
		if(lookNextToken() == ')'){
			consumeNextToken();
		}else{
			return handle_error();
		}
		
		return retval;
	}

	// single factor except for object id
	if(lookNextToken() == NEW){
		/* expr := new TYPE */
		consumeNextToken();
		if(lookNextToken() == TYPEID){
			YYSTYPE retval;
			retval.expression = new_(cool_yylval.symbol);
			consumeNextToken();
			return retval;
		}else{
			return handle_error();
		}
	}else if(lookNextToken() == INT_CONST){
		/* expr := integer */
		YYSTYPE retval;
		retval.expression = int_const(cool_yylval.symbol);
		consumeNextToken();
		return retval;
	}else if(lookNextToken() == STR_CONST){
		/* expr := string */
		YYSTYPE retval;
		retval.expression = string_const(cool_yylval.symbol);
		consumeNextToken();
		return retval;
	}else if(lookNextToken() == BOOL_CONST){
		/* expr := true | false */
		YYSTYPE retval;
		retval.expression = bool_const(cool_yylval.boolean);
		consumeNextToken();
		return retval;
	}

	// finally we work on ID
	if(lookNextToken() == OBJECTID){
		Symbol oid = cool_yylval.symbol;
		consumeNextToken();

		if(lookNextToken() == '('){
			/* expr := ID(expr,*) */
			YYSTYPE multi_exprs = cool_expressions_parenthesis();
			if(!errorstate){
				YYSTYPE retval;
				retval.expression = dispatch(object(idtable.add_string("self")), oid, multi_exprs.expressions);
				// consume )
				if(lookNextToken() == ')'){
					consumeNextToken();
				}else{
					return handle_error();
				}
				// return
				return retval;
			}else{
				return multi_exprs;
			}
		}else if(lookNextToken() == ASSIGN){
			/* expr := ID <- expr */
			consumeNextToken();
			YYSTYPE assign_expr = cool_expression();
			if(!errorstate){
				YYSTYPE retval;
				retval.expression = assign(oid, assign_expr.expression);
				return retval;
			}else{
				return assign_expr;
			}
		}else{
			/* expr := ID */
			YYSTYPE retval;
			retval.expression = object(oid);
			return retval;
		}
	}

	// all missed
	return handle_error();
}


YYSTYPE cool_expressions_parenthesis(){
	/* (exp,*) */
	// left parenthesis
	if(lookNextToken() == '('){
		consumeNextToken();
	}else{
		return handle_error();
	}

	Expressions expressions = nil_Expressions();

	// arguments
	while(lookNextToken() != ')'){
		YYSTYPE one_arg_exp = cool_expression();
		if(!errorstate){
			expressions = append_Expressions(expressions, single_Expressions(one_arg_exp.expression));
		}else{
			return one_arg_exp;
		}
		// then consumes the optional comma
		if(lookNextToken() == ','){
			consumeNextToken();
		}
	}
	// finalize
	YYSTYPE retval;
	retval.expressions = expressions;

	// consume the riight parenthesis
	return retval;
}


YYSTYPE cool_expression_let(){
	/* expr := let [ID:TYPE [<-expr]],+ in expr
	A recursive approach */
	Symbol oid;
	Symbol tid;
	Expression opt_assign_expression = no_expr();

	// ID
	if(lookNextToken() == OBJECTID){
		oid = cool_yylval.symbol;
		consumeNextToken();
	}else{
		return handle_error();
	}
	// ;
	if(lookNextToken() == ':'){
		consumeNextToken();
	}else{
		return handle_error();
	}
	// type
	if(lookNextToken() == TYPEID){
		tid = cool_yylval.symbol;
		consumeNextToken();
	}else{
		return handle_error();
	}
	// optional assignment
	if(lookNextToken() == ASSIGN){
		consumeNextToken();
		YYSTYPE opt_assign_exp = cool_expression();
		if(!errorstate){
			opt_assign_expression = opt_assign_exp.expression;
		}else{
			return opt_assign_exp;
		}
	}

	// if the next symbol is , recurse, if "in" then finish
	if(lookNextToken() == ','){
		consumeNextToken();
		YYSTYPE continued_let_exp = cool_expression_let();
		if(!errorstate){
			YYSTYPE retval;
			retval.expression = let(oid, tid, opt_assign_expression, continued_let_exp.expression);
			return retval;
		}else{
			return continued_let_exp;
		}
	}else if(lookNextToken() == IN){
		consumeNextToken();
		// read in the let expression till end;
		YYSTYPE let_exp_til_end = cool_expression();
		if(!errorstate){
			YYSTYPE retval;
			retval.expression = let(oid, tid, opt_assign_expression, let_exp_til_end.expression);
			return retval;
		}else{
			return let_exp_til_end;
		}
	}else{
		// wrong let syntax
		return handle_error();
	}
}
