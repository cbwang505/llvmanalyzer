/**
 * @file src/examples/dump_rules_ast/dumper.h
 * @brief Implementation of main for AST dumper.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <string>
#include <vector>

#include <yaramod/yaramod.h>

#include "dumper.h"

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.size() != 1)
	{
		std::cout << "Usage: dump-rules-ast YARA_FILE" << std::endl;
		return 1;
	}

	Dumper dumper;

	yaramod::Yaramod yaramod;

	auto yaraFile = yaramod.parseFile(args[0]);
	for (const auto& rule : yaraFile->getRules())
	{
		std::cout << "==== RULE: " << rule->getName() << std::endl;
		dumper.observe(rule->getCondition());
	}
}
