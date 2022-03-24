import unittest
import yaramod


class BuilderTests(unittest.TestCase):
    def setUp(self):
        self.new_file = yaramod.YaraFileBuilder(yaramod.ImportFeatures.AllCurrent)
        self.new_rule = yaramod.YaraRuleBuilder()

    def test_empty_file(self):
        yara_file = self.new_file.get()

        self.assertEqual(yara_file.text, '')

    def test_pure_imports(self):
        yara_file = self.new_file \
            .with_module('phish') \
            .with_module('pe') \
            .with_module('elf') \
            .get()

        self.assertEqual(yara_file.text_formatted, '''import "elf"
import "pe"
import "phish"

''')
        self.assertEqual(yara_file.text, '''import "elf"
import "pe"
import "phish"
''')

    def test_empty_rule(self):
        rule = self.new_rule \
            .with_name('empty_rule') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule empty_rule
{
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule empty_rule {
	condition:
		true
}''')

    def test_rule_with_metas(self):
        rule = self.new_rule \
            .with_name('rule_with_metas') \
            .with_string_meta('string_meta', 'string value') \
            .with_int_meta('int_meta', 42) \
            .with_hex_int_meta('hex_int_meta', 0x42) \
            .with_bool_meta('bool_meta', False) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_metas
{
	meta:
		string_meta = "string value"
		int_meta = 42
		hex_int_meta = 0x42
		bool_meta = false
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_metas {
	meta:
		string_meta = "string value"
		int_meta = 42
		hex_int_meta = 0x42
		bool_meta = false
	condition:
		true
}''')

    def test_rule_with_tags(self):
        rule = self.new_rule \
            .with_name('rule_with_tags') \
            .with_tag('Tag1') \
            .with_tag('Tag2') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_tags : Tag1 Tag2
{
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_tags : Tag1 Tag2 {
	condition:
		true
}''')

    def test_rule_with_modifiers(self):
        rule = self.new_rule \
            .with_name('private_rule') \
            .with_modifier(yaramod.RuleModifier.Private) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''private rule private_rule
{
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''private rule private_rule {
	condition:
		true
}''')

    def test_rule_with_comments(self):
        rule = self.new_rule \
            .with_name('private_rule') \
            .with_modifier(yaramod.RuleModifier.Private) \
            .with_comment("multiline comment") \
            .with_comment("oneline comment", 0) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''/* multiline comment */
// oneline comment
private rule private_rule
{
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''private rule private_rule {
	condition:
		true
}''')

    def test_rule_with_plain_string(self):
        rule = self.new_rule \
            .with_name('rule_with_plain_string') \
            .with_plain_string('$1', 'This is plain string.').ascii().wide() \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_plain_string
{
	strings:
		$1 = "This is plain string." ascii wide
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_plain_string {
	strings:
		$1 = "This is plain string." ascii wide
	condition:
		true
}''')

    def test_rule_with_hex_string(self):
        rule = self.new_rule \
            .with_name('rule_with_hex_string') \
            .with_hex_string('$1', yaramod.YaraHexStringBuilder([0x10, 0x11]).get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_hex_string
{
	strings:
		$1 = { 10 11 }
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_hex_string {
	strings:
		$1 = { 10 11 }
	condition:
		true
}''')

    def test_rule_with_regexp(self):
        rule = self.new_rule \
            .with_name('rule_with_regexp') \
            .with_regexp('$1', '[a-z0-9]{32}', 'i') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_regexp
{
	strings:
		$1 = /[a-z0-9]{32}/i
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_regexp {
	strings:
		$1 = /[a-z0-9]{32}/i
	condition:
		true
}''')

    def test_rule_with_double_values(self):
        cond = yaramod.double_val(3.14159) > yaramod.double_val(2.71828)
        rule = self.new_rule \
            .with_name('rule_with_double_values') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_double_values
{
	condition:
		3.14159 > 2.71828
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_double_values {
	condition:
		3.14159 > 2.71828
}''')

    def test_multiple_rules(self):
        rule1 = self.new_rule \
            .with_name('rule_1') \
            .with_tag('Tag1') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .get()
        rule2 = self.new_rule \
            .with_name('rule_2') \
            .with_tag('Tag2') \
            .with_plain_string('$2', 'This is plain string 2.') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule1) \
            .with_rule(rule2) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_1 : Tag1
{
	strings:
		$1 = "This is plain string 1."
	condition:
		true
}

rule rule_2 : Tag2
{
	strings:
		$2 = "This is plain string 2."
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_1 : Tag1 {
	strings:
		$1 = "This is plain string 1."
	condition:
		true
}

rule rule_2 : Tag2 {
	strings:
		$2 = "This is plain string 2."
	condition:
		true
}''')

    def test_rule_with_string_id_condition(self):
        cond = yaramod.string_ref('$1')
        rule = self.new_rule \
            .with_name('rule_with_string_id_condition') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_string_id_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		$1
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_string_id_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		$1
}''')

    def test_rule_with_string_at_condition(self):
        cond = yaramod.match_at('$1', yaramod.int_val(100))
        rule = self.new_rule \
            .with_name('rule_with_string_id_condition') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_string_id_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		$1 at 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_string_id_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		$1 at 100
}''')

    def test_rule_with_match_in_range_condition(self):
        cond = yaramod.match_in_range('$1', yaramod.range(yaramod.int_val(100), yaramod.int_val(200)))
        rule = self.new_rule \
            .with_name('rule_with_match_in_range_condition') \
            .with_condition(cond.get()) \
            .with_plain_string('$1', 'This is plain string 1.') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_in_range_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		$1 in (100 .. 200)
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_in_range_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		$1 in (100 .. 200)
}''')

    def test_rule_with_match_count_condition(self):
        cond = yaramod.match_count('$1')
        rule = self.new_rule \
            .with_name('rule_with_match_count_condition') \
            .with_condition(cond.get()) \
            .with_plain_string('$1', 'This is plain string 1.') \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_count_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		#1
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_count_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		#1
}''')

    def test_rule_with_match_length_condition(self):
        cond = yaramod.match_length('$1')
        rule = self.new_rule \
            .with_name('rule_with_match_length_condition') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_length_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		!1
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_length_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		!1
}''')

    def test_rule_with_match_length_with_index_condition(self):
        cond = yaramod.match_length('$1', yaramod.int_val(0))
        rule = self.new_rule \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_name('rule_with_match_length_with_index_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_length_with_index_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		!1[0]
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_length_with_index_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		!1[0]
}''')

    def test_rule_with_match_offset_condition(self):
        cond = yaramod.match_offset('$1')
        rule = self.new_rule \
            .with_name('rule_with_match_offset_condition') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_offset_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		@1
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_offset_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		@1
}''')

    def test_rule_with_match_offset_with_index_condition(self):
        cond = yaramod.match_offset('$1', yaramod.int_val(0))
        rule = self.new_rule \
            .with_name('rule_with_match_offset_with_index_condition') \
            .with_plain_string('$1', 'This is plain string 1.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_match_offset_with_index_condition
{
	strings:
		$1 = "This is plain string 1."
	condition:
		@1[0]
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_match_offset_with_index_condition {
	strings:
		$1 = "This is plain string 1."
	condition:
		@1[0]
}''')

    def test_rule_with_lt_condition(self):
        cond = yaramod.filesize() < yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_gt_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_gt_condition
{
	condition:
		filesize < 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_gt_condition {
	condition:
		filesize < 100
}''')

    def test_rule_with_le_condition(self):
        cond = yaramod.filesize() <= yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_ge_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_ge_condition
{
	condition:
		filesize <= 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_ge_condition {
	condition:
		filesize <= 100
}''')

    def test_rule_with_gt_condition(self):
        cond = yaramod.filesize() > yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_gt_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_gt_condition
{
	condition:
		filesize > 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_gt_condition {
	condition:
		filesize > 100
}''')

    def test_rule_with_ge_condition(self):
        cond = yaramod.filesize() >= yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_ge_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_ge_condition
{
	condition:
		filesize >= 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_ge_condition {
	condition:
		filesize >= 100
}''')

    def test_rule_with_eq_condition(self):
        cond = yaramod.filesize() == yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_eq_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_eq_condition
{
	condition:
		filesize == 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_eq_condition {
	condition:
		filesize == 100
}''')

    def test_rule_with_neq_condition(self):
        cond = yaramod.filesize() != yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_neq_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_neq_condition
{
	condition:
		filesize != 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_neq_condition {
	condition:
		filesize != 100
}''')

    def test_rule_with_plus_condition(self):
        cond = yaramod.filesize() + yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_plus_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_plus_condition
{
	condition:
		filesize + 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_plus_condition {
	condition:
		filesize + 100
}''')

    def test_rule_with_minus_condition(self):
        cond = yaramod.filesize() - yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_minus_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_minus_condition
{
	condition:
		filesize - 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_minus_condition {
	condition:
		filesize - 100
}''')

    def test_rule_with_multiply_condition(self):
        cond = yaramod.filesize() * yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_multiply_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_multiply_condition
{
	condition:
		filesize * 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_multiply_condition {
	condition:
		filesize * 100
}''')

    def test_rule_with_divide_condition(self):
        cond = yaramod.filesize() / yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_divide_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, r'''rule rule_with_divide_condition
{
	condition:
		filesize \ 100
}
''')
        self.assertEqual(yara_file.text, r'''rule rule_with_divide_condition {
	condition:
		filesize \ 100
}''')

    def test_rule_with_modulo_condition(self):
        cond = yaramod.filesize() % yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_modulo_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_modulo_condition
{
	condition:
		filesize % 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_modulo_condition {
	condition:
		filesize % 100
}''')

    def test_rule_with_xor_condition(self):
        cond = yaramod.filesize() ^ yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_xor_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_xor_condition
{
	condition:
		filesize ^ 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_xor_condition {
	condition:
		filesize ^ 100
}''')

    def test_rule_with_bitwise_and_condition(self):
        cond = yaramod.filesize() & yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_bitwise_and_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_bitwise_and_condition
{
	condition:
		filesize & 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_bitwise_and_condition {
	condition:
		filesize & 100
}''')

    def test_rule_with_bitwise_or_condition(self):
        cond = yaramod.filesize() | yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_bitwise_or_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_bitwise_or_condition
{
	condition:
		filesize | 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_bitwise_or_condition {
	condition:
		filesize | 100
}''')

    def test_rule_with_and_condition(self):
        cond = yaramod.conjunction([yaramod.filesize() > yaramod.int_val(100), yaramod.filesize() < yaramod.int_val(200)])
        rule = self.new_rule \
            .with_name('rule_with_and_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_and_condition
{
	condition:
		filesize > 100 and
		filesize < 200
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_and_condition {
	condition:
		filesize > 100 and filesize < 200
}''')

    def test_rule_with_or_condition(self):
        cond = yaramod.disjunction([yaramod.filesize() > yaramod.int_val(100), yaramod.filesize() < yaramod.int_val(200)])
        rule = self.new_rule \
            .with_name('rule_with_or_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_or_condition
{
	condition:
		filesize > 100 or
		filesize < 200
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_or_condition {
	condition:
		filesize > 100 or filesize < 200
}''')

    def test_rule_with_and_condition_with_comments(self):
        cond = yaramod.conjunction([[yaramod.filesize() > yaramod.int_val(100), 'comment1'], [yaramod.filesize() < yaramod.int_val(200), 'comment2']])
        rule = self.new_rule \
            .with_name('rule_with_and_condition_with_comments') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_and_condition_with_comments
{
	condition:
		/* comment1 */
		filesize > 100 and
		/* comment2 */
		filesize < 200
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_and_condition_with_comments {
	condition:
		filesize > 100 and
		filesize < 200
}''')

    def test_rule_with_or_condition_with_comments(self):
        cond = yaramod.disjunction([[yaramod.filesize() > yaramod.int_val(100), 'skip small files'], [yaramod.filesize() < yaramod.int_val(200), 'also too big files']])
        rule = self.new_rule \
            .with_name('rule_with_or_condition_with_comments') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_or_condition_with_comments
{
	condition:
		/* skip small files */
		filesize > 100 or
		/* also too big files */
		filesize < 200
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_or_condition_with_comments {
	condition:
		filesize > 100 or
		filesize < 200
}''')

    def test_rule_with_unary_minus_condition(self):
        cond = -yaramod.int_val(10)
        rule = self.new_rule \
            .with_name('rule_with_unary_minus_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_unary_minus_condition
{
	condition:
		-10
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_unary_minus_condition {
	condition:
		-10
}''')

    def test_rule_with_not_condition(self):
        cond = yaramod.not_(yaramod.filesize() < yaramod.int_val(100))
        rule = self.new_rule \
            .with_name('rule_with_not_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_not_condition
{
	condition:
		not filesize < 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_not_condition {
	condition:
		not filesize < 100
}''')

    def test_rule_with_bitwise_not_condition(self):
        cond = ~yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_bitwise_not_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_bitwise_not_condition
{
	condition:
		~100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_bitwise_not_condition {
	condition:
		~100
}''')

    def test_rule_with_shift_left_condition(self):
        cond = yaramod.filesize() << yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_shift_left_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_shift_left_condition
{
	condition:
		filesize << 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_shift_left_condition {
	condition:
		filesize << 100
}''')

    def test_rule_with_shift_right_condition(self):
        cond = yaramod.filesize() >> yaramod.int_val(100)
        rule = self.new_rule \
            .with_name('rule_with_shift_right_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_shift_right_condition
{
	condition:
		filesize >> 100
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_shift_right_condition {
	condition:
		filesize >> 100
}''')

    def test_rule_with_function_call_condition(self):
        cond = yaramod.id('pe').access('is_dll')()
        rule = self.new_rule \
            .with_name('rule_with_function_call_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_module('pe') \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''import "pe"

rule rule_with_function_call_condition
{
	condition:
		pe.is_dll()
}
''')
        self.assertEqual(yara_file.text, '''import "pe"

rule rule_with_function_call_condition {
	condition:
		pe.is_dll()
}''')

    def test_rule_with_structure_access_condition(self):
        cond = yaramod.id('pe').access('linker_version').access('major')
        rule = self.new_rule \
            .with_name('rule_with_structure_access_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_module('pe') \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''import "pe"

rule rule_with_structure_access_condition
{
	condition:
		pe.linker_version.major
}
''')
        self.assertEqual(yara_file.text, '''import "pe"

rule rule_with_structure_access_condition {
	condition:
		pe.linker_version.major
}''')

    def test_rule_with_array_access_condition(self):
        cond = yaramod.id('pe').access('sections')[yaramod.int_val(0)].access('name')
        rule = self.new_rule \
            .with_name('rule_with_array_access_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_module('pe') \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''import "pe"

rule rule_with_array_access_condition
{
	condition:
		pe.sections[0].name
}
''')
        self.assertEqual(yara_file.text, '''import "pe"

rule rule_with_array_access_condition {
	condition:
		pe.sections[0].name
}''')

    def test_rule_with_dictionary_access_condition(self):
        cond = yaramod.id('pe').access('version_info')[yaramod.string_val('CompanyName')]
        rule = self.new_rule \
            .with_name('rule_with_dictionary_access_condition') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_module('pe') \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''import "pe"

rule rule_with_dictionary_access_condition
{
	condition:
		pe.version_info["CompanyName"]
}
''')
        self.assertEqual(yara_file.text, '''import "pe"

rule rule_with_dictionary_access_condition {
	condition:
		pe.version_info["CompanyName"]
}''')

    def test_rule_with_complex_condition(self):
        cond = yaramod.for_loop(
                yaramod.any(),
                'i',
                yaramod.set([
                    yaramod.int_val(1),
                    yaramod.int_val(2),
                    yaramod.int_val(3)
                ]),
                yaramod.match_at(
                    '$1',
                    yaramod.paren(
                        yaramod.entrypoint() + yaramod.id('i')
                    )
                )
            )
        rule = self.new_rule \
            .with_name('rule_with_complex_condition') \
            .with_plain_string('$1', 'This is plain string.') \
            .with_condition(cond.get()) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()


        self.assertEqual(yara_file.text_formatted, '''rule rule_with_complex_condition
{
	strings:
		$1 = "This is plain string."
	condition:
		for any i in (1, 2, 3) : ( $1 at (entrypoint + i) )
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_complex_condition {
	strings:
		$1 = "This is plain string."
	condition:
		for any i in (1, 2, 3) : ( $1 at (entrypoint + i) )
}''')

    def test_rule_with_string_modifiers(self):
        rule = self.new_rule \
            .with_name('rule_with_string_modifiers') \
            .with_plain_string('$1', 'Hello').ascii().wide() \
            .with_plain_string('$2', 'Hello').wide().fullword().nocase() \
            .with_plain_string('$3', 'Hello').xor() \
            .with_plain_string('$4', 'Hello').xor(128) \
            .with_plain_string('$5', 'Hello').wide().xor(1, 255) \
            .get()
        yara_file = self.new_file \
            .with_rule(rule) \
            .get()

        self.assertEqual(yara_file.text_formatted, '''rule rule_with_string_modifiers
{
	strings:
		$1 = "Hello" ascii wide
		$2 = "Hello" wide fullword nocase
		$3 = "Hello" xor
		$4 = "Hello" xor(128)
		$5 = "Hello" wide xor(1-255)
	condition:
		true
}
''')
        self.assertEqual(yara_file.text, '''rule rule_with_string_modifiers {
	strings:
		$1 = "Hello" ascii wide
		$2 = "Hello" wide nocase fullword
		$3 = "Hello" xor
		$4 = "Hello" xor(128)
		$5 = "Hello" wide xor(1-255)
	condition:
		true
}''')
