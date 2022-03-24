import sys

import yaramod


class BoolSimplifier(yaramod.ModifyingVisitor):
    def visit_AndExpression(self, expr):
        context = yaramod.TokenStreamContext(expr)

        left_expr = expr.left_operand.accept(self)
        right_expr = expr.right_operand.accept(self)

        left_bool = left_expr if (left_expr and isinstance(left_expr, yaramod.BoolLiteralExpression)) else None
        right_bool = right_expr if (right_expr and isinstance(right_expr, yaramod.BoolLiteralExpression)) else None

        # If both sides of AND are boolean constants then determine the value based on truth table of AND
        # T and T = T
        # T and F = F
        # F and T = F
        # F and F = F
        if left_bool and right_bool:
            output = yaramod.bool_val(left_bool.value and right_bool.value).get()
            expr.exchange_tokens(output)
            return output
        # Only left-hand side is boolean constant
        elif left_bool:
            # F and X = F
            # T and X = X
            output = yaramod.bool_val(False).get() if not left_bool.value else expr.right_operand
            expr.exchange_tokens(output)
            return output
        # Only right-hand side is boolean constant
        elif right_bool:
            # X and F = F
            # X and T = X
            output yaramod.bool_val(False).get() if not right_bool.value else expr.left_operand
            self.cleanUpTokenStreams(context, output)
            return output

        return self.default_handler(context, expr, left_expr, right_expr)

    def visit_OrExpression(self, expr):
        context = yaramod.TokenStreamContext(expr)

        left_expr = expr.left_operand.accept(self)
        right_expr = expr.right_operand.accept(self)

        left_bool = left_expr if (left_expr and isinstance(left_expr, yaramod.BoolLiteralExpression)) else None
        right_bool = right_expr if (right_expr and isinstance(right_expr, yaramod.BoolLiteralExpression)) else None

        # If both sides of OR are boolean constants then determine the value based on truth table of OR
        # T or T = T
        # T or F = T
        # F or T = T
        # F or F = F
        if left_bool and right_bool:
            output = yaramod.bool_val(left_bool.value or right_bool.value).get()
            expr.exchange_tokens(output)
            return output
        # Only left-hand side is boolean constant
        elif left_bool:
            # T or X = T
            # F or X = X
            output = yaramod.bool_val(True).get() if left_bool.value else expr.right_operand
            expr.exchange_tokens(output)
            return output
        # Only right-hand side is boolean constant
        elif right_bool:
            # X or T = T
            # X or F = X
            output = yaramod.bool_val(True).get() if right_bool.value else expr.left_operand
            expr.exchange_tokens(output)
            return output

        return self.default_handler(context, expr, left_expr, right_expr)

    def visit_NotExpression(self, expr):
        context = yaramod.TokenStreamContext(expr)
        new_expr = expr.operand.accept(self)

        # Negate the value of boolean constant
        bool_val = new_expr if (new_expr and isinstance(new_expr, yaramod.BoolLiteralExpression)) else None
        if bool_val:
            output = yaramod.bool_val(not bool_val.value).get()
            expr.exchange_tokens(output)
            return output

        return self.default_handler(context, expr, new_expr)

    def visit_ParenthesesExpression(self, expr):
        context = yaramod.TokenStreamContext(expr)
        new_expr = expr.enclosed_expr.accept(self)

        # Remove parentheses around boolean constants and lift their value up
        bool_val = new_expr if (new_expr and isinstance(new_expr, yaramod.BoolLiteralExpression)) else None
        if bool_val:
            output = yaramod.bool_val(bool_val.value).get()
            expr.exchange_tokens(output)
            return output

        return self.default_handler(context, expr, new_expr)

    def visit_BoolLiteralExpression(self, expr):
        # Lift up boolean value
        output = yaramod.bool_val(expr.value).get()
        expr.exchange_tokens(output)
        return output


def main():
    if len(sys.argv) != 2:
        sys.exit('Usage: {} YARA_FILE'.format(sys.argv[0]))

    simplifier = BoolSimplifier()

    ymod_parser = yaramod.Yaramod()
    yara_file = ymod_parser.parse_file(sys.argv[1])
    for rule in yara_file.rules:
        print('==== RULE: {}'.format(rule.name))
        print('==== BEFORE')
        print(rule.text)
        rule.condition = simplifier.modify(rule.condition, when_deleted=yaramod.bool_val(False).get())
        print('==== AFTER')
        print(rule.text)


if __name__ == '__main__':
    main()
