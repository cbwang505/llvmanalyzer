import unittest
import yaramod


class ParserTests(unittest.TestCase):
    def test_empty_input(self):
        yara_file = yaramod.Yaramod().parse_string('')

        self.assertEqual(yara_file.text, '')

    def test_empty_rule(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule empty_rule {
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'empty_rule')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 0)
        self.assertEqual(len(rule.tags), 0)

    def test_rule_with_tags(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_tags : Tag1 Tag2 Tag3 {
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_tags')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 0)
        self.assertListEqual(rule.tags, ['Tag1', 'Tag2', 'Tag3'])

    def test_rule_with_metas(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_metas {
    meta:
        str_meta = "string meta"
        int_meta = 42
        bool_meta = true
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_metas')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 3)
        self.assertEqual(len(rule.strings), 0)
        self.assertEqual(len(rule.tags), 0)

        self.assertEqual(rule.metas[0].key, 'str_meta')
        self.assertTrue(rule.metas[0].value.is_string)
        self.assertEqual(rule.metas[0].value.text, '"string meta"')
        self.assertEqual(rule.metas[0].value.pure_text, 'string meta')

        self.assertEqual(rule.metas[1].key, 'int_meta')
        self.assertTrue(rule.metas[1].value.is_int)
        self.assertEqual(rule.metas[1].value.text, '42')

        self.assertEqual(rule.metas[2].key, 'bool_meta')
        self.assertTrue(rule.metas[2].value.is_bool)
        self.assertEqual(rule.metas[2].value.text, 'true')

    def test_add_meta(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_added_metas {
	condition:
		true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_added_metas')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        rule.add_meta('int_meta', yaramod.Literal(42))
        rule.add_meta('new_meta', yaramod.Literal(False))
        self.assertEqual(len(rule.metas), 2)

        self.assertEqual(rule.metas[0].key, 'int_meta')
        self.assertTrue(rule.metas[0].value.is_int)
        self.assertEqual(rule.metas[0].value.pure_text, '42')

        self.assertEqual(rule.metas[1].key, 'new_meta')
        self.assertTrue(rule.metas[1].value.is_bool)
        self.assertEqual(rule.metas[1].value.text, 'false')


        expected = r'''
rule rule_with_added_metas
{
	meta:
		int_meta = 42
		new_meta = false
	condition:
		true
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_modify_meta(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_metas {
    meta:
        str_meta = "string meta"
        int_meta = 42
        bool_meta = true
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_metas')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 3)
        meta = rule.get_meta_with_name('int_meta')
        meta.value = yaramod.Literal('forty two')

        self.assertEqual(rule.metas[0].key, 'str_meta')
        self.assertTrue(rule.metas[0].value.is_string)
        self.assertEqual(rule.metas[0].value.text, '"string meta"')
        self.assertEqual(rule.metas[0].value.pure_text, 'string meta')

        self.assertEqual(rule.metas[1].key, 'int_meta')
        self.assertTrue(rule.metas[1].value.is_string)
        self.assertEqual(rule.metas[1].value.text, '"forty two"')

        self.assertEqual(rule.metas[2].key, 'bool_meta')
        self.assertTrue(rule.metas[2].value.is_bool)
        self.assertEqual(rule.metas[2].value.text, 'true')

        expected = r'''
rule rule_with_metas
{
	meta:
		str_meta = "string meta"
		int_meta = "forty two"
		bool_meta = true
	condition:
		true
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_rule_with_plain_strings(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_plain_strings {
    strings:
        $1 = "Hello World!"
        $2 = "Bye World."
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_plain_strings')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 2)
        self.assertEqual(len(rule.tags), 0)

        hello_world = rule.strings[0]
        self.assertEqual(hello_world.identifier, '$1')
        self.assertEqual(hello_world.text, '"Hello World!"')
        self.assertEqual(hello_world.pure_text, b'Hello World!')
        self.assertTrue(hello_world.is_ascii)

        bye_world = rule.strings[1]
        self.assertEqual(bye_world.identifier, '$2')
        self.assertEqual(bye_world.text, '"Bye World."')
        self.assertEqual(bye_world.pure_text, b'Bye World.')
        self.assertTrue(bye_world.is_ascii)

    def test_multiple_rules(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_1 {
    strings:
        $1 = "String from Rule 1"
    condition:
        true
}

rule rule_2 {
    strings:
        $1 = "String from Rule 2"
    condition:
        true
}

rule rule_3 {
    strings:
        $1 = "String from Rule 3"
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 3)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_1')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(rule.strings[0].identifier, '$1')
        self.assertEqual(rule.strings[0].pure_text, b'String from Rule 1')

        rule = yara_file.rules[1]
        self.assertEqual(rule.name, 'rule_2')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(rule.strings[0].identifier, '$1')
        self.assertEqual(rule.strings[0].pure_text, b'String from Rule 2')

        rule = yara_file.rules[2]
        self.assertEqual(rule.name, 'rule_3')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(rule.strings[0].identifier, '$1')
        self.assertEqual(rule.strings[0].pure_text, b'String from Rule 3')

    def test_links_between_rules_after_rename(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_1 {
    condition:
        true
}

rule rule_2 {
    condition:
        rule_1
}''')

        self.assertEqual(len(yara_file.rules), 2)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_1')
        rule.name = 'RULE_1'
        self.assertEqual(rule.name, 'RULE_1')

        rule = yara_file.rules[1]
        self.assertEqual(rule.name, 'rule_2')
        self.assertEqual(rule.condition.text, 'RULE_1')

    def test_plain_strings_with_modifiers(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_plain_strings_with_modifiers {
    strings:
        $1 = "Hello World!" nocase wide
        $2 = "Bye World." fullword
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_plain_strings_with_modifiers')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.strings), 2)

        string = rule.strings[0]
        self.assertFalse(string.is_ascii)
        self.assertTrue(string.is_wide)
        self.assertTrue(string.is_nocase)
        self.assertFalse(string.is_fullword)

        string = rule.strings[1]
        self.assertTrue(string.is_ascii)
        self.assertFalse(string.is_wide)
        self.assertFalse(string.is_nocase)
        self.assertTrue(string.is_fullword)

    def test_rule_with_hex_string(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_hex_string {
    strings:
        $1 = { 01 23 45 67 89 AB CD EF }
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_hex_string')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.strings), 1)

        string = rule.strings[0]
        self.assertTrue(string.is_hex)
        self.assertEqual(string.identifier, '$1')
        self.assertEqual(string.text, '{ 01 23 45 67 89 AB CD EF }')

    def test_rule_with_regexp(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule rule_with_regexp {
    strings:
        $1 = /abcd/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.strings), 1)

        string = rule.strings[0]
        self.assertTrue(string.is_regexp)
        self.assertEqual(string.identifier, '$1')
        self.assertEqual(string.text, '/abcd/')

    def test_global_rule(self):
        yara_file = yaramod.Yaramod().parse_string('''
global rule global_rule {
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'global_rule')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Global)
        self.assertTrue(rule.is_global)
        self.assertFalse(rule.is_private)
        rule.modifier = yaramod.RuleModifier.PrivateGlobal
        self.assertEqual(rule.modifier, yaramod.RuleModifier.PrivateGlobal)
        self.assertTrue(rule.is_global)
        self.assertTrue(rule.is_private)
        rule.modifier = yaramod.RuleModifier.Private
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Private)
        self.assertFalse(rule.is_global)
        self.assertTrue(rule.is_private)

    def test_private_rule(self):
        yara_file = yaramod.Yaramod().parse_string('''
private rule private_rule {
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'private_rule')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Private)
        self.assertFalse(rule.is_global)
        self.assertTrue(rule.is_private)
        rule.modifier = yaramod.RuleModifier.PrivateGlobal
        self.assertEqual(rule.modifier, yaramod.RuleModifier.PrivateGlobal)
        self.assertTrue(rule.is_global)
        self.assertTrue(rule.is_private)
        rule.modifier = yaramod.RuleModifier.Empty
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertFalse(rule.is_global)
        self.assertFalse(rule.is_private)

    def test_import(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"
import "phish"

rule dummy_rule {
    condition:
        true and new_file
}''')

        self.assertEqual(len(yara_file.imports), 2)
        self.assertEqual(len(yara_file.rules), 1)

        module = yara_file.imports[0]
        self.assertEqual(module.name, 'pe')

    def test_imports_without_avast_symbols(self):
        input_text = '''
import "pe"
import "phish"

rule dummy_rule {
    condition:
        true
}'''
        ymod = yaramod.Yaramod(yaramod.ImportFeatures.VirusTotal)
        with self.assertRaises(yaramod.ParserError):
            ymod.parse_string(input_text)

    def test_imports_without_virus_total_symbols(self):
        input_text = '''
import "pe"

rule dummy_rule {
    condition:
        true and new_file
}'''
        ymod = yaramod.Yaramod(yaramod.ImportFeatures.Avast)
        with self.assertRaises(yaramod.ParserError):
            ymod.parse_string(input_text)

    def test_imports_with_deprecated_symbols(self):
        yara_file = yaramod.Yaramod(yaramod.ImportFeatures.Everything).parse_string('''
import "cuckoo"

rule dummy_rule {
    condition:
        cuckoo.signature.hits(/regexp1/) and cuckoo.signature.name(/regexp2/) and new_file
}''')

        self.assertEqual(len(yara_file.imports), 1)
        self.assertEqual(len(yara_file.rules), 1)

        module = yara_file.imports[0]
        self.assertEqual(module.name, 'cuckoo')
        rule = yara_file.rules[0]
        self.assertEqual(rule.condition.text, 'cuckoo.signature.hits(/regexp1/) and cuckoo.signature.name(/regexp2/) and new_file')

    def test_bool_literal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule bool_literal_condition {
    condition:
        false
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.BoolLiteralExpression))
        self.assertEqual(rule.condition.text, 'false')

    def test_int_literal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule int_literal_condition {
    condition:
        10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, '10')

    def test_double_literal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule double_literal_condition {
    condition:
        1.23
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.DoubleLiteralExpression))
        self.assertEqual(rule.condition.text, '1.23')

    def test_string_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule string_condition {
    strings:
        $1 = "Hello World!"
    condition:
        $1
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringExpression))
        self.assertEqual(rule.condition.text, '$1')

    def test_string_at_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule string_at_condition {
    strings:
        $1 = "Hello World!"
    condition:
        $1 at entrypoint
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringAtExpression))
        self.assertTrue(isinstance(rule.condition.at_expr, yaramod.EntrypointExpression))
        self.assertEqual(rule.condition.text, '$1 at entrypoint')

    def test_string_in_range_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule string_in_condition {
    strings:
        $1 = "Hello World!"
    condition:
        $1 in (10 .. 20)
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringInRangeExpression))
        self.assertTrue(isinstance(rule.condition.range_expr, yaramod.RangeExpression))
        self.assertEqual(rule.condition.text, '$1 in (10 .. 20)')

    def test_not_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule not_condition {
    condition:
        not true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.NotExpression))
        self.assertTrue(isinstance(rule.condition.operand, yaramod.BoolLiteralExpression))
        self.assertEqual(rule.condition.text, 'not true')

    def test_unary_minus_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule unary_minus_condition {
    condition:
        -10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.UnaryMinusExpression))
        self.assertTrue(isinstance(rule.condition.operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, '-10')

    def test_and_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule and_condition {
    condition:
        true and not false
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.AndExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.BoolLiteralExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.NotExpression))
        self.assertEqual(rule.condition.text, 'true and not false')

    def test_or_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule or_condition {
    condition:
        true or not false
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.OrExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.BoolLiteralExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.NotExpression))
        self.assertEqual(rule.condition.text, 'true or not false')

    def test_less_than_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule less_than_condition {
    condition:
        filesize < 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.LtExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize < 10')

    def test_greater_than_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule greater_than_condition {
    condition:
        filesize > 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.GtExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize > 10')

    def test_less_equal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule less_equal_condition {
    condition:
        filesize <= 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.LeExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize <= 10')

    def test_greater_equal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule greater_equal_condition {
    condition:
        filesize >= 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.GeExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize >= 10')

    def test_equal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule equal_condition {
    condition:
        filesize == 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.EqExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize == 10')

    def test_not_equal_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule equal_condition {
    condition:
        filesize != 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.NeqExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize != 10')

    def test_parentheses_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule parentheses_condition {
    condition:
        (true)
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ParenthesesExpression))
        self.assertTrue(isinstance(rule.condition.enclosed_expr, yaramod.BoolLiteralExpression))
        self.assertEqual(rule.condition.text, '(true)')

    def test_plus_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule plus_condition {
    condition:
        filesize + 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.PlusExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize + 10')

    def test_minus_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule minus_condition {
    condition:
        filesize - 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.MinusExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize - 10')

    def test_multiply_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule multiply_condition {
    condition:
        filesize * 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.MultiplyExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize * 10')

    def test_divide_condition(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule divide_condition {
    condition:
        filesize \ 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.DivideExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, r'filesize \ 10')

    def test_modulo_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule modulo_condition {
    condition:
        filesize % 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ModuloExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize % 10')

    def test_shift_left_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule shift_left_condition {
    condition:
        filesize << 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ShiftLeftExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize << 10')

    def test_shift_right_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule shift_right_condition {
    condition:
        filesize >> 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ShiftRightExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize >> 10')

    def test_bitwise_not_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule xor_condition {
    condition:
        ~10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.BitwiseNotExpression))
        self.assertTrue(isinstance(rule.condition.operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, '~10')

    def test_xor_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule xor_condition {
    condition:
        filesize ^ 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.BitwiseXorExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize ^ 10')

    def test_bitwise_and_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule bitwise_and_condition {
    condition:
        filesize & 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.BitwiseAndExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize & 10')

    def test_bitwise_or_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule bitwise_or_condition {
    condition:
        filesize | 10
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.BitwiseOrExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.KeywordExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, 'filesize | 10')

    def test_int_function_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule int_function_condition {
    condition:
        int32be(5)
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.IntFunctionExpression))
        self.assertTrue(isinstance(rule.condition.argument, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.function, 'int32be')
        self.assertEqual(rule.condition.text, 'int32be(5)')

    def test_contains_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule contains_condition {
    condition:
        "Hello" contains "Hell"
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ContainsExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.StringLiteralExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.StringLiteralExpression))
        self.assertEqual(rule.condition.text, '"Hello" contains "Hell"')

    def test_matches_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule matches_condition {
    condition:
        "Hello" matches /^Hell.*$/
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.MatchesExpression))
        self.assertTrue(isinstance(rule.condition.left_operand, yaramod.StringLiteralExpression))
        self.assertTrue(isinstance(rule.condition.right_operand, yaramod.RegexpExpression))
        self.assertEqual(rule.condition.text, '"Hello" matches /^Hell.*$/')

    def test_match_count_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule match_count_condition {
    strings:
        $1 = "Hello World"
    condition:
        #1
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringCountExpression))
        self.assertEqual(rule.condition.text, '#1')

    def test_match_offset_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule match_offset_condition {
    strings:
        $1 = "Hello World"
    condition:
        @1
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringOffsetExpression))
        self.assertEqual(rule.condition.index_expr, None)
        self.assertEqual(rule.condition.text, '@1')

    def test_match_offset_with_index_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule match_offset_with_index_condition {
    strings:
        $1 = "Hello World"
    condition:
        @1[0]
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringOffsetExpression))
        self.assertTrue(isinstance(rule.condition.index_expr, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, '@1[0]')

    def test_match_length_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule match_length_condition {
    strings:
        $1 = "Hello World"
    condition:
        !1
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringLengthExpression))
        self.assertEqual(rule.condition.index_expr, None)
        self.assertEqual(rule.condition.text, '!1')

    def test_match_length_with_index_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
rule match_length_with_index_condition {
    strings:
        $1 = "Hello World"
    condition:
        !1[0]
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StringLengthExpression))
        self.assertTrue(isinstance(rule.condition.index_expr, yaramod.IntLiteralExpression))
        self.assertEqual(rule.condition.text, '!1[0]')

    def test_function_call_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule function_call_condition {
    condition:
        pe.is_dll()
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.FunctionCallExpression))
        self.assertEqual(rule.condition.text, 'pe.is_dll()')

    def test_structure_access_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule structure_access_condition {
    condition:
        pe.linker_version.major
}''')


        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.StructAccessExpression))
        self.assertEqual(rule.condition.text, 'pe.linker_version.major')

    def test_array_access_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule array_access_condition {
    condition:
        pe.sections[0]
}''')


        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ArrayAccessExpression))
        self.assertEqual(rule.condition.text, 'pe.sections[0]')

    def test_for_integer_set_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule for_integer_set_condition {
    condition:
        for all i in (1,2,3) : ( i )
}''')


        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ForIntExpression))
        self.assertTrue(isinstance(rule.condition.variable, yaramod.AllExpression))
        self.assertTrue(isinstance(rule.condition.iterated_set, yaramod.SetExpression))
        self.assertTrue(isinstance(rule.condition.body, yaramod.IdExpression))
        self.assertEqual(rule.condition.text, 'for all i in (1, 2, 3) : ( i )')

    def test_for_string_set_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule for_string_set_condition {
    strings:
        $a = "dummy1"
        $b = "dummy2"
    condition:
        for any of ($a,$b) : ( $ at entrypoint )
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.ForStringExpression))
        self.assertTrue(isinstance(rule.condition.variable, yaramod.AnyExpression))
        self.assertTrue(isinstance(rule.condition.iterated_set, yaramod.SetExpression))
        self.assertTrue(isinstance(rule.condition.body, yaramod.StringAtExpression))
        self.assertEqual(rule.condition.text, 'for any of ($a, $b) : ( $ at entrypoint )')

    def test_of_condition(self):
        yara_file = yaramod.Yaramod().parse_string('''
import "pe"

rule of_condition {
    strings:
        $a = "dummy1"
        $b = "dummy2"
    condition:
        1 of ($a,$b)
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertTrue(isinstance(rule.condition, yaramod.OfExpression))
        self.assertTrue(isinstance(rule.condition.variable, yaramod.IntLiteralExpression))
        self.assertTrue(isinstance(rule.condition.iterated_set, yaramod.SetExpression))
        self.assertEqual(rule.condition.body, None)
        self.assertEqual(rule.condition.text, '1 of ($a, $b)')

    def test_string_with_invalid_utf8_sequences(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_invalid_utf8_sequence {
    strings:
        $1 = "eKfI+`fKyD\xf4X h\xff\xf7\x98"
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_invalid_utf8_sequence')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        hello_world = rule.strings[0]
        self.assertEqual(hello_world.identifier, '$1')
        self.assertEqual(hello_world.text, r'"eKfI+`fKyD\xf4X h\xff\xf7\x98"')
        self.assertEqual(hello_world.pure_text, b'eKfI+`fKyD\xf4X h\xff\xf7\x98')
        self.assertTrue(hello_world.is_ascii)

    def test_parser_error(self):
        self.assertRaises(yaramod.ParserError, yaramod.Yaramod().parse_string, 'rule {')

    def test_simple_regular_expression_in_strings(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp {
    strings:
        $1 = /asd/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        regexp_string = rule.strings[0]
        self.assertEqual(regexp_string.identifier, '$1')
        self.assertTrue(regexp_string.is_regexp)
        self.assertTrue(isinstance(regexp_string.unit, yaramod.RegexpConcat))
        self.assertEqual(regexp_string.text, '/asd/')
        self.assertEqual(regexp_string.pure_text, b'asd')
        self.assertEqual(regexp_string.suffix_modifiers, '')
        self.assertEqual(regexp_string.modifiers_text, '')
        self.assertEqual(len(regexp_string.unit.units), 3)
        self.assertEqual(regexp_string.unit.units[0].text, 'a')
        self.assertEqual(regexp_string.unit.units[1].text, 's')
        self.assertEqual(regexp_string.unit.units[2].text, 'd')

    def test_complex_regular_expression_in_strings(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_complex_regexp {
    strings:
        $1 = /asd|1234/ nocase ascii wide fullword
        $2 = /md5: [0-9a-zA-Z]{32}/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_complex_regexp')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 2)
        self.assertEqual(len(rule.tags), 0)

        regexp_string = rule.strings[0]
        self.assertEqual(regexp_string.identifier, '$1')
        self.assertTrue(regexp_string.is_regexp)
        self.assertEqual(regexp_string.text, '/asd|1234/ ascii wide nocase fullword')
        self.assertEqual(regexp_string.pure_text, b'asd|1234')
        self.assertEqual(regexp_string.suffix_modifiers, '')
        self.assertEqual(regexp_string.modifiers_text, ' ascii wide nocase fullword')
        self.assertTrue(isinstance(regexp_string.unit, yaramod.RegexpOr))
        self.assertEqual(regexp_string.unit.text, 'asd|1234')
        self.assertEqual(len(regexp_string.unit.left.units), 3)
        self.assertTrue(isinstance(regexp_string.unit.left.units[0], yaramod.RegexpText))
        self.assertEqual(regexp_string.unit.left.units[0].text, 'a')
        self.assertEqual(regexp_string.unit.left.units[1].text, 's')
        self.assertEqual(regexp_string.unit.left.units[2].text, 'd')
        self.assertEqual(len(regexp_string.unit.right.units), 4)
        self.assertTrue(isinstance(regexp_string.unit.right.units[0], yaramod.RegexpText))
        self.assertEqual(regexp_string.unit.right.units[0].text, '1')
        self.assertEqual(regexp_string.unit.right.units[1].text, '2')
        self.assertEqual(regexp_string.unit.right.units[2].text, '3')
        self.assertEqual(regexp_string.unit.right.units[3].text, '4')

        regexp_string = rule.strings[1]
        self.assertEqual(regexp_string.identifier, '$2')
        self.assertTrue(regexp_string.is_regexp)
        self.assertEqual(regexp_string.text, '/md5: [0-9a-zA-Z]{32}/')
        self.assertEqual(regexp_string.pure_text, b'md5: [0-9a-zA-Z]{32}')
        self.assertEqual(regexp_string.modifiers_text, '')
        self.assertEqual(len(regexp_string.unit.units), 6)
        self.assertEqual(regexp_string.unit.units[0].text, 'm')
        self.assertEqual(regexp_string.unit.units[1].text, 'd')
        self.assertEqual(regexp_string.unit.units[2].text, '5')
        self.assertEqual(regexp_string.unit.units[3].text, ':')
        self.assertEqual(regexp_string.unit.units[4].text, ' ')
        self.assertTrue(isinstance(regexp_string.unit.units[5], yaramod.RegexpRange))
        self.assertTrue(regexp_string.unit.units[5].is_greedy)
        self.assertEqual(regexp_string.unit.units[5].range[0], 32)
        self.assertEqual(regexp_string.unit.units[5].range[1], 32)
        self.assertEqual(regexp_string.unit.units[5].operation, ' ')
        self.assertTrue(isinstance(regexp_string.unit.units[5].operand, yaramod.RegexpClass))
        self.assertEqual(regexp_string.unit.units[5].operand.is_negative, False)
        self.assertEqual(regexp_string.unit.units[5].operand.characters, '0-9a-zA-Z')

    def test_regular_expression_meta_characters(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_metachars {
    strings:
        $1 = /^\\\/\|\(\)\[\]$/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_metachars')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(isinstance(rule.strings[0], yaramod.Regexp))
        regexp = rule.strings[0]
        self.assertTrue(isinstance(regexp.unit, yaramod.RegexpConcat))
        self.assertEqual(len(regexp.unit.units), 9)
        self.assertTrue(all(isinstance(u, yaramod.RegexpText) for u in regexp.unit.units))
        self.assertTrue(isinstance(regexp.unit.units[0], yaramod.RegexpStartOfLine))
        self.assertEqual(regexp.unit.units[0].text, '^')
        self.assertEqual(regexp.unit.units[1].text, '\\\\')
        self.assertEqual(regexp.unit.units[2].text, r'\/')
        self.assertEqual(regexp.unit.units[3].text, r'\|')
        self.assertEqual(regexp.unit.units[4].text, r'\(')
        self.assertEqual(regexp.unit.units[5].text, r'\)')
        self.assertEqual(regexp.unit.units[6].text, r'\[')
        self.assertEqual(regexp.unit.units[7].text, r'\]')
        self.assertTrue(isinstance(regexp.unit.units[8], yaramod.RegexpEndOfLine))
        self.assertEqual(regexp.unit.units[8].text, '$')

    def test_regular_expression_quantifiers(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_quantifiers {
    strings:
        $1 = /a*b+c?d{100}e{200,}f{,300}g{400,500}/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_quantifiers')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(isinstance(rule.strings[0], yaramod.Regexp))
        regexp = rule.strings[0]
        self.assertTrue(isinstance(regexp.unit, yaramod.RegexpConcat))
        self.assertEqual(len(regexp.unit.units), 7)
        self.assertTrue(all(isinstance(u, yaramod.RegexpOperation) for u in regexp.unit.units))
        self.assertTrue(all(u.is_greedy for u in regexp.unit.units))

        self.assertTrue(isinstance(regexp.unit.units[0], yaramod.RegexpIteration))
        self.assertEqual(regexp.unit.units[0].text, 'a*')
        self.assertTrue(isinstance(regexp.unit.units[0].operand, yaramod.RegexpText))
        self.assertEqual(regexp.unit.units[0].operand.text, 'a')
        self.assertTrue(isinstance(regexp.unit.units[1], yaramod.RegexpPositiveIteration))
        self.assertEqual(regexp.unit.units[1].text, 'b+')
        self.assertTrue(isinstance(regexp.unit.units[2], yaramod.RegexpOptional))
        self.assertEqual(regexp.unit.units[2].text, 'c?')
        self.assertTrue(isinstance(regexp.unit.units[3], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[3].text, 'd{100}')
        self.assertEqual(regexp.unit.units[3].range[0], 100)
        self.assertEqual(regexp.unit.units[3].range[1], 100)
        self.assertTrue(isinstance(regexp.unit.units[4], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[4].text, 'e{200,}')
        self.assertEqual(regexp.unit.units[4].range[0], 200)
        self.assertEqual(regexp.unit.units[4].range[1], None)
        self.assertTrue(isinstance(regexp.unit.units[5], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[5].text, 'f{,300}')
        self.assertEqual(regexp.unit.units[5].range[0], None)
        self.assertEqual(regexp.unit.units[5].range[1], 300)
        self.assertTrue(isinstance(regexp.unit.units[6], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[6].text, 'g{400,500}')
        self.assertEqual(regexp.unit.units[6].range[0], 400)
        self.assertEqual(regexp.unit.units[6].range[1], 500)

    def test_regular_expression_quantifiers_nongreedy(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_quantifiers_nongreedy {
    strings:
        $1 = /a*?b+?c??d{100}?e{200,}?f{,300}?g{400,500}?/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_quantifiers_nongreedy')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(isinstance(rule.strings[0], yaramod.Regexp))
        regexp = rule.strings[0]
        self.assertTrue(isinstance(regexp.unit, yaramod.RegexpConcat))
        self.assertEqual(len(regexp.unit.units), 7)
        self.assertTrue(all(isinstance(u, yaramod.RegexpOperation) for u in regexp.unit.units))
        self.assertTrue(all(not u.is_greedy for u in regexp.unit.units))

        self.assertTrue(isinstance(regexp.unit.units[0], yaramod.RegexpIteration))
        self.assertEqual(regexp.unit.units[0].text, 'a*?')
        self.assertTrue(isinstance(regexp.unit.units[0].operand, yaramod.RegexpText))
        self.assertEqual(regexp.unit.units[0].operand.text, 'a')
        self.assertTrue(isinstance(regexp.unit.units[1], yaramod.RegexpPositiveIteration))
        self.assertEqual(regexp.unit.units[1].text, 'b+?')
        self.assertTrue(isinstance(regexp.unit.units[2], yaramod.RegexpOptional))
        self.assertEqual(regexp.unit.units[2].text, 'c??')
        self.assertTrue(isinstance(regexp.unit.units[3], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[3].text, 'd{100}?')
        self.assertEqual(regexp.unit.units[3].range[0], 100)
        self.assertEqual(regexp.unit.units[3].range[1], 100)
        self.assertTrue(isinstance(regexp.unit.units[4], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[4].text, 'e{200,}?')
        self.assertEqual(regexp.unit.units[4].range[0], 200)
        self.assertEqual(regexp.unit.units[4].range[1], None)
        self.assertTrue(isinstance(regexp.unit.units[5], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[5].text, 'f{,300}?')
        self.assertEqual(regexp.unit.units[5].range[0], None)
        self.assertEqual(regexp.unit.units[5].range[1], 300)
        self.assertTrue(isinstance(regexp.unit.units[6], yaramod.RegexpRange))
        self.assertEqual(regexp.unit.units[6].text, 'g{400,500}?')
        self.assertEqual(regexp.unit.units[6].range[0], 400)
        self.assertEqual(regexp.unit.units[6].range[1], 500)

    def test_regular_expression_escape_sequences(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_escape_sequences {
    strings:
        $1 = /\t\n\r\f\a/
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_escape_sequences')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(isinstance(rule.strings[0], yaramod.Regexp))
        regexp = rule.strings[0]
        self.assertTrue(isinstance(regexp.unit, yaramod.RegexpConcat))
        self.assertEqual(len(regexp.unit.units), 5)
        self.assertTrue(all(isinstance(u, yaramod.RegexpText) for u in regexp.unit.units))
        self.assertEqual(regexp.unit.units[0].text, '\\t')
        self.assertEqual(regexp.unit.units[1].text, '\\n')
        self.assertEqual(regexp.unit.units[2].text, '\\r')
        self.assertEqual(regexp.unit.units[3].text, '\\f')
        self.assertEqual(regexp.unit.units[4].text, '\\a')

    def test_regular_expression_character_classes(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_character_classes {
    strings:
        $1 = /\w\W\s\S\d\D\b\B./
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_character_classes')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 1)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(isinstance(rule.strings[0], yaramod.Regexp))
        regexp = rule.strings[0]
        self.assertTrue(isinstance(regexp.unit, yaramod.RegexpConcat))
        self.assertEqual(len(regexp.unit.units), 9)
        self.assertTrue(all(isinstance(u, yaramod.RegexpText) for u in regexp.unit.units))
        self.assertTrue(isinstance(regexp.unit.units[0], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[0].text, '\\w')
        self.assertTrue(isinstance(regexp.unit.units[1], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[1].text, '\\W')
        self.assertTrue(isinstance(regexp.unit.units[2], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[2].text, '\\s')
        self.assertTrue(isinstance(regexp.unit.units[3], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[3].text, '\\S')
        self.assertTrue(isinstance(regexp.unit.units[4], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[4].text, '\\d')
        self.assertTrue(isinstance(regexp.unit.units[5], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[5].text, '\\D')
        self.assertTrue(isinstance(regexp.unit.units[6], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[6].text, '\\b')
        self.assertTrue(isinstance(regexp.unit.units[7], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[7].text, '\\B')
        self.assertTrue(isinstance(regexp.unit.units[8], yaramod.RegexpUnit))
        self.assertEqual(regexp.unit.units[8].text, '.')

    def test_complex_regular_expression_in_fnc_call(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
import "cuckoo"

rule rule_with_regexp_in_fnc_call {
    condition:
        cuckoo.network.http_request(/http:\/\/someone\.doingevil\.com/)
}''')

        self.assertEqual(len(yara_file.rules), 1)

        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_in_fnc_call')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 0)
        self.assertEqual(len(rule.tags), 0)

        cond = rule.condition
        self.assertTrue(isinstance(cond, yaramod.FunctionCallExpression))
        self.assertEqual(len(cond.arguments), 1)
        self.assertTrue(isinstance(cond.arguments[0], yaramod.RegexpExpression))
        self.assertTrue(isinstance(cond.arguments[0].regexp_string, yaramod.Regexp))
        self.assertEqual(cond.arguments[0].regexp_string.text, r'/http:\/\/someone\.doingevil\.com/')
        self.assertEqual(cond.arguments[0].regexp_string.pure_text, rb'http:\/\/someone\.doingevil\.com')
        self.assertTrue(isinstance(cond.arguments[0].regexp_string.unit, yaramod.RegexpConcat))
        self.assertEqual(len(cond.arguments[0].regexp_string.unit.units), 28)

    def test_regular_expression_suffix_modifiers(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
rule rule_with_regexp_suffix_modifiers {
    strings:
        $1 = /none/
        $2 = /just_i/i
        $3 = /just_s/s
        $4 = /all/s nocase ascii wide fullword
        $5 = /all/i nocase ascii wide fullword
    condition:
        true
}''')

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule_with_regexp_suffix_modifiers')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 5)
        self.assertEqual(len(rule.tags), 0)

        self.assertTrue(all(rstring.is_regexp for rstring in rule.strings))
        self.assertEqual(rule.strings[0].suffix_modifiers, '')
        self.assertEqual(rule.strings[0].modifiers_text, '')
        self.assertEqual(rule.strings[0].text, '/none/')
        self.assertEqual(rule.strings[1].suffix_modifiers, 'i')
        self.assertEqual(rule.strings[1].modifiers_text, '')
        self.assertEqual(rule.strings[1].text, '/just_i/i')
        self.assertEqual(rule.strings[2].suffix_modifiers, 's')
        self.assertEqual(rule.strings[2].modifiers_text, '')
        self.assertEqual(rule.strings[2].text, '/just_s/s')
        self.assertEqual(rule.strings[3].suffix_modifiers, 's')
        self.assertEqual(rule.strings[3].modifiers_text, ' ascii wide nocase fullword')
        self.assertEqual(rule.strings[3].text, '/all/s ascii wide nocase fullword')
        self.assertEqual(rule.strings[4].suffix_modifiers, 'i')
        self.assertEqual(rule.strings[4].modifiers_text, ' ascii wide nocase fullword')
        self.assertEqual(rule.strings[4].text, '/all/i ascii wide nocase fullword')

    def test_nonutf_comments(self):
        yara_file = yaramod.Yaramod().parse_string(r'''
import "cuckoo"

rule nonutf_condition
{
	/* /내/ and Pop\x83\xe9 */
	strings:
		$s1 = "a" // /내/
		$s3 = "c" // // pe.rich_signature == "Pop\x83\xe9"
	condition:
		false or // /내/
		true // // pe.rich_signature == "Pop\x00\x83\x00\xe9"
}
''')
        rule = yara_file.rules[0]

        expected = r'''
import "cuckoo"

rule nonutf_condition
{
	/* /내/ and Pop\x83\xe9 */
	strings:
		$s1 = "a" // /내/
		$s3 = "c" // // pe.rich_signature == "Pop\x83\xe9"
	condition:
		false or // /내/
		true     // // pe.rich_signature == "Pop\x00\x83\x00\xe9"
}
'''
        self.assertEqual(expected, yara_file.text_formatted)

    def test_multiple_parse_phases_with_single_yaramod_instance(self):
        ymod = yaramod.Yaramod();
        yara_file = ymod.parse_string(r'''
rule rule1 : Tag1 {
    meta:
        author = "Mr. Avastian"
    strings:
        $1 = /abc/is
        $2 = "Hello World!"
    condition:
        all of them
}''')

        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule1')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 1)
        self.assertEqual(len(rule.strings), 2)
        self.assertEqual(len(rule.tags), 1)
        self.assertEqual(rule.strings[0].identifier, '$1')
        self.assertEqual(rule.strings[0].suffix_modifiers, 'is')
        self.assertEqual(rule.strings[0].modifiers_text, '')
        self.assertEqual(rule.strings[0].text, '/abc/is')

        self.assertEqual(rule.strings[1].identifier, '$2')
        self.assertEqual(rule.strings[1].text, '"Hello World!"')
        self.assertEqual(rule.strings[1].pure_text, b'Hello World!')
        self.assertTrue(rule.strings[1].is_ascii)

        yara_file = ymod.parse_string(r'''
rule rule2 : Tag1 {
    strings:
        $3 = /def/
        $4 = "Good bye world!"
    condition:
        true
}''')
        self.assertEqual(len(yara_file.rules), 1)
        rule = yara_file.rules[0]
        self.assertEqual(rule.name, 'rule2')
        self.assertEqual(rule.modifier, yaramod.RuleModifier.Empty)
        self.assertEqual(len(rule.metas), 0)
        self.assertEqual(len(rule.strings), 2)
        self.assertEqual(len(rule.tags), 1)
        self.assertEqual(rule.strings[0].identifier, '$3')
        self.assertEqual(rule.strings[0].suffix_modifiers, '')
        self.assertEqual(rule.strings[0].modifiers_text, '')
        self.assertEqual(rule.strings[0].text, '/def/')

        self.assertEqual(rule.strings[1].identifier, '$4')
        self.assertEqual(rule.strings[1].text, '"Good bye world!"')
        self.assertEqual(rule.strings[1].pure_text, b'Good bye world!')
        self.assertTrue(rule.strings[1].is_ascii)
