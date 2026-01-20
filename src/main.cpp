
// std lib
#include<iostream>
#include<fstream>
#include<sstream>

// local 
#include "tokenizer.hpp"
#include "parser.hpp"
#include "generation.hpp"

using namespace std;


/*
prec : requires two files, input .c file and output .s file names
postc: outputs .s file into above file name or otherwise throws an error
*/
int main(int argc, char* argv[]){
	if(argc != 3){ // num args we use + 1
		cerr << "Incorrect number of args" << endl;
		return EXIT_FAILURE;
	}



	string contents;
	{ // reading input scope

		fstream in(argv[1], ios::in); // open input .c file
		stringstream content_stream;

		content_stream << in.rdbuf();
		in.close();
		contents = content_stream.str();
	}

	Tokenizer tokenizer(move(contents));
	vector<Token> tokens = tokenizer.tokenize();

	Parser parser(std::move(tokens));
	optional<node::Program> ast = parser.parse_program();

	if(!ast.has_value()){
		cerr << "No exit statement found" << endl;
		exit(EXIT_FAILURE);
	}

	Generator generator(ast.value());
	{
		fstream out(argv[2], ios::out); // open out .s file
		out << generator.generate_asm_x86();
		out.close();
	}


	return EXIT_SUCCESS;
}
