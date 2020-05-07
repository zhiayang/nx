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

		Substitution(nx::string&& s, nx::string&& r) : str(krt::move(s)), replacement(krt::move(r)) { }
	};

	nx::array<Substitution> operatorSubsts;
	nx::array<Substitution> builtinTypeSubsts;
	nx::array<Substitution> stdlibTemplateSubsts;

	nx::array<Substitution> unaryOperators;
	nx::array<Substitution> binaryOperators;
	nx::array<Substitution> ternaryOperators;

	void initDemangler()
	{
		new (&operatorSubsts) array<Substitution>();
		new (&builtinTypeSubsts) array<Substitution>();
		new (&stdlibTemplateSubsts) array<Substitution>();


		// New has special syntax (not currently supported).
		operatorSubsts.emplace("nw", " new"),
		operatorSubsts.emplace("na", " new[]");

		// Works except that the 'gs' prefix is not supported.
		operatorSubsts.emplace("dl", " delete");
		operatorSubsts.emplace("da", " delete[]");

		// unary operators
		operatorSubsts.emplace("ps", "+");     unaryOperators.emplace("ps", "+");
		operatorSubsts.emplace("ng", "-");     unaryOperators.emplace("ng", "-");
		operatorSubsts.emplace("ad", "&");     unaryOperators.emplace("ad", "&");
		operatorSubsts.emplace("de", "*");     unaryOperators.emplace("de", "*");
		operatorSubsts.emplace("co", "~");     unaryOperators.emplace("co", "~");
		operatorSubsts.emplace("nt", "!");     unaryOperators.emplace("nt", "!");
		operatorSubsts.emplace("pp", "++");    unaryOperators.emplace("pp", "++");
		operatorSubsts.emplace("mm", "--");    unaryOperators.emplace("mm", "--");

		// binary ops
		operatorSubsts.emplace("pl", "+");     binaryOperators.emplace("pl", "+");
		operatorSubsts.emplace("mi", "-");     binaryOperators.emplace("mi", "-");
		operatorSubsts.emplace("ml", "*");     binaryOperators.emplace("ml", "*");
		operatorSubsts.emplace("dv", "/");     binaryOperators.emplace("dv", "/");
		operatorSubsts.emplace("rm", "%");     binaryOperators.emplace("rm", "%");
		operatorSubsts.emplace("an", "&");     binaryOperators.emplace("an", "&");
		operatorSubsts.emplace("or", "|");     binaryOperators.emplace("or", "|");
		operatorSubsts.emplace("eo", "^");     binaryOperators.emplace("eo", "^");
		operatorSubsts.emplace("aS", "=");     binaryOperators.emplace("aS", "=");
		operatorSubsts.emplace("pL", "+=");    binaryOperators.emplace("pL", "+=");
		operatorSubsts.emplace("mI", "-=");    binaryOperators.emplace("mI", "-=");
		operatorSubsts.emplace("mL", "*=");    binaryOperators.emplace("mL", "*=");
		operatorSubsts.emplace("dV", "/=");    binaryOperators.emplace("dV", "/=");
		operatorSubsts.emplace("rM", "%=");    binaryOperators.emplace("rM", "%=");
		operatorSubsts.emplace("aN", "&=");    binaryOperators.emplace("aN", "&=");
		operatorSubsts.emplace("oR", "|=");    binaryOperators.emplace("oR", "|=");
		operatorSubsts.emplace("eO", "^=");    binaryOperators.emplace("eO", "^=");
		operatorSubsts.emplace("ls", "<<");    binaryOperators.emplace("ls", "<<");
		operatorSubsts.emplace("rs", ">>");    binaryOperators.emplace("rs", ">>");
		operatorSubsts.emplace("lS", "<<=");   binaryOperators.emplace("lS", "<<=");
		operatorSubsts.emplace("rS", ">>=");   binaryOperators.emplace("rS", ">>=");
		operatorSubsts.emplace("eq", "==");    binaryOperators.emplace("eq", "==");
		operatorSubsts.emplace("ne", "!=");    binaryOperators.emplace("ne", "!=");
		operatorSubsts.emplace("lt", "<");     binaryOperators.emplace("lt", "<");
		operatorSubsts.emplace("gt", ">");     binaryOperators.emplace("gt", ">");
		operatorSubsts.emplace("le", "<=");    binaryOperators.emplace("le", "<=");
		operatorSubsts.emplace("ge", ">=");    binaryOperators.emplace("ge", ">=");
		operatorSubsts.emplace("aa", "&&");    binaryOperators.emplace("aa", "&&");
		operatorSubsts.emplace("oo", "||");    binaryOperators.emplace("oo", "||");
		operatorSubsts.emplace("cm", ",");     binaryOperators.emplace("cm", ",");
		operatorSubsts.emplace("pm", "->*");   binaryOperators.emplace("pm", "->*");
		operatorSubsts.emplace("ix", "[]");    binaryOperators.emplace("ix", "[]");

		// ternary.
		operatorSubsts.emplace("qu", "?");     ternaryOperators.emplace("qu", "?");


		operatorSubsts.emplace("pt", "->");
		operatorSubsts.emplace("cl", "()");
		operatorSubsts.emplace("st", " sizeof");
		operatorSubsts.emplace("sz", " sizeof");

		builtinTypeSubsts.emplace("v", "void");
		builtinTypeSubsts.emplace("w", "wchar_t");
		builtinTypeSubsts.emplace("b", "bool");
		builtinTypeSubsts.emplace("c", "char");
		builtinTypeSubsts.emplace("a", "signed char");
		builtinTypeSubsts.emplace("h", "unsigned char");
		builtinTypeSubsts.emplace("s", "short");
		builtinTypeSubsts.emplace("t", "unsigned short");
		builtinTypeSubsts.emplace("i", "int");
		builtinTypeSubsts.emplace("j", "unsigned int");
		builtinTypeSubsts.emplace("l", "long");
		builtinTypeSubsts.emplace("m", "unsigned long");
		builtinTypeSubsts.emplace("x", "long long");
		builtinTypeSubsts.emplace("y", "unsigned long long");
		builtinTypeSubsts.emplace("n", "__int128");
		builtinTypeSubsts.emplace("o", "unsigned __int128");
		builtinTypeSubsts.emplace("f", "float");
		builtinTypeSubsts.emplace("d", "double");
		builtinTypeSubsts.emplace("e", "long double");
		builtinTypeSubsts.emplace("g", "__float128");
		builtinTypeSubsts.emplace("z", "...");
		builtinTypeSubsts.emplace("Di", "char32_t");
		builtinTypeSubsts.emplace("Ds", "char16_t");
		builtinTypeSubsts.emplace("Da", "auto");
		builtinTypeSubsts.emplace("Dc", "decltype(auto)");
		builtinTypeSubsts.emplace("Dn", "std::nullptr_t");

		stdlibTemplateSubsts.emplace("St", "std::");
		stdlibTemplateSubsts.emplace("Sa", "std::allocator");
		stdlibTemplateSubsts.emplace("Sb", "std::basic_string");
		stdlibTemplateSubsts.emplace("Ss", "std::string");
		stdlibTemplateSubsts.emplace("Si", "std::istream");
		stdlibTemplateSubsts.emplace("So", "std::ostream");
		stdlibTemplateSubsts.emplace("Sd", "std::iostream");
	}











	static bool isdigit(char c) { return c >= '0' && c <= '9'; }

	struct State
	{
		nx::array<nx::string> subs;
		nx::array<nx::string> templateSubs;
		nx::stack<nx::string> unqualifiedNameStack;

		// only template funcs & function types get their return type encoded
		// remember to reset this!!!
		bool didJustParseTemplate = false;
	};

	static nx::string parseType(nx::string_view& s, State& st);
	static nx::string parseName(nx::string_view& s, State& st, bool type = false);
	static nx::string parseEncoding(nx::string_view& s, State& st);
	static nx::string parseLambdaType(nx::string_view& s, State& st);
	static nx::string parseMangledName(nx::string_view& s, State& st);
	static nx::string parseBareFunctionType(nx::string_view& s, State& st);
	static nx::string parseUnqualifiedName(nx::string_view& s, State& st, bool type);

	static nx::string collapseRefs(const nx::string& thing, bool rvalue);

	static nx::string parseExpr(nx::string_view& s, State& st);
	static nx::string parsePrimaryExpr(nx::string_view& s, State& st);



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

	static int parseSeqId(nx::string_view& s)
	{
		// <seq-id> ::= <0-9A-Z>+

		int len = 0;
		while(isdigit(s[0]) || (s[0] >= 'A' && s[0] <= 'Z'))
		{
			len *= 36;

			if(isdigit(s[0]))   len += s[0] - '0';
			else                len += s[0] - 'A';

			s.remove_prefix(1);
		}

		return len;
	}

	static nx::string collapseRefs(const nx::string& thing, bool rvalue)
	{
		// i'd really rather not build an AST, so we do textual search to perform reference collapsing.
		// for reference (haha):
		// T& &   ->  T&
		// T& &&  ->  T&
		// T&& &  ->  T&
		// T&& && ->  T&&

		if(thing.empty())
			return thing;

		if(rvalue)
		{
			if(thing.back() == '&')
			{
				// don't do anything. looking at the collapsing rules, if we want to add an rvalue ref (&&), if the
				// existing type is already some kind of reference, the additional && (us) collapses to nothing.
				return thing;
			}
			else
			{
				// not an existing reference.
				return thing + "&&";
			}
		}
		else
		{
			if(thing.back() == '&')
			{
				// check collapserino
				if(thing.size() > 1 && thing[thing.size() - 2] == '&')
				{
					// this is a '&&'. we have a '&'. collapse to '&'.
					return thing.substring(0, thing.size() - 1);
				}
				else
				{
					// this is a '&', we have a '&'. collapse to '&'.
					// ie to say don't do anything, because we already have a '&'.
					return thing;
				}
			}
			else
			{
				// not a ref -- just add the & then.
				return thing + "&";
			}
		}
	}


	static nx::string parseSubstitution(nx::string_view& s, State& st)
	{
		if(s[0] != 'S')
			return "";

		string ret;

		if(s[1] == '_')
		{
			// 0th backref.
			assert(st.subs.size() > 0);
			ret += st.subs.front();

			s.remove_prefix(2);
		}
		else if(isdigit(s[1]) || (s[1] >= 'A' && s[1] <= 'Z'))
		{
			// numbered backref.
			s.remove_prefix(1);
			int n = parseSeqId(s);

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

			if(!found) abort("invalid substitution! (current: %s)", s.data());

			s.remove_prefix(2);
			return ret;
		}

		return ret;
	}

	static nx::string parseTemplateParam(nx::string_view& s, State& st)
	{
		/*
			<template-param>    ::= T_	# first template parameter
								::= T <parameter-2 non-negative number> _
		*/

		if(s[0] != 'T')
			return "";

		s.remove_prefix(1);

		string ret;
		if(s[0] == '_')
		{
			s.remove_prefix(1);

			// T_.
			// if we have nothing inside, insert an 'auto'...
			if(st.templateSubs.size() > 0)
			{
				ret = st.templateSubs[0];
			}
			else
			{
				ret = "auto:1";
				st.templateSubs.append(ret);
			}
		}
		else
		{
			assert(isdigit(s[0]));
			size_t n = parseLength(s) + 1;

			assert(s[0] == '_');
			s.remove_prefix(1);

			if(n < st.templateSubs.size())
			{
				ret = st.templateSubs[n];
			}
			else
			{
				ret = sprint("auto:%d", n + 1);
				st.templateSubs.append(ret);
			}
		}

		st.subs.append(ret);
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
			s.remove_prefix(1);
			auto ret = parseExpr(s, st);

			if(s[0] != 'E')
			{
				assert(s[0] == 'E');
			}

			s.remove_prefix(1);

			st.templateSubs.append(ret);
			return ret;
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

			st.templateSubs.append(ret);
			return ret;
		}
		else
		{
			string ret;
			if(s[0] == 'L') ret = parsePrimaryExpr(s, st);
			else            ret = parseType(s, st);

			st.templateSubs.append(ret);
			return ret;
		}
	}

	static nx::string parseTemplateArgumentList(nx::string_view& s, State& st)
	{
		/*
			<template-args> ::= I <template-arg>+ E
		*/

		if(s[0] != 'I')
			return "";

		s.remove_prefix(1);

		string ret = "<";
		while(s.size() > 0)
		{
			auto arg = parseTemplateArgument(s, st);
			ret += arg;

			if(s[0] != 'E') ret += ", ";
			else            break;
		}

		assert(s[0] == 'E');
		s.remove_prefix(1);

		return ret + ">";
	}







	static nx::string parseSourceName(nx::string_view& s, State& st)
	{
		int len = parseLength(s);
		if(len == 0)
			return "";

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
		s.remove_prefix(1);

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
			return "struct " + parseName(s, st, true);
		}
		else if(s.find("Tu") == 0)
		{
			return "union " + parseName(s, st, true);
		}
		else if(s.find("Te") == 0)
		{
			return "enum " + parseName(s, st, true);
		}
		else
		{
			return parseName(s, st, true);
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
		int n = 0;
		if(isdigit(s[0])) n = parseLength(s) + 1;

		ret += sprint("#%d", n + 1);

		assert(s[0] == '_');
		s.remove_prefix(1);

		return ret + "}";
	}

	static nx::string trySubstituteOperators(nx::string_view& s, State& st, bool prependOperator = true)
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
					return (prependOperator ? "operator" : "") + sub.replacement;
				}
			}
		}

		return "";
	}

	static nx::string parseUnqualifiedName(nx::string_view& s, State& st, bool type)
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

			// st.unqualifiedNameStack.push(ret);
			return ret;
		}
		else if(s[0] == 'L')
		{
			s.remove_prefix(1);
			auto ret = parseSourceName(s, st);

			// st.unqualifiedNameStack.push(ret);
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
		else if(!type)
		{
			auto op = trySubstituteOperators(s, st);
			if(op.size() > 0) return op;
		}

		return "";
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


		// string cvQuals;
		int restricts = 0;
		int volatiles = 0;
		int consts = 0;
		int refs = 0;

		while(s[0] == 'r' || s[0] == 'V' || s[0] == 'K')
		{
			if(s[0] == 'r') restricts++; // cvQuals += " restrict";
			if(s[0] == 'V') volatiles++; // cvQuals += " volatile";
			if(s[0] == 'K') consts++; // cvQuals += " const";

			s.remove_prefix(1);
		}

		if(s[0] == 'R')         { refs += 1; s.remove_prefix(1); }
		else if(s[0] == 'O')    { refs += 2; s.remove_prefix(1); }

		// parse the 'prefix'.
		string ret;

		while(s[0] != 'E')
		{
			bool subst = false;

			string cur;

			// check for substitutions.
			if(s[0] == 'S')         subst = true, cur = parseSubstitution(s, st);
			else                    cur = parseUnqualifiedName(s, st, false);

			ret += cur;

			// apparently the last segment does not make a substitution?
			if(s[0] == 'E')         break;

			{
				if(!subst)
					st.subs.append(ret);

				if(s[0] == 'I')
				{
					auto templs = parseTemplateArgumentList(s, st);

					cur += templs;
					ret += templs;

					// add the sub for the whole instantiated thing
					// but only if we're not the last item??

					if(s[0] != 'E')
						st.subs.append(ret);

					else // if this is the final piece, prime the parser to receive a return type later.
						st.didJustParseTemplate = true;
				}

				// if the thing already ended with '::', don't add ::
				// TODO this is some ugly-ass hack
				if(s[0] != 'E' && ret.size() > 2 && ret[ret.size() - 2] != ':' && ret[ret.size() - 1] != ':')
					ret += "::";
			}

			st.unqualifiedNameStack.push(cur);
		}

		assert(s[0] == 'E');
		s.remove_prefix(1);

		while(restricts--)  ret += " restrict";
		while(volatiles--)  ret += " volatile";
		while(consts--)     ret += " const";
		while(refs--)       ret += "&";

		return ret;
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

	static nx::string parseName(nx::string_view& s, State& st, bool type)
	{
		/*
			<name>  ::= <nested-name>
					::= <unscoped-name>
					::= <unscoped-template-name> <template-args>
					::= <local-name>

			<unscoped-name> ::= <unqualified-name>
							::= St <unqualified-name>   # ::std::

			<unscoped-template-name>	::= <unscoped-name>
										::= <substitution>
		*/

		if(s[0] == 'N')
		{
			return parseNestedName(s, st);
		}
		else if(s[0] == 'Z')
		{
			return parseLocalName(s, st);
		}
		else
		{
			// unscoped-name

			nx::string ret;
			if(s.find("St") == 0)
			{
				ret += "std::";
				s.remove_prefix(2);

				ret += parseUnqualifiedName(s, st, type);

				st.subs.append(ret);
			}
			else if(s[0] == 'S')
			{
				ret += parseSubstitution(s, st);
			}
			else
			{
				ret += parseUnqualifiedName(s, st, type);
			}

			if(s.size() > 0 && s[0] == 'I')
				ret += parseTemplateArgumentList(s, st);

			return ret;
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
					::= Dp <type>       # pack expansion (C++11)
					::= <substitution>
		*/

		// println("parseType: %s", s.data());

		assert(s.size() > 0);

		if(s[0] == 'P') { s.remove_prefix(1); auto ret = parseType(s, st); ret += "*"; st.subs.append(ret); return ret; }
		if(s[0] == 'K') { s.remove_prefix(1); auto ret = parseType(s, st); ret += " const"; st.subs.append(ret); return ret; }
		if(s[0] == 'r') { s.remove_prefix(1); auto ret = parseType(s, st); ret += " restrict"; st.subs.append(ret); return ret; }
		if(s[0] == 'V') { s.remove_prefix(1); auto ret = parseType(s, st); ret += " volatile"; st.subs.append(ret); return ret; }
		if(s[0] == 'R')
		{
			s.remove_prefix(1);
			auto ret = collapseRefs(parseType(s, st), /* rvalue: */ false);

			st.subs.append(ret);
			return ret;
		}
		if(s[0] == 'O')
		{
			s.remove_prefix(1);
			auto ret = collapseRefs(parseType(s, st), /* rvalue: */ true);

			st.subs.append(ret);
			return ret;
		}
		if(s[0] == 'F')
		{
			// function type.
			s.remove_prefix(1);

			// no idea what 'Y' means.
			if(s[0] == 'Y') s.remove_prefix(1);

			// for types, the return type is the first 'type'.
			auto retty = parseType(s, st);
			assert(retty.size() > 0);

			retty += " (*)";
			retty += parseBareFunctionType(s, st);

			if(s[0] == 'R') { s.remove_prefix(1); retty += " &"; }
			if(s[0] == 'O') { s.remove_prefix(1); retty += " &&"; }

			assert(s[0] == 'E');
			s.remove_prefix(1);

			st.subs.append(retty);
			return retty;
		}
		if(s[0] == 'A')
		{
			// array type.
			s.remove_prefix(1);

			nx::string dim = " []";
			if(s[0] != '_')
			{
				if(isdigit(s[0]))
					dim = sprint(" [%d]", parseLength(s));

				else
					dim = sprint(" [%s]", parseExpr(s, st));
			}

			assert(s[0] == '_');
			s.remove_prefix(1);

			auto ret = parseType(s, st);
			ret += dim;

			st.subs.append(ret);
			return ret;
		}
		if(s.find("Do") == 0 || s.find("DO") == 0 || s.find("Dw") == 0)
		{
			// exception specifier.
			s.remove_prefix(2);
			auto ret = parseType(s, st);
			ret += " noexcept";
			return ret;
		}
		if(s.find("Dp") == 0)
		{
			// parameter pack
			s.remove_prefix(2);
			auto ret = parseType(s, st);
			ret += "...";
			return ret;
		}
		if(s[0] != 'E')
		{
			if(auto cls = parseClassType(s, st); cls.size() > 0)
			{
				st.subs.append(cls);
				return cls;
			}
			else
			{
				// see if we can substitute stuff.
				for(const auto& sub : builtinTypeSubsts)
				{
					if(strncmp(s.data(), sub.str.cstr(), sub.str.size()) == 0)
					{
						s.remove_prefix(sub.str.size());
						return sub.replacement;
					}
				}
			}
		}
		if(s[0] == 'S')
		{
			// substitution.
			return parseSubstitution(s, st);
		}
		if(s[0] == 'T')
		{
			return parseTemplateParam(s, st);
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
		if(st.didJustParseTemplate)
		{
			// reset the flag.
			st.didJustParseTemplate = false;

			// the first type is our return type!
			ret = parseType(s, st) + " " + ret;
		}

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

		// return parseEncoding(s, st);
		auto ret = parseEncoding(s, st);
		return ret;
	}

	static nx::string parseDecltype(nx::string_view& s, State& st)
	{
		if(s.find("Dt") == 0 || s.find("DT") == 0)
		{
			s.remove_prefix(2);
			nx::string ret = "decltype(";

			ret += parseExpr(s, st);
			assert(s[0] == 'E');

			return ret + ")";
		}

		return "";
	}

	static nx::string parseUnresolvedName(nx::string_view& s, State& st)
	{
		/*

			<unresolved-name>   ::= [gs] <base-unresolved-name>                     # x or (with "gs") ::x
								::= sr <unresolved-type> <base-unresolved-name>     # T::x / decltype(p)::x

								# T::N::x /decltype(p)::N::x
								::= srN <unresolved-type> <unresolved-qualifier-level>+ E <base-unresolved-name>

								# A::x, N::y, A<T>::z; "gs" means leading "::"
								::= [gs] sr <unresolved-qualifier-level>+ E <base-unresolved-name>

			<unresolved-type>   ::= <template-param> [ <template-args> ]            # T:: or T<X,Y>::
								::= <decltype>                                      # decltype(p)::
								::= <substitution>

			<unresolved-qualifier-level> ::= <simple-id>

			<simple-id> ::= <source-name> [ <template-args> ]

			<base-unresolved-name>  ::= <simple-id>                                # unresolved name
			                      	::= on <operator-name>                         # unresolved operator-function-id
			                      	::= on <operator-name> <template-args>         # unresolved operator template-id
			                      	::= dn <destructor-name>                       # destructor or pseudo-destructor;
			                                                                       # e.g. ~X or ~X<N-1>

			<destructor-name>       ::= <unresolved-type>                               # e.g., ~T or ~decltype(f())
									::= <simple-id>                                     # e.g., ~A<2*N>
		*/

		nx::string ret;
		if(s.find("gs") == 0)
			ret += "::", s.remove_prefix(2);

		auto parse_unresolved_type = [](nx::string_view& s, State& st) -> nx::string {
			if(auto tp = parseTemplateParam(s, st); !tp.empty())
				return tp + parseTemplateArgumentList(s, st);

			else if(auto dt = parseDecltype(s, st); !dt.empty())
				return dt;

			else if(auto sb = parseSubstitution(s, st); !sb.empty())
				return sb;

			return "";
		};

		auto parse_simple_id = [](nx::string_view& s, State& st) -> nx::string {
			auto ret = parseSourceName(s, st);
			ret += parseTemplateArgumentList(s, st);

			return ret;
		};

		auto parse_base_unresolved_name = [&parse_unresolved_type, &parse_simple_id]
			(nx::string_view& s, State& st) -> nx::string
		{
			if(s.find("on") == 0)
			{
				s.remove_prefix(2);

				// parseTemplateArgumentList returns "" if there's no list. (ie. it doesn't start with 'I')
				return trySubstituteOperators(s, st).append(parseTemplateArgumentList(s, st));
			}
			else if(s.find("dn") == 0)
			{
				if(auto ut = parse_unresolved_type(s, st); !ut.empty())
					return "~" + ut;

				return "~" + parse_simple_id(s, st);
			}
			else
			{
				return parse_simple_id(s, st);
			}
		};


		if(s.find("srN") == 0)
		{
			s.remove_prefix(3);
			ret += "::";
			ret += parse_unresolved_type(s, st);

			while(s[0] != 'E')
			{
				ret += "::";
				ret += parse_simple_id(s, st);
			}

			ret += parse_base_unresolved_name(s, st);
			return ret;
		}
		else if(s.find("sr") == 0)
		{
			bool have_qual_levels = (ret == "::");
			s.remove_prefix(2);

			if(have_qual_levels)
			{
				ret = "::";
				while(s[0] != 'E')
				{
					ret += "::";
					ret += parse_simple_id(s, st);
				}

				assert(s[0] == 'E');

				ret += parse_base_unresolved_name(s, st);
				return ret;
			}
			else
			{
				ret += parse_unresolved_type(s, st);
				ret += parse_base_unresolved_name(s, st);

				// hack: idk, but it appears we can have additional source names (6foozle) after this...
				auto sn = parse_simple_id(s, st);
				while(!sn.empty())
				{
					ret += "::";
					ret += sn;

					sn = parse_simple_id(s, st);
				}

				return ret;
			}
		}
		else
		{
			return parse_base_unresolved_name(s, st);
		}
	}


	// welcome to the fucked up part: expression parsing.
	static nx::string parseExpr(nx::string_view& s, State& st)
	{
		// TODO: currently stuck on template param substitutions.


		/*
			<expression>    ::= <unary operator-name> <expression>
							::= <binary operator-name> <expression> <expression>
							::= <ternary operator-name> <expression> <expression> <expression>
							::= pp_ <expression>                             # prefix ++
							::= mm_ <expression>                             # prefix --

							::= cl <expression>+ E                           # expression (expr-list), call
							::= cv <type> <expression>                       # type (expression), conversion with one argument
							::= cv <type> _ <expression>* E                  # type (expr-list), conversion with other than one argument
							::= tl <type> <braced-expression>* E             # type {expr-list}, conversion with braced-init-list argument
							::= il <braced-expression>* E                    # {expr-list}, braced-init-list in any other context
							::= [gs] nw <expression>* _ <type> E             # new (expr-list) type
							::= [gs] nw <expression>* _ <type> <initializer> # new (expr-list) type (init)
							::= [gs] na <expression>* _ <type> E             # new[] (expr-list) type
							::= [gs] na <expression>* _ <type> <initializer> # new[] (expr-list) type (init)
							::= [gs] dl <expression>                         # delete expression
							::= [gs] da <expression>                         # delete[] expression
							::= dc <type> <expression>                       # dynamic_cast<type> (expression)
							::= sc <type> <expression>                       # static_cast<type> (expression)
							::= cc <type> <expression>                       # const_cast<type> (expression)
							::= rc <type> <expression>                       # reinterpret_cast<type> (expression)
							::= ti <type>                                    # typeid (type)
							::= te <expression>                              # typeid (expression)
							::= st <type>                                    # sizeof (type)
							::= sz <expression>                              # sizeof (expression)
							::= at <type>                                    # alignof (type)
							::= az <expression>                              # alignof (expression)
							::= nx <expression>                              # noexcept (expression)
							::= <template-param>
							::= <function-param>
							::= dt <expression> <unresolved-name>   # expr.name
							::= pt <expression> <unresolved-name>   # expr->name
							::= ds <expression> <expression>        # expr.*expr
							::= sZ <template-param>                 # sizeof...(T), size of a template parameter pack
							::= sZ <function-param>                 # sizeof...(parameter), size of a function parameter pack
							::= sP <template-arg>* E                # sizeof...(T), size of a captured template parameter pack from an alias template
							::= sp <expression>                     # expression..., pack expansion
							::= tw <expression>                     # throw expression
							::= tr                                  # throw with no operand (rethrow)
							::= <unresolved-name>                   # f(p), N::f(p), ::f(p),
																	# freestanding dependent name (e.g., T::x),
																	# objectless nonstatic member reference
							::= <expr-primary>
		*/

		// note: prefix ++ and -- have the same string prefix (i think) as their suffix counterparts,
		// so try them first.
		if(s.find("pp_") == 0)  { s.remove_prefix(3); return "++" + parseExpr(s, st); }
		if(s.find("mm_") == 0)  { s.remove_prefix(3); return "--" + parseExpr(s, st); }

		for(const auto& un : unaryOperators)
		{
			if(s.find(un.str.cstr()) == 0)
			{
				s.remove_prefix(un.str.size());
				return un.replacement + parseExpr(s, st);
			}
		}

		for(const auto& bi : binaryOperators)
		{
			if(s.find(bi.str.cstr()) == 0)
			{
				s.remove_prefix(bi.str.size());
				return bi.replacement + parseExpr(s, st).append(parseExpr(s, st));
			}
		}

		for(const auto& te : ternaryOperators)
		{
			if(s.find(te.str.cstr()) == 0)
			{
				s.remove_prefix(te.str.size());
				return te.replacement + parseExpr(s, st).append(parseExpr(s, st)).append(parseExpr(s, st));
			}
		}









		if(s[0] == 'T')
			return parseTemplateParam(s, st);

		if(auto un = parseUnresolvedName(s, st); !un.empty())
			return un;

		return parsePrimaryExpr(s, st);
	}

	static nx::string parsePrimaryExpr(nx::string_view& s, State& st)
	{
		/*
			<expr-primary>  ::= L <type> <value number> E                          # integer literal
							::= L <type> <value float> E                           # floating literal
							::= L <string type> E                                  # string literal
							::= L <nullptr type> E                                 # nullptr literal (i.e., "LDnE")
							::= L <pointer type> 0 E                               # null pointer template argument
							::= L <type> <real-part float> _ <imag-part float> E   # complex floating point literal (C 2000)
							::= L _Z <encoding> E                                  # external name
		*/

		assert(s[0] == 'L');
		s.remove_prefix(1);

		// check for builtin number types.
		string ret;
		switch(s[0])
		{
			case 'b':   s.remove_prefix(1); ret += (s[0] - '0' ? "true" : "false"); s.remove_prefix(1); break;
			case 'a':
			case 'c':   s.remove_prefix(1); ret += sprint("'%c'", parseLength(s)); break;

			case 'n':   s.remove_prefix(1); ret += "-"; // fallthrough

			case 'h':
			case 's':
			case 't':
			case 'i':
			case 'j':
			case 'l':
			case 'm':
			case 'x':
			case 'y':   s.remove_prefix(1); ret += sprint("%d", parseLength(s)); break;

			// don't support floating point literals for now...
			case 'f':
			case 'd':
			case 'e':
			case 'g':   s.remove_prefix(1); while(isdigit(s[0]) || (s[0] >= 'a' && s[0] <= 'f')) s.remove_prefix(1); break;
		}

		if(s.find("Dn") == 0)
		{
			s.remove_prefix(2);
			ret += "nullptr";
		}
		else if(s.find("_Z") == 0)
		{
			s.remove_prefix(2);
			ret = parseEncoding(s, st);
		}
		else
		{
			// just try to parse a regular type? we'll always use a number then.
			ret += "(";
			ret += parseType(s, st);
			ret += ") ";
			ret += sprint("%d", parseLength(s));
		}

		assert(s[0] == 'E');
		s.remove_prefix(1);

		return ret;
	}














































	nx::string demangleSymbol(const nx::string& mangled)
	{
		// then it's not mangled.
		if(mangled.find("_Z") != 0) return mangled;

		auto input = string_view(mangled);
		serial::debugprintf("%s\n", mangled.cstr());

		// _ZN3krt6stringIN2nx10_allocatorENS1_8_aborterEEC1ERKS4_
		// _ZN3krt5arrayIPN2nx3vfs10FilesystemENS1_10_allocatorENS1_8_aborterEEaSEOS7_
		// _ZN3krt4moveIRcEEONS_16remove_referenceIT_E4typeEOS3_
		// _ZN3krt15prio_q_internal11skip_vectorIN2nx10interrupts13irq_handler_tELm32ENS2_10_allocatorENS2_8_aborterEE7destroyIS4_EENSt9enable_ifIXntsrSt6is_podIT_E5valueEvE4typeEv
		// _ZZN2nxL8__loggerIJRjS1_EEEviPKcS3_DpOT_ENUlPvS3_mE_4_FUNES7_S3_m
		// _ZN2nx9_internal23_consume_neither_sprintIRPKcJRA_cRiRmEvPvEEmPFmS9_S3_mES9_RKNS_11format_argsES3_OT_DpOT0_

		// TODO: this shit don't work:
		//! _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm.isra.51
		//! _ZN3vfs10concatPathERKSt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS6_EE.cold.61

		State st;
		auto ret = parseMangledName(input, st);

		// serial::debugprintf("**  %s\n", ret.cstr());
		return ret;
	}


}
}







































