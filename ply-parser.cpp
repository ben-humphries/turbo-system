#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "common.h"
#include "list.h"
#include "ply-parser.h"

enum Token_Type {
	PLY_EOF,
	PLY_END_HEADER,

	PLY_PLY,
	PLY_FORMAT,
	PLY_ASCII,
	PLY_BINARY_BIG_ENDIAN,
	PLY_BINARY_LITTLE_ENDIAN,

	PLY_ELEMENT,

	PLY_PROPERTY,
	PLY_LIST,

	PLY_NAME,
	PLY_TYPE,
	PLY_INT_LITERAL,
	PLY_FLOAT_LITERAL,
};

enum Data_Type {
	DATA_CHAR,
	DATA_UCHAR,
	DATA_SHORT,
	DATA_USHORT,
	DATA_INT,
	DATA_UINT,
	DATA_FLOAT,
	DATA_DOUBLE,
	DATA_LIST,
};

struct Token {
	Token_Type type;
	union {
		int    int_literal;
		float  float_literal;
		const char * name;
		Data_Type type_literal;
	};
};

struct Parser {
	const char * filename;
	const char * source;
	size_t position;
	Token token;
};

void __ply_fatal(Parser * parser, const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "encountered error in mesh file %s:\n", parser->filename);
	vfprintf(stderr, fmt, args);
	printf("\n");

	va_end(args);
	exit(1);
}

#define fatal(fmt, ...) __ply_fatal(parser, fmt, ##__VA_ARGS__)

// : Lexer

#define advance() (parser->position++)
#define current() (parser->source[parser->position])

const char * keywords[] = {
	"end_header",
	"ply",     "format",
	"element", "property",
	"ascii",
	"binary_big_endian",
	"binary_little_endian",
	"list",
};

Token_Type keyword_tokens[] = {
	PLY_END_HEADER,
	PLY_PLY,     PLY_FORMAT,
	PLY_ELEMENT, PLY_PROPERTY,
	PLY_ASCII,
	PLY_BINARY_BIG_ENDIAN,
	PLY_BINARY_LITTLE_ENDIAN,
	PLY_LIST,
};

const char * type_names[] = {
	"char",  "uchar",
	"short", "ushort",
	"int",   "uint",
	"float", "double",
};

Data_Type type_types[] = {
	DATA_CHAR,  DATA_UCHAR,
	DATA_SHORT, DATA_USHORT,
	DATA_INT,   DATA_UINT,
	DATA_FLOAT, DATA_DOUBLE,
};

const char * dup_source_range(Parser * parser, size_t start, size_t end)
{
	char * buffer = (char*) malloc((end - start) + 1);
	buffer[end - start] = '\0';
	strncpy(buffer, parser->source + start, end - start);
	return (const char*) buffer;
}

Token read_token(Parser * parser)
{
 read_token_reset:
	if (current() == '\0') {
		return (Token) { PLY_EOF };
	}
	
	// Skip whitespace
	while (isspace(current())) {
		advance();
		goto read_token_reset;
	}
	
	// Names
	if (current() == '_' || isalpha(current())) {
		size_t start = parser->position;
		advance();
		while (isalpha(current()) || isdigit(current()) || current() == '_') {
			advance();
		}
		size_t end = parser->position;

		// Check for comment
		if (strncmp(parser->source + start, "comment", end - start) == 0) {
			while (current() != '\n' && current() != EOF) {
				advance();
			}
			goto read_token_reset;
		}

		// Check for keyword
		for (int i = 0; i < sizeof(keywords) / sizeof(char*); i++) {
			if( (end - start) != strlen(keywords[i])) continue;
			if (strncmp(parser->source + start,
						keywords[i],
						end - start) == 0) {
				return (Token) { keyword_tokens[i] };
			}
		}

		// Check for type
		for (int i = 0; i < sizeof(type_names) / sizeof(char*); i++) {
			if( (end - start) != strlen(type_names[i])) continue;
			if (strncmp(parser->source + start,
						type_names[i],
						end - start) == 0) {
				return (Token) { PLY_TYPE, type_types[i] };
			}
		}

		// Default to name
		const char * name = dup_source_range(parser, start, end);
		{
			Token t;
			t.type = PLY_NAME;
			t.name = name;
			return t;
		}
	}

	// TODO(pixlark): +/- signs
	// Integer/Float literals
	if (isdigit(current()) || current() == '.' || current() == '+' || current() == '-') {
		size_t start = parser->position;
		bool has_decimal_point = false;
		if (current() == '.') has_decimal_point = true;
		advance();
		while (true) {
			if (current() == '.') {
				has_decimal_point = true;
			} else if (!isdigit(current())) {
				break;
			}
			advance();
		}
		size_t end = parser->position;
		const char * to_convert = dup_source_range(parser, start, end);
		if (has_decimal_point) {
			// Float literal
			Token token;
			token.type = PLY_FLOAT_LITERAL;
			token.float_literal = strtod(to_convert, NULL);
			return token;
		} else {
			// Integer literal
			Token token;
			token.type = PLY_INT_LITERAL;
			token.int_literal = strtol(to_convert, NULL, 10);
			free((char*) to_convert);
			return token;
		}
	}
	
	fatal("Encountered invalid character '%c' (%d) in mesh file \"%s\"\n",
		  current(), current(),
		  parser->filename);
}

void advance_lexer(Parser * parser)
{
	parser->token = read_token(parser);
}

#undef advance
#undef current

// :\ Lexer

// : Parser

enum Property_Type {
	PROP_SCALAR,
	PROP_LIST,
};

struct Property {
	Property_Type type;
	const char * name;
	union {
		struct {
			Data_Type size;
			Data_Type elem;
		} list;
		struct {
			Data_Type type;
		} scalar;
	};
};

struct Element {
	const char * name;
	size_t count;
	List<Property> properties;
};

#define current() (parser->token)

#define advance() advance_lexer(parser);

#define is(t) (current().type == (t))

bool __match(Parser * parser, Token_Type type)
{
	if (is(type)) {
		advance();
		return true;
	}
	return false;
}

#define match(t) __match(parser, (t))

bool __expect(Parser * parser, Token_Type type)
{
	if (!match(type)) {
		fatal("Encountered parse error in mesh file \"%s\"\n", parser->filename);
	}
}

bool __weak_expect(Parser * parser, Token_Type type)
{
	if (!is(type)) {
		fatal("Encountered parse error in mesh file \"%s\"\n", parser->filename);
	}
}

#define expect(t) __expect(parser, (t))
#define weak_expect(t) __weak_expect(parser, (t))

bool parse_header(Parser * parser)
{
	if (!match(PLY_PLY))    return false;
	if (!match(PLY_FORMAT)) return false;
	if (!match(PLY_ASCII))  return false;
	if (!is(PLY_FLOAT_LITERAL))         return false;
	if (current().float_literal != 1.0) return false;
	advance();
	return true;
}

Property parse_list_property(Parser * parser)
{
	expect(PLY_LIST);
	Property property;
	property.type = PROP_LIST;
	
	weak_expect(PLY_TYPE);
	property.list.size = current().type_literal;
	advance();

	weak_expect(PLY_TYPE);
	property.list.elem = current().type_literal;
	advance();

	weak_expect(PLY_NAME);
	property.name = current().name;
	advance();

	return property;
}

Property parse_scalar_property(Parser * parser)
{
	Property property;
	property.type = PROP_SCALAR;

	weak_expect(PLY_TYPE);
	property.scalar.type = current().type_literal;
	advance();

	weak_expect(PLY_NAME);
	property.name = current().name;
	advance();

	return property;
}

Element parse_element(Parser * parser)
{
	Element element;
	expect(PLY_ELEMENT);
	weak_expect(PLY_NAME);
	element.name = current().name;
	advance();
	weak_expect(PLY_INT_LITERAL);
	element.count = current().int_literal;
	advance();

	// Properties
	element.properties.alloc();
	while (match(PLY_PROPERTY)) {
		if (is(PLY_LIST)) {
			element.properties.push(parse_list_property(parser));
		} else {
			element.properties.push(parse_scalar_property(parser));
		}
	}
	
	return element;
}

Renderer::Mesh * parse_file(Parser * parser)
{
	if (!parse_header(parser)) {
		fatal("Invalid header in mesh file \"%s\"\n", parser->filename);
	}

	// Parse header
	List<Element> elements;
	elements.alloc();
	while (is(PLY_ELEMENT)) {
		elements.push(parse_element(parser));
	}
	expect(PLY_END_HEADER);
	
	// Parse into mesh
	bool found_uv = false;
	Renderer::Mesh * mesh = (Renderer::Mesh*) malloc(sizeof(Renderer::Mesh));
	for (int i = 0; i < elements.size; i++) {
		auto element = elements[i];
		if (strcmp(element.name, "vertex") == 0) {
			mesh->vertices_count = element.count * 5;
			mesh->vertices = (float*) malloc(sizeof(float) * mesh->vertices_count);
			for (int i = 0; i < mesh->vertices_count; i++) {
				weak_expect(PLY_FLOAT_LITERAL);
				mesh->vertices[i] = current().float_literal;
				advance();
			}
		} else if (strcmp(element.name, "face") == 0) {
			/* We do some weird index math here to get around the fact
			 * that each list literal is prefix by the # of
			 * items... there's probably a cleaner way to do this but
			 * for now it's OK.  
			 *   -Paul T. Tue Jan 8 06:04:09 2019 */
			mesh->indices_count = element.count * 3;
			mesh->indices = (int*) malloc(sizeof(int) * mesh->indices_count);
			for (int i = 0; i < element.count * 4; i++) {
				if (i % 4 == 0) {
					expect(PLY_INT_LITERAL);
				} else {
					weak_expect(PLY_INT_LITERAL);
					mesh->indices[i - 1 - (i / 4)] = current().int_literal;
					advance();
				}
			}
		} else if (strcmp(element.name, "uv") == 0) {
			found_uv = true;
			assert(false);
			// TODO(pixlark): Parse texture coordinates if they exist.
		} else {
			fatal("Invalid property \"%s\" in mesh file \"%s\"\n",
				  element.name, parser->filename);
		}
	}

	// If the file did not specify any texture coordinates, specify dummy ones.
	if (!found_uv) {
		mesh->texture_coordinates_count = mesh->vertices_count * 2 / 3;
		mesh->texture_coordinates = (float*)
			calloc(mesh->texture_coordinates_count,
				   sizeof(float));
	}

	// Cleanup
	for (int i = 0; i < elements.size; i++) {
		elements[i].properties.dealloc();
	}
	elements.dealloc();
	
	return mesh;
}

#undef weak_expect
#undef expect
#undef match
#undef is
#undef advance
#undef current

// :\ Parser

Renderer::Mesh * PLY::load_mesh_from_ply_file(const char * path)
{
	const char * source = load_string_from_file(path);
	if (!source) {
		return NULL;
	}
	
	Parser parser;
	parser.filename = path;
	parser.source = source;
	parser.position = 0;
	advance_lexer(&parser); // Prime the parser

	Renderer::Mesh * mesh = parse_file(&parser);
	
	return mesh;
}

void print_mesh(Renderer::Mesh * mesh)
{
	printf("Vertices (%d):\n", mesh->vertices_count);
	for (int i = 0; i < mesh->vertices_count; i++) {
		printf("%f\n", mesh->vertices[i]);
	}
	printf("Indices (%d):\n", mesh->indices_count);
	for (int i = 0; i < mesh->indices_count; i++) {
		printf("%d\n", mesh->indices[i]);
	}
	printf("Texture Coordinates (%d):\n", mesh->texture_coordinates_count);
	for (int i = 0; i < mesh->texture_coordinates_count; i++) {
		printf("%f\n", mesh->texture_coordinates[i]);
	}
}

void PLY::mesh_test()
{
	Renderer::Mesh * mesh = load_mesh_from_ply_file("example.ply");
	print_mesh(mesh);
}
