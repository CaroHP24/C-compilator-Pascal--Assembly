//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"


#include <string>
#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <FlexLexer.h>
#include "tokeniser.h"
#include <cstring>


using namespace std;

enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR};
enum OPADD {ADD, SUB, OR, WTFA};
enum OPMUL {MUL, DIV, MOD, AND ,WTFM};
enum DATATYPE {INTEGER, BOOLEAN, CHAR, DOUBLE};

TOKEN current;				// Current token


FlexLexer* lexer = new yyFlexLexer; // This is the flex tokeniser
// tokens can be read using lexer->yylex()
// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
// and lexer->YYText() returns the lexicon entry as a string

	
map<string, enum DATATYPE> DeclaredVariables;
unsigned long TagNumber=0;

bool IsDeclared(const char *id)
{
	return DeclaredVariables.find(id)!=DeclaredVariables.end();
}

void Error(string s)
{
	cerr << "Ligne n°"<<lexer->lineno()<<", lu : '"<<lexer->YYText()<<"'("<<current<<"), mais ";
	cerr<< s << endl;
	exit(-1);
}

bool IsCompatible(DATATYPE x, DATATYPE y)
{
    return x == y || (x == BOOLEAN && y == INTEGER) || (x == INTEGER && y == BOOLEAN);
}

bool IsDouble(const char* str)
{
    const char* dot = strchr(str, '.');
    
    if (dot == nullptr) //pas de . = pas double
	{
        return false;
    }
    return true;
}

// Program := [DeclarationPart] StatementPart
// DeclarationPart := "[" Letter {"," Letter} "]"
// StatementPart := Statement {";" Statement} "."
// Statement := AssignementStatement
// AssignementStatement := Letter "=" Expression

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
// SimpleExpression := Term {AdditiveOperator Term}
// Term := Factor {MultiplicativeOperator Factor}
// Factor := Number | Letter | "(" Expression ")"| "!" Factor
// Number := Digit{Digit}

// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"
	
		
enum DATATYPE Identifier()
{
	enum DATATYPE type;
	if(!IsDeclared(lexer->YYText())) //si pas déclaré
	{
		Error("Variable non déclarée");
	}
	type=DeclaredVariables[lexer->YYText()];
	cout << "\tpush "<<lexer->YYText()<<endl;
	current=(TOKEN) lexer->yylex();
	return type;//retourne le type
}

enum DATATYPE Number(void){
	bool is_a_decimal=false;
	double d;					// 64-bit float
	unsigned int *i;			// pointer to a 32 bit unsigned int 
	string	number=lexer->YYText();
	if(IsDouble(lexer->YYText()))
	{
		// Floating point constant number
		d=atof(lexer->YYText());
		i=(unsigned int *) &d; // i points to the const double
		//cout <<"\tpush $"<<*i<<"\t# Conversion of "<<d<<endl;
		// Is equivalent to : 
		cout <<"\tsubq $8,%rsp\t\t\t# allocate 8 bytes on stack's top"<<endl;
		cout <<"\tmovl	$"<<*i<<", (%rsp)\t# Conversion of "<<d<<" (32 bit high part)"<<endl;
		cout <<"\tmovl	$"<<*(i+1)<<", 4(%rsp)\t# Conversion of "<<d<<" (32 bit low part)"<<endl;
		current=(TOKEN) lexer->yylex();
		return DOUBLE;
	}
	else{ // Si pas double = integer
		cout <<"\tpush $"<<atoi(lexer->YYText())<<endl;
		current=(TOKEN) lexer->yylex();
		return INTEGER;
	}
}


enum DATATYPE Char()
{
	cout<<"\tmovq $0, %rax"<<endl;
	cout<<"\tmovb $"<<lexer->YYText()<<",%al"<<endl;
	cout<<"\tpush %rax\t# push a 64-bit version of "<<lexer->YYText()<<endl;
	current=(TOKEN) lexer->yylex();
	return CHAR;

}

enum DATATYPE Expression();			// Called by Term() and calls Term()

enum DATATYPE Factor(void)
{
	enum DATATYPE type;
	switch(current)
	{
		case RPARENT:
			current=(TOKEN) lexer->yylex();
			type=Expression();
			if(current!=LPARENT)
				Error("')' était attendu");		// ")" expected
			else
				current=(TOKEN) lexer->yylex();
			break;
		case NUMBER:
			type=Number();
			break;
		case ID:
			type=Identifier();
			break;
		case CHARCONST:
			type=Char();
			break;
		default:
			Error("'(', ou constante ou variable attendue.");
	};
	return type;
}



// MultiplicativeOperator := "*" | "/" | "%" | "&&"
OPMUL MultiplicativeOperator(void)
{
	OPMUL opmul;
	if(strcmp(lexer->YYText(),"*")==0)
		opmul=MUL;
	else if(strcmp(lexer->YYText(),"/")==0)
		opmul=DIV;
	else if(strcmp(lexer->YYText(),"%")==0)
		opmul=MOD;
	else if(strcmp(lexer->YYText(),"&&")==0)
		opmul=AND;
	else opmul=WTFM;
	current=(TOKEN) lexer->yylex();
	return opmul;
}

// Term := Factor {MultiplicativeOperator Factor}
enum DATATYPE Term(void)
{
    OPMUL mulop;
    DATATYPE type = Factor();
    while (current == MULOP)
    {
        mulop = MultiplicativeOperator(); // Save operator in local variable
        DATATYPE factorType = Factor();
        if (!IsCompatible(type, factorType))
        {
            Error("Les 2 types sont incompatible");
        }
        cout << "\tpop %rbx" << endl; // get first operand
        cout << "\tpop %rax" << endl; // get second operand
        switch (mulop)
        {
            case AND:
                cout << "\tmulq %rbx" << endl; // a * b -> %rdx:%rax
                cout << "\tpush %rax\t# AND" << endl; // store result
                break;
            case MUL:
                cout << "\tmulq %rbx" << endl; // a * b -> %rdx:%rax
                cout << "\tpush %rax\t# MUL" << endl; // store result
                break;
            case DIV:
                cout << "\tmovq $0, %rdx" << endl; // Higher part of numerator  
                cout << "\tdiv %rbx" << endl; // quotient goes to %rax
                cout << "\tpush %rax\t# DIV" << endl; // store result
                break;
            case MOD:
                cout << "\tmovq $0, %rdx" << endl; // Higher part of numerator  
                cout << "\tdiv %rbx" << endl; // remainder goes to %rdx
                cout << "\tpush %rdx\t# MOD" << endl; // store result
                break;
            default:
                Error("opérateur multiplicatif attendu");
        }
    }
    return type;
}


// AdditiveOperator := "+" | "-" | "||"
OPADD AdditiveOperator(void)
{
	OPADD opadd;
	if(strcmp(lexer->YYText(),"+")==0)
	{
		opadd=ADD;
	}
		
	else if(strcmp(lexer->YYText(),"-")==0)
	{
		opadd=SUB;
	}
		
	else if(strcmp(lexer->YYText(),"||")==0)
	{
		opadd=OR;
	}
	else 
	{
		opadd=WTFA;
	}
	current=(TOKEN) lexer->yylex();
	return opadd;
}

// SimpleExpression := Term {AdditiveOperator Term}
enum DATATYPE SimpleExpression()
{
    OPADD adop;
    DATATYPE type = Term();
    while (current == ADDOP)
    {
        adop = AdditiveOperator(); // Save operator in local variable
        DATATYPE type2 = Term();
        if (!IsCompatible(type, type2))
        {
            Error("Types incompatibles");
        }
        cout << "\tpop %rbx" << endl; // get first operand
        cout << "\tpop %rax" << endl; // get second operand
        switch (adop)
        {
            case OR:
                cout << "\taddq %rbx, %rax\t# OR" << endl; // operand1 OR operand2
                break;
            case ADD:
                cout << "\taddq %rbx, %rax\t# ADD" << endl; // add both operands
                break;
            case SUB:
                cout << "\tsubq %rbx, %rax\t# SUB" << endl; // substract both operands
                break;
            default:
                Error("opérateur additif inconnu");
        }
        cout << "\tpush %rax" << endl; // store result
    }
    return type;
}

int IsKeyWord(const char *kw);
enum DATATYPE Type()
{
	if(current!=KEYWORD)
	{
		Error("type attendu");
	}
	if(strcmp(lexer->YYText(),"BOOLEAN")==0)
	{
		current=(TOKEN) lexer->yylex();
		return BOOLEAN;
	}	
	else if(strcmp(lexer->YYText(),"INTEGER")==0)
	{
		current=(TOKEN) lexer->yylex();
		return INTEGER;
	}
	else if(strcmp(lexer->YYText(),"DOUBLE")==0)
	{
		current=(TOKEN) lexer->yylex();
		return DOUBLE;
	}
	else if(strcmp(lexer->YYText(),"CHAR")==0)
	{
		current=(TOKEN) lexer->yylex();
		return CHAR;
	}
	else
	{
		Error("type inconnu");
	}
}


// VarDeclaration := Ident {"," Ident} ":" Type
void VarDeclaration() 
{
    set<string> idents;
    enum DATATYPE type;

    if (current != ID) 
	{
        Error("Un identificateur était attendu");
    }
    idents.insert(lexer->YYText());
    current = (TOKEN) lexer->yylex();

    while (current == COMMA) 
	{
        current = (TOKEN) lexer->yylex();
        if (current != ID) 
		{
            Error("Un identificateur était attendu");
        }
        idents.insert(lexer->YYText());
        current = (TOKEN) lexer->yylex();
    }

    if (current != COLON) 
	{
        Error("caractère ':' attendu");
    }
    current = (TOKEN) lexer->yylex();

    type = Type();

    for (const auto& ident : idents) 
	{
        switch (type) 
		{
            case BOOLEAN:
            case INTEGER:
                cout << ident << ":\t.quad 0" << endl;
                break;
            case DOUBLE:
                cout << ident << ":\t.double 0.0" << endl;
                break;
            case CHAR:
                cout << ident << ":\t.byte 0" << endl;
                break;
            default:
                Error("type inconnu.");
        };
        DeclaredVariables[ident] = type;
    }
}

// VarDeclarationPart := "VAR" VarDeclaration {";" VarDeclaration} "."
void VarDeclarationPart()
{
	if(!IsKeyWord("VAR"))
	{
		Error("VAR attendu");
	}
	current=(TOKEN)lexer->yylex();//consomme VAR
	VarDeclaration();
	
	while(current==SEMICOLON)
	{
		current=(TOKEN) lexer->yylex();
		VarDeclaration();
	}
	if(current!=DOT)
	{
		Error("Un . est attendu");
	}
	current=(TOKEN)lexer->yylex();//consomme .
}
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void)
{
	OPREL oprel;
	if(strcmp(lexer->YYText(),"==")==0)
		oprel=EQU;
	else if(strcmp(lexer->YYText(),"!=")==0)
		oprel=DIFF;
	else if(strcmp(lexer->YYText(),"<")==0)
		oprel=INF;
	else if(strcmp(lexer->YYText(),">")==0)
		oprel=SUP;
	else if(strcmp(lexer->YYText(),"<=")==0)
		oprel=INFE;
	else if(strcmp(lexer->YYText(),">=")==0)
		oprel=SUPE;
	else oprel=WTFR;
	current=(TOKEN) lexer->yylex();
	return oprel;
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
enum DATATYPE Expression()
{
    OPREL oprel;
    enum DATATYPE type = SimpleExpression();
    if (current == RELOP)
    {
        oprel = RelationalOperator();
        enum DATATYPE type2 = SimpleExpression();
        if (!IsCompatible(type, type2))
        {
            Error("Types incompatibles dans Expression");
        } 
		if(type==DOUBLE)
		{
			cout<<"\tfldl	(%rsp)\t"<<endl;
			cout<<"\tfldl	8(%rsp)\t# first operand -> %st(0) ; second operand -> %st(1)"<<endl;
			cout<<"\t addq $16, %rsp\t# 2x pop nothing"<<endl;
			cout<<"\tfcomip %st(1)\t\t# compare op1 and op2 -> %RFLAGS and pop"<<endl;
			cout<<"\tfaddp %st(1)\t# pop nothing"<<endl;

		} 
		else
		{
			cout << "\tpop %rax" << endl;
			cout << "\tpop %rbx" << endl;
			cout << "\tcmpq %rax, %rbx" << endl;
		}
        switch(oprel)
        {
            case EQU:
                cout << "\tje Vrai" << ++TagNumber << "\t# If equal" << endl;
                break;
            case DIFF:
                cout << "\tjne Vrai" << ++TagNumber << "\t# If different" << endl;
                break;
            case SUPE:
                cout << "\tjae Vrai" << ++TagNumber << "\t# If above or equal" << endl;
                break;
            case INFE:
                cout << "\tjbe Vrai" << ++TagNumber << "\t# If below or equal" << endl;
                break;
            case INF:
                cout << "\tjb Vrai" << ++TagNumber << "\t# If below" << endl;
                break;
            case SUP:
                cout << "\tja Vrai" << ++TagNumber << "\t# If above" << endl;
                break;
            default:
                Error("Opérateur de comparaison inconnu");
        }
        cout << "\tpush $0\t\t# False" << endl;
        cout << "\tjmp Suite" << TagNumber << endl;
        cout << "Vrai" << TagNumber << ":\tpush $0xFFFFFFFFFFFFFFFF\t\t# True" << endl;    
        cout << "Suite" << TagNumber << ":" << endl;
        return BOOLEAN; // This ensures a value is returned for this path
    }
    return type; 
}

int IsKeyWord(const char *kw)
{
	if(current != KEYWORD) //Si pas un KeyWord, erreur
	{
		return 0;
	}
	else
	{
		return !strcmp(kw,(lexer->YYText())); //return le char 
	}
}
void ReadKeyWord(const char * kw)
{
	if(!IsKeyWord(kw))
	{
		Error("je veux un keyword");
	}
	else
	{
		current=(TOKEN)lexer->yylex();
	}
}
void IfStatement();
void WhileStatement();
void ForStatement();
void BlockStatement();
void Display();
// AssignementStatement := Identifier ":=" Expression
void AssignementStatement(void)
{
	string variable;
	if(current!=ID)
	{
		Error("Identificateur attendu");
	}

	if(!IsDeclared(lexer->YYText()))
	{
		cerr << "Erreur : Variable '"<<lexer->YYText()<<"' non déclarée"<<endl;
		exit(-1);
	}

	variable=lexer->YYText();
	enum DATATYPE type=DeclaredVariables[variable];
	current=(TOKEN) lexer->yylex();
	
	if(current!=ASSIGN)
	{
		Error("caractères ':=' attendus");
	}
	current=(TOKEN) lexer->yylex();
	enum DATATYPE type2=Expression();
	if(type2!=type)
	{
		cerr<<"Type variable "<<type<<endl;
		cerr<<"Type Expression "<<type2<<endl;
		Error("types incompatibles dans l'affectation");
	}
	if(type==CHAR)
	{
		cout << "\tpop %rax"<<endl;
		cout << "\tmovb %al,"<<variable<<endl;
	}
	
	else
		cout << "\tpop "<<variable<<endl;

}

// Statement := AssignementStatement
void Statement(void)
{
	if(strcmp(lexer->YYText(), "IF") == 0)
	{
		IfStatement();
	}
	else if (strcmp(lexer->YYText(), "BEGIN") == 0)
	{
		BlockStatement();
	}
	else if(strcmp(lexer->YYText(), "WHILE")==0)
	{
		WhileStatement();
	}
	else if(strcmp(lexer->YYText(), "FOR")==0)
	{
		ForStatement();
	}
	else if(strcmp(lexer->YYText(), "DISPLAY")==0)
	{
		Display();
	}
	else
	{
		AssignementStatement();
	}
}
//IfStatement := "IF" Expression "THEN" Statement [ "ELSE" Statement ]
void IfStatement() // if condition then instruction else instruction 2
{
	int tag_local=TagNumber++;

    if (current != KEYWORD || strcmp(lexer->YYText(), "IF") != 0) //If current != IF
	{
        Error("Expected 'IF' keyword");
    }
	
    current=(TOKEN)lexer->yylex(); // Consume the 'if' 
    Expression(); // Parse the condition
	cout << "IfBlock" <<tag_local<<":"<< endl;
	cout<<"\tpop %rax"<<endl;
	cout<<"\tcmpq $0, %rax"<<endl;
	cout<<"\tje ElseBlock"<<tag_local<<endl;

    if (current != KEYWORD || strcmp(lexer->YYText(), "THEN") != 0) 
	{
        Error("Expected 'THEN' keyword");
    }

    current=(TOKEN)lexer->yylex(); // Consume the 'then' 
	
    Statement(); // Parse the statement after 'then'
	cout<<"\tjmp FinBlock"<<tag_local<<endl;
	cout << "ElseBlock"<<tag_local<<":"<< endl;
	// Check if there's an 'else' part
    if (current == KEYWORD && strcmp(lexer->YYText(), "ELSE") == 0)
	{
        current=(TOKEN)lexer->yylex(); // Consume the 'else' 
        
        Statement(); // Parse the statement after 'else'
    }
	cout<<"FinBlock"<<tag_local<<":"<< endl;
}

//BlockStatement := "BEGIN" Statement { ";" Statement } "END"
void BlockStatement()
{
	int tag_local=TagNumber++;

	if (current != KEYWORD || strcmp(lexer->YYText(), "BEGIN") != 0) //SI current != BEGIN
	{
        Error("Expected 'BEGIN' keyword");
    }

	current=(TOKEN)lexer->yylex(); // Consume the 'begin' 
	Statement();

	while((strcmp(lexer->YYText(), ";") == 0))//if ; continue
	{
		current=(TOKEN)lexer->yylex();
		Statement();
	}
	if (current != KEYWORD || strcmp(lexer->YYText(), "END") != 0) //doit se terminer par END
	{
        Error("Expected 'END' keyword");
    }
	current = (TOKEN)lexer->yylex(); // Consume the 'END'
	cout<<"EndBlock"<<tag_local<<":"<< endl;
}

void WhileStatement()
{
	int tag_local=TagNumber++;

	if(current !=KEYWORD || strcmp(lexer->YYText(), "WHILE")!= 0)
	{
		Error("Expected 'WHILE' keyword");
	}
	cout << "TantQue" <<tag_local<<":"<< endl;
	current=(TOKEN)lexer->yylex();//consume WHILE
	Expression();//parse condition

	cout << "\tpop %rax" << endl; 
    cout << "\tcmpq $0, %rax" << endl; 
    cout << "\tje FinTantQue" << tag_local << endl;

	if(current !=KEYWORD || strcmp(lexer->YYText(), "DO")!=0)
	{
		Error("Expected 'DO' keyword");
	}
	current=(TOKEN)lexer->yylex();// Consomme 'DO'
	Statement();
	cout<<"\tjmp TantQue"<<tag_local<<endl;
	cout<<"FinTantQue"<<tag_local<<":"<<endl;
}
void ForStatement()
{
    int tag_local = TagNumber++;
    string varName;
    
    if (current != KEYWORD || strcmp(lexer->YYText(), "FOR") != 0) {
        Error("Expected 'FOR' keyword");
    }
    current = (TOKEN) lexer->yylex(); // Consume 'FOR'
    
    if (current != ID) {
        Error("Identificateur attendu");
    }
    varName = lexer->YYText();
    
    if (!IsDeclared(varName.c_str())) {
        Error("Variable non déclarée");
    }
    
    current = (TOKEN) lexer->yylex();
    if (current != ASSIGN) {
        Error("caractères ':=' attendus");
    }
    current = (TOKEN) lexer->yylex();
    
    // Initialisation de la boucle
    Expression();
    cout << "\tpop %rax" << endl;
    cout << "\tmov %rax, " << varName << endl;
    
    cout << "ForStart" << tag_local << ":" << endl;
    
    // Condition de la boucle
    if (current != KEYWORD || strcmp(lexer->YYText(), "TO") != 0) {
        Error("Expected 'TO' keyword");
    }
    current = (TOKEN) lexer->yylex(); // Consume 'TO'
    Expression();
    cout << "\tpop %rbx" << endl;
    cout << "\tmov " << varName << ", %rax" << endl;
    cout << "\tcmp %rbx, %rax" << endl;
    cout << "\tjg ForEnd" << tag_local << endl;
    
    if (current != KEYWORD || strcmp(lexer->YYText(), "DO") != 0) {
        Error("Expected 'DO' keyword");
    }
    current = (TOKEN) lexer->yylex(); // Consume 'DO'
    Statement();
    
    // Incrémentation de la boucle
    cout << "\tmov " << varName << ", %rax" << endl;
    cout << "\tadd $1, %rax" << endl;
    cout << "\tmov %rax, " << varName << endl;
    cout << "\tjmp ForStart" << tag_local << endl;
    
    cout << "ForEnd" << tag_local << ":" << endl;
}

// Display <expression> 
//si c'est un integer

void Display()
{
	enum DATATYPE type;
	current=(TOKEN)lexer->yylex();
	type=Expression();
	cout << "\tpop %rsi\t# The value to be displayed"<<endl;
	cout << "\tmovq $FormatString1, %rdi\t# \"%llu\\n\""<<endl;
	cout << "\tmovl	$0, %eax"<<endl;
	cout<<"\tpush %rbp \t#save the value in %rbp"<<endl;
	cout << "\tcall	printf@PLT"<<endl;
	cout<<"\t pop %rbp"<<endl;
}


// StatementPart := Statement {";" Statement} "."
void StatementPart(void)
{
	cout << "\t.text\t\t# The following lines contain the program"<<endl;
	cout << "\t.globl main\t# The main function must be visible from outside"<<endl;
	cout << "main:\t\t\t# The main function body :"<<endl;
	cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top"<<endl;
	Statement();
	while(current==SEMICOLON)
	{
		current=(TOKEN) lexer->yylex();
		Statement();
	}
	if(current!=DOT)
		Error("caractère '.' attendu");
	current=(TOKEN) lexer->yylex();
}

// Program := [VarDeclarationPart] StatementPart
void Program(void)
{
	if(current==KEYWORD && strcmp(lexer->YYText(),"VAR")==0)
	{
		VarDeclarationPart();
	}
	StatementPart();	
}

int main(void)
{	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	cout << "\t\t\t# This code was produced by the CERI Compiler"<<endl;
	cout << ".data"<<endl;
	cout << "FormatString1:"<<endl;
	cout<<"\t.string \"%llu\""<<endl;
	// Let's proceed to the analysis and code production
	current=(TOKEN) lexer->yylex();
	Program();
	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top"<<endl;
	cout << "\tret\t\t\t# Return from main function"<<endl;
	if(current!=FEOF){
		cerr <<"Caractères en trop à la fin du programme : ["<<current<<"]";
		Error("."); // unexpected characters at the end of program
	}

}