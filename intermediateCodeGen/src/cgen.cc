//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully and add code to build an LLVM program
//**************************************************************

/* Author: Ziang Wan
 * 72 hour coding challenge
 */

#define EXTERN
#include "cgen.h"
#include <sstream>
#include <string>
#include <stdio.h>

// the debug flag
extern int cgen_debug;

// define some names
#define CLASS_VTABLE "__vtable_ptr"
#define CLASS_TAG "__tag"
#define CLASS_INTPTR "__intptr"
#define CLASS_NAME "__name"

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.  Feel free to add your
// own definitions as you see fit.
//
//////////////////////////////////////////////////////////////////////

EXTERN Symbol
	// required classes
	Object,
	IO, String, Int, Bool, Main,

	// class methods
	cool_abort, type_name, cool_copy,
	out_string, out_int, in_string, in_int,
	length, concat, substr,

	// class members
	val,

	// special symbols
	No_class,  // symbol that can't be the name of any user-defined class
	No_type,   // If e : No_type, then no code is generated for e.
	SELF_TYPE, // Special code is generated for new SELF_TYPE.
	self,      // self generates code differently than other references

	// extras
	arg, arg2, prim_string, prim_int, prim_bool;

//********************************************************
//
// PREDEFINED FUNCTIONS:
//
// The following functions are already coded, you should
// not need to modify them, although you may if necessary.
//
//********************************************************

// Initializing the predefined symbols.
static void initialize_constants(void) {
	Object = idtable.add_string("Object");
	IO = idtable.add_string("IO");
	String = idtable.add_string("String");
	Int = idtable.add_string("Int");
	Bool = idtable.add_string("Bool");
	Main = idtable.add_string("Main");

	cool_abort = idtable.add_string("abort");
	type_name = idtable.add_string("type_name");
	cool_copy = idtable.add_string("copy");
	out_string = idtable.add_string("out_string");
	out_int = idtable.add_string("out_int");
	in_string = idtable.add_string("in_string");
	in_int = idtable.add_string("in_int");
	length = idtable.add_string("length");
	concat = idtable.add_string("concat");
	substr = idtable.add_string("substr");

	val = idtable.add_string("val");

	No_class = idtable.add_string("_no_class");
	No_type = idtable.add_string("_no_type");
	SELF_TYPE = idtable.add_string("SELF_TYPE");
	self = idtable.add_string("self");

	arg = idtable.add_string("arg");
	arg2 = idtable.add_string("arg2");
	prim_string = idtable.add_string("sbyte*");
	prim_int = idtable.add_string("int");
	prim_bool = idtable.add_string("bool");
}

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) {
	initialize_constants();
	class_table = new CgenClassTable(classes, os);
}

// Sets up declarations for extra functions needed for code generation
// You should not need to modify this code for MP3
void CgenClassTable::setup_external_functions() {
	if(cgen_debug){
		std::cerr << "CgenClassTable::setup_external_functions()" << endl;
	}

	ValuePrinter vp(*this->ct_stream);

	// setup function: external int strcmp(sbyte*, sbyte*)
	op_type i32_type(INT32), i8ptr_type(INT8_PTR), vararg_type(VAR_ARG);
	vector<op_type> strcmp_args;
	strcmp_args.push_back(i8ptr_type);
	strcmp_args.push_back(i8ptr_type);
	vp.declare(i32_type, "strcmp", strcmp_args);

	// setup function: external int printf(sbyte*, ...)
	vector<op_type> printf_args;
	printf_args.push_back(i8ptr_type);
	printf_args.push_back(vararg_type);
	vp.declare(i32_type, "printf", printf_args);

	// setup function: external void abort(void)
	op_type void_type(VOID);
	vector<op_type> abort_args;
	vp.declare(void_type, "abort", abort_args);

	// setup function: external i8* malloc(i32)
	vector<op_type> malloc_args;
	malloc_args.push_back(i32_type);
	vp.declare(i8ptr_type, "malloc", malloc_args);

#ifdef MP4
	// ADD CODE HERE
	// Setup external functions for built in object class functions
	
	// declare %Object* @Object_new()
	// declare %Object* @Object_abort(%Object*)
	// declare %String* @Object_type_name(%Object*)
	// declare %Object* @Object_copy(%Object*)
	op_type obj_ptr_type("Object*"), str_ptr_type("String*");

	vector<op_type> object_new_args;
	vp.declare(obj_ptr_type, "Object_new", object_new_args);

	vector<op_type> object_abort_args;
	object_abort_args.push_back(obj_ptr_type);
	vp.declare(obj_ptr_type, "Object_abort", object_abort_args);

	vector<op_type> object_type_args;
	object_type_args.push_back(obj_ptr_type);
	vp.declare(str_ptr_type, "Object_type_name", object_type_args);

	vector<op_type> object_copy_args;
	object_copy_args.push_back(obj_ptr_type);
	vp.declare(obj_ptr_type, "Object_copy", object_copy_args);

	// declare %IO* @IO_new()
	// declare %IO* @IO_out_string(%IO*, %String*)
	// declare %IO* @IO_out_int(%IO*, i32)
	// declare %String* @IO_in_string(%IO*)
	// declare i32 @IO_in_int(%IO*)
	op_type io_ptr_type("IO*");

	vector<op_type> io_new_args;
	vp.declare(io_ptr_type, "IO_new", io_new_args);

	vector<op_type> io_out_string_args;
	io_out_string_args.push_back(io_ptr_type);
	io_out_string_args.push_back(str_ptr_type);
	vp.declare(io_ptr_type, "IO_out_string", io_out_string_args);

	vector<op_type> io_out_int_args;
	io_out_int_args.push_back(io_ptr_type);
	io_out_int_args.push_back(i32_type);
	vp.declare(io_ptr_type, "IO_out_int", io_out_int_args);

	vector<op_type> io_in_string_args;
	io_in_string_args.push_back(io_ptr_type);
	vp.declare(str_ptr_type, "IO_in_string", io_in_string_args);

	vector<op_type> io_in_int_args;
	io_in_int_args.push_back(io_ptr_type);
	vp.declare(i32_type, "IO_in_int", io_in_int_args);

	// declare %String* @String_new()
	// declare i32 @String_length(%String*)
	// declare %String* @String_concat(%String*, %String*)
	// declare %String* @String_substr(%String*, i32, i32)
	vector<op_type> string_new_args;
	vp.declare(str_ptr_type, "String_new", string_new_args);
	
	vector<op_type> string_length_args;
	string_length_args.push_back(str_ptr_type);
	vp.declare(i32_type, "String_length", string_length_args);
	
	vector<op_type> string_concat_args;
	string_concat_args.push_back(str_ptr_type);
	string_concat_args.push_back(str_ptr_type);
	vp.declare(str_ptr_type, "String_concat", string_concat_args);
	
	vector<op_type> string_substr_args;
	string_substr_args.push_back(str_ptr_type);
	string_substr_args.push_back(i32_type);
	string_substr_args.push_back(i32_type);
	vp.declare(str_ptr_type, "String_substr", string_substr_args);

	// declare %Int* @Int_new()
	// declare void @Int_init(%Int*, i32)
	op_type int_ptr_type("Int*");

	vector<op_type> int_new_args;
	vp.declare(int_ptr_type, "Int_new", int_new_args);

	vector<op_type> int_init_args;
	int_init_args.push_back(int_ptr_type);
	int_init_args.push_back(i32_type);
	vp.declare(void_type, "Int_init", int_init_args);

	// declare %Bool* @Bool_new()
	// declare void @Bool_init(%Bool*, i1)
	op_type bool_ptr_type("Bool*"), i1_type(INT1);

	vector<op_type> bool_new_args;
	vp.declare(bool_ptr_type, "Bool_new", bool_new_args);

	vector<op_type> bool_init_args;
	bool_init_args.push_back(bool_ptr_type);
	bool_init_args.push_back(i1_type);
	vp.declare(void_type, "Bool_init", bool_init_args);
#endif
}

// Creates AST nodes for the basic classes and installs them in the class list
void CgenClassTable::install_basic_classes() {
	// The tree package uses these globals to annotate the classes built below.
	curr_lineno = 0;
	Symbol filename = stringtable.add_string("<basic class>");

	// A few special class names are installed in the lookup table but not
	// the class list.  Thus, these classes exist, but are not part of the
	// inheritance hierarchy.

	// No_class serves as the parent of Object and the other special classes.
	Class_ noclasscls = class_(No_class, No_class, nil_Features(), filename);
	install_special_class(new CgenNode(noclasscls, CgenNode::Basic, this));
	delete noclasscls;

#ifdef MP4
	// SELF_TYPE is the self class; it cannot be redefined or inherited.
	Class_ selftypecls = class_(SELF_TYPE, No_class, nil_Features(), filename);
	install_special_class(new CgenNode(selftypecls, CgenNode::Basic, this));
	delete selftypecls;
	//
	// Primitive types masquerading as classes.  This is done so we can
	// get the necessary Symbols for the innards of String, Int, and Bool
	//
	Class_ primstringcls = class_(prim_string, No_class, nil_Features(), filename);
	install_special_class(new CgenNode(primstringcls, CgenNode::Basic, this));
	delete primstringcls;
#endif

	// basic class: primitive int
	Class_ primintcls = class_(prim_int, No_class, nil_Features(), filename);
	install_special_class(new CgenNode(primintcls, CgenNode::Basic, this));
	delete primintcls;

	// basic class: primitive bool
	Class_ primboolcls = class_(prim_bool, No_class, nil_Features(), filename);
	install_special_class(new CgenNode(primboolcls, CgenNode::Basic, this));
	delete primboolcls;

	// The Object class has no parent class. Its methods are
	//        cool_abort() : Object    aborts the program
	//        type_name() : Str    returns a string representation of class name
	//        copy() : SELF_TYPE       returns a copy of the object
	// There is no need for method bodies in the basic classes---these
	// are already built in to the runtime system.
	Class_ objcls = class_(Object, No_class, append_Features(append_Features(
						single_Features(method(cool_abort, nil_Formals(),Object, no_expr())),
						single_Features(method(type_name, nil_Formals(), String, no_expr()))),
						single_Features(method(cool_copy, nil_Formals(), SELF_TYPE, no_expr()))),
					filename);
	install_class(new CgenNode(objcls, CgenNode::Basic, this));
	delete objcls;

	// The Int class has no methods and only a single attribute, the "val" for the integer.
	Class_ intcls = class_(Int, Object, single_Features(attr(val, prim_int, no_expr())), filename);
	install_class(new CgenNode(intcls, CgenNode::Basic, this));
	delete intcls;

	// Bool also has only the "val" slot.
	Class_ boolcls = class_(Bool, Object, single_Features(attr(val, prim_bool, no_expr())), filename);
	install_class(new CgenNode(boolcls, CgenNode::Basic, this));
	delete boolcls;

#ifdef MP4
	//
	// The class String has a number of slots and operations:
	//       val                                  the string itself
	//       length() : Int                       length of the string
	//       concat(arg: Str) : Str               string concatenation
	//       substr(arg: Int, arg2: Int): Str     substring
	//
	Class_ stringcls =
			class_(String, Object, append_Features(append_Features(append_Features(
												single_Features(attr(val, prim_string, no_expr())),
												single_Features(method(length, nil_Formals(), Int, no_expr()))),
										 		single_Features(method(concat, single_Formals(formal(arg, String)), String, no_expr()))),
								 				single_Features(method(substr, append_Formals(single_Formals(formal(arg, Int)), single_Formals(formal(arg2, Int))), String, no_expr()))),
					filename);
	install_class(new CgenNode(stringcls, CgenNode::Basic, this));
	delete stringcls;
#endif

#ifdef MP4
	//
	// The IO class inherits from Object. Its methods are
	//        out_string(Str) : SELF_TYPE          writes a string to the output
	//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
	//        in_string() : Str                    reads a string from the input
	//        in_int() : Int                         "   an int     "  "     "
	//
	Class_ iocls = class_(IO, Object, append_Features(append_Features(append_Features(
									single_Features(method(out_string, single_Formals(formal(arg, String)), SELF_TYPE, no_expr())),
									single_Features(method(out_int, single_Formals(formal(arg, Int)), SELF_TYPE, no_expr()))),
									single_Features(method(in_string, nil_Formals(), String, no_expr()))),
									single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
						filename);
	install_class(new CgenNode(iocls, CgenNode::Basic, this));
	delete iocls;
#endif
}

// install_classes enters a list of classes in the symbol table.
void CgenClassTable::install_classes(Classes cs) {
	for (int i = cs->first(); cs->more(i); i = cs->next(i)) {
		install_class(new CgenNode(cs->nth(i), CgenNode::NotBasic, this));
	}
}

// Add this CgenNode to the class list and the lookup table
void CgenClassTable::install_class(CgenNode *nd) {
	Symbol name = nd->get_name();

	if (probe(name)){
		return;
	}

	// The class name is legal, so add it to the list of classes
	// and the symbol table.
	nds = new List<CgenNode>(nd, nds);
	addid(name, nd);
}


// Add this CgenNode to the special class list and the lookup table
void CgenClassTable::install_special_class(CgenNode *nd) {
	Symbol name = nd->get_name();

	if (probe(name)){
		return;
	}

	// The class name is legal, so add it to the list of special classes and the symbol table.
	special_nds = new List<CgenNode>(nd, special_nds);
	addid(name, nd);
}

void CgenClassTable::build_inheritance_tree() {
	for (List<CgenNode> *l = nds; l; l = l->tl()){
		set_relations(l->hd());
	}
}


// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
void CgenClassTable::set_relations(CgenNode *nd) {
	CgenNode *parent_node = probe(nd->get_parent());
	nd->set_parentnd(parent_node);
	// modified
	if(parent_node){
		parent_node->add_child(nd);
	}
}

// Get the root of the class tree.
CgenNode *CgenClassTable::root() {
	return probe(Object);
}

// added

int CgenClassTable::get_global_str_index(string input_str){
	for(int i = 0; i < this->map_string.size(); i++){
		if(this->map_string[i] == input_str){
			return i;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
//
// Special-case functions used for the method Int Main::main() for
// MP3 only.
//
//////////////////////////////////////////////////////////////////////

#ifndef MP4

CgenNode *CgenClassTable::getMainmain(CgenNode *c) {
	if (c && !c->basic())
		return c; // Found it!

	List<CgenNode> *children = c->get_children();
	for (List<CgenNode> *child = children; child; child = child->tl()) {
		if (CgenNode *foundMain = this->getMainmain(child->hd()))
			return foundMain; // Propagate it up the recursive calls
	}

	return 0; // Make the recursion continue
}

#endif

//-------------------------------------------------------------------
//
// END OF PREDEFINED FUNCTIONS
//
//-------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
// coding string, int, and boolean constants
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type stringEntry.  stringEntry methods are defined both for string
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

// CgenClassTable constructor orchestrates all code generation - the entry
CgenClassTable::CgenClassTable(Classes classes, ostream &s) : nds(0) {
	if (cgen_debug){
		std::cerr << "Building CgenClassTable" << endl;
	}
	ct_stream = &s;

	// Make sure we have a scope, both for classes and for constants - create the tbl
	enterscope();

	// Create an inheritance tree with one CgenNode per class.
	install_basic_classes();
	install_classes(classes);
	build_inheritance_tree();

	// First pass
	setup();

	// Second pass
	code_module();

	// Done with code generation: exit scopes
	exitscope();
}

CgenClassTable::~CgenClassTable() {}

// The code generation first pass.  Define these two functions to traverse the tree and setup each CgenNode
void CgenClassTable::setup() {
	if(cgen_debug){
		std::cerr << "CgenClassTable::setup()" << endl;
	}
	setup_external_functions();
	setup_classes(root(), 0);
}

void CgenClassTable::setup_classes(CgenNode *c, int depth) {
	// MAY ADD CODE HERE
	// if you want to give classes more setup information
	c->setup(current_tag++, depth);
	List<CgenNode> *children = c->get_children();
	for (List<CgenNode> *child = children; child; child = child->tl()){
		setup_classes(child->hd(), depth + 1);
	}

	c->set_max_child(current_tag - 1);

	if (cgen_debug){
		std::cerr << "Class " << c->get_name() << " assigned tag " << c->get_tag() << ", max child " << c->get_max_child() << endl;
	}
}

// The code generation second pass. Add code here to traverse the tree and
// emit code for each CgenNode
void CgenClassTable::code_module() {
	code_constants();

	// This must be after code_module() since that emits constants needed by the code() method for expressions
#ifndef MP4
	CgenNode *mainNode = getMainmain(root());
	mainNode->codeGenMainmain();
#endif

	code_main();

#ifdef MP4
	// code classes starting at the root
	code_classes(root());
#else
#endif
}

// Create global definitions for constant Cool objects

// code constant recurrence
void code_constant_recurrence(CgenNode *c){
	c->declare_class();
	List<CgenNode> *children = c->get_children();
	for(List<CgenNode> *child = children; child; child = child->tl()){
		code_constant_recurrence(child->hd());
	}
}

void CgenClassTable::code_constants() {
#ifdef MP4
	// ADD CODE HERE
	if(cgen_debug){
		std::cerr << "CgenClassTable::code_constants()" << endl;
	}

	// regular class declaration
	code_constant_recurrence(this->root());

	// string and int const declaration
	stringtable.code_string_table(*this->ct_stream, this);
#endif
}


// Create definitions for all String constants
void StrTable::code_string_table(ostream &s, CgenClassTable *ct) {
	for (List<StringEntry> *l = tbl; l; l = l->tl()) {
		l->hd()->code_def(s, ct);
	}
}

// Create definitions for all Int constants
void IntTable::code_string_table(ostream &s, CgenClassTable *ct) {
	for (List<IntEntry> *l = tbl; l; l = l->tl()) {
		l->hd()->code_def(s, ct);
	}
}

// generate code to define a global string constant
void StringEntry::code_def(ostream &s, CgenClassTable *ct) {
#ifdef MP4
	// ADD CODE HERE
	ValuePrinter vp(s);

	string str_entry_string(this->str);

	op_arr_type str_arr_type(INT8, str_entry_string.length() + 1);
	op_arr_type str_arr_ptr_type(INT8_PTR, str_entry_string.length() + 1);

	// init the string const
	const_value oprd_global_str(str_arr_type, str_entry_string, true);
	string str_global_name = "str." + itos(index);
	vp.init_constant(str_global_name, oprd_global_str);

	// init the const string object, directly using output stream
	string str_struct_name = "String." + itos(index);
	// dec
	s << "@" << str_struct_name << " = constant %String {\n";

	// vtable pointer init
	s << "\t%String_vtable* @String_vtable_prototype,\n";

	// ptr to global const
	s << "\ti8* getelementptr (" << str_arr_ptr_type.get_name() << " @"
		<< str_global_name << ", i32 0, i32 0)\n";

	// finish struct
	s << "}\n\n";

	// add to the mapping
	global_value oprd_global_const(op_type("String*"), str_struct_name);
	ct->map_string.push_back(str_entry_string);
	ct->map_global.push_back(oprd_global_const);
#endif
}

// generate code to define a global int constant
void IntEntry::code_def(ostream &s, CgenClassTable *ct) {
	// Leave this method blank, since we are not going to use global
	// declarations for int constants.

	// Okay, I am leaving this blank.
}

// my added function
void CgenNode::declare_class(){
	if(cgen_debug){
		std::cerr << "Declare class: " << name->get_string() << endl;
	}

	ValuePrinter vp(*this->class_table->ct_stream);

	// declare string constant
	string str_class_name(name->get_string());
	string str_const_name = "str." + str_class_name;
	op_arr_type str_arr_type(INT8, str_class_name.length() + 1);
	op_arr_type str_arr_ptr_type(INT8_PTR, str_class_name.length() + 1);

	// print out the constant: @str.String = internal constant [7 x i8] c"String\00"
	const_value oprd_class_name(str_arr_type, str_class_name, true);
	vp.init_constant(str_const_name, oprd_class_name);
	// get the oprd_rep
	global_value oprd_class_name_ptr(str_arr_ptr_type, str_const_name);

	// init the class type
	vp.type_define(str_class_name, this->class_features);

	// init the class vtable type
	op_type i32_type(INT32), i8ptr_type(INT8_PTR);
	vector<op_type> vtable_types;
	vtable_types.push_back(i32_type);
	vtable_types.push_back(i32_type);
	vtable_types.push_back(i8ptr_type);
	for(int i = 0; i < this->class_vtable.size(); i++){
		vtable_types.push_back(this->class_vtable[i]);
	}
	vp.type_define(str_class_name + "_vtable", vtable_types);

	// create the vtable constant, perform bitcast if necessary, ValuePrinter won't work in this case
	// manual outstreaming
	ostream &out_stream = *this->class_table->ct_stream;
	// dec
	out_stream << "@" << str_class_name << "_vtable_prototype = constant %" <<
				str_class_name << "_vtable {\n";

	// add to ct mapping
	global_value oprd_const_global(op_type(str_class_name + "_vtable*"),
								str_class_name + "_vtable_prototype");
	this->class_table->map_class_name.push_back(str_class_name);
	this->class_table->map_global.push_back(oprd_const_global);

	// class tag
	out_stream << "\ti32 " << this->tag << ",\n";

	// intptr -> don't really understand why
	string str_class_ptr = "%" + str_class_name + "*";
	out_stream << "\ti32 ptrtoint (" << str_class_ptr << " getelementptr (" <<
				str_class_ptr << " null, i32 1) to i32),\n";

	// class name
	out_stream << "\ti8* getelementptr (" << str_arr_ptr_type.get_name() << " @"
				<< str_const_name << ", i32 0, i32 0),\n";

	// new
	out_stream << "\t" << str_class_ptr << " () * @" << str_class_name << "_new";

	// inherited -> skip first dummy
	for(int i = 1; i < this->inherited_original_vtable.size(); i++){
		string str_vtable_name = this->class_vtable_names[i];
		string str_self_type = this->class_vtable[i].get_name();
		string str_original_type = this->inherited_original_vtable[i].get_name();
		// print out
		out_stream << ",\n\t" << str_self_type << " bitcast (" << str_original_type
					<< " @" << str_vtable_name << " to " << str_self_type << ")";
	}

	// self
	for(int i = this->inherited_original_vtable.size(); i < this->class_vtable_names.size(); i++){
		string str_vtable_name = this->class_vtable_names[i];
		string str_vtable_type = this->class_vtable[i].get_name();
		// print out
		out_stream << ",\n\t" << str_vtable_type << " @" << str_vtable_name;
	}

	// finish
	out_stream << "\n}\n\n";
}

#ifdef MP4
void CgenClassTable::code_classes(CgenNode *c) {
	// ADD CODE HERE 
	c->code_class();
	List<CgenNode> *children = c->get_children();
	for (List<CgenNode> *child = children; child; child = child->tl()){
		code_classes(child->hd());
	}
}
#endif

// Create LLVM entry point. This function will initiate our Cool program by generating the code to execute (new Main).main()
void CgenClassTable::code_main(){
	if(cgen_debug){
		std::cerr << "code_main" << endl;
	}

	// use the given value printer and the table ostream
	ValuePrinter vp(*ct_stream);
	ostream &o = *ct_stream;


#ifndef MP4
	// global string and name
	string str_mainprint = "Main.main() returned %d\n";
	string str_mainprint_name = "main.printout.str";

	// types
	op_type i8ptr_type(INT8_PTR), i32_type(INT32), vararg_type(VAR_ARG);
	op_arr_type str_arr_25_type(INT8, str_mainprint.length() + 1), str_arr_25_ptr_type(INT8_PTR, str_mainprint.length() + 1);

	// init the global string
	const_value oprd_mainprint(str_arr_25_type, str_mainprint, false);
	vp.init_constant(str_mainprint_name, oprd_mainprint);
	global_value oprd_mainprint_ptr(str_arr_25_ptr_type, str_mainprint_name);
	
	// Define a function main that has no parameters and returns an i32
	vector<operand> main_args_type;
	vp.define(i32_type, "main", main_args_type);

	// Define an entry basic block
	vp.begin_block("entry");

	// Call Main_main(). This returns int for phase 1, Object for phase 2
	vector<op_type> main_main_args_type;
	vector<operand> main_main_args;
	operand oprd_tmp0 = vp.call(main_main_args_type, i32_type, "Main_main", true, main_main_args);

	// Get the address of the string "Main_main() returned %d\n" using getelementptr
	int_value oprd_int_zero(0);
	operand oprd_tmp1 = vp.getelementptr(oprd_mainprint_ptr, oprd_int_zero, oprd_int_zero, i8ptr_type);

	// Call printf with the string address of "Main_main() returned %d\n"
	// and the return value of Main_main() as its arguments
	vector<op_type> printf_args_type;
	printf_args_type.push_back(i8ptr_type);
	printf_args_type.push_back(vararg_type);
	vector<operand> printf_args;
	printf_args.push_back(oprd_tmp1);
	printf_args.push_back(oprd_tmp0);
	vp.call(printf_args_type, i32_type, "printf", true, printf_args);

	// Insert return 0
	vp.ret(oprd_int_zero);

#else
	// define i32 @main()
	o << "define i32 @main() {";

	//entry:
	vp.begin_block("entry");

	// %main.obj = call %Main*()* @Main_new( )
	vector<op_type> main_arg_types;
	vector<operand> main_args;
	op_type no_op_type;
	operand no_op;
	main_arg_types.push_back(no_op_type);
	main_args.push_back(no_op);

	op_type main_ptr_type("Main*");
	operand oprd_main_obj(main_ptr_type, "main.obj");
	vp.call(*ct_stream, main_arg_types, "Main_new", true, main_args, oprd_main_obj);

	//%main.retval = call RETURN_TYPE(%Main*)* @Main_main( %Main* %main.obj )
	vector<operand> main_retval_args;
	vector<op_type> main_retval_args_types;
	main_retval_args.push_back(oprd_main_obj);
	main_retval_args_types.push_back(main_ptr_type);

	op_type main_ret_type("Object*");
	operand oprd_main_ret(main_ret_type,"main.retval");
	vp.call(*ct_stream, main_retval_args_types, "Main_main", true, main_retval_args, oprd_main_ret);

	//ret i32 0
	int_value oprd_zero(0);
	vp.ret(oprd_zero);
#endif

	// end main function
	vp.end_define();
}

///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTable *ct)
	: class__class((const class__class &)*nd),
	parentnd(NULL),
	children(NULL),
	basic_status(bstatus),
	class_table(ct),
	tag(-1){
	// ADD CODE HERE
}

void CgenNode::add_child(CgenNode *n) {
	children = new List<CgenNode>(n, children);
}

void CgenNode::set_parentnd(CgenNode *p) {
	assert(parentnd == NULL);
	assert(p != NULL);
	parentnd = p;
}

// additioanl CgenNode access method: added by me

int CgenNode::get_feature_index(string name){
	for(int i = 0; i < this->class_feature_names.size(); i++){
		if(this->class_feature_names[i] == name){
			return i;
		}
	}
	return -1;
}

int CgenNode::get_vtable_index(string name){
	for(int i = 0; i < this->class_vtable_names.size(); i++){
		if(this->class_vtable_names[i] == name){
			return i;
		}
	}
	return -1;
}


// Class setup.  You may need to add parameters to this function so that
// the classtable can provide setup information (such as the class tag
// that should be used by this class).
//
// Things that setup should do:
//  - layout the features of the class
//  - create the types for the class and its vtable
//  - create global definitions used by the class such as the class vtable

// helper function
op_func_type inheritance_vtable_downgrade(op_func_type parent_vtable,
									string parent_vtable_name,
									string parent_class_name, string class_name){
	string parent_ptr_name = "%" + parent_class_name + "*";
	string class_ptr_name = class_name + "*";
	op_type class_ptr(class_ptr_name);

	// cast return type except for the function Object_abort
	op_type new_res_type;
	if(parent_vtable_name != "Object_abort" && parent_vtable.get_res_type().get_name() == parent_ptr_name){
		new_res_type = class_ptr;
	}else{
		new_res_type = parent_vtable.get_res_type();
	}

	// cast the formal types
	vector<op_type> new_arg_types;
	vector<op_type> old_arg_types = parent_vtable.get_arg_types();
	for(int i = 0; i < old_arg_types.size(); i++){
		if(old_arg_types[i].get_name() == parent_ptr_name){
			new_arg_types.push_back(class_ptr);
		}else{
			new_arg_types.push_back(old_arg_types[i]);
		}
	}

	// create the new op and return
	op_func_type class_vtable(new_res_type, new_arg_types);
	return class_vtable;
}

void CgenNode::setup(int tag_v, int depth) {
	if(cgen_debug){
		std::cerr << "Setup class: " << name->get_string() << endl;
	}

	// tag
	tag = tag_v;
#ifdef MP4
	string class_name(this->name->get_string());
	string vtable_ptr_name = class_name + "_vtable*";
	string self_ptr_name = class_name + "*";

	// before laying out features, I need to add the vtable pointers to type
	op_type vtable_ptr_type(vtable_ptr_name);
	// vtable pointer and name ref
	this->class_feature_names.push_back(CLASS_VTABLE);
	this->class_features.push_back(vtable_ptr_type);

	// then, deal with inheritance feature, skip the vtable one and inherit all others
	if(this->parentnd){
		// skip vtable pointer
		for(int i = 1; i < this->parentnd->class_feature_names.size(); i++){
			string parent_feature_name = this->parentnd->class_feature_names[i];
			op_type parent_feature = this->parentnd->class_features[i];
			// not vtable pointers
			this->class_feature_names.push_back(parent_feature_name);
			this->class_features.push_back(parent_feature);
		}
	}

	// add default fields for vtable
	op_type	self_ptr(self_ptr_name);
	vector<op_type> class_new_arg_types;
	op_func_type class_new_func_type(self_ptr, class_new_arg_types);
	// tag, intptr, name string -> no need to keep track of in the vector
	// pointer to new
	this->class_vtable_names.push_back(class_name + "_new");
	this->class_vtable.push_back(class_new_func_type);

	// push a dummy entry -> the field for new to align
	this->inherited_original_vtable.push_back(class_new_func_type);

	// then, deal with inherited vtables, keep track of the original type for a later bitcast
	if(this->parentnd){
		string parent_class_name(this->parentnd->get_type_name());
		// skip new
		for(int i = 1; i < this->parentnd->class_vtable_names.size(); i++){
			string parent_vtable_name = this->parentnd->class_vtable_names[i];
			op_func_type parent_vtable = this->parentnd->class_vtable[i];
			op_func_type self_vtable = inheritance_vtable_downgrade(parent_vtable, parent_vtable_name,
										parent_class_name, class_name);

			if(i < this->parentnd->inherited_original_vtable.size()){
				// super-super class -> needs to record its original type
				op_func_type parent_vtable_original = this->parentnd->inherited_original_vtable[i];
				this->class_vtable_names.push_back(parent_vtable_name);
				this->class_vtable.push_back(self_vtable);
				this->inherited_original_vtable.push_back(parent_vtable_original);
			}else{
				// just super class
				this->class_vtable_names.push_back(parent_vtable_name);
				this->class_vtable.push_back(self_vtable);
				this->inherited_original_vtable.push_back(parent_vtable);
			}
		}
	}

	// then layout features based on self functions & attributes
	layout_features();

	// ADD CODE HERE

#endif
}

#ifdef MP4
// Class codegen. This should performed after every class has been setup.
// Generate code for each method of the class.
void CgenNode::code_class() {
	// No code generation for basic classes. The runtime will handle that.
	if (basic())
		return;

	// ADD CODE HERE

	// firstly, code all methods
	CgenEnvironment *env = new CgenEnvironment(*(this->get_classtable()->ct_stream), this);
	env->methods_only = true;
	for(int i = this->features->first(); features->more(i); i = this->features->next(i)){
		env->enter_local();
		this->features->nth(i)->code(env);
		env->kill_local();
	}

	// then set up the class_new method, code attribute only on the way
	ValuePrinter vp(*(env->cur_stream));
	ostream &o = *(env->cur_stream);

	// types
	op_type i8ptr_type(INT8_PTR), i32_type(INT32), i32ptr_type(INT32_PTR);
	// const
	int_value oprd_zero(0), oprd_one(1);

	string str_class_name = this->get_type_name();
	op_type class_ptr_type(str_class_name + "*");
	vector<operand> class_args;
	vp.define(class_ptr_type, this->get_type_name() + "_new", class_args);

	// entry block
	vp.begin_block("entry");

	// get object size
	global_value oprd_class_vtable_ptr(op_type(this->get_type_name() + "_vtable*"), str_class_name + "_vtable_prototype");
	operand oprd_size_ptr(i32ptr_type, env->new_name());
	vp.getelementptr(o, oprd_class_vtable_ptr, oprd_zero, oprd_one, oprd_size_ptr);

	// load object size
	operand oprd_size(i32_type, env->new_name());
	vp.load(o, oprd_size_ptr, oprd_size);

	// allocate object memory
	vector<op_type> malloc_arg_types;
	vector<operand> malloc_args;
	malloc_arg_types.push_back(i32_type);
	malloc_args.push_back(oprd_size);
	operand oprd_malloc_ptr(i8ptr_type, env->new_name());
	vp.call(o, malloc_arg_types, "malloc", true, malloc_args, oprd_malloc_ptr); 

	// cast to class ptr
	operand oprd_class_ptr(class_ptr_type, env->new_name());
	vp.bitcast(o, oprd_malloc_ptr, class_ptr_type, oprd_class_ptr);

	// get the pointer to the object vtable pointer, for attribute init
	operand oprd_class_vtable_ptr_ptr(op_type(str_class_name + "_vtable**"), env->new_name());
	vp.getelementptr(o, oprd_class_ptr, oprd_zero, oprd_zero, oprd_class_vtable_ptr_ptr);

	// store the vtable pointer
	vp.store(o, oprd_class_vtable_ptr, oprd_class_vtable_ptr_ptr);

	// allocate an additional pointer pointer
	op_type class_ptr_ptr_type(str_class_name + "**");
	operand oprd_class_ptr_ptr(class_ptr_ptr_type, env->new_name());
	vp.alloca_mem(o, class_ptr_type, oprd_class_ptr_ptr);

	// store temp pointer value onto it
	vp.store(o, oprd_class_ptr, oprd_class_ptr_ptr);

	// init attributes, with the self
	env->add_local(self, oprd_class_ptr);
	env->methods_only = false;
	for(int i = this->features->first(); this->features->more(i); i = this->features->next(i)){
		this->features->nth(i)->code(env);
	}
	env->kill_local();

	// return class_ptr
	vp.ret(o, oprd_class_ptr);

	// abort section
	vp.begin_block("abort");
	vector<op_type> abort_args_types;
	vector<operand> abort_args;
	vp.call(abort_args_types, op_type(VOID), "abort", true, abort_args);
	vp.unreachable();

	// free up space
	delete env;

	// done with defining the class_new() func
	vp.end_define();
}

// Laying out the features involves creating a Function for each method
// and assigning each attribute a slot in the class structure.
void CgenNode::layout_features() {
	// ADD CODE HERE
	for(int i = features->first(); features->more(i); i = features->next(i)){
		features->nth(i)->layout_feature(this);
	}
}

// helper func
op_type process_method_type(string x, string class_name){
	if(x == "SELF_TYPE"){
		op_type temp(class_name + "*");
		return temp;
	}if(x == "Bool" || x == "bool"){
		op_type temp(INT1);
		return temp;
	}else if(x == "Int" || x == "int"){
		op_type temp(INT32);
		return temp;
	}else if(x == "sbyte*"){
		op_type temp(INT8_PTR);
		return temp;
	}else{
		op_type temp(x + "*");
		return temp;
	}
}

// Create the LLVM Function corresponding to this method.
void method_class::layout_feature(CgenNode *cls) {
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	// ADD CODE HERE

	// method will be added to the vtable
	string class_name = cls->get_type_name();
	op_type class_ptr(class_name + "*");
	operand oprd_self(class_ptr, "self");

	// return type
	vector<op_type> formal_types;
	// push self
	formal_types.push_back(class_ptr);
	// other formals
	for(int i = this->formals->first(); this->formals->more(i); i = this->formals->next(i)){
		string this_formal_type_name(this->formals->nth(i)->get_type_decl()->get_string());
		op_type formal_type = process_method_type(this_formal_type_name, class_name);
		formal_types.push_back(formal_type);
	}

	// return type
	string ret_type_name(this->return_type->get_string());
	op_type ret_type = process_method_type(ret_type_name, class_name);

	// the total func type
	op_func_type this_method(ret_type, formal_types);

	// function name
	string func_name(this->name->get_string());

	// append to vtable
	cls->class_vtable_names.push_back(class_name + "_" + func_name);
	cls->class_vtable.push_back(this_method);

#endif
}

// Assign this attribute a slot in the class structure
void attr_class::layout_feature(CgenNode *cls) {
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	// ADD CODE HERE
	string attr_name(this->name->get_string());
	op_type attr_type = process_method_type(this->type_decl->get_string(), cls->get_type_name());
	cls->class_feature_names.push_back(attr_name);
	cls->class_features.push_back(attr_type);
#endif
}

#else

// code-gen function main() in class Main
void CgenNode::codeGenMainmain() {
	ValuePrinter vp;

	// In Phase 1, this can only be class Main.  Get method_class for main().
	assert(std::string(this->name->get_string()) == std::string("Main"));
	method_class *mainMethod = (method_class *) features->nth(features->first());

	// ADD CODE HERE TO GENERATE THE FUNCTION int Mainmain().
	// Generally what you need to do are:
	// -- setup or create the environment, env, for translating this method
	CgenEnvironment *env = new CgenEnvironment(*this->class_table->ct_stream, this);

	// -- invoke mainMethod->code(env) to translate the method
	mainMethod->code(env);

	// maybe?
	delete env;
}

#endif

///////////////////////////////////////////////////////////////////////
//
// CgenEnvironment Methods
//
///////////////////////////////////////////////////////////////////////

// Class CgenEnvironment should be constructed by a class prior to code
// generation for each method.  You may need to add parameters to this
// constructor.
CgenEnvironment::CgenEnvironment(std::ostream &o, CgenNode *c) {
	cur_class = c;
	cur_stream = &o;
	var_table.enterscope();
	tmp_count = 0;
	block_count = 0;
	ok_count = 0;

	// ADD CODE HERE
}

// Look up a CgenNode given a symbol
CgenNode *CgenEnvironment::type_to_class(Symbol t) {
	return t == SELF_TYPE ? get_class() : get_class()->get_classtable()->lookup(t);
}

// Provided CgenEnvironment methods
// Generate unique string names
std::string CgenEnvironment::new_name() {
	std::stringstream s;
	s << tmp_count++;
	return "tmp." + s.str();
}

std::string CgenEnvironment::new_ok_label() {
	std::stringstream s;
	s << ok_count++;
	return "ok." + s.str();
}

const std::string CgenEnvironment::new_label(const std::string &prefix, bool increment) {
	std::string suffix = itos(block_count);
	block_count += increment;
	return prefix + suffix;
}

void CgenEnvironment::enter_local() {
	var_table.enterscope();
}

void CgenEnvironment::add_local(Symbol name, operand &vb) {
	var_table.enterscope();
	var_table.addid(name, &vb);
}

void CgenEnvironment::kill_local() {
	var_table.exitscope();
}

// added helper

int CgenEnvironment::get_formal_index(string name){
	for(int i = 0; i < this->map_formal_string.size(); i++){
		if(this->map_formal_string[i] == name){
			return i;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////
//
// APS class methods
//
////////////////////////////////////////////////////////////////////////////

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.handcode.h'.
//
//*****************************************************************

//
// helper function for code
//

op_type CgenNode::process_type_in_class(string str_type){
	if(str_type == "SELF_TYPE"){
		op_type temp(this->get_type_name() + "*");
		return temp;
	}else if(str_type == "Bool" || str_type == "bool"){
		return INT1;
	}else if(str_type == "Int" || str_type == "int"){
		return INT32;
	}else{
		op_type temp(str_type + "*");
		return temp;
	}
}

op_type get_type_from_string(const string type_str){
	if(type_str.compare("Int") == 0){
		op_type int32_type(INT32);
		return int32_type;
	}else if(type_str.compare("Bool") == 0){
		op_type int1_type(INT1);
		return int1_type;
	}else if(type_str.compare("String") == 0){
		op_type int8ptr_type(INT8_PTR);
		return int8ptr_type;
	}else{
		op_type obj_type(OBJ);
		return obj_type;
	}
}

operand get_default_value_for_type(op_type type){
	if(type.get_id() == INT1){
		bool_value false_val(false, true);
		return false_val;
	}else if(type.get_id() == INT32){
		int_value int_val(0);
		return int_val;
	}else{
		// Phase 2:
		operand oprd_null(type, "null");
		return oprd_null;
	}
}

//
// Attribute init -> in new
//

void attr_class::code(CgenEnvironment *env) {
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	// ADD CODE HERE

	// if it is the first pass of the features, do nothing
    if(env->methods_only){
    	return;
    }

    // it is the second pass, do necessary initialization using the self pointer
    if (cgen_debug){
		std::cerr << "attribute: " << this->name->get_string() << endl;
	}

    ValuePrinter vp(*(env->cur_stream));
    ostream &o = *(env->cur_stream);
    string str_feature_name(this->name->get_string());

    // look up attribute index and name, form the operand
	int attr_index = env->get_class()->get_feature_index(str_feature_name);
	op_type feature_type = env->get_class()->class_features[attr_index];
	operand oprd_attr(feature_type, env->new_name());

	// evaluate the expression
	operand oprd_attr_expr = init->code(env);
	if(oprd_attr_expr.get_name() == ""){
		// do the default value
		oprd_attr_expr = get_default_value_for_type(feature_type);
	}

	// loop up self pointer
	operand &oprd_class_ptr = *env->lookup(self);
	
	// get pointer to that attribute
	vp.getelementptr(o, oprd_class_ptr, int_value(0), int_value(attr_index), oprd_attr);
	
	// store value
	vp.store(o, oprd_attr_expr, oprd_attr);
#endif
}

//
// Method
//

void method_class::code(CgenEnvironment *env) {
	// if it is the second pass->new, do nothing
	if(!(env->methods_only)){
		return;
	}

	if (cgen_debug) {
		std::cerr << "method: " << this->name->get_string() << endl;
	}

	// ADD CODE HERE
	ValuePrinter vp(*env->cur_stream);
	ostream &o = *(env->cur_stream);

	// get the return type and class_type
	op_type ret_type = env->get_class()->process_type_in_class(return_type->get_string());
	op_type class_ptr_type(env->get_class()->get_type_name() + "*");

	// get function name ClassName_funcname
	string str_func_name(env->get_class()->get_type_name() + "_" + string(name->get_string()));

	// prepare the formals

	// clear out map
	vector<string> new_str_vec;
	vector<operand> new_oprd_vec;
	env->map_formal_string = new_str_vec;
	env->map_formal_operand = new_oprd_vec;

	// implicit self
	operand oprd_self(class_ptr_type, "self");
	env->map_formal_string.push_back("self");
	env->map_formal_operand.push_back(oprd_self);

	// other formals
	for(int i = this->formals->first(); this->formals->more(i); i = this->formals->next(i)){
		// construct formal
		op_type formal_type = env->get_class()->process_type_in_class(this->formals->nth(i)->get_type_decl()->get_string());
		string formal_name(this->formals->nth(i)->get_name()->get_string());
		operand oprd_formal(formal_type, formal_name);
		// push to env
		env->map_formal_string.push_back(formal_name);
		env->map_formal_operand.push_back(oprd_formal);
	}

	// define the function
	vp.define(ret_type, str_func_name, env->map_formal_operand);

	// do the entry block
	vp.begin_block("entry");

	// allocate formal and enter scope
	for(int i = 0; i < env->map_formal_string.size(); i++){
		// get the ref type and operand for that formal
		op_type formal_type = env->map_formal_operand[i].get_type();
		// allocate memory
		operand oprd_formal_alloca(formal_type.get_ptr_type(), env->new_name());
		vp.alloca_mem(o, formal_type, oprd_formal_alloca);
		// store
		vp.store(env->map_formal_operand[i], oprd_formal_alloca);
		// update the vector
		env->map_formal_operand[i] = oprd_formal_alloca;
	}

	// generate the expression
	operand oprd_rslt = expr->code(env);

	// do bitcast if necessary
	if(oprd_rslt.get_type().get_name() != ret_type.get_name()){
		operand oprd_bitcast(ret_type, env->new_name());
		vp.bitcast(o, oprd_rslt, ret_type, oprd_bitcast);
		vp.ret(oprd_bitcast);
	}else{
		vp.ret(oprd_rslt);
	}

	// do the abort section at the end
	vp.begin_block("abort");
	vector<op_type> abort_args_types;
	vector<operand> abort_args;
	vp.call(abort_args_types, op_type(VOID), "abort", true, abort_args);
	vp.unreachable();

	// end function definition
	vp.end_define();
}

//
// Codegen for expressions.  Note that each expression has a value.
//

operand assign_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "assign" << endl;
	
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL

	// bit cast needed for assignment

	// compute the right hand side first
	operand rhs = expr->code(env);
	operand lhs;

	// printer
	ValuePrinter vp(*env->cur_stream);
	ostream &o = *env->cur_stream;
	
	// local env first

	operand *oprd_local = env->lookup(this->name);
	if(oprd_local){
		lhs = *oprd_local;
	}

	// then formals

	string str_name(this->name->get_string());

	int formal_index = env->get_formal_index(str_name);
	if(formal_index >= 0){
		lhs = env->map_formal_operand[formal_index];
	}

	// then, attribute

	operand oprd_self_ptr = env->map_formal_operand[0];

	// load self
	operand oprd_self = operand(oprd_self_ptr.get_type().get_deref_type(), env->new_name());
	vp.load(o, oprd_self_ptr, oprd_self);

	// attribute indexing
	int attr_index = env->get_class()->get_feature_index(str_name);
	op_type attr_type = env->get_class()->class_features[attr_index];

	if(attr_index >= 0){
		// get the pointer
		operand oprd_attr_ptr(attr_type.get_ptr_type(), env->new_name());
		vp.getelementptr(o, oprd_self, int_value(0), int_value(attr_index), oprd_attr_ptr);
		// store the value
		lhs = oprd_attr_ptr;
	}

	// do bitcast if necessary
	op_type lhs_deref_type = lhs.get_type().get_deref_type();
	if(lhs_deref_type.get_name() != rhs.get_type().get_name()){
		operand oprd_bitcast(lhs_deref_type, env->new_name());
		// bitcast and store
		vp.bitcast(o, rhs, lhs_deref_type, oprd_bitcast);
		vp.store(oprd_bitcast, lhs);
		return oprd_bitcast;
	}else{
		vp.store(rhs, lhs);
		return rhs;
	}
}

operand cond_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "cond" << endl;
	
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	// get necessary starting register and labels
	string str_mergetmp = env->new_name();
	string label_true(env->new_label("true.", false));
	string label_false(env->new_label("false.", false));
	string label_end(env->new_label("end.", true));

	// local buff string to delay the branch body -> get the type
	std::stringstream branch_stream;
	ValuePrinter vp(branch_stream);

	// conditional branch
	operand oprd_cond = this->pred->code(env);
	vp.branch_cond(oprd_cond, label_true, label_false);

	// do the true block
	vp.begin_block(label_true);
	operand oprd_true = this->then_exp->code(env);

	// then we know the type
	op_type type_branch = oprd_true.get_type();
	operand oprd_branch = operand(type_branch.get_ptr_type(), str_mergetmp);
	
	// finish the true branch
	vp.store(oprd_true, oprd_branch);
	vp.branch_uncond(label_end);

	// do the false block
	vp.begin_block(label_false);
	operand oprd_false = this->else_exp->code(env);
	vp.store(oprd_false, oprd_branch);
	vp.branch_uncond(label_end);

	// do the end block
	vp.begin_block(label_end);
	string str_loadtmp = env->new_name();
	operand oprd_load = operand(type_branch, str_loadtmp);
	vp.load(branch_stream, oprd_branch, oprd_load);

	// get the substream onto the main out stream
	ValuePrinter vp_main(*env->cur_stream);
	vp_main.alloca_mem(*env->cur_stream, type_branch, oprd_branch);
	*env->cur_stream << branch_stream.str();

	// operand load is the final value of the branch statement
	return oprd_load;
}

operand loop_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "loop" << endl;
	
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// get the loop labels
	string label_true(env->new_label("true.", false));
	string label_false(env->new_label("false.", false));
	string label_loop(env->new_label("loop.", true));

	// first uncond jump -> no idea why necessary
	vp.branch_uncond(label_loop);

	// do loop block
	vp.begin_block(label_loop);
	operand oprd_pred = this->pred->code(env);
	vp.branch_cond(oprd_pred, label_true, label_false);

	// do true block
	vp.begin_block(label_true);
	this->body->code(env);
	vp.branch_uncond(label_loop);

	// do the false block
	vp.begin_block(label_false);

	// MP3: int value 0
	int_value oprd_zero(0);
	return oprd_zero;
}

operand block_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "block" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	// keep track of last operand
	operand oprd_last;

	// traverse through the expressions and generate
	for (int i = this->body->first(); this->body->more(i); i = this->body->next(i)) {
		oprd_last = this->body->nth(i)->code(env);
	}

	// block value is the value of the last expression
	return oprd_last;
}

operand let_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "let" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// conver the init part first
	operand oprd_init = this->init->code(env);

	// get the init -> new scope
	string str_type(this->type_decl->get_string());
	string str_idtmp = env->new_name();
	// get type - local helper function
	op_type type_id = get_type_from_string(str_type);
	operand oprd_id = operand(type_id.get_ptr_type(), str_idtmp);

	// no init
	if(oprd_init.get_type().get_id() == EMPTY){
		oprd_init = get_default_value_for_type(type_id);
	}
	
	// allocate and store
	vp.alloca_mem(*env->cur_stream, type_id, oprd_id);
	vp.store(oprd_init, oprd_id);

	// enter scope
	env->add_local(this->identifier, oprd_id);
	// finish the rest of the expression
	operand oprd_let = this->body->code(env);
	// exit scope
	env->kill_local();

	// the let operand is the value of this statement
	return oprd_let;
}

operand plus_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "plus" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	string str_add = env->new_name();
	operand oprd_add = operand(oprd_left.get_type(), str_add);
	vp.add(*env->cur_stream, oprd_left, oprd_right, oprd_add);

	return oprd_add;
}

operand sub_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "sub" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	string str_sub = env->new_name();
	operand oprd_sub = operand(oprd_left.get_type(), str_sub);
	vp.sub(*env->cur_stream, oprd_left, oprd_right, oprd_sub);

	return oprd_sub;
}

operand mul_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "mul" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	string str_mul = env->new_name();
	operand oprd_mul = operand(oprd_left.get_type(), str_mul);
	vp.mul(*env->cur_stream, oprd_left, oprd_right, oprd_mul);

	return oprd_mul;
}

operand divide_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "div" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	string str_div = env->new_name();
	operand oprd_div = operand(oprd_left.get_type(), str_div);
	vp.div(*env->cur_stream, oprd_left, oprd_right, oprd_div);

	return oprd_div;
}

operand neg_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "neg" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// get the neg part
	operand oprd_negexp = this->e1->code(env);
	// do the neg
	string str_neg = env->new_name();
	operand oprd_neg = operand(oprd_negexp.get_type(), str_neg);
	int_value oprd_negone(-1);
	vp.mul(*env->cur_stream, oprd_negone, oprd_negexp, oprd_neg);

	return oprd_neg;
}

operand lt_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "lt" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	op_type bool_type(INT1);
	string str_lt = env->new_name();
	operand oprd_lt = operand(bool_type, str_lt);
	// generate code using icmp
	vp.icmp(*env->cur_stream, LT, oprd_left, oprd_right, oprd_lt);

	return oprd_lt;
}

operand eq_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "eq" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	op_type bool_type(INT1);
	string str_eq = env->new_name();
	operand oprd_eq = operand(bool_type, str_eq);
	// generate code using icmp
	vp.icmp(*env->cur_stream, EQ, oprd_left, oprd_right, oprd_eq);

	return oprd_eq;
}

operand leq_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "leq" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// evaluate left and right
	operand oprd_left = this->e1->code(env);
	operand oprd_right = this->e2->code(env);

	// get the new name and add
	op_type bool_type(INT1);
	string str_leq = env->new_name();
	operand oprd_leq = operand(bool_type, str_leq);
	// generate code using icmp
	vp.icmp(*env->cur_stream, LE, oprd_left, oprd_right, oprd_leq);

	return oprd_leq;
}

operand comp_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "complement" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);

	// get the comp part
	operand oprd_compexp = this->e1->code(env);

	// do the comp
	string str_comp = env->new_name();
	operand oprd_comp = operand(oprd_compexp.get_type(), str_comp);
	bool_value oprd_true(true, true);
	vp.xor_in(*env->cur_stream, oprd_compexp, oprd_true, oprd_comp);

	return oprd_comp;
}

operand int_const_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "Integer Constant" << endl;
	
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	int val = atoi(this->token->get_string());
	int_value oprd_int(val);
	return oprd_int;
}

operand bool_const_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "Boolean Constant" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	bool val = this->val == 1;
	bool_value oprd_bool(val, true);
	return oprd_bool;
}

operand string_const_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "string_const" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	int const_index = env->get_class()->get_classtable()->get_global_str_index(string(this->token->get_string()));
	if(const_index >= 0){
		// directly return the string const object
		return env->get_class()->get_classtable()->map_global[const_index];
	}else{
		// no such string const
		operand nothing;
		return nothing;
	}
#endif
}

operand object_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "Object" << endl;

	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	ValuePrinter vp(*env->cur_stream);
	ostream &o = *env->cur_stream;

	// prioritize local env

	// find
	operand *oprd_id = env->lookup(this->name);
	if(oprd_id){
		// load
		operand oprd_load(oprd_id->get_type().get_deref_type(), env->new_name());
		vp.load(o, *oprd_id, oprd_load);
		return oprd_load;
	}

	// then formals
	string str_name(this->name->get_string());

	int formal_index = env->get_formal_index(str_name);
	if(formal_index >= 0){
		// load
		operand oprd_formal = env->map_formal_operand[formal_index];
		operand oprd_load(oprd_formal.get_type().get_deref_type(), env->new_name());
		vp.load(o, oprd_formal, oprd_load);
		return oprd_load;
	}

	// then, attribute
	operand oprd_self_ptr = env->map_formal_operand[0];

	// load self
	operand oprd_self = operand(oprd_self_ptr.get_type().get_deref_type(), env->new_name());
	vp.load(o, oprd_self_ptr, oprd_self);

	// attribute indexing
	int attr_index = env->get_class()->get_feature_index(str_name);
	
	if(attr_index >= 0){
		// get the pointer
		op_type attr_type = env->get_class()->class_features[attr_index];
		operand oprd_attr_ptr(attr_type.get_ptr_type(), env->new_name());
		vp.getelementptr(o, oprd_self, int_value(0), int_value(attr_index), oprd_attr_ptr);
		// load the value from pointer
		operand oprd_load(attr_type, env->new_name());
		vp.load(o, oprd_attr_ptr, oprd_load);
		return oprd_load;
	}else{
		// a semantic error, return nothing
		operand nothing;
		return nothing;
	}
}

operand no_expr_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "No_expr" << endl;
	
	// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING MORE MEANINGFUL
	// TODO: may be needed in Phase 2
	operand nothing;
	return nothing;
}

//*****************************************************************
// The next few functions are for node types not supported in Phase 1
// but these functions must be defined because they are declared as
// methods via the Expression_SHARED_EXTRAS hack.
//*****************************************************************

operand static_dispatch_class::code(CgenEnvironment *env) {
	operand nothing;
	if (cgen_debug)
		std::cerr << "static dispatch" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
		// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING
		// MORE MEANINGFUL
#endif
	return nothing;
}

operand dispatch_class::code(CgenEnvironment *env) {
	operand nothing;
	if (cgen_debug)
		std::cerr << "dispatch" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
		// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING
		// MORE MEANINGFUL
#endif
	return nothing;
}

// conform - If necessary, emit a bitcast or boxing/unboxing operations
// to convert an object to a new type. This can assume the object
// is known to be (dynamically) compatible with the target type.
// It should only be called when this condition holds.
// (It's needed by the supplied code for typecase)
operand conform(operand src, op_type type, CgenEnvironment *env) {
	// ADD CODE HERE - may not be needed in Phase 1
	return operand();
}

// Retrieve the class tag from an object record.
// src is the object we need the tag from.
// src_class is the CgenNode for the *static* class of the expression.
// You need to look up and return the class tag for it's dynamic value
operand get_class_tag(operand src, CgenNode *src_cls, CgenEnvironment *env) {
	// ADD CODE HERE - may not be needed in Phase 1
	return operand();
}

operand typcase_class::code(CgenEnvironment *env) {
	if (cgen_debug)
		std::cerr << "typecase::code()" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	ValuePrinter vp;
	ostream *o = env->cur_stream;
	CgenClassTable *ct = env->get_class()->get_classtable();

	string header_label = env->new_label("case.hdr.", false);
	string exit_label = env->new_label("case.exit.", false);
	operand code_val = expr->code(env);
	operand expr_val = code_val;
	string code_val_t = code_val.get_typename();
	op_type join_type = env->type_to_class(type)->get_type_name();
	CgenNode *cls = env->type_to_class(expr->get_type());
	// Check for case on void
	if ((code_val_t != string("%Int*")) && (code_val_t != string("%Bool*"))) {
		op_type bool_type(INT1), empty_type;
		null_value null_op(code_val.get_type());
		operand icmp_result(bool_type, env->new_name());
		vp.icmp(*o, EQ, code_val, null_op, icmp_result);
		string ok_label = env->new_ok_label();
		vp.branch_cond(*o, icmp_result, "abort", ok_label);
		*o << "\n" << ok_label << ":\n";
	}
	operand tag = get_class_tag(expr_val, cls, env);
	vp.branch_uncond(*o, header_label);
	string prev_label = header_label;
	*o << "\n" << header_label << ":\n";
	branch_class *b = (branch_class *)cases->nth(cases->first());
	string case_result_type = b->get_expr()->get_type()->get_string();
	if (case_result_type == "SELF_TYPE")
		case_result_type = env->get_class()->get_type_name();
	op_type alloca_type(case_result_type, 1);
	operand alloca_final(alloca_type, env->new_name());
	env->branch_operand = alloca_final;
	vp.alloca_mem(*o, alloca_type, alloca_final);
	std::vector<operand> values;
	std::vector<string> labels;
	env->next_label = exit_label;
	bool done;
	for (int i = ct->get_num_classes() - 1; i >= 0; --i) {
		for (int j = cases->first(); cases->more(j); j = cases->next(j)) {
			if (i == ct->lookup(cases->nth(j)->get_type_decl())->get_tag()) {
				done = i;
				string prefix = string("case.") + itos(i) + ".";
				string case_label = env->new_label(prefix, false);
				vp.branch_uncond(*o, case_label);
				*o << "\n" << case_label << ":\n";
				operand val = cases->nth(j)->code(expr_val, tag, join_type, env);
				values.push_back(val);
			}
		}
	}
	vp.branch_uncond(*o, "abort");
	env->new_label("", true);
	*o << "\n" << exit_label << ":\n";
	operand final_result(alloca_type, env->new_name());
	alloca_final.set_type(alloca_final.get_type().get_ptr_type());
	vp.load(*o, alloca_final, final_result);
	alloca_final.set_type(alloca_final.get_type().get_deref_type());

	if (cgen_debug)
		cerr << "Done typcase::code()" << endl;
	return final_result;
#endif
}

operand new__class::code(CgenEnvironment *env) {
	operand nothing;
	if (cgen_debug)
		std::cerr << "newClass" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
		// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING
		// MORE MEANINGFUL
#endif
	return nothing;
	;
}

operand isvoid_class::code(CgenEnvironment *env) {
	operand nothing;
	if (cgen_debug)
		std::cerr << "isvoid" << endl;
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
		// ADD CODE HERE AND REPLACE "return nothing" WITH SOMETHING
		// MORE MEANINGFUL
#endif
	return nothing;
	;
}

// If the source tag is >= the branch tag and <= (max child of the branch class)
// tag, then the branch is a superclass of the source
operand branch_class::code(operand expr_val, operand tag, op_type join_type,
													 CgenEnvironment *env) {
#ifndef MP4
	assert(0 && "Unsupported case for phase 1");
#else
	operand empty;
	ostream *o = env->cur_stream;
	ValuePrinter vp(*o);
	if (cgen_debug)
		cerr << "In branch_class::code()" << endl;

	CgenNode *cls = env->get_class()->get_classtable()->lookup(type_decl);
	int my_tag = cls->get_tag();
	int max_child = cls->get_max_child();

	// Generate unique labels for branching into >= branch tag and <= max child
	string sg_label =
			env->new_label(string("src_gte_br") + "." + itos(my_tag) + ".", false);
	string sl_label =
			env->new_label(string("src_lte_mc") + "." + itos(my_tag) + ".", false);
	string exit_label =
			env->new_label(string("br_exit") + "." + itos(my_tag) + ".", false);

	op_type bool_type(INT1);
	int_value my_tag_val(my_tag);
	op_type old_tag_t(tag.get_type()), i32_t(INT32);
	tag.set_type(i32_t);

	// Compare the source tag to the class tag
	operand icmp_result = vp.icmp(LT, tag, my_tag_val);
	vp.branch_cond(icmp_result, exit_label, sg_label);
	*o << "\n" << sg_label << ":\n";
	int_value max_child_val(max_child);

	// Compare the source tag to max child
	operand icmp2_result = vp.icmp(GT, tag, max_child_val);
	vp.branch_cond(icmp2_result, exit_label, sl_label);
	*o << "\n" << sl_label << ":\n";
	tag.set_type(old_tag_t);

	// Handle casts of *arbitrary* types to Int or Bool.  We need to:
	// (a) cast expr_val to boxed type (struct Int* or struct Bool*)
	// (b) unwrap value field from the boxed type
	// At run-time, if source object is not Int or Bool, this will never
	// be invoked (assuming no bugs in the type checker).
	if (cls->get_type_name() == "Int" || cls->get_type_name() == "Bool") {
		op_type lbl_t(cls->get_type_name(), 1);
		expr_val = conform(expr_val, lbl_t, env);
	}
	op_type alloc_type(cls->get_type_name(), 1);
	operand alloc_op = vp.alloca_mem(alloc_type);
	operand conf_result = conform(expr_val, alloc_type, env);
	vp.store(conf_result, alloc_op);
	env->add_local(name, alloc_op);
	operand val = conform(expr->code(env), join_type.get_ptr_type(), env);
	operand conformed = conform(val, env->branch_operand.get_type(), env);
	env->branch_operand.set_type(env->branch_operand.get_type().get_ptr_type());
	vp.store(conformed, env->branch_operand);
	env->branch_operand.set_type(env->branch_operand.get_type().get_deref_type());
	env->kill_local();
	vp.branch_uncond(env->next_label);
	*o << "\n" << exit_label << ":\n";
	if (cgen_debug)
		cerr << "Done branch_class::code()" << endl;
	return conformed;
#endif
}
