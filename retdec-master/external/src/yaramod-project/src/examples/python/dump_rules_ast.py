import sys

import yaramod


class Dumper(yaramod.ObservingVisitor):
    def __init__(self):
        super(Dumper, self).__init__()
        self.indent = 0

    def visit_StringExpression(self, expr):
        self.dump('String', expr, ' id=', expr.id)

    def visit_StringWildcardExpression(self, expr):
        self.dump('StringWildcard', expr, ' id=', expr.id)

    def visit_StringAtExpression(self, expr):
        self.dump('StringAt', expr, ' id=', expr.id)
        self.indent_up()
        expr.at_expr.accept(self)
        self.indent_down()

    def visit_StringInRangeExpression(self, expr):
        self.dump('StringInRange', expr, ' id=', expr.id)
        self.indent_up()
        expr.range_expr.accept(self)
        self.indent_down()

    def visit_StringCountExpression(self, expr):
        self.dump('StringCount', expr, ' id=', expr.id)

    def visit_StringOffsetExpression(self, expr):
        self.dump('StringOffset', expr, ' id=', expr.id)
        if expr.index_expr:
            self.indent_up()
            expr.index_expr.accept(self)
            self.indent_down()

    def visit_StringLengthExpression(self, expr):
        self.dump('StringLength', expr, ' id=', expr.id)
        if expr.index_expr:
            self.indent_up()
            expr.index_expr.accept(self)
            self.indent_down()

    def visit_NotExpression(self, expr):
        self.dump('Not', expr)
        self.indent_up()
        expr.operand.accept(self)
        self.indent_down()

    def visit_UnaryMinusExpression(self, expr):
        self.dump('UnaryMinus', expr)
        self.indent_up()
        expr.operand.accept(self)
        self.indent_down()

    def visit_BitwiseNotExpression(self, expr):
        self.dump('BitwiseNot', expr)
        self.indent_up()
        expr.operand.accept(self)
        self.indent_down()

    def visit_AndExpression(self, expr):
        self.dump('And', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_OrExpression(self, expr):
        self.dump('Or', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_LtExpression(self, expr):
        self.dump('Lt', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_GtExpression(self, expr):
        self.dump('Gt', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_LeExpression(self, expr):
        self.dump('Le', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_GeExpression(self, expr):
        self.dump('Ge', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_EqExpression(self, expr):
        self.dump('Eq', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_NeqExpression(self, expr):
        self.dump('Neq', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_ContainsExpression(self, expr):
        self.dump('Contains', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_MatchesExpression(self, expr):
        self.dump('Matches', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_PlusExpression(self, expr):
        self.dump('Plus', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_MinusExpression(self, expr):
        self.dump('Minus', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_MultiplyExpression(self, expr):
        self.dump('Multiply', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_DivideExpression(self, expr):
        self.dump('Divide', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_ModuloExpression(self, expr):
        self.dump('Modulo', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_BitwiseXorExpression(self, expr):
        self.dump('BitwiseXor', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_BitwiseAndExpression(self, expr):
        self.dump('BitwiseAnd', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_BitwiseOrExpression(self, expr):
        self.dump('BitwiseOr', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_ShiftLeftExpression(self, expr):
        self.dump('ShiftLeft', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_ShiftRightExpression(self, expr):
        self.dump('ShiftRight', expr)
        self.indent_up()
        expr.left_operand.accept(self)
        expr.right_operand.accept(self)
        self.indent_down()

    def visit_ForIntExpression(self, expr):
        self.dump('ForInt', expr, ' id=', expr.id)
        self.indent_up()
        expr.variable.accept(self)
        expr.iterated_set.accept(self)
        expr.body.accept(self)
        self.indent_down()

    def visit_ForStringExpression(self, expr):
        self.dump('ForString', expr)
        self.indent_up()
        expr.variable.accept(self)
        expr.iterated_set.accept(self)
        expr.body.accept(self)
        self.indent_down()

    def visit_OfExpression(self, expr):
        self.dump('Of', expr)
        self.indent_up()
        expr.variable.accept(self)
        expr.iterated_set.accept(self)
        self.indent_down()

    def visit_SetExpression(self, expr):
        self.dump('Set', expr)
        self.indent_up()
        for elem in expr.elements:
            elem.accept(self)
        self.indent_down()

    def visit_RangeExpression(self, expr):
        self.dump('Range', expr)
        self.indent_up()
        expr.low.accept(self)
        expr.high.accept(self)
        self.indent_down()

    def visit_IdExpression(self, expr):
        self.dump('Id', expr, ' id=', expr.symbol.name)

    def visit_StructAccessExpression(self, expr):
        self.dump('StructAccess', expr, ' id=', expr.symbol.name)
        self.indent_up()
        expr.structure.accept(self)
        self.indent_down()

    def visit_ArrayAccessExpression(self, expr):
        self.dump('ArrayAccess', expr, ' id=', expr.symbol.name)
        self.indent_up()

        self.dump('[array]', expr)
        self.indent_up()
        expr.array.accept(self)
        self.indent_down()

        self.dump('[accessor]', expr)
        self.indent_up()
        expr.accessor.accept(self)
        self.indent_down()

        self.indent_down()

    def visit_FunctionCallExpression(self, expr):
        self.dump('FunctionCall', expr, ' args_count=', len(expr.arguments))
        self.indent_up()

        self.dump('[symbol]', expr)
        self.indent_up()
        expr.function.accept(self)
        self.indent_down()

        self.dump('[args]', expr)
        self.indent_up()
        for arg in expr.arguments:
            arg.accept(self)
        self.indent_down()

        self.indent_down()

    def visit_BoolLiteralExpression(self, expr):
        self.dump('BoolLiteral', expr, ' value=', expr.text)

    def visit_StringLiteralExpression(self, expr):
        self.dump('StringLiteral', expr, ' value=', expr.text)

    def visit_IntLiteralExpression(self, expr):
        self.dump('IntLiteral', expr, ' value=', expr.text)

    def visit_DoubleLiteralExpression(self, expr):
        self.dump('DoubleLiteral', expr, ' value=', expr.text)

    def visit_FilesizeExpression(self, expr):
        self.dump('Filesize', expr)

    def visit_EntrypointExpression(self, expr):
        self.dump('Entrypoint', expr)

    def visit_AllExpression(self, expr):
        self.dump('All', expr)

    def visit_AnyExpression(self, expr):
        self.dump('Any', expr)

    def visit_ThemExpression(self, expr):
        self.dump('Them', expr)

    def visit_ParenthesesExpression(self, expr):
        self.dump('Parentheses', expr)
        self.indent_up()
        expr.enclosed_expr.accept(self)
        self.indent_down()

    def visit_IntFunctionExpression(self, expr):
        self.dump('IntFunction', expr, ' function=', expr.function)
        self.indent_up()
        expr.argument.accept(self)
        self.indent_down()

    def visit_RegexpExpression(self, expr):
        self.dump('Regexp', expr, ' text=', expr.regexp_string.pure_text)

    def indent_up(self):
        self.indent += 4

    def indent_down(self):
        self.indent -= 4

    def dump(self, name, expr, *args):
        print('{}{}[0x{:x}] {}'.format(' ' * self.indent, name, id(expr), ''.join([str(arg) for arg in args])))


def main():
    if len(sys.argv) != 2:
        sys.exit('Usage: {} YARA_FILE'.format(sys.argv[0]))

    dumper = Dumper()

    ymod_parser = yaramod.Yaramod()
    yara_file = ymod_parser.parse_file(sys.argv[1])
    for rule in yara_file.rules:
        print('==== RULE: {}'.format(rule.name))
        dumper.observe(rule.condition)


if __name__ == '__main__':
    main()
