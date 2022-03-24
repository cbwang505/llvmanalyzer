================
Parsing rulesets
================

Parsing a ruleset from a file is as easy as this.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        import yaramod

        y = yaramod.Yaramod(yaramod.ImportFeatures.AllCurrent)
        yara_file = y.parse_file('/opt/ruleset.yar')
        print(yara_file.text)

    .. tab:: C++

      .. code-block:: cpp

        #include <iostream>
        #include <yaramod/yaramod.h>

        int main() {
            auto y = yaramod::Yaramod(yaramod::ImportFeatures::AllCurrent);
            auto yaraFile = y.parseFile("/opt/ruleset.yar");
            std::cout << yaraFile->getText() << std::endl;
            return 0;
        }

You can alternatively also parse from memory.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        import yaramod

        y = yaramod.Yaramod(yaramod.ImportFeatures.AllCurrent)
        yara_file = y.parse_string(r'''
        rule abc {
            condition:
                true
        }
        ''')
        print(yara_file.text)

    .. tab:: C++

      .. code-block:: cpp

        #include <iostream>
        #include <sstream>
        #include <yaramod/yaramod.h>

        int main() {
            auto y = yaramod::Yaramod(yaramod::ImportFeatures::AllCurrent);
            std::istringstream input(R"(
            rule abc {
                condition:
                    true
            }
            )");
            auto yaraFile = y.parseStream(input);
            std::cout << yaraFile->getText() << std::endl;
            return 0;
        }

Rules
=====

You can iterate over all rules in the file.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for rule in yara_file.rules:
            print(rule.name)
            print(f'  Global: {rule.is_global}')
            print(f'  Private: {rule.is_private}')

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& rule: yaraFile->getRules()) {
            std::cout << rule->getName() << '\n'
                << "  Global: " << rule->isGlobal() << '\n'
                << "  Private: " << rule->isPrivate() << std::endl;
        }

Metas
=====

You can also access meta information of each rule

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for rule in yara_file.rules:
            for meta in rule.metas:
                if meta.value.is_string:
                    print('String meta: ', end='')
                elif meta.value.is_int:
                    print('Int meta: ', end='')
                elif meta.value.is_bool:
                    print('Bool meta: ', end='')
                print(f'{meta.key} = {meta.value.pure_text}')

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& rule : yaraFile->getRules()) {
            for (const auto& meta : rule->getMetas()) {
                if (meta->getValue()->isString())
                    std::cout << "String meta: ";
                else if (meta->getValue()->isInt())
                    std::cout << "Int meta: ";
                else if (meta->getValue()->isBool())
                    std::cout << "Bool meta: ";
                std::cout << meta->getName() << " = " << meta->getValue()->getPureText() << std::endl;
            }
        }

Strings
=======

Iterating over available strings is also possible and you can distinguish which kind of string you are dealing with.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for rule in yara_file.rules:
            for string in rule.strings:
                if string.is_plain:
                    print('Plain string: ', end='')
                elif string.is_hex:
                    print('Hex string: ', end='')
                elif string.is_regexp:
                    print('Regexp: ', end='')
                print(f'{string.identifier} = {string.text}')
                print(f'  ascii: {string.is_ascii}')
                print(f'  wide: {string.is_wide}')
                print(f'  nocase: {string.is_nocase}')
                print(f'  fullword: {string.is_fullword}')
                print(f'  private: {string.is_private}')
                print(f'  xor: {string.is_xor}')

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& rule : yaraFile->getRules()) {
            for (const auto& string : rule->getStrings()) {
                if (string->isPlain())
                    std::cout << "Plain string: ";
                else if (string->isHex())
                    std::cout << "Hex string: ";
                else if (string->isRegexp())
                    std::cout << "Regexp: ";
                std::cout << string->getIdentifier() << " = " << string->getText() << '\n'
                    << "  ascii: " << string->isAscii() << '\n'
                    << "  wide: " << string->isWide() << '\n'
                    << "  nocase: " << string->isNocase() << '\n'
                    << "  fullword: " << string->isFullword() << '\n'
                    << "  private: " << string->isPrivate() << '\n'
                    << "  xor: " << string->isXor() << std::endl;
            }
        }

Condition
=========

There are 2 ways you can look at the condition. The first one is that you just care about the textual representation of the condition and you don't care about the contents.
That one is pretty straightforward.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for rule in yara_file.rules:
            print(rule.condition.text)

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& rule : yaraFile->getRules())
            std::cout << rule->getCondition()->getText() << std::endl;

The second way is that you care about the contents of the condition and you would like to perform some kind of analysis over the condition. This part is a bit tricky because
the hierarchy of the whole condition is unknown to you so you would have to write a lot of recursive algorithms or other kinds of traversals on abstract syntax tree of your
condition. To ease this all, we have adopted similar approach as LLVM and provide you with an option to use `visitor design pattern <https://en.wikipedia.org/wiki/Visitor_pattern>`_ to perform the traversal.

.. note::

  If you are not faimilar with this kind of design pattern, just imagine that there are several types of expressions and statements that can be in the condition (integers, logical operations, arithmetic operations, ...).
  You want to perform *your operation* on all of them, taking their type into account. With visitor design pattern, you just define *your operation* for each type of expression or statement and that's it. You then
  *visit* each node of abstract syntax tree with *your operation* which is performed there.

Condition visitors
******************

Let's say we want to print each function that is in called in the rule condition.


.. tabs::

    .. tab:: Python

      .. code-block:: python

        class FunctionCallDumper(yaramod.ObservingVisitor):
            def visit_FunctionCallExpression(self, expr):
                print('Function call: {}'.format(expr.function.text))
                # Visit arguments because they can contain nested function calls
                for arg in expr.arguments:
                    arg.accept(self)

    .. tab:: C++

      .. code-block:: cpp

        class FunctionCallDumper : public yaramod::ObservingVisitor {
        public:
            void visit(FunctionCallExpression* expr) override {
                std::cout << "Function call: " << expr->getFunction()->getText() << '\n';
                // Visit arguments because they can contain nested function calls
                for (auto& param : expr->getArguments())
                    param->accept(this);
            }
        };

.. note::

    As you can see, visitors depend heavily on recursion and that can represent problems sometimes with a huge rulesets where depth of AST is rather large.
    Python has a limit on how many stack frames you can have at the sime time in order to prevent stack overflow. This limit can be however sometimes very limiting
    and set too low for certain huge conditions. You might need to run `sys.setrecursionlimit <https://docs.python.org/3/library/sys.html#sys.setrecursionlimit>`_
    to process those.

Expression types
****************

There are a lot of expression types that you can visit. Here is a list of them all:

**String expressions**

  * ``StringExpression`` - reference to string in ``strings`` section (``$a01``, ``$sa02``, ``$str``, ...)
  * ``StringWildcardExpression`` - reference to multiple strings using wildcard (``$a*``, ``$*``, ...)
  * ``StringAtExpression`` - refers to ``$str at <offset>``
  * ``StringInRangeExpression`` - refers to ``$str in (<offset1> .. <offset2>)``
  * ``StringCountExpression`` - reference to number of matched string of certain string identifier (``#a01``, ``#str``)
  * ``StringOffsetExpression`` - reference to first match offset (or Nth match offset) of string identifier (``@a01``, ``@a01[N]``)
  * ``StringLengthExpression`` - reference to length of first match (or Nth match) of string identifier (``!a01``, ``!a01[N]1``)

**Unary operations**

All of these provide method ``getOperand()`` (``operand`` in Python) to return operand of an expression.

  * ``NotExpression`` - refers to logical ``not`` operator (``!(@str > 10)``)
  * ``UnaryMinusExpression`` - refers to unary ``-`` operator (``-20``)
  * ``BitwiseNotExpression`` - refers to bitwise not (``~uint8(0x0)``)

**Binary operations**

All of these provide methods ``getLeftOperand()`` and ``getRightOperand()`` (``left_operand`` and ``right_operand`` in Python) to return both operands of an expression.

  * ``AndExpression`` - refers to logical ``and`` (``$str1 and $str2``)
  * ``OrExpression`` - refers to logical ``or`` (``$str1 or $str2``)
  * ``LtExpression`` - refers to ``<`` operator (``$str1 < $str2``)
  * ``GtExpression`` - refers to ``>`` operator (``$str1 > $str2``)
  * ``LeExpression`` - refers to ``<=`` operator (``@str1 <= $str2``)
  * ``GeExpression`` - refers to ``>=`` operator (``@str1 >= @str2``)
  * ``EqExpression`` - refers to ``==`` operator (``!str1 == !str2``)
  * ``NeqExpression`` - refers to ``!=`` operator (``!str1 != !str2``)
  * ``ContainsExpression`` - refers to ``contains`` operator (``pe.sections[0] contains "text"``)
  * ``MatchesExpression`` - refers to ``matches`` operator (``pe.sections[0] matches /(text|data)/``)
  * ``PlusExpression`` - refers to ``+`` operator (``@str1 + 0x100``)
  * ``MinusExpression`` - refers to ``-`` operator (``@str1 - 0x100``)
  * ``MultiplyExpression`` - refers to ``*`` operator (``@str1 * 0x100``)
  * ``DivideExpression`` - refers to ``/`` operator (``@str1 / 0x100``)
  * ``ModuloExpression`` - refers to ``%`` operator (``@str1 % 0x100``)
  * ``BitwiseXorExpression`` - refers to ``^`` operator (``uint8(0x10) ^ uint8(0x20)``)
  * ``BitwiseAndExpression`` - refers to ``&`` operator (``pe.characteristics & pe.DLL``)
  * ``BitwiseOrExpression`` - refers to ``|`` operator (``pe.characteristics | pe.DLL``)
  * ``ShiftLeftExpression`` - refers to ``<<`` operator (``uint8(0x10) << 2``)
  * ``ShiftRightExpression`` - refers to ``>>`` operator (``uint8(0x10) >> 2``)

**For expressions**

All of these provide method ``getVariable()`` (``variable`` in Python) to return variable used for iterating over the set of values (can also be ``any`` or ``all``),
``getIteratedSet()`` (``iterated_set`` in Python) to return an iterated set (can also be ``them``) and ``getBody()`` (``body`` in Python) to return the body of a for expression. For ``OfExpression``, ``getBody()`` always returns ``nullptr`` (``None`` in Python).

  * ``ForIntExpression`` - refers to ``for`` which operates on set of integers (``for all i in (1 .. 5) : ( ... )``)
  * ``ForStringExpression`` - refers to ``for`` which operates on set of string identifiers (``for all of ($str1, $str2) : ( ... )``)
  * ``OfExpression`` - refers to ``of`` (``all of ($str1, $str2)``)

**Identificator expressions**

All of these provide method ``getSymbol()`` (``symbol`` in Python) to return symbol of an associated identifier.

  * ``IdExpression`` - refers to identifier (``rule1``, ``pe``)
  * ``StructAccessExpression`` - refers to ``.`` operator for accessing structure memebers (``pe.number_of_sections``)
  * ``ArrayAccessExpression`` - refers to ``[]`` operator for accessing items in arrays (``pe.sections[0]``)
  * ``FunctionCallExpression`` - refers to function call (``pe.exports("ExitProcess")``)

**Literal expressions**

  * ``BoolLiteralExpression`` - refers to ``true`` or ``false``
  * ``StringLiteralExpression`` - refers to any sequence of characters enclosed in double-quotes (``"text"``)
  * ``IntLiteralExpression`` - refers to any integer value be it decimal, hexadecimal or with multipliers (``KB``, ``MB``) (``42``, ``-42``, ``0x100``, ``100MB``)
  * ``DoubleLiteralExpression`` - refers to any floating point value (``72.0``, ``-72.0``)

**Keyword expressions**

  * ``FilesizeExpression`` - refers to keyword ``filesize``
  * ``EntrypointExpression`` - refers to keyword ``entrypoint``
  * ``AllExpression`` - refers to keyword ``all``
  * ``AnyExpression`` - refers to keyword ``any``
  * ``ThemExpression`` - refers to keyword ``them``

**Other expressions**

  * ``SetExpression`` - refers to set of either integers or string identifiers (``(1,2,3,4,5)``, ``($str*,$1,$2)``)
  * ``RangeExpression`` - refers to range of integers (``(0x100 .. 0x200)``)
  * ``ParenthesesExpression`` - refers to expression enclosed in parentheses (``((5 + 6) * 30)``)
  * ``IntFunctionExpression`` - refers to special built-in functions ``(u)int(8|16|32)`` (``uint16(<offset>)``)
  * ``RegexpExpression`` - refers to regular expression (``/<regexp>/<mods>``)


Includes
========

YARA language supports inclusion of other files on the filesystem. Path provided in include directive is always relative to the YARA file being parsed. Since yaramod can also parse from memory, relative paths are only allowed when parsing from the actual file.

Whenever yaramod runs into include, it takes the content of included file and starts parsing it as if it was in place of an include. Therefore, included content is merged with all other content in the file. You can distinguish where the rule comes from
using a location attribute of the rule.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for rule in yara_file.rules:
            print(f'{rule.name}: {rule.location.file_path}:{rule.location.line_number}')

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& rule : yaraFile->getRules())
            std::cout << rule->getName() << ": "
                << rule->getLocation().filePath << ':'
                << rule->getLocation().lineNumber << std::endl;


Yaramod can also provide you with something what YARA doesn't handle well - including the same file multiple times. If you do this in YARA then you will get error that you have duplicate
rules in your ruleset. This is however not something you would like to run into when doing static analyses. You can allow duplicate includes by using

.. tabs::

    .. tab:: Python

      .. code-block:: python

        ymod = yaramod.Yaramod()
        ymod.parse_file('/path/to/file', yaramod.ParserMode.IncludeGuarded)

    .. tab:: C++

      .. code-block:: cpp

        auto ymod = yaramod::Yaramod();
        ymod.parse_file("/path/to/file", yaramod::ParserMode::IncludeGuarded);

Imports
=======

Checking what modules are imported. Keep in mind that imports are merged from all included files.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        for module in yara_file.imports:
            print(f'{module.name}')

    .. tab:: C++

      .. code-block:: cpp

        for (const auto& module : yaraFile->getImports())
            std::cout << module->getName() << std::endl;
