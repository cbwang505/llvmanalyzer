===============
Getting Started
===============

Before we start, we would like to introduce some basic concepts we use in yaramod so you can get familiar with them and know how to use them.

Yaramod as a library serves two main purposes:

* Parsing of YARA rulesets into intermediate form which is friendly for inspection, analysis or even transformations
* Creation of new YARA rulesets through code using declarative description of how your YARA rule should look like

These two together form a strong tooling for you to build more advanced systems, perform different kind of analyses over your rulesets, etc.

We have our own custom parser for YARA, so we don't use the one in `VirusTotal/yara <https://github.com/VirusTotal/yara>`_, but we try to stay consistent as much as
we can. Generally, you shouldn't run into situation where YARA does parse your rule and yaramod does not (or vice versa) but this should be considered as bug and should be reported
to us using `issue tracker <https://github.com/avast/yaramod/issues>`_ on GitHub.

It is also important to state that yaramod in no way does any kind of matching because it does not understand your ruleset in a sense what you want to match.
Yaramod just knows what is inside of your rules and gives you an option to process it. Despite that, yaramod performs semantic checks where possible so you
for example need to provide correct types of parameters to module function calls or you have to import a module in order to use it etc.

Modules
=======

YARA is easily extensible with modules which provide you way to call C functions from your YARA ruleset. We realize that modules are important
and yaramod therefore supports them. You might however run into functions which you do not recognize and can' find anywhere in upstream YARA.
We at Avast use YARA daily and sometimes there are things we would like to match in our YARA rules something what is not accessible to the outside
world so we improve existing or develop our own modules. What we can we share with the community and in the future, we would really like to see
all the functions we have also in upstream YARA but that doesn't entirely depend on us. It's not like we are keeping it away becuase we want to keep
it secret.

Our extensions of modules are integrated into yaramod but we realize that you might want to use this library even without our symbols and therefore
there is a way you can request yaramod to not provide you anything additional.

We have also incorporated this amazing `androguard-yara <https://github.com/androguard/androguard>`_ module which is not merged into upstream of YARA
but we think it is very cool.

VirusTotal symbols
==================

YARA allows you to define so called *external variables* which are values outside your YARA environment but they are constant across the whole YARA scan. You can
reference them in your rule conditions to give an additional data to your rule so it can provide you with the matches you want.

These *external variables* are widely used on VirusTotal as you can see `here <https://www.virustotal.com/intelligence/help/malware-hunting/>`_. We realize this
and we also realize that you might want to process rulesets that you are using in VirusTotal so we also provide these additional symbols but you can opt-out of this
feature if you want.

Yaramod instance
================

The entrypoint of all yaramod is class ``Yaramod`` which you should instantiate and keep it alive while you are doing anything with yaramod. Accessing internal representation
of YARA rules which were returned by ``Yaramod`` is completely unsafe and can lead to crashes of your application if you do it after ``Yaramod`` has been destructed. Creation of ``Yaramod`` object
is performance heavy so you should keep the amount of instances low (ideally just one). ``Yaramod`` itself is not thread-safe, so in parallel environment we would suggest
you to create one instance per thread or process.

``Yaramod`` accepts one optional parameter when creating it and that is *import features*. This option specify in what kind of rulesets you are interested in and you can choose from:

* *All current* - This is the default option which provides you with both Avast-specific and VirusTotal-specific symbols.
* *Everything* - This also includes deprecated functions which should no longer be used.
* *Basic* - This represents that you are not interested in any additional symbols in your rules.
* *Avast* - You are interested in basic and Avast-specific symbols.
* *VirusTotal* - You are interested in basic and VirusTotal-specific symbols.
