import unittest
import yaramod


class VisitorTests(unittest.TestCase):
    def test_simple_modifying_visitor(self):
        class StringExpressionUpper(yaramod.ModifyingVisitor):
            def add(self, yara_file: yaramod.YaraFile):
                for rule in yara_file.rules:
                    self.modify(rule.condition)

            def visit_StringExpression(self, expr: yaramod.Expression):
                expr.id = expr.id.upper()

        yara_file = yaramod.Yaramod().parse_string(r'''
import "cuckoo"
rule rule_with_regexp_in_fnc_call {
	strings:
		$str1 = "s"
		$str2 = "s"
	condition:
		$str1 and $str2
}''')

        visitor = StringExpressionUpper()
        visitor.add(yara_file)

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        cond = rule.condition
        self.assertTrue(isinstance(cond, yaramod.AndExpression))
        self.assertTrue(isinstance(cond.right_operand, yaramod.StringExpression))
        self.assertEqual(cond.left_operand.id, "$STR1")
        self.assertTrue(isinstance(cond.left_operand, yaramod.StringExpression))
        self.assertEqual(cond.right_operand.id, "$STR2")

        self.assertEqual(r'''import "cuckoo"

rule rule_with_regexp_in_fnc_call {
	strings:
		$STR1 = "s"
		$STR2 = "s"
	condition:
		$STR1 and $STR2
}''', yara_file.text)
        expected = r'''
import "cuckoo"

rule rule_with_regexp_in_fnc_call
{
	strings:
		$STR1 = "s"
		$STR2 = "s"
	condition:
		$STR1 and
		$STR2
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_modifying_visitor_inpact_on_regexp_expression(self):
        class RegexpCaseInsesitiveAdder(yaramod.ModifyingVisitor):
            def add(self, yara_file: yaramod.YaraFile):
                for rule in yara_file.rules:
                    self.modify(rule.condition)

            def visit_RegexpExpression(self, expr: yaramod.Expression):
                output = yaramod.regexp('abc', 'i').get()
                expr.exchange_tokens(output)
                return output

        yara_file = yaramod.Yaramod().parse_string(r'''
import "cuckoo"
rule rule_with_regexp_in_fnc_call {
	condition:
		cuckoo.network.http_request(/http:\/\/someone\.doingevil\.com/)
}''')

        regexp_icase_adder = RegexpCaseInsesitiveAdder()
        regexp_icase_adder.add(yara_file)

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        cond = rule.condition
        self.assertTrue(isinstance(cond, yaramod.FunctionCallExpression))
        self.assertEqual(len(cond.arguments), 1)
        self.assertTrue(isinstance(cond.arguments[0], yaramod.RegexpExpression))
        self.assertTrue(isinstance(cond.arguments[0].regexp_string, yaramod.Regexp))
        self.assertEqual(cond.arguments[0].regexp_string.text, r'/abc/i')
        self.assertEqual(cond.arguments[0].regexp_string.pure_text, rb'abc')

        self.assertEqual(r'''import "cuckoo"

rule rule_with_regexp_in_fnc_call {
	condition:
		cuckoo.network.http_request(/abc/i)
}''', yara_file.text)
        expected = r'''
import "cuckoo"

rule rule_with_regexp_in_fnc_call
{
	condition:
		cuckoo.network.http_request(/abc/i)
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_modifying_visitor_eq_expression(self):
        class EqModifyer(yaramod.ModifyingVisitor):
            def add(self, yara_file: yaramod.YaraFile):
                for rule in yara_file.rules:
                    rule.condition = self.modify(rule.condition)

            def visit_EqExpression(self, expr: yaramod.Expression):
                context = yaramod.TokenStreamContext(expr)
                expr.left_operand.accept(self)
                expr.right_operand.accept(self)
                output = (yaramod.YaraExpressionBuilder(expr.right_operand) != yaramod.YaraExpressionBuilder(expr.left_operand)).get()

                self.cleanUpTokenStreams(context, output)
                return output

        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_in_fnc_call {
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str1 == !str2222
}''')

        regexp_icase_adder = EqModifyer()
        regexp_icase_adder.add(yara_file)

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        cond = rule.condition
        self.assertTrue(isinstance(cond, yaramod.NeqExpression))
        self.assertTrue(isinstance(cond.left_operand, yaramod.StringLengthExpression))
        self.assertTrue(isinstance(cond.right_operand, yaramod.StringLengthExpression))

        self.assertEqual(r'''rule rule_with_regexp_in_fnc_call {
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str2222 != !str1
}''', yara_file.text)
        expected = r'''
rule rule_with_regexp_in_fnc_call
{
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str2222 != !str1
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_modifying_visitor_eq_expression(self):
        class EqModifyer(yaramod.ModifyingVisitor):
            def add(self, yara_file: yaramod.YaraFile):
                for rule in yara_file.rules:
                    rule.condition = self.modify(rule.condition)

            def visit_EqExpression(self, expr: yaramod.Expression):
                context = yaramod.TokenStreamContext(expr)
                expr.left_operand.accept(self)
                expr.right_operand.accept(self)
                output = (yaramod.YaraExpressionBuilder(expr.right_operand) != yaramod.YaraExpressionBuilder(expr.left_operand)).get()

                self.cleanUpTokenStreams(context, output)
                return output

        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_in_fnc_call {
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str1 == !str2222
}''')

        regexp_icase_adder = EqModifyer()
        regexp_icase_adder.add(yara_file)

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        cond = rule.condition
        self.assertTrue(isinstance(cond, yaramod.NeqExpression))
        self.assertTrue(isinstance(cond.left_operand, yaramod.StringLengthExpression))
        self.assertTrue(isinstance(cond.right_operand, yaramod.StringLengthExpression))

        self.assertEqual(r'''rule rule_with_regexp_in_fnc_call {
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str2222 != !str1
}''', yara_file.text)
        expected = r'''
rule rule_with_regexp_in_fnc_call
{
	strings:
		$str1 = "a"
		$str2222 = "b"
	condition:
		!str2222 != !str1
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_modifying_visitor_delete_rules(self):
        class RulesDeleter(yaramod.ModifyingVisitor):
            def __init__(self):
                super(RulesDeleter, self).__init__()
                self.rule_map = {}
                self.rules_for_remove = set()

            def remove_disabled_rules(self, yara_file: yaramod.YaraFile) -> None:
                for rule in yara_file.rules:
                    if rule.name.startswith('delete'):
                        self.rules_for_remove.add(rule.name)

                yara_file.remove_rules(lambda r: r.name in self.rules_for_remove)

                for rule in yara_file.rules:
                    rule.condition = self.modify(rule.condition, when_deleted=yaramod.bool_val(False).get())

            def visit_IdExpression(self, expr):
                if expr.symbol.name in self.rules_for_remove:
                    return yaramod.VisitAction.Delete

        yara_file = yaramod.Yaramod().parse_string(r'''
rule delete_rule_1 {
	strings:
		$str0 = "a"
	condition:
		$str0
}

rule rule_2 {
	strings:
		$str1 = "b"
	condition:
		$str1 or not delete_rule_1
}

rule delete_rule_3 {
	condition:
		delete_rule_1
}

rule rule_4 {
	condition:
		not delete_rule_3 and
		not rule_2
}

rule rule_5 {
	strings:
		$str1 = "c"
	condition:
		not delete_rule_1 and
		not rule_2 and
		not delete_rule_3 and
		$str1
}
''')

        visitor = RulesDeleter()
        visitor.remove_disabled_rules(yara_file)

        self.assertEqual(len(yara_file.rules), 3)

        self.assertEqual(r'''rule rule_2 {
	strings:
		$str1 = "b"
	condition:
		$str1
}

rule rule_4 {
	condition:
		not rule_2
}

rule rule_5 {
	strings:
		$str1 = "c"
	condition:
		not rule_2 and $str1
}''', yara_file.text)
        expected = r'''
rule rule_2
{
	strings:
		$str1 = "b"
	condition:
		$str1
}

rule rule_4
{
	condition:
		not rule_2
}

rule rule_5
{
	strings:
		$str1 = "c"
	condition:
		not rule_2 and
		$str1
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_pe_iconhash_deleter(self):
        class PeIconhashDeleter(yaramod.ModifyingVisitor):
            """Temporary pe.iconhash() remover which removes pe.iconhash()
            until we get it into the upstream.
            """

            def delete_pe_iconhash(self, yara_file):
                pe_symbol = yara_file.find_symbol('pe')
                if not pe_symbol:
                    return

                for rule in yara_file.rules:
                    rule.condition = self.modify(rule.condition, when_deleted=yaramod.bool_val(False).get())

            def visit_AndExpression(self, expr):
                return self._visit_logical_ops(expr)

            def visit_OrExpression(self, expr):
                return self._visit_logical_ops(expr)

            def visit_LeExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_LtExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_GeExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_GtExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_EqExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_NeqExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_PlusExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_MinusExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_MultiplyExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_DivideExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ModuloExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseXorExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseAndExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseOrExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ShiftLeftExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ShiftRightExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ContainsExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_MatchesExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_StringOffsetExpression(self, expr):
                return self._visit_string_manipulation_ops(expr)

            def visit_StringLengthExpression(self, expr):
                return self._visit_string_manipulation_ops(expr)

            def visit_FunctionCallExpression(self, expr):
                if expr.function.text == 'pe.iconhash':
                    return yaramod.VisitAction.Delete

            def _visit_logical_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                left_result = expr.left_operand.accept(self)
                right_result = expr.right_operand.accept(self)
                if left_result == yaramod.VisitAction.Delete or right_result == yaramod.VisitAction.Delete:
                    if left_result == yaramod.VisitAction.Delete:
                        new_operand = yaramod.bool_val(False).get()
                        expr.left_operand.exchange_tokens(new_operand)
                        expr.left_operand = new_operand
                    if right_result == yaramod.VisitAction.Delete:
                        new_operand = yaramod.bool_val(False).get()
                        expr.right_operand.exchange_tokens(new_operand)
                        expr.right_operand = new_operand
                else:
                    return self.default_handler(context, expr, left_result, right_result)

            def _visit_binary_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                left_result = expr.left_operand.accept(self)
                right_result = expr.right_operand.accept(self)
                if left_result == yaramod.VisitAction.Delete or right_result == yaramod.VisitAction.Delete:
                    return yaramod.VisitAction.Delete
                else:
                    self.default_handler(context, expr, left_result, right_result)

            def _visit_string_manipulation_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                index_result = None
                if expr.index_expr:
                    index_result = expr.index_expr.accept(self)
                    if index_result == yaramod.VisitAction.Delete:
                        return yaramod.VisitAction.Delete
                return self.default_handler(context, expr, index_result)

        yara_file = yaramod.Yaramod().parse_string(r'''
import "pe"

rule rule_1 {
	strings:
		$str1 = "a"
		$str2 = "b"
	condition:
		$str1 and
		(
			pe.iconhash() == "9d0bd50f710" or
			pe.iconhash() != "9d0bd50f711" or
			"9d0bd50f712" == pe.iconhash() or
			pe.iconhash() or
			$str2 or
			pe.iconhash() == "9d0bd50f714" or
			pe.iconhash() == "9d0bd50f715"
		)
}
''')

        visitor = PeIconhashDeleter()
        visitor.delete_pe_iconhash(yara_file)

        self.assertEqual(len(yara_file.rules), 1)

        self.assertEqual(r'''import "pe"

rule rule_1 {
	strings:
		$str1 = "a"
		$str2 = "b"
	condition:
		$str1 and (false or false or false or false or $str2 or false or false)
}''', yara_file.text)
        expected = r'''
import "pe"

rule rule_1
{
	strings:
		$str1 = "a"
		$str2 = "b"
	condition:
		$str1 and
		(
			false or
			false or
			false or
			false or
			$str2 or
			false or
			false
		)
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_cuckoo_function_replacer(self):
        class CuckooFunctionReplacer(yaramod.ModifyingVisitor):
            def __init__(self):
                super(CuckooFunctionReplacer, self).__init__()
                self.filesystem_symbol = None
                self.registry_symbol = None
                self.FILESYSTEM_REPLACE = set([
                    'cuckoo.filesystem.file_write',
                ])

                self.REGISTRY_REPLACE = set([
                    'cuckoo.registry.key_write',
                ])

                self.WHITELIST = set([
                    'cuckoo.network.http_post',
                ])

            def replace_functions(self, yara_file):
                cuckoo_symbol = yara_file.find_symbol('cuckoo')
                if not cuckoo_symbol:
                    return

                if not self.filesystem_symbol and not self.registry_symbol:
                    self.filesystem_symbol = yara_file.find_symbol('cuckoo').get_attribute('filesystem').get_attribute('file_access')
                    self.registry_symbol = yara_file.find_symbol('cuckoo').get_attribute('registry').get_attribute('key_access')

                for rule in yara_file.rules:
                    rule.condition = self.modify(rule.condition, when_deleted=yaramod.bool_val(False).get())

            def visit_AndExpression(self, expr):
                return self._visit_logical_ops(expr)

            def visit_OrExpression(self, expr):
                return self._visit_logical_ops(expr)

            def visit_LeExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_LtExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_GeExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_GtExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_EqExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_NeqExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_PlusExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_MinusExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_MultiplyExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_DivideExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ModuloExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseXorExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseAndExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_BitwiseOrExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ShiftLeftExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_ShiftRightExpression(self, expr):
                return self._visit_binary_ops(expr)

            def visit_StringOffsetExpression(self, expr):
                return self._visit_string_manipulation_ops(expr)

            def visit_StringLengthExpression(self, expr):
                return self._visit_string_manipulation_ops(expr)

            def _visit_logical_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                left_result = expr.left_operand.accept(self)
                right_result = expr.right_operand.accept(self)
                if left_result == yaramod.VisitAction.Delete or right_result == yaramod.VisitAction.Delete:
                    if left_result == yaramod.VisitAction.Delete:
                        new_operand = yaramod.bool_val(False).get()
                        expr.left_operand.exchange_tokens(new_operand)
                        expr.left_operand = new_operand
                    if right_result == yaramod.VisitAction.Delete:
                        new_operand = yaramod.bool_val(False).get()
                        expr.right_operand.exchange_tokens(new_operand)
                        expr.right_operand = new_operand
                else:
                    return self.default_handler(context, expr, left_result, right_result)

            def _visit_binary_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                left_result = expr.left_operand.accept(self)
                right_result = expr.right_operand.accept(self)
                if left_result == yaramod.VisitAction.Delete or right_result == yaramod.VisitAction.Delete:
                    return yaramod.VisitAction.Delete
                else:
                    self.default_handler(context, expr, left_result, right_result)

            def _visit_string_manipulation_ops(self, expr):
                context = yaramod.TokenStreamContext(expr)
                index_result = None
                if expr.index_expr:
                    index_result = expr.index_expr.accept(self)
                    if index_result == yaramod.VisitAction.Delete:
                        return yaramod.VisitAction.Delete
                return self.default_handler(context, expr, index_result)

            def visit_FunctionCallExpression(self, expr):
                function_name = expr.function.text
                if function_name.startswith('cuckoo.'):
                    if function_name in self.FILESYSTEM_REPLACE:
                        expr.function.symbol = self.filesystem_symbol
                    elif function_name in self.REGISTRY_REPLACE:
                        expr.function.symbol = self.registry_symbol
                    elif function_name not in self.WHITELIST:
                        return yaramod.VisitAction.Delete

        yara_file = yaramod.Yaramod().parse_string(r'''
import "cuckoo"

rule rule_1 {
	strings:
		$str1 = "a"
	condition:
		$str1 and
		(
			cuckoo.filesystem.file_write(/C:\\Users\\Avastian\\file1.exe/i) or
			cuckoo.filesystem.file_read(/C:\\Users\\Avastian\\file1.exe/i) or
			cuckoo.registry.key_write(/\\Microsoft\\Windows NT\\CurrentVersion/i) or
			cuckoo.network.http_post(/\/.*\/tasks\.php/) or
			cuckoo.process.executed_command(/(^|\\)a(\.exe|\s)/i)
		)
}
''')

        visitor = CuckooFunctionReplacer()
        visitor.replace_functions(yara_file)

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        cond = rule.condition
        print(cond.text)
        self.assertEqual(r'''$str1 and (cuckoo.filesystem.file_access(/C:\\Users\\Avastian\\file1.exe/i) or false or cuckoo.registry.key_access(/\\Microsoft\\Windows NT\\CurrentVersion/i) or cuckoo.network.http_post(/\/.*\/tasks\.php/) or false)''', cond.text)

        self.assertEqual(r'''import "cuckoo"

rule rule_1 {
	strings:
		$str1 = "a"
	condition:
		$str1 and (cuckoo.filesystem.file_access(/C:\\Users\\Avastian\\file1.exe/i) or false or cuckoo.registry.key_access(/\\Microsoft\\Windows NT\\CurrentVersion/i) or cuckoo.network.http_post(/\/.*\/tasks\.php/) or false)
}''', yara_file.text)
        expected = r'''
import "cuckoo"

rule rule_1
{
	strings:
		$str1 = "a"
	condition:
		$str1 and
		(
			cuckoo.filesystem.file_access(/C:\\Users\\Avastian\\file1.exe/i) or
			false or
			cuckoo.registry.key_access(/\\Microsoft\\Windows NT\\CurrentVersion/i) or
			cuckoo.network.http_post(/\/.*\/tasks\.php/) or
			false
		)
}
'''
        self.assertEqual(expected, yara_file.text_formatted)
