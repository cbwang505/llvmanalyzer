/**
 * @file src/examples/simplify_bools/main.cpp
 * @brief Implementation of main for boolean simplifier.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <string>
#include <vector>

#include <yaramod/yaramod.h>

#include "bool_simplifier.h"

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv + 1, argv + argc);
	if (args.size() != 1)
	{
		std::cout << "Usage: dump-rules-ast YARA_FILE" << std::endl;
		return 1;
	}

	BoolSimplifier simplifier;

	yaramod::Yaramod yaramod;

	auto yaraFile = yaramod.parseFile(args[0]);
	for (auto& rule : yaraFile->getRules())
	{
		std::cout << "==== RULE: " << rule->getName() << std::endl;
		std::cout << "==== BEFORE" << std::endl;
		std::cout << rule->getText() << std::endl;
		auto result = simplifier.modify(rule->getCondition(), std::make_shared<yaramod::BoolLiteralExpression>(false));
		rule->setCondition(result);
		std::cout << "==== AFTER" << std::endl;
		std::cout << rule->getText() << std::endl;
	}
}
