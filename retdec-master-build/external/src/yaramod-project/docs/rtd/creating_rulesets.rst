================
Creating ruleset
================

Creating of new YARA rulesets is available in yaramod through interface based on `builder design pattern <https://en.wikipedia.org/wiki/Builder_pattern>`_.

.. note::

  Following list of functions is made for C++. Python alternatives follow the same naming but instead of ``CamelCase`` they use ``snake_case``. Sometimes there is
  keyword needed to be used as a function name (like ``xor``). In that case, it is followed by underscore character (so it would be ``xor_``).


The main goal of the yaramod interface is to make the creation of new YARA rules declarative. We wanted to avoid just concatenating strings together and we also wanted to end up
with the same internal representation of YARA rules as if we parsed them. We also wanted to make it composable so that you can easily build for example part of your condition
in one function, the other part in the second function and join them together.


File
====

It all starts with ``YaraFileBuilder`` which is a main building block for creating new YARA ruleset. here is a short example on how to create new YARA file with import of ``pe`` module and
*some* rule (will be explained later).

.. tabs::

    .. tab:: Python

      .. code-block:: python

        new_file = yaramod.YaraFileBuilder()
        yara_file = new_file \
            .with_module('pe')
            .with_rule(rule)
            .get()

    .. tab:: C++

      .. code-block:: cpp

        yaramod::YaraFileBuilder newFile;
        auto yaraFile = newFile
            .withModule("pe")
            .withRule(std::move(rule))
            .get();

Rules
=====

In order to create a rule that we saw while creating new YARA file, we will need ``YaraRuleBuilder``.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        rule = yaramod.YaraRuleBuilder() \
            .with_tag('tag1') \
            .with_tag('tag2') \
            .with_string_meta('author', 'foo') \
            .with_plain_string('$str', 'Hello World').ascii().wide().xor(1,255) \
            .with_condition(condition) \
            .get()

    .. tab:: C++

      .. code-block:: cpp

        auto rule = yaramod::YaraRuleBuilder{}
            .withTag("tag1")
            .withTag("tag2")
            .withStringMeta("author", "foo")
            .withPlainString("$str", "Hello World").ascii().wide().xor_(1,255)
            .withCondition(std::move(condition))
            .get();

Hex strings
===========

Creation of plain strings is really simple as shown in the code above. When it comes to hex strings, things get complicated. Hex strings in YARA have several features which make them more like *weak regular expressions*. Those features are:

* Wildcarded nibbles
* Jumps
* Alternations

In order to fully unlock the whole potential of hex strings, they need to be created through ``YaraHexStringBuilder``.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        # Simple hex string - { 10 20 30 }
        yaramod.YaraHexStringBuilder([0x10, 0x20, 0x30]).get()
        # Hex string with wildcard - { 10 ?? 30 }
        yaramod.YaraHexStringBuilder() \
            .add(yaramod.YaraHexStringBuilder(0x10)) \
            .add(yaramod.wildcard()) \
            .add(yaramod.YaraHexStringBuilder(0x30)) \
            .get()
        # Hex string with all features - { 10 ?? 30 [4] ( 40 | 50 ) }
        yaramod.YaraHexStringBuilder() \
            .add(yaramod.YaraHexStringBuilder(0x10)) \
            .add(yaramod.wildcard()) \
            .add(yaramod.YaraHexStringBuilder(0x30)) \
            .add(yaramod.jump_fixed(4)) \
            .add(yaramod.alt([
                yaramod.YaraHexStringBuilder(0x40),
                yaramod.YaraHexStringBuilder(0x50)
            ])) \
            .get()

    .. tab:: C++

      .. code-block:: cpp

        // Simple hex string - { 10 20 30 }
        yaramod::YaraHexStringBuilder{std::vector<std::uint8_t>{0x10, 0x20, 0x30}}.get()
        // Hex string with wildcard - { 10 ?? 30 }
        yaramod::YaraHexStringBuilder{}
            .add(yaramod::YaraHexStringBuilder(0x10))
            .add(yaramod::wildcard())
            .add(yaramod::YaraHexStringBuilder(0x30))
            .get();
        // Hex string with all features - { 10 ?? 30 [4] ( 40 | 50 ) }
        yaramod::YaraHexStringBuilder{}
            .add(yaramod::YaraHexStringBuilder(0x10))
            .add(yaramod::wildcard())
            .add(yaramod::YaraHexStringBuilder(0x30))
            .add(yaramod::jumpFixed(4))
            .add(yaramod::alt(
                yaramod.YaraHexStringBuilder(0x40),
                yaramod.YaraHexStringBuilder(0x50)
            ))
            .get();

Regular expressions
===================

Regular expressions are the same story as hex strings, they just have more expressive power so they also require much more builder methods. Unfortunately right now we don't provide any kind of ``RegexpBuilder``
which would help you with it. We expect you to provide regular expression as a plain string. Building of regular expression is however something which we would like to add in the future.


Conditions
==========

Building of conditions is heavily based on ``YaraExpressionBuilder`` which provides you with many functions that make it easy to express the condition. It makes use of operator overloading in both C++ and Python
to make your builder code readable as much as possible.

.. note::

  Python does not allow to override logical operators such as ``and``, ``or`` or ``not`` therefore there are special function which you can used instead which are ``conjunction``, ``disjunction`` and ``not_``.

.. tabs::

    .. tab:: Python

      .. code-block:: python

        # all of them
        of(all(), them()).get()
        # $1 and ($2 or $3)
        (conjunction([
            string_ref('$1'),
            parent(disjunction([
                string_ref('$2'),
                string_ref('$3')
            ]))
        ])).get()
        # pe.sections[0].name matches /\.(text|data)/i
        (id('pe').access('sections')[int_val(0)].access('name').matches(regexp(r'(/\.(text|data)/)', 'i'))).get()
        # filesize <= 1MB
        (filesize() <= int_val(1, IntMultiplier.Megabytes)).get()

    .. tab:: C++

      .. code-block:: cpp

        using namespace yaramod;

        // all of them
        of(all(), them()).get();
        // $1 and ($2 or $3)
        (stringRef("$1") && paren(stringRef("$2") || stringRef("$3"))).get();
        // pe.sections[0].name matches /\.(text|data)/i
        (id("pe").access("sections")[intVal(0)].access("name").matches(regexp(R"(/\.(text|data)/)", "i"))).get();
        // filesize <= 1MB
        (filesize() <= intVal(1, IntMultiplier::Megabytes)).get();

List
====

Here is a list of everything available in builder and how it maps to YARA language. Functions listed as *basic* are basic building block for YARA expressions. You always want to start from these expressions and build upon them to form complex expressions.
Each of these functions returns you an object of type `YaraExpressionBuilder`. Those functions with parameters also mostly accept object of these types, so whenever you are not sure what kind of expression to put there, just look at the list of all
basic expressions and find the most suitable one.

.. tabs::

    .. tab:: Python

        **Basic expression functions**

        * ``filesize()`` - represents ``filesize`` keyword
        * ``entrypoint()`` - represents ``entrypoint`` keyword
        * ``all()`` - represents ``all`` keyword
        * ``any()`` - represents ``any`` keyword
        * ``them()`` - represents ``them`` keyword
        * ``int_val(val, [mult])`` - represents signed integer with multiplier (default: ``IntMultiplier.Empty``) (``int_val(10)``, ``int_val(10, IntMultiplier.Kilobytes)``, ``int_val(10, IntMultiplier.Megabytes)``)
        * ``uint_val(val, [mult])`` - represents unsigned integer with multiplier (default: ``IntMultiplier.Empty``) (``int_val(10)``, ``int_val(10, IntMultiplier.Kilobytes)``, ``int_val(10, IntMultiplier.Megabytes)``)
        * ``hex_int_val(val)`` - represents hexadecimal integer (``hex_int_val(0x10)``)
        * ``double_val(val)`` - represents double floating-point value (``double_val(3.14)``)
        * ``string_val(str)`` - represents string literal (``string_val("Hello World!")``)
        * ``bool_val(bool)`` - represents boolean literal (``bool_val(True)``)
        * ``id(id)`` - represents single identifier with name ``id`` (``id("pe")``)
        * ``string_ref(ref)`` - represents reference to string identifier ``ref`` (``string_ref("$1")``)
        * ``set(elements)`` - represents ``(item1, item2, ...)`` (``set({string_ref("$1"), string_ref("$2")})``)
        * ``range(low, high)`` - represents ``(low .. high)`` (``range(int_val(100), int_val(200))``)
        * ``match_count(ref)`` - represents match count of string identifier ``ref`` (``match_count("$1")``)
        * ``match_length(ref, [n])`` - represent ``n``th match (default: 0) length of string identifier ``ref`` (``match_length("$1", int_val(1))``)
        * ``match_offset(ref, [n])`` - represents ``n``th match (default: 0) offset of string identifier ``ref`` (``match_offset("$1", int_val(1))``)
        * ``match_at(ref, expr)`` - represents ``<ref> at <expr>`` (``match_at("$1", int_val(100))``)
        * ``match_inRange(ref, range)`` - represents ``<ref> in <range>`` (``match_in_range("$1", range(int_val(100), int_val(200)))``)
        * ``regexp(regexp, mods) - represents regular expression in form ``/<regexp>/<mods>`` (``regexp("^a.*b$", "i")``)
        * ``for_loop(spec, var, set, body)`` - represents ``for`` loop over set of integers (``for_loop(any(), "i", range(int_val(100), int_val(200)), match_at("$1", id("i")))``)
        * ``for_loop(spec, set, body)`` - represents ``for`` loop over set of string references (``for_loop(any(), set({string_ref("$*")}), match_at("$", int_val(100))``)
        * ``of(spec, set)`` - represents ``<spec> of <set>`` (``of(all(), them())``)
        * ``paren(expr, [newline])`` - represents parentheses around expressions and ``newline`` indicator for putting enclosed expression on its own line (``paren(int_val(10))``)
        * ``conjunction(terms, [newline])`` - represents conjunction of ``terms`` and optionally puts them on each separate line if ``newline`` is set (``conjunction({id("rule1"), id("rule2")})``)
        * ``disjunction(terms, [newline])`` - represents disjunction of ``terms`` and optionally puts them on each separate line if ``newline`` is set (``disjunction({id("rule1"), id("rule2")})``)

        **Complex expression methods**

        * ``__invert__`` - represents bitwise not (``~hex_int_val(0x100)``)
        * ``__neg__`` - represents unary operator ``-`` (``-id("i")``)
        * ``__lt__`` - represents operator ``<`` (``match_offset("$1") < int_val(100)``)
        * ``__gt__`` - represents operator ``>`` (``match_offset("$1") > int_val(100)``)
        * ``__le__`` - represents operator ``<=`` (``match_offset("$1") <= int_val(100)``)
        * ``__ge__`` - represents operator ``>=`` (``match_offset("$1") >= int_val(100)``)
        * ``__add__`` - represents operator ``+`` (``match_offset("$1") + int_val(100)``)
        * ``__sub__`` - represents operator ``-`` (``match_offset("$1") - int_val(100)``)
        * ``__mul__`` - represents operator ``*`` (``match_offset("$1") * int_val(100)``)
        * ``__truediv__`` - represents operator ``/`` (``match_offset("$1") / int_val(100)``)
        * ``__mod__`` - represents operator ``%`` (``match_offset("$1") % int_val(100)``)
        * ``__xor__`` - represents bitwise xor (``match_offset("$1") ^ int_val(100)``)
        * ``__and__`` - represents bitwise and (``match_offset("$1") & int_val(100)``)
        * ``__or__`` - represents bitwise or (``match_offset("$1") | int_val(100)``)
        * ``__lshift__`` - represents bitwise shift left (``match_offset("$1") << int_val(10)``)
        * ``__rshift__`` - represents bitwise shift right (``match_offset("$1") >> int_val(10)``)
        * ``__call__`` - represent call to function (``id("func")(int_val(100), int_val(200))``)
        * ``call(args)`` - represents call to function (``id("func").call({int_val(100), int_val(200)})``)
        * ``contains(rhs)`` - represents operator ``contains`` (``id("signature").contains(string_val("hello"))``)
        * ``matches(rhs)`` - represents operator ``matches`` (``id("signature").matches(regexp("^a.*b$", "i"))``)
        * ``access(rhs)`` - represents operator ``.`` as access to structure (``id("pe").access("numer_of_sections")``)
        * ``__getitem__`` - represents operator ``[]`` as access to array (``id("pe").access("sections")[int_val(0)]``)
        * ``read_int8(be)`` - represents call to special function ``int8(be)`` (``int_val(100).read_int8()``)
        * ``read_int16(be)`` - represents call to special function ``int16(be)`` (``int_val(100).read_int16()``)
        * ``read_int32(be)`` - represents call to special function ``int32(be)`` (``int_val(100).read_int32()``)
        * ``read_uint8(be)`` - represents call to special function ``uint8(be)`` (``int_val(100).read_uint8()``)
        * ``read_uint16(be)`` - represents call to special function ``uint16(be)`` (``int_val(100).read_uint16()``)
        * ``read_uint32(be)`` - represents call to special function ``uint32(be)`` (``int_val(100).read_uint32()``)

        **Hex strings**

        * ``YaraHexStringBuilder(byte)`` - creates two nibbles out of byte value.
        * ``wildcard()`` - creates ``??``
        * ``wildcard_low(nibble)`` - ``<nibble>?``
        * ``wildcard_high(nibble)`` - ``?<nibble>``
        * ``jump_varying()`` - ``[-]``
        * ``jump_fixed(offset)`` - ``[<offset>]``
        * ``jump_varyingRange(low)`` - ``[<low>-]``
        * ``jump_range(low, high)`` - ``[<low>-<high>]``
        * ``alt([units])`` - ``(unit1|unit2|...)``

        **Rule**

        * ``with_name(name)`` - specify rule name
        * ``with_modifier(mod)`` - specify whether rule is private or public (``Rule::Modifier::Private`` or ``Rule::Modifier::Public``)
        * ``with_tag(tag)`` - specify rule tag
        * ``with_string_meta(key, value)`` - specify string meta
        * ``with_int_meta(key, value)`` - specify integer meta
        * ``with_uint_meta(key, value)`` - specify unsigned integer meta
        * ``with_hex_int_meta(key, value)`` - specify hexadecimal integer meta
        * ``with_bool_meta(key, value)`` - specify boolean meta
        * ``with_plain_string(id, value)`` - specify plain string with identifier ``id`` and content ``value``
        * ``with_hex_string(id, str)`` - specify hex string (``str`` is of type ``std::shared_ptr<HexString>``)
        * ``with_regexp(id, value, mod)`` - specify regular expression with identifier ``id`` and content ``value with modifiers ``mod`` (These modifiers are tied to the regular expression and come after last ``/``.)
        * ``with_condition(cond)`` - specify condition
        * ``ascii()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``ascii`` modifier to the list of its modifiers
        * ``wide()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``wide`` modifier to the list of its modifiers
        * ``nocase()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``nocase`` modifier to the list of its modifiers
        * ``fullword()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``fullword`` modifier to the list of its modifiers
        * ``private()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``private`` modifier to the list of its modifiers
        * ``xor()`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``xor`` modifier to the list of its modifiers
        * ``xor(key)`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``xor(key)`` modifier to the list of its modifiers
        * ``xor(low, high)`` - ties to the latest defined ``with_plain_string()``, ``with_hex_string()`` or ``with_regexp()`` and adds ``xor(low-high)`` modifier to the list of its modifiers

        **YARA file**

        * ``with_module(name)`` - specifies ``import`` of module named ``name``
        * ``with_rule(rule)`` - adds the rule into file

    .. tab:: C++

        **Basic expression functions**

        * ``filesize()`` - represents ``filesize`` keyword
        * ``entrypoint()`` - represents ``entrypoint`` keyword
        * ``all()`` - represents ``all`` keyword
        * ``any()`` - represents ``any`` keyword
        * ``them()`` - represents ``them`` keyword
        * ``intVal(val, [mult])`` - represents signed integer with multiplier (default: ``IntMultiplier::None``) (``intVal(10)``, ``intVal(10, IntMultiplier::Kilobytes)``, ``intVal(10, IntMultiplier::Megabytes)``)
        * ``uintVal(val, [mult])`` - represents unsigned integer with multiplier (default: ``IntMultiplier::None``) (``intVal(10)``, ``intVal(10, IntMultiplier::Kilobytes)``, ``intVal(10, IntMultiplier::Megabytes)``)
        * ``hexIntVal(val)`` - represents hexadecimal integer (``hexIntVal(0x10)``)
        * ``doubleVal(val)`` - represents double floating-point value (``doubleVal(3.14)``)
        * ``stringVal(str)`` - represents string literal (``stringVal("Hello World!")``)
        * ``boolVal(bool)`` - represents boolean literal (``boolVal(true)``)
        * ``id(id)`` - represents single identifier with name ``id`` (``id("pe")``)
        * ``stringRef(ref)`` - represents reference to string identifier ``ref`` (``stringRef("$1")``)
        * ``set(elements)`` - represents ``(item1, item2, ...)`` (``set({stringRef("$1"), stringRef("$2")})``)
        * ``range(low, high)`` - represents ``(low .. high)`` (``range(intVal(100), intVal(200))``)
        * ``matchCount(ref)`` - represents match count of string identifier ``ref`` (``matchCount("$1")``)
        * ``matchLength(ref, [n])`` - represent ``n``th match (default: 0) length of string identifier ``ref`` (``matchLength("$1", intVal(1))``)
        * ``matchOffset(ref, [n])`` - represents ``n``th match (default: 0) offset of string identifier ``ref`` (``matchOffset("$1", intVal(1))``)
        * ``matchAt(ref, expr)`` - represents ``<ref> at <expr>`` (``matchAt("$1", intVal(100))``)
        * ``matchInRange(ref, range)`` - represents ``<ref> in <range>`` (``matchInRange("$1", range(intVal(100), intVal(200)))``)
        * ``regexp(regexp, mods) - represents regular expression in form ``/<regexp>/<mods>`` (``regexp("^a.*b$", "i")``)
        * ``forLoop(spec, var, set, body)`` - represents ``for`` loop over set of integers (``forLoop(any(), "i", range(intVal(100), intVal(200)), matchAt("$1", id("i")))``)
        * ``forLoop(spec, set, body)`` - represents ``for`` loop over set of string references (``forLoop(any(), set({stringRef("$*")}), matchAt("$", intVal(100))``)
        * ``of(spec, set)`` - represents ``<spec> of <set>`` (``of(all(), them())``)
        * ``paren(expr, [newline])`` - represents parentheses around expressions and ``newline`` indicator for putting enclosed expression on its own line (``paren(intVal(10))``)
        * ``conjunction(terms, [newline])`` - represents conjunction of ``terms`` and optionally puts them on each separate line if ``newline`` is set (``conjunction({id("rule1"), id("rule2")})``)
        * ``disjunction(terms, [newline])`` - represents disjunction of ``terms`` and optionally puts them on each separate line if ``newline`` is set (``disjunction({id("rule1"), id("rule2")})``)

        **Complex expression methods**

        * ``operator!`` - represents logical ``not`` (``!boolVal(true)``)
        * ``operator~`` - represents bitwise not (``~hexIntVal(0x100)``)
        * ``operator-`` - represents unary operator ``-`` (``-id("i")``)
        * ``operator&&`` - represents logical ``and`` (``id("rule1") && id("rule2")``)
        * ``operator||`` - represents logical ``or`` (``id("rule1") || id("rule2")``)
        * ``operator<`` - represents operator ``<`` (``matchOffset("$1") < intVal(100)``)
        * ``operator>`` - represents operator ``>`` (``matchOffset("$1") > intVal(100)``)
        * ``operator<=`` - represents operator ``<=`` (``matchOffset("$1") <= intVal(100)``)
        * ``operator>=`` - represents operator ``>=`` (``matchOffset("$1") >= intVal(100)``)
        * ``operator+`` - represents operator ``+`` (``matchOffset("$1") + intVal(100)``)
        * ``operator-`` - represents operator ``-`` (``matchOffset("$1") - intVal(100)``)
        * ``operator*`` - represents operator ``*`` (``matchOffset("$1") * intVal(100)``)
        * ``operator/`` - represents operator ``/`` (``matchOffset("$1") / intVal(100)``)
        * ``operator%`` - represents operator ``%`` (``matchOffset("$1") % intVal(100)``)
        * ``operator^`` - represents bitwise xor (``matchOffset("$1") ^ intVal(100)``)
        * ``operator&`` - represents bitwise and (``matchOffset("$1") & intVal(100)``)
        * ``operator|`` - represents bitwise or (``matchOffset("$1") | intVal(100)``)
        * ``operator<<`` - represents bitwise shift left (``matchOffset("$1") << intVal(10)``)
        * ``operator>>`` - represents bitwise shift right (``matchOffset("$1") >> intVal(10)``)
        * ``operator()`` - represent call to function (``id("func")(intVal(100), intVal(200))``)
        * ``call(args)`` - represents call to function (``id("func").call({intVal(100), intVal(200)})``)
        * ``contains(rhs)`` - represents operator ``contains`` (``id("signature").contains(stringVal("hello"))``)
        * ``matches(rhs)`` - represents operator ``matches`` (``id("signature").matches(regexp("^a.*b$", "i"))``)
        * ``access(rhs)`` - represents operator ``.`` as access to structure (``id("pe").access("numer_of_sections")``)
        * ``operator[]`` - represents operator ``[]`` as access to array (``id("pe").access("sections")[intVal(0)]``)
        * ``readInt8(be)`` - represents call to special function ``int8(be)`` (``intVal(100).readInt8()``)
        * ``readInt16(be)`` - represents call to special function ``int16(be)`` (``intVal(100).readInt16()``)
        * ``readInt32(be)`` - represents call to special function ``int32(be)`` (``intVal(100).readInt32()``)
        * ``readUInt8(be)`` - represents call to special function ``uint8(be)`` (``intVal(100).readUInt8()``)
        * ``readUInt16(be)`` - represents call to special function ``uint16(be)`` (``intVal(100).readUInt16()``)
        * ``readUInt32(be)`` - represents call to special function ``uint32(be)`` (``intVal(100).readUInt32()``)

        **Hex strings**

        * ``YaraHexStringBuilder(byte)`` - creates two nibbles out of byte value.
        * ``wildcard()`` - creates ``??``
        * ``wildcardLow(nibble)`` - ``<nibble>?``
        * ``wildcardHigh(nibble)`` - ``?<nibble>``
        * ``jumpVarying()`` - ``[-]``
        * ``jumpFixed(offset)`` - ``[<offset>]``
        * ``jumpVaryingRange(low)`` - ``[<low>-]``
        * ``jumpRange(low, high)`` - ``[<low>-<high>]``
        * ``alt(units)`` - ``(unit1|unit2|...)``

        **Rule**

        * ``withName(name)`` - specify rule name
        * ``withModifier(mod)`` - specify whether rule is private or public (``Rule::Modifier::Private`` or ``Rule::Modifier::Public``)
        * ``withTag(tag)`` - specify rule tag
        * ``withStringMeta(key, value)`` - specify string meta
        * ``withIntMeta(key, value)`` - specify integer meta
        * ``withUIntMeta(key, value)`` - specify unsigned integer meta
        * ``withHexIntMeta(key, value)`` - specify hexadecimal integer meta
        * ``withBoolMeta(key, value)`` - specify boolean meta
        * ``withPlainString(id, value, mod)`` - specify plain string with identifier ``id`` and content ``value`` with modifiers ``mod`` (``String::Modifiers::Ascii``, ``String::Modifiers::Wide``, ``String::Modifiers::Nocase``, ``String::Modifiers::Fullword``)
        * ``withHexString(id, str)`` - specify hex string (``str`` is of type ``std::shared_ptr<HexString>``)
        * ``withRegexp(id, value, mod)`` - specify regular expression with identifier ``id`` and content ``value with modifiers ``mod`` (Modifiers here are different than modifiers in plain string. These modifiers are tied to the regular expression and come after last ``/``.)
        * ``withCondition(cond)`` - specify condition
        * ``ascii()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``ascii`` modifier to the list of its modifiers
        * ``wide()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``wide`` modifier to the list of its modifiers
        * ``nocase()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``nocase`` modifier to the list of its modifiers
        * ``fullword()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``fullword`` modifier to the list of its modifiers
        * ``private()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``private`` modifier to the list of its modifiers
        * ``xor()`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``xor`` modifier to the list of its modifiers
        * ``xor(key)`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``xor(key)`` modifier to the list of its modifiers
        * ``xor(low, high)`` - ties to the latest defined ``withPlainString()``, ``withHexString()`` or ``withRegexp()`` and adds ``xor(low-high)`` modifier to the list of its modifiers

        **YARA file**

        * ``withModule(name)`` - specifies ``import`` of module named ``name``
        * ``withRule(rule)`` - adds the rule into file
