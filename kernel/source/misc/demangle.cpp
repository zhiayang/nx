// demangle.cpp
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

#include "nx.h"

namespace nx {
namespace util
{
	struct Substitution
	{
		nx::string str;
		nx::string replacement;

		Substitution(const nx::string& s, const nx::string& r) : str(s), replacement(r) { }
	};


	nx::array<Substitution> operatorSubsts;
	nx::array<Substitution> builtinTypeSubsts;
	nx::array<Substitution> stdlibTemplateSubsts;

	void initDemangler()
	{
		operatorSubsts = array<Substitution>();
		builtinTypeSubsts = array<Substitution>();
		stdlibTemplateSubsts = array<Substitution>();


		// New has special syntax (not currently supported).
		operatorSubsts.append(Substitution("nw", "new")),
		operatorSubsts.append(Substitution("na", "new[]"));

		// Works except that the 'gs' prefix is not supported.
		operatorSubsts.append(Substitution("dl", "delete"));
		operatorSubsts.append(Substitution("da", "delete[]"));

		// unary operators
		operatorSubsts.append(Substitution("ps", "+"));
		operatorSubsts.append(Substitution("ng", "-"));
		operatorSubsts.append(Substitution("ad", "&"));
		operatorSubsts.append(Substitution("de", "*"));
		operatorSubsts.append(Substitution("co", "~"));
		operatorSubsts.append(Substitution("nt", "!"));
		operatorSubsts.append(Substitution("pp", "++"));
		operatorSubsts.append(Substitution("mm", "--"));

		// binary ops
		operatorSubsts.append(Substitution("pl", "+"));
		operatorSubsts.append(Substitution("mi", "-"));
		operatorSubsts.append(Substitution("ml", "*"));
		operatorSubsts.append(Substitution("dv", "/"));
		operatorSubsts.append(Substitution("rm", "%"));
		operatorSubsts.append(Substitution("an", "&"));
		operatorSubsts.append(Substitution("or", "|"));
		operatorSubsts.append(Substitution("eo", "^"));
		operatorSubsts.append(Substitution("aS", "="));
		operatorSubsts.append(Substitution("pL", "+="));
		operatorSubsts.append(Substitution("mI", "-="));
		operatorSubsts.append(Substitution("mL", "*="));
		operatorSubsts.append(Substitution("dV", "/="));
		operatorSubsts.append(Substitution("rM", "%="));
		operatorSubsts.append(Substitution("aN", "&="));
		operatorSubsts.append(Substitution("oR", "|="));
		operatorSubsts.append(Substitution("eO", "^="));
		operatorSubsts.append(Substitution("ls", "<<"));
		operatorSubsts.append(Substitution("rs", ">>"));
		operatorSubsts.append(Substitution("lS", "<<="));
		operatorSubsts.append(Substitution("rS", ">>="));
		operatorSubsts.append(Substitution("eq", "=="));
		operatorSubsts.append(Substitution("ne", "!="));
		operatorSubsts.append(Substitution("lt", "<"));
		operatorSubsts.append(Substitution("gt", ">"));
		operatorSubsts.append(Substitution("le", "<="));
		operatorSubsts.append(Substitution("ge", ">="));
		operatorSubsts.append(Substitution("aa", "&&"));
		operatorSubsts.append(Substitution("oo", "||"));
		operatorSubsts.append(Substitution("cm", ","));
		operatorSubsts.append(Substitution("pm", "->*"));
		operatorSubsts.append(Substitution("ix", "[]"));

		operatorSubsts.append(Substitution("qu", "?"));

		operatorSubsts.append(Substitution("pt", "->"));
		operatorSubsts.append(Substitution("cl", "()"));
		operatorSubsts.append(Substitution("st", " sizeof"));
		operatorSubsts.append(Substitution("sz", " sizeof"));

		builtinTypeSubsts.append(Substitution("v", "void"));
		builtinTypeSubsts.append(Substitution("w", "wchar_t"));
		builtinTypeSubsts.append(Substitution("b", "bool"));
		builtinTypeSubsts.append(Substitution("c", "char"));
		builtinTypeSubsts.append(Substitution("a", "signed char"));
		builtinTypeSubsts.append(Substitution("h", "unsigned char"));
		builtinTypeSubsts.append(Substitution("s", "short"));
		builtinTypeSubsts.append(Substitution("t", "unsigned short"));
		builtinTypeSubsts.append(Substitution("i", "int"));
		builtinTypeSubsts.append(Substitution("j", "unsigned int"));
		builtinTypeSubsts.append(Substitution("l", "long"));
		builtinTypeSubsts.append(Substitution("m", "unsigned long"));
		builtinTypeSubsts.append(Substitution("x", "long long"));
		builtinTypeSubsts.append(Substitution("y", "unsigned long long"));
		builtinTypeSubsts.append(Substitution("n", "__int128"));
		builtinTypeSubsts.append(Substitution("o", "unsigned __int128"));
		builtinTypeSubsts.append(Substitution("f", "float"));
		builtinTypeSubsts.append(Substitution("d", "double"));
		builtinTypeSubsts.append(Substitution("e", "long double"));
		builtinTypeSubsts.append(Substitution("g", "__float128"));
		builtinTypeSubsts.append(Substitution("z", "..."));
		builtinTypeSubsts.append(Substitution("Di", "char32_t"));
		builtinTypeSubsts.append(Substitution("Ds", "char16_t"));
		builtinTypeSubsts.append(Substitution("Da", "auto"));
		builtinTypeSubsts.append(Substitution("Dc", "decltype(auto)"));
		builtinTypeSubsts.append(Substitution("Dn", "std::nullptr_t"));

		stdlibTemplateSubsts.append(Substitution("St", "std::"));
		stdlibTemplateSubsts.append(Substitution("Sa", "std::allocator"));
		stdlibTemplateSubsts.append(Substitution("Sb", "std::basic_string"));
		stdlibTemplateSubsts.append(Substitution("Ss", "std::string"));
		stdlibTemplateSubsts.append(Substitution("Si", "std::istream"));
		stdlibTemplateSubsts.append(Substitution("So", "std::ostream"));
		stdlibTemplateSubsts.append(Substitution("Sd", "std::iostream"));
	}













	static bool isdigit(char c) { return c >= '0' && c <= '9'; }

	struct State
	{
		nx::array<nx::string> subs;
		nx::stack<nx::string> unqualifiedNameStack;
	};

	static nx::string parseType(nx::string_view& s, State& st);
	static nx::string parseName(nx::string_view& s, State& st);
	static nx::string parseEncoding(nx::string_view& s, State& st);
	static nx::string parseLambdaType(nx::string_view& s, State& st);
	static nx::string parseMangledName(nx::string_view& s, State& st);
	static nx::string parseBareFunctionType(nx::string_view& s, State& st);

	static int parseLength(nx::string_view& s)
	{
		int len = 0;
		while(isdigit(s[0]))
		{
			len *= 10;
			len += s[0] - '0';

			s.remove_prefix(1);
		}

		return len;
	}

	static nx::string parseSubstitution(nx::string_view& s, State& st)
	{
		assert(s[0] == 'S');

		string ret;

		if(s[1] == '_')
		{
			// 0th backref.
			assert(st.subs.size() > 0);
			ret += st.subs.front();

			s.remove_prefix(2);
		}
		else if(isdigit(s[1]))
		{
			// numbered backref.
			s.remove_prefix(1);
			int n = parseLength(s);

			// 0 is the second index backref...
			n += 1;

			assert((size_t) n < st.subs.size());
			ret += st.subs[n];

			assert(s[0] == '_');
			s.remove_prefix(1);
		}
		else
		{
			auto x = s.substring(0, 2);
			bool found = false;
			for(const auto& sub : stdlibTemplateSubsts)
			{
				if(x == sub.str)
				{
					found = true, ret += sub.replacement;
					break;
				}
			}

			if(!found) abort("invalid substitution!");

			s.remove_prefix(2);
		}

		return ret;
	}

	static nx::string parseTemplateArgument(nx::string_view& s, State& st)
	{
		/*
			<template-arg>  ::= <type>                                             # type or template
							::= X <expression> E                                   # expression
							::= <expr-primary>                                     # simple expressions
							::= J <template-arg>* E                                # argument pack
		*/

		if(s[0] == 'X')
		{
			abort("no!");
		}
		else if(s[0] == 'J')
		{
			s.remove_prefix(1);

			string ret;
			while(s[0] != 'E')
			{
				ret += parseTemplateArgument(s, st);
				if(s[0] != 'E') ret += ", ";
			}

			assert(s[0] == 'E');
			s.remove_prefix(1);

			return ret;
		}
		else
		{
			auto t = parseType(s, st);
			if(t.size() > 0)    return t;
			else                abort("no!!!");
		}
	}

	static nx::string parseTemplateArgumentList(nx::string_view& s, State& st)
	{
		/*
			<template-args> ::= I <template-arg>+ E
		*/

		assert(s[0] == 'I');
		s.remove_prefix(1);

		string ret = "<";
		while(s.size() > 0)
		{
			auto arg = parseTemplateArgument(s, st);
			ret += arg;

			if(s[0] != 'E') ret += ", ";
			else            break;
		}

		return ret + ">";
	}







	static nx::string parseSourceName(nx::string_view& s, State& st)
	{
		int len = parseLength(s);
		assert(len > 0);

		auto ret = s.substring(0, len).str();
		s.remove_prefix(len);

		return ret;
	}

	static nx::string parseUnnamedTypeName(nx::string_view& s, State& st)
	{
		assert(s.find("Ut") == 0);
		s.remove_prefix(2);

		int n = 0;
		if(s[0] != '_')
		{
			assert(isdigit(s[0]));
			n = parseLength(s) + 1;
		}

		assert(s[0] == '_');
		return sprint("unnamed-type#%d", n);
	}

	static nx::string parseClassType(nx::string_view& s, State& st)
	{
		/*
			<class-enum-type>   ::= <name>     # non-dependent type name, dependent type name, or dependent typename-specifier
								::= Ts <name>  # dependent elaborated type specifier using 'struct' or 'class'
								::= Tu <name>  # dependent elaborated type specifier using 'union'
								::= Te <name>  # dependent elaborated type specifier using 'enum'
		*/

		if(s.find("Ts") == 0)
		{
			return "struct " + parseName(s, st);
		}
		else if(s.find("Tu") == 0)
		{
			return "union " + parseName(s, st);
		}
		else if(s.find("Te") == 0)
		{
			return "enum " + parseName(s, st);
		}
		else
		{
			return parseName(s, st);
		}
	}

	static nx::string parseLambdaType(nx::string_view& s, State& st)
	{
		/*
			<closure-type-name> ::= Ul <lambda-sig> E [ <nonnegative number> ] _
			<lambda-sig>        ::= <parameter type>+  # Parameter types or "v" if the lambda has no parameters
		*/

		assert(s.find("Ul") == 0);
		s.remove_prefix(2);

		string ret = "{lambda" + parseBareFunctionType(s, st);

		assert(s[0] == 'E');
		s.remove_prefix(1);

		// check if we have a number
		if(isdigit(s[0]))
		{
			int n = parseLength(s) + 1;
			ret += sprint("#%d", n);
		}

		assert(s[0] == '_');
		s.remove_prefix(1);

		return ret + "}";
	}

	static nx::string trySubstituteOperators(nx::string_view& s, State& st)
	{
		// cast operator needs special handling.
		if(s.find("cv") == 0)
		{
			/*
				<operator-name> ::= cv <type>	# (cast)
			*/

			s.remove_prefix(2);
			return "operator " + parseType(s, st);
		}
		else
		{
			// parse some ops.
			for(const auto& sub : operatorSubsts)
			{
				if(strncmp(sub.str.cstr(), s.data(), sub.str.size()) == 0)
				{
					s.remove_prefix(sub.str.size());
					return "operator" + sub.replacement;
				}
			}
		}

		return "";
	}

	static nx::string parseUnqualifiedName(nx::string_view& s, State& st)
	{
		/*
			<unqualified-name>  ::= <operator-name> [<abi-tags>]
								::= <ctor-dtor-name>
								::= <source-name>
								::= <unnamed-type-name>
								::= DC <source-name>+ E      # structured binding declaration

			# from gcc sources: https://github.com/gcc-mirror/gcc/blob/master/gcc/cp/mangle.c#L1390
			# this is the only place i can find that uses 'L' in this way... but the usage appears to fit.
			# it's definitely not the 'external-name', which is L _Z <encoded-name> E
			# (we don't have the _Z, or the E)

			<internal-linkage-name>     ::= L <source-name>
		*/

		if(isdigit(s[0]))
		{
			auto ret = parseSourceName(s, st);

			st.unqualifiedNameStack.push(ret);
			return ret;
		}
		else if(s[0] == 'L')
		{
			s.remove_prefix(1);
			auto ret = parseSourceName(s, st);

			st.unqualifiedNameStack.push(ret);
			return ret;
		}
		else if(s.find("DC") == 0)
		{
			string ret = "[ ";

			s.remove_prefix(2);

			while(true)
			{
				ret += parseSourceName(s, st);
				if(s[0] != 'E') ret += ", ";
				else            break;
			}

			ret += " ]";
			return ret;
		}
		else if(s.find("C0") == 0 || s.find("C1") == 0 || s.find("C2") == 0)
		{
			s.remove_prefix(2);

			assert(st.unqualifiedNameStack.size() > 0);
			return st.unqualifiedNameStack.top();
		}
		else if(s.find("D0") == 0 || s.find("D1") == 0 || s.find("D2") == 0)
		{
			s.remove_prefix(2);

			assert(st.unqualifiedNameStack.size() > 0);
			return "~" + st.unqualifiedNameStack.top();
		}
		else if(s.find("Ut") == 0)
		{
			auto ret = parseUnnamedTypeName(s, st);

			st.unqualifiedNameStack.push(ret);
			return ret;
		}
		else if(s.find("Ul") == 0)
		{
			auto ret = parseLambdaType(s, st);

			st.unqualifiedNameStack.push(ret);
			return ret;
		}
		else
		{
			auto op = trySubstituteOperators(s, st);
			if(op.size() > 0) return op;

			abort("malformed unqualified-name (%s)", s.data());
		}
	}

	static nx::string parseNestedName(nx::string_view& s, State& st)
	{
		/*
			<nested-name>       ::= N [<CV-qualifiers>] [<ref-qualifier>] <prefix> <unqualified-name> E
								::= N [<CV-qualifiers>] [<ref-qualifier>] <template-prefix> <template-args> E

			<prefix>            ::= <unqualified-name>                 # global class or namespace
								::= <prefix> <unqualified-name>        # nested class or namespace
								::= <template-prefix> <template-args>  # class template specialization
								::= <template-param>                   # template type parameter
								::= <decltype>                         # decltype qualifier
								::= <prefix> <data-member-prefix>      # initializer of a data member
								::= <substitution>

			<template-prefix>   ::= <template unqualified-name>           # global template
								::= <prefix> <template unqualified-name>  # nested template
								::= <template-param>                      # template template parameter
								::= <substitution>

			<source-name>       ::= <positive length number> <identifier>
			<identifier>        ::= <unqualified source code identifier>
		*/


		assert(s[0] == 'N');
		s.remove_prefix(1);


		string cvQuals;
		while(s[0] == 'r' || s[0] == 'V' || s[0] == 'K')
		{
			if(s[0] == 'r') cvQuals += " restrict";
			if(s[0] == 'V') cvQuals += " volatile";
			if(s[0] == 'K') cvQuals += " const";

			s.remove_prefix(1);
		}

		string refQuals;
		if(s[0] == 'R')         { refQuals = "&"; s.remove_prefix(1); }
		else if(s[0] == 'O')    { refQuals = "&&"; s.remove_prefix(1); }

		// parse the 'prefix'.
		string ret;

		while(true)
		{
			// check for substitutions.
			if(s[0] == 'S')         ret += parseSubstitution(s, st);
			else if(s[0] == 'I')    ret += parseTemplateArgumentList(s, st);
			else                    ret += parseUnqualifiedName(s, st);

			st.subs.append(ret);

			if(s[0] == 'E') break;

			if(s[0] != 'I') ret += "::";
		}

		assert(s[0] == 'E');
		s.remove_prefix(1);

		return ret + cvQuals + refQuals;
	}

	static nx::string parseLocalName(nx::string_view& s, State& st)
	{
		/*
			<local-name>    ::= Z <function encoding> E <entity name> [<discriminator>]
							::= Z <function encoding> E s [<discriminator>]

			<discriminator> ::= _ <non-negative number>      # when number < 10
							::= __ <non-negative number> _   # when number >= 10
		*/

		assert(s[0] == 'Z');
		s.remove_prefix(1);

		string ret = parseEncoding(s, st);

		assert(s[0] == 'E');
		s.remove_prefix(1);

		if(s[0] != 's')
			ret += "::" + parseName(s, st);

		else
			s.remove_prefix(1);

		if(s.size() > 0 && s[0] == '_')
		{
			s.remove_prefix(1);
			if(s[0] == '_')
				s.remove_prefix(1);

			int n = parseLength(s) + 1;
			ret += sprint("#%d", n);
		}

		return ret;
	}

	static nx::string parseName(nx::string_view& s, State& st)
	{
		/*
			<name>  ::= <nested-name>
					::= <unscoped-name>
					::= <unscoped-template-name> <template-args>
					::= <local-name>
					::= St <unqualified-name>                   # ::std::
		*/

		if(s[0] == 'N')
		{
			return parseNestedName(s, st);
		}
		else if(s[0] == 'Z')
		{
			return parseLocalName(s, st);
		}
		else if(s.find("St") == 0)
		{
			s.remove_prefix(2);
			return "std::" + parseUnqualifiedName(s, st);
		}
		else
		{
			string ret;
			if(s.find("St") == 0)
				ret = "std::";

			return ret + parseUnqualifiedName(s, st);
		}
	}



	static nx::string parseType(nx::string_view& s, State& st)
	{
		/*
			<type>  ::= <builtin-type>
					::= <qualified-type>
					::= <function-type>
					::= <class-enum-type>
					::= <array-type>
					::= <pointer-to-member-type>
					::= <template-param>
					::= <template-template-param> <template-args>
					::= <decltype>
					::= P <type>        # pointer
					::= R <type>        # l-value reference
					::= O <type>        # r-value reference (C++11)
					::= C <type>        # complex pair (C99)
					::= G <type>        # imaginary (C99)
					::= <substitution>
		*/

		// println("parseType: %s", s.data());

		assert(s.size() > 0);

		if(s[0] == 'P')         { s.remove_prefix(1); auto ret = parseType(s, st) + "*"; st.subs.append(ret); return ret; }
		else if(s[0] == 'R')    { s.remove_prefix(1); auto ret = parseType(s, st) + "&"; st.subs.append(ret); return ret; }
		else if(s[0] == 'O')    { s.remove_prefix(1); auto ret = parseType(s, st) + "&&"; st.subs.append(ret); return ret; }
		else if(s[0] == 'K')    { s.remove_prefix(1); auto ret = parseType(s, st) + " const"; st.subs.append(ret); return ret; }
		else if(s[0] == 'r')    { s.remove_prefix(1); auto ret = parseType(s, st) + " restrict"; st.subs.append(ret); return ret; }
		else if(s[0] == 'V')    { s.remove_prefix(1); auto ret = parseType(s, st) + " volatile"; st.subs.append(ret); return ret; }
		else if(s[0] == 'F')
		{
			// function type.
			s.remove_prefix(1);

			// no idea what 'Y' means.
			if(s[0] == 'Y') s.remove_prefix(1);

			// for types, the return type is the first 'type'.
			auto retty = parseType(s, st);
			assert(retty.size() > 0);

			string ret = retty + " (*)" + parseBareFunctionType(s, st);


			if(s[0] == 'R') { s.remove_prefix(1); ret += " &"; }
			if(s[0] == 'O') { s.remove_prefix(1); ret += " &&"; }

			assert(s[0] == 'E');
			s.remove_prefix(1);

			st.subs.append(ret);
			return ret;
		}
		else if(s[0] == 'A')
		{
			// array type.
			s.remove_prefix(1);

			if(!isdigit(s[0]))
				abort("non-constant array not supported!");

			int n = parseLength(s);

			assert(s[0] == '_');
			s.remove_prefix(1);

			auto ret = parseType(s, st) + sprint(" [%d]", n);
			st.subs.append(ret);

			return ret;
		}
		else if(s.find("Do") == 0 || s.find("DO") == 0 || s.find("Dw") == 0)
		{
			// exception specifier.
			s.remove_prefix(2);
			return parseType(s, st) + " noexcept";
		}
		else if(s[0] == 'S')
		{
			// substitution.
			return parseSubstitution(s, st);
		}
		else
		{
			auto op = trySubstituteOperators(s, st);
			if(op.size() > 0) return op;


			// see if we can substitute stuff.
			for(const auto& sub : builtinTypeSubsts)
			{
				if(strncmp(s.data(), sub.str.cstr(), sub.str.size()) == 0)
				{
					s.remove_prefix(sub.str.size());
					return sub.replacement;
				}
			}

			// else... try something.
			if(s[0] != 'E')
			{
				auto ret = parseClassType(s, st);
				st.subs.append(ret);

				return ret;
			}
		}

		return "";
	}

	static nx::string parseBareFunctionType(nx::string_view& s, State& st)
	{
		/*
			<bare-function-type>    ::= <signature type>+
		*/
		string ret;

		while(s.size() > 0)
		{
			if(s[0] == 'v') { s.remove_prefix(1); break; }

			auto t = parseType(s, st);

			if(ret.size() > 0 && t.size() > 0)
				t = ", " + t;

			if(t.size() > 0)    ret += t;
			else                break;
		}

		return "(" + ret + ")";
	}

	static nx::string parseEncoding(nx::string_view& s, State& st)
	{
		/*
			<encoding>      ::= <function name> <bare-function-type>
							::= <data name>
							::= <special-name>
		*/

		auto ret = parseName(s, st);
		if(s.size() > 0) ret += parseBareFunctionType(s, st);

		return ret;
	}

	static nx::string parseMangledName(nx::string_view& s, State& st)
	{
		/*
			<mangled-name>  ::= _Z <encoding>
							::= _Z <encoding> . <vendor-specific suffix>
		*/

		assert(s.find("_Z") == 0);
		s.remove_prefix(2);

		return parseEncoding(s, st);
	}


	nx::string demangleSymbol(const nx::string& mangled)
	{
		// then it's not mangled.
		if(mangled.find("_Z") != 0) return mangled;

		auto input = string_view(mangled);

		State st;
		return parseMangledName(input, st);
	}


}
}







































