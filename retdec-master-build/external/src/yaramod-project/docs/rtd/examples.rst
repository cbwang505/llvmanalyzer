========
Examples
========

There are 2 examples available in the repository as of now. They are both implemented in C++ and Python so that you can check out the language which you prefer.

Dump rule AST
=============

Dump rule AST example shows you how you can implement traversal of the whole AST of the YARA rule condition. It prints out each node it visits and dumps some information to the output.

Boolean simplifier
==================

Boolean simplifier shows you how you can implement visitor which also changes the condition based on some kind of analysis. In this example, it tries to evaluate the logical operators
``and``, ``or`` and ``not`` and simplify the condition. It is built on the fact that ``<anything> and false = false`` and ``<anything> or true = true``. Then it just uses typical
truth tables for ``and``, ``or`` and ``not``.
