Formatting rulesets
===================

Our recent addition to the things yaramod should do is automatic formatting of YARA rules into form which is mostly used for our YARA rules and in which we expect YARA rules when they come from other sources.
Having formatted YARA rules has same importance as having properly formatted code. It is readable even if you are not the original author of the ruleset and also formatting gives additional semantics without explicitly stating it if you are used to it.
Formatting that is designed well can also help you catch problems in your ruleset even before you actually submit it and start using it. Therefore we came up with this formatting best to suite our needs which incorporates:

* Use of TABs
* ``{`` and ``}`` on their own lines
* ``meta``, ``strings`` and ``condition`` indented once
* Their contents idented twice
* New lines when using ``and``, ``or``, ``(`` and ``)`` in condition
* Comments that are on the consecutive lines are properly aligned
* Single space in every other place wher you can put multiple whitespaces

Here is an example of the rule before and after formatting:

.. code::

  rule abc {
  strings:
    $s01  = "Hello"
    $s02  = "World"
  condition:
    $s01 and $s02
  }

.. code::

  // This is my rule
  rule abc
  {
          strings:
                  $s01 = "Hello" // String 1
                  $s02 = "World" // String 2
          condition:
                  $s01 and
                  $s02
  }


In order to use it access ``getTextFormatted()`` (``text_formatted`` in Python) on YARA file. It is important to state that our formatter **does not remove comments**.

This feature is brand new and is still evolving. We would really appreciate your feedback here so we can improve it further. There is a `staging area <https://github.com/avast/yaramod/wiki/Autoformatting>`_ on our wiki where we collect ideas
on where to go with autoformatting. Just file an issue if you have any remarks.
