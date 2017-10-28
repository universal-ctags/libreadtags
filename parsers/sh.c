/*
*   Copyright (c) 2000-2002, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License version 2 or (at your option) any later version.
*
*   This module contains functions for generating tags for scripts for the
*   Bourne shell (and its derivatives, the Korn and Z shells).
*/

/*
*   INCLUDE FILES
*/
#include "general.h"  /* must always come first */

#include <string.h>

#include "entry.h"
#include "kind.h"
#include "parse.h"
#include "read.h"
#include "promise.h"
#include "routines.h"
#include "vstring.h"
#include "xtag.h"

/*
*   DATA DEFINITIONS
*/
typedef enum {
	K_NOTHING = -1,		/* place holder. Never appears on tags file. */
	K_ALIAS,
	K_FUNCTION,
	K_SOURCE,
	K_HEREDOCLABEL,
} shKind;

typedef enum {
	R_SOURCE_GENERIC,
} shScriptRole;

static roleDesc ShScriptRoles [] = {
	{ true, "loaded", "loaded" },
};

static kindDefinition ShKinds [] = {
	{ true, 'a', "alias", "aliases"},
	{ true, 'f', "function", "functions"},
	{ true, 's', "script", "script files",
	  .referenceOnly = true, ATTACH_ROLES (ShScriptRoles) },
	{ true, 'h', "heredoc", "label for here document" },
};

/*
*   FUNCTION DEFINITIONS
*/

static bool isFileChar  (int c)
{
	return (isalnum (c)
		|| c == '_' || c == '-'
		|| c == '/' || c == '.'
		|| c == '+' || c == '^'
		|| c == '%' || c == '@'
		|| c == '~');
}

static bool isIdentChar (int c)
{
	return (isalnum (c) || c == '_' || c == '-');
}

/* bash allows all kinds of crazy stuff as the identifier after 'function' */
static bool isBashFunctionChar (int c)
{
	return (c > 1 /* NUL and SOH are disallowed */ && c != 0x7f &&
	        /* blanks are disallowed, but VT and FF (and CR to some extent, but
	         * let's not fall into the pit of craziness) */
	        c != ' ' && c != '\t' && c != '\n' && c != '\r' &&
	        c != '"' && c != '\'' && c != '$' && c != '`' && c != '\\' &&
	        c != '&' && c != ';' &&
	        c != '(' && c != ')' &&
	        c != '<' && c != '>');
}

static const unsigned char *skipDoubleString (const unsigned char *cp)
{
	const unsigned char* prev = cp;
	cp++;
	while ((*cp != '"' || *prev == '\\') && *cp != '\0')
	{
		prev = cp;
		cp++;
	}
	return cp;
}

static const unsigned char *skipSingleString (const unsigned char *cp)
{
	cp++;
	while (*cp != '\'' && *cp != '\0')
		cp++;
	return cp;
}

static bool isEnvCommand (const vString *cmd)
{
	const char *lc = vStringValue(cmd);
	const char * tmp = baseFilename (lc);

	return (strcmp(tmp, "env") == 0);
}

static void findShTags (void)
{
	vString *name = vStringNew ();
	const unsigned char *line;
	vString *hereDocDelimiter = NULL;
	bool hereDocIndented = false;
	int hereDocTagIndex = CORK_NIL;
	bool (* check_char)(int);

	vString *args[2];
	langType sublang = LANG_IGNORE;
	unsigned long startLine;
	long startCharOffset;

	args[0] = vStringNew ();
	args[1] = vStringNew ();

	while ((line = readLineFromInputFile ()) != NULL)
	{
		const unsigned char* cp = line;
		shKind found_kind = K_NOTHING;

		if (hereDocDelimiter)
		{
			if (hereDocIndented)
			{
				while (*cp == '\t')
					cp++;
			}
			if (strcmp ((const char *) cp, vStringValue (hereDocDelimiter)) == 0)
			{
				if (hereDocTagIndex != CORK_NIL)
				{
					tagEntryInfo *tag = getEntryInCorkQueue (hereDocTagIndex);
					tag->extensionFields.endLine = getInputLineNumber ();
					hereDocTagIndex = CORK_NIL;
				}

				if (sublang != LANG_IGNORE)
					makePromise (getLanguageName(sublang),
								 startLine, startCharOffset,
								 getInputLineNumber(), 0,
								 startCharOffset);
				sublang = LANG_IGNORE;

				vStringDelete (hereDocDelimiter);
				hereDocDelimiter = NULL;
			}
			continue;
		}

		vStringClear(args[0]);
		vStringClear(args[1]);
		while (*cp != '\0')
		{
			/* jump over whitespace */
			while (isspace ((int)*cp))
				cp++;

			/* jump over strings */
			if (*cp == '"')
				cp = skipDoubleString (cp);
			else if (*cp == '\'')
				cp = skipSingleString (cp);
			/* jump over comments */
			else if (*cp == '#')
				break;
			/* jump over here-documents */
			else if (cp[0] == '<' && cp[1] == '<')
			{
				const unsigned char *start, *end;
				bool trimEscapeSequences = false;
				bool quoted = false;
				cp += 2;
				/* an optional "-" strips leading tabulations from the heredoc lines */
				if (*cp != '-')
					hereDocIndented = false;
				else
				{
					hereDocIndented = true;
					cp++;
				}
				while (isspace (*cp))
					cp++;
				start = end = cp;
				/* the delimiter can be surrounded by quotes */
				if (*cp == '"')
				{
					start++;
					end = cp = skipDoubleString (cp);
					/* we need not to worry about variable substitution, they
					 * don't happen in heredoc delimiter definition */
					trimEscapeSequences = true;
					quoted = true;
				}
				else if (*cp == '\'')
				{
					start++;
					end = cp = skipSingleString (cp);
					quoted = true;
				}
				else
				{
					while (isIdentChar ((int) *cp))
						cp++;
					end = cp;
				}
				if (end > start || quoted)
				{
					/* The input may be broken as a shell script but we need to avoid
					   memory leaking. */
					if (hereDocDelimiter)
						vStringClear(hereDocDelimiter);
					else
						hereDocDelimiter = vStringNew ();
					for (; end > start; start++)
					{
						if (trimEscapeSequences && *start == '\\')
							start++;
						vStringPut (hereDocDelimiter, *start);
					}
					if (vStringLength(hereDocDelimiter) > 0)
						hereDocTagIndex = makeSimpleTag(hereDocDelimiter, K_HEREDOCLABEL);

					if (!vStringIsEmpty(args[0]))
					{
						const char *cmd;

						cmd = vStringValue(args[0]);
						if (isEnvCommand (args[0]))
						{
							cmd = NULL;
							if (!vStringIsEmpty(args[1]))
								cmd = vStringValue(args[1]);
						}
						if (cmd)
							cmd = baseFilename(cmd);

						sublang = getLanguageForCommand (cmd, 0);
						if (sublang != LANG_IGNORE)
						{
							startLine = getInputLineNumber () + 1;
							startCharOffset = 0;
						}
					}
				}
			}

			check_char = isBashFunctionChar;

			if (strncmp ((const char*) cp, "function", (size_t) 8) == 0  &&
				isspace ((int) cp [8]))
			{
				found_kind = K_FUNCTION;
				cp += 8;
			}
			else if (strncmp ((const char*) cp, "alias", (size_t) 5) == 0  &&
				isspace ((int) cp [5]))
			{
				check_char = isIdentChar;
				found_kind = K_ALIAS;
				cp += 5;
			}
			else if (cp [0] == '.'
				    && isspace((int) cp [1]))
			{
				found_kind = K_SOURCE;
				++cp;
				check_char = isFileChar;
			}
			else if (strncmp ((const char*) cp, "source", (size_t) 6) == 0
					 && isspace((int) cp [6]))
			{
				found_kind = K_SOURCE;
				cp += 6;
				check_char = isFileChar;
			}

			if (found_kind != K_NOTHING)
				while (isspace ((int) *cp))
					++cp;

			// Get the name of the function, alias or file to be read by source
			if (! check_char ((int) *cp))
			{
				found_kind = K_NOTHING;
				if (*cp != '\0')
					++cp;
				continue;
			}
			while (check_char ((int) *cp))
			{
				vStringPut (name, (int) *cp);
				++cp;
			}

			while (isspace ((int) *cp))
				++cp;

			if ((found_kind != K_SOURCE)
			    && *cp == '(')
			{
				++cp;
				while (isspace ((int) *cp))
					++cp;
				if (*cp == ')')
				{
					found_kind = K_FUNCTION;
					++cp;
				}
			}

			if (found_kind != K_NOTHING)
			{
				if (found_kind == K_SOURCE)
				{
					if (isXtagEnabled (XTAG_REFERENCE_TAGS)
						&& ShKinds [K_SOURCE].enabled)
						makeSimpleRefTag (name, K_SOURCE, R_SOURCE_GENERIC);
				}
				else
					makeSimpleTag (name, found_kind);
				found_kind = K_NOTHING;
			}
			else if (!hereDocDelimiter)
			{
				if (vStringIsEmpty(args[0]))
					vStringCopy(args[0], name);
				else if (vStringIsEmpty(args[1]))
					vStringCopy(args[1], name);
			}
			vStringClear (name);
		}
	}
	vStringDelete (args[0]);
	vStringDelete (args[1]);
	vStringDelete (name);
	if (hereDocDelimiter)
		vStringDelete (hereDocDelimiter);
}

extern parserDefinition* ShParser (void)
{
	static const char *const extensions [] = {
		"sh", "SH", "bsh", "bash", "ksh", "zsh", "ash", NULL
	};
	static const char *const aliases [] = {
		"sh", "bash", "ksh", "zsh", "ash",
		/* major mode name in emacs */
		"shell-script",
		NULL
	};
	parserDefinition* def = parserNew ("Sh");
	def->kindTable      = ShKinds;
	def->kindCount  = ARRAY_SIZE (ShKinds);
	def->extensions = extensions;
	def->aliases = aliases;
	def->parser     = findShTags;
	def->useCork    = true;
	return def;
}
