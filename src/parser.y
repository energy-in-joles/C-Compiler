// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

// TODO: you can either continue adding to this grammar file or
// rename parser_full.y to parser.y once you're happy with
// how this example works.

%debug
%code requires{
    #include "ast.hpp"

	using namespace ast;

	extern int yylineno;
	extern char* yytext;
	extern Node* g_root;
	extern FILE* yyin;

	int yylex(void);
	void yyerror(const char*);
	int yylex_destroy(void);

}

%define parse.error detailed
%define parse.lac full

%union {
  Node*				    node;
  NodeList*			  node_list;
  int          		number_int;
  double       		number_float;
  std::string*		string; // identifier
  TypeSpecifier 	type_specifier;
  yytokentype  		token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL CHAR_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token <string> MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN '='
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token UNKNOWN

%type <node> external_declaration function_definition declaration primary_expression postfix_expression
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression expression declarator direct_declarator statement compound_statement jump_statement iteration_statement selection_statement
%type <node> init_declarator labeled_statement
%type <node> parameter_declaration
%type <node> initializer enum_specifier enumerator enum_declaration
%type <node> expression_statement
%type <node_list> statement_list init_declarator_list parameter_list argument_expression_list translation_unit enumerator_list initializer_list
%type <string> assignment_operator
%type <number_int> INT_CONSTANT pointer
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER CHAR_LITERAL STRING_LITERAL
%type <type_specifier> type_specifier declaration_specifiers

%nonassoc NOELSE
%nonassoc ELSE
%nonassoc '?'
%nonassoc ':'

%start ROOT
%%

ROOT
	: translation_unit { g_root = new TranslationUnit(NodePtr($1)); }

translation_unit
	: external_declaration { $$ = new NodeList(NodePtr($1)); }
	| translation_unit external_declaration {
		$1->PushBack(NodePtr($2));
		$$ = $1;
	}
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration { $$ = $1; }
	;

function_definition
	: declaration_specifiers declarator compound_statement {
		$$ = new FunctionDefinition($1, NodePtr($2), NodePtr($3));
	}
	| declarator compound_statement {
		$$ = new FunctionDefinition(TypeSpecifier::INT, NodePtr($1), NodePtr($2));
	}
	;

declaration
	: declaration_specifiers init_declarator_list ';' {
		$$ = new Declaration($1, NodePtr($2));
	}
    | enum_declaration { $$ = $1; }
	;

declaration_specifiers
	: type_specifier { $$ = $1; }
	;

type_specifier
	: VOID {
		$$ = TypeSpecifier::VOID;
	}
	| INT {
		$$ = TypeSpecifier::INT;
	}
    | FLOAT {
        $$ = TypeSpecifier::FLOAT;
    }
    | DOUBLE {
        $$ = TypeSpecifier::DOUBLE;
    }
	| CHAR {
		$$ = TypeSpecifier::CHAR;
	}
	| UNSIGNED {
		$$ = TypeSpecifier::UNSIGNED;
	}
	;

init_declarator_list
	: init_declarator {
		$$ = new NodeList(NodePtr($1));
	}
	| init_declarator_list ',' init_declarator {
		$1->PushBack(NodePtr($3));
		$$ = $1;
	}
	;

init_declarator
	: declarator {$$ = new InitDeclarator(NodePtr($1), nullptr);}
    | declarator '=' initializer { $$ = new InitDeclarator(NodePtr($1), NodePtr($3)); }
	;

enum_declaration
    : enum_specifier ';' { $$ = $1; }
    ;

enum_specifier
    : ENUM IDENTIFIER '{' enumerator_list '}' { $$ = new EnumDeclaration(*$2, NodePtr($4)); delete $2; }
    | ENUM '{' enumerator_list '}' { $$ = new EnumDeclaration("", NodePtr($3)); }
    | ENUM IDENTIFIER { $$ = new EnumDeclaration(*$2, nullptr); delete $2; }
    ;

enumerator_list
    : enumerator { $$ = new NodeList(NodePtr($1)); }
    | enumerator_list ',' enumerator { $1->PushBack(NodePtr($3)); $$ = $1; }
    ;

enumerator
    : IDENTIFIER { $$ = new Enumerator(std::move(*$1), nullptr); delete $1; }
    | IDENTIFIER '=' conditional_expression { $$ = new Enumerator(std::move(*$1), NodePtr($3)); delete $1; }
    ;

declarator
	: pointer direct_declarator {$$ = new PointerDeclarator($1, NodePtr($2));}
	| direct_declarator
	;

direct_declarator
	: IDENTIFIER {
		$$ = new Identifier(std::move(*$1));
		delete $1;
	}
	| direct_declarator '(' ')' {
		$$ = new DirectDeclarator(NodePtr($1), nullptr);
	}
	| direct_declarator '(' parameter_list ')' {
		$$ = new DirectDeclarator(NodePtr($1), NodePtr($3));
	}
	| direct_declarator '[' ']' {
		$$ = new ArrayDeclarator(NodePtr($1), nullptr);
	}
	| direct_declarator '[' conditional_expression ']' {
		$$ = new ArrayDeclarator(NodePtr($1), NodePtr($3));
	}
	;

pointer
	: '*' { $$ = 1;}
	| '*' pointer { $$ = $2 + 1; }
	;

parameter_list
	: parameter_declaration {$$ = new NodeList(NodePtr($1));}
	| parameter_list ',' parameter_declaration {$1->PushBack(NodePtr($3)); $$ = $1;}
	;

parameter_declaration
	: declaration_specifiers declarator { $$ = new ParameterDeclaration($1, NodePtr($2)); }
	/* | declaration_specifiers abstract_declarator
	| declaration_specifiers */
	;

initializer
	: assignment_expression { $$ = $1; }
	| '{' initializer_list '}' { $$ = $2; }
	/* | '{' initializer_list ',' '}'  */
	;

initializer_list
	: initializer { $$ = new NodeList(NodePtr($1)); }
	| initializer_list ',' initializer { $1->PushBack(NodePtr($3)); $$=$1; }
	;

statement
	: jump_statement
	| declaration
	| selection_statement
	| compound_statement
	| iteration_statement
    | expression_statement
	| labeled_statement
	;

labeled_statement
	: CASE conditional_expression ':' statement { NodeList *statement = new NodeList(NodePtr($4));
												  $$ = new CaseStatement(NodePtr($2), NodePtr(new Scope(std::move(*statement))));
												  delete statement; }
	| DEFAULT ':' statement { $$ = new DefaultStatement(NodePtr($3)); }
	;

compound_statement
    : '{' statement_list '}' { $$ = new Scope(std::move(*$2)); delete $2; }
	| '{' '}' { $$ = new NodeList(nullptr); }
	;

statement_list
	: statement { $$ = new NodeList(NodePtr($1)); }
	| statement_list statement { $1->PushBack(NodePtr($2)); $$=$1; }
	;

expression_statement
	: ';' { $$ = nullptr; }
	| expression ';' { $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement {
		$$ = new IfStatement(NodePtr($3), NodePtr($5), NodePtr(nullptr));
	} %prec NOELSE
	| IF '(' expression ')' statement ELSE statement {
		$$ = new IfStatement(NodePtr($3), NodePtr($5), NodePtr($7));
	}
	| SWITCH '(' expression ')' statement {
		$$ = new SwitchStatement(NodePtr($3), NodePtr($5));
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement {
		$$ = new WhileLoop(NodePtr($3), NodePtr($5));
	}
	| FOR '(' expression_statement expression_statement expression ')' statement {
		$$ = new ForStatement(NodePtr($3), NodePtr($4), NodePtr($5), NodePtr($7));
	}
	;

jump_statement
	: RETURN ';'  			 { $$ = new ReturnStatement(nullptr);	}
	| RETURN expression ';'  { $$ = new ReturnStatement(NodePtr($2)); }
    | CONTINUE ';' 			 { $$ = new ContinueStatement(); }
	| BREAK ';'    			 { $$ = new BreakStatement(); }
	;

primary_expression
	: IDENTIFIER {
		$$ = new Identifier(std::move(*$1));
		delete $1;
	}
	| INT_CONSTANT {
		std::string raw_str = yytext;
		$$ = new IntConstant($1, raw_str);
	}
    | FLOAT_CONSTANT {
		std::string raw_str = yytext;
        $$ = new FloatConstant($1, raw_str);
    }
	| CHAR_LITERAL {
		$$ = new CharLiteral(*$1);
		delete $1;
	}
	| STRING_LITERAL {
		$$ = new StringLiteral(*$1);
		delete $1;
	}
	| '(' expression ')' {$$ = $2;}
	;

postfix_expression
	: primary_expression
    | postfix_expression '(' ')' {$$ = new FunctionCall(NodePtr($1), nullptr);}
    | postfix_expression '(' argument_expression_list ')' {$$ = new FunctionCall(NodePtr($1), NodePtr($3));}
	| postfix_expression INC_OP {$$ = new PostIncAndDecOp('+', NodePtr($1));}
	| postfix_expression DEC_OP {$$ = new PostIncAndDecOp('-', NodePtr($1));}
	| postfix_expression '[' expression ']' {$$ = new ArrayIndex(NodePtr($1), NodePtr($3));}
	;

argument_expression_list
	: assignment_expression { $$ = new NodeList(NodePtr($1)); }
	| argument_expression_list ',' assignment_expression { $1->PushBack(NodePtr($3)); $$ = $1; }
	;
/* incomplete implementation of unary from full */
unary_expression
	: postfix_expression
	| INC_OP unary_expression {
		$$ = new PreIncAndDecOp('+', NodePtr($2));
	}
	| DEC_OP unary_expression {
		$$ = new PreIncAndDecOp('-', NodePtr($2));
	}
	| '-' cast_expression {
		$$ = new UnaryMinusOp(NodePtr($2));
    }
    | '+' cast_expression {
        $$ = new UnaryPlusOp(NodePtr($2));
    }
    | '!' cast_expression {
        $$ = new UnaryLogicalNotOp(NodePtr($2));
    }
    | '~' cast_expression {
        $$ = new UnaryBitwiseNotOp(NodePtr($2));
    }
	| '&' cast_expression {
		$$ = new UnaryAddressOp(NodePtr($2));
	}
	| '*' cast_expression {
		$$ = new UnaryDereferenceOp(NodePtr($2));
	}
	| SIZEOF '(' unary_expression ')' {
		$$ = new SizeOfVar(NodePtr($3));
	}
	| SIZEOF '(' type_specifier ')' {
		$$ = new SizeOfType($3);
	}
	;

/* incomplete implementation of cast from full */
cast_expression
	: unary_expression
	/* | '(' type_name ')' cast_expression */
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression {
		$$ = new BinaryOp('*', NodePtr($1), NodePtr($3));
	}
	| multiplicative_expression '/' cast_expression {
		$$ = new BinaryOp('/', NodePtr($1), NodePtr($3));
	}
	| multiplicative_expression '%' cast_expression {
		$$ = new BinaryOp('%', NodePtr($1), NodePtr($3));
	}
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression {$$ = new BinaryOp('+', NodePtr($1), NodePtr($3));}
	| additive_expression '-' multiplicative_expression {$$ = new BinaryOp('-', NodePtr($1), NodePtr($3));}
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression {$$ = new BinaryOp('l', NodePtr($1), NodePtr($3));}
	| shift_expression RIGHT_OP additive_expression {$$ = new BinaryOp('r', NodePtr($1), NodePtr($3));}
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression {$$ = new LessThan(NodePtr($1), NodePtr($3));}
	| relational_expression '>' shift_expression {$$ = new GreaterThan(NodePtr($1), NodePtr($3));}
	| relational_expression LE_OP shift_expression {$$ = new LessThanEqual(NodePtr($1), NodePtr($3));}
	| relational_expression GE_OP shift_expression {$$ = new GreaterThanEqual(NodePtr($1), NodePtr($3));}
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression {$$ = new Equal(NodePtr($1), NodePtr($3));}
	| equality_expression NE_OP relational_expression {$$ = new NotEqual(NodePtr($1), NodePtr($3));}
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression {$$ =new BinaryOp('&', NodePtr($1), NodePtr($3));}
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression {$$ = new BinaryOp('^', NodePtr($1), NodePtr($3));}
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression {$$ = new BinaryOp('|', NodePtr($1), NodePtr($3));}
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression {
		$$ = new LogicalAnd(NodePtr($1), NodePtr($3));
	}
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression {
		$$ = new LogicalOr(NodePtr($1), NodePtr($3));
	}
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression {
		$$ = new IfStatement(NodePtr($1), NodePtr($3), NodePtr($5));
	}
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression {
		$$ = new Assignment(NodePtr($1), *$2, NodePtr($3));
		delete $2;
	}
	;

assignment_operator
	: '='           {$$ = new std::string("=");}
	| MUL_ASSIGN    {$$ = new std::string("*=");}
	| DIV_ASSIGN    {$$ = new std::string("/=");}
	| MOD_ASSIGN    {$$ = new std::string("%=");}
	| ADD_ASSIGN    {$$ = new std::string("+=");}
	| SUB_ASSIGN    {$$ = new std::string("-=");}
	| LEFT_ASSIGN   {$$ = new std::string("<<=");}
	| RIGHT_ASSIGN  {$$ = new std::string(">>=");}
	| AND_ASSIGN    {$$ = new std::string("&=");}
	| XOR_ASSIGN    {$$ = new std::string("^=");}
	| OR_ASSIGN     {$$ = new std::string("|=");}
	;

expression
	: assignment_expression
	;

%%

void yyerror (const char *s)
{
  std::cerr << "Error: " << s << " at line " << yylineno;
  std::cerr << " near '" << yytext << "'" << std::endl;
  std::exit(1);
}

Node* g_root;

NodePtr ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if (yyin == nullptr) {
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    std::exit(1);
  }

  g_root = nullptr;
  yydebug = 1;
  yyparse();

  fclose(yyin);
  yylex_destroy();

  return NodePtr(g_root);
}
