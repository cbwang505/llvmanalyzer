/**
 * @file src/python/py_visitor.h
 * @brief Declaration of yaramod python bindings for visitors.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <pybind11/pybind11.h>

#include <yaramod/types/expression.h>
#include <yaramod/utils/observing_visitor.h>
#include <yaramod/utils/modifying_visitor.h>

#define PURE_VISIT(type) \
	virtual yaramod::VisitResult visit(yaramod::type* expr) override { \
		PYBIND11_OVERLOAD_PURE_NAME( \
				yaramod::VisitResult, \
				yaramod::Visitor, \
				"visit_"#type, \
				visit, \
				expr \
			); \
	}

class PyVisitor : public yaramod::Visitor
{
public:
	using yaramod::Visitor::Visitor;

	PURE_VISIT(StringExpression)
	PURE_VISIT(StringWildcardExpression)
	PURE_VISIT(StringAtExpression)
	PURE_VISIT(StringInRangeExpression)
	PURE_VISIT(StringCountExpression)
	PURE_VISIT(StringOffsetExpression)
	PURE_VISIT(StringLengthExpression)
	PURE_VISIT(NotExpression)
	PURE_VISIT(UnaryMinusExpression)
	PURE_VISIT(BitwiseNotExpression)
	PURE_VISIT(AndExpression)
	PURE_VISIT(OrExpression)
	PURE_VISIT(LtExpression)
	PURE_VISIT(GtExpression)
	PURE_VISIT(LeExpression)
	PURE_VISIT(GeExpression)
	PURE_VISIT(EqExpression)
	PURE_VISIT(NeqExpression)
	PURE_VISIT(ContainsExpression)
	PURE_VISIT(MatchesExpression)
	PURE_VISIT(PlusExpression)
	PURE_VISIT(MinusExpression)
	PURE_VISIT(MultiplyExpression)
	PURE_VISIT(DivideExpression)
	PURE_VISIT(ModuloExpression)
	PURE_VISIT(BitwiseXorExpression)
	PURE_VISIT(BitwiseAndExpression)
	PURE_VISIT(BitwiseOrExpression)
	PURE_VISIT(ShiftLeftExpression)
	PURE_VISIT(ShiftRightExpression)
	PURE_VISIT(ForIntExpression)
	PURE_VISIT(ForStringExpression)
	PURE_VISIT(OfExpression)
	PURE_VISIT(SetExpression)
	PURE_VISIT(RangeExpression)
	PURE_VISIT(IdExpression)
	PURE_VISIT(StructAccessExpression)
	PURE_VISIT(ArrayAccessExpression)
	PURE_VISIT(FunctionCallExpression)
	PURE_VISIT(BoolLiteralExpression)
	PURE_VISIT(StringLiteralExpression)
	PURE_VISIT(IntLiteralExpression)
	PURE_VISIT(DoubleLiteralExpression)
	PURE_VISIT(FilesizeExpression)
	PURE_VISIT(EntrypointExpression)
	PURE_VISIT(AllExpression)
	PURE_VISIT(AnyExpression)
	PURE_VISIT(ThemExpression)
	PURE_VISIT(ParenthesesExpression)
	PURE_VISIT(IntFunctionExpression)
	PURE_VISIT(RegexpExpression)
};

#define VISIT(parent, type) \
	virtual yaramod::VisitResult visit(yaramod::type* expr) override { \
		PYBIND11_OVERLOAD_NAME( \
				yaramod::VisitResult, \
				yaramod::parent, \
				"visit_"#type, \
				visit, \
				expr \
			); \
	}

class PyObservingVisitor : public yaramod::ObservingVisitor
{
public:
	using yaramod::ObservingVisitor::ObservingVisitor;

	VISIT(ObservingVisitor, StringExpression)
	VISIT(ObservingVisitor, StringWildcardExpression)
	VISIT(ObservingVisitor, StringAtExpression)
	VISIT(ObservingVisitor, StringInRangeExpression)
	VISIT(ObservingVisitor, StringCountExpression)
	VISIT(ObservingVisitor, StringOffsetExpression)
	VISIT(ObservingVisitor, StringLengthExpression)
	VISIT(ObservingVisitor, NotExpression)
	VISIT(ObservingVisitor, UnaryMinusExpression)
	VISIT(ObservingVisitor, BitwiseNotExpression)
	VISIT(ObservingVisitor, AndExpression)
	VISIT(ObservingVisitor, OrExpression)
	VISIT(ObservingVisitor, LtExpression)
	VISIT(ObservingVisitor, GtExpression)
	VISIT(ObservingVisitor, LeExpression)
	VISIT(ObservingVisitor, GeExpression)
	VISIT(ObservingVisitor, EqExpression)
	VISIT(ObservingVisitor, NeqExpression)
	VISIT(ObservingVisitor, ContainsExpression)
	VISIT(ObservingVisitor, MatchesExpression)
	VISIT(ObservingVisitor, PlusExpression)
	VISIT(ObservingVisitor, MinusExpression)
	VISIT(ObservingVisitor, MultiplyExpression)
	VISIT(ObservingVisitor, DivideExpression)
	VISIT(ObservingVisitor, ModuloExpression)
	VISIT(ObservingVisitor, BitwiseXorExpression)
	VISIT(ObservingVisitor, BitwiseAndExpression)
	VISIT(ObservingVisitor, BitwiseOrExpression)
	VISIT(ObservingVisitor, ShiftLeftExpression)
	VISIT(ObservingVisitor, ShiftRightExpression)
	VISIT(ObservingVisitor, ForIntExpression)
	VISIT(ObservingVisitor, ForStringExpression)
	VISIT(ObservingVisitor, OfExpression)
	VISIT(ObservingVisitor, SetExpression)
	VISIT(ObservingVisitor, RangeExpression)
	VISIT(ObservingVisitor, IdExpression)
	VISIT(ObservingVisitor, StructAccessExpression)
	VISIT(ObservingVisitor, ArrayAccessExpression)
	VISIT(ObservingVisitor, FunctionCallExpression)
	VISIT(ObservingVisitor, BoolLiteralExpression)
	VISIT(ObservingVisitor, StringLiteralExpression)
	VISIT(ObservingVisitor, IntLiteralExpression)
	VISIT(ObservingVisitor, DoubleLiteralExpression)
	VISIT(ObservingVisitor, FilesizeExpression)
	VISIT(ObservingVisitor, EntrypointExpression)
	VISIT(ObservingVisitor, AllExpression)
	VISIT(ObservingVisitor, AnyExpression)
	VISIT(ObservingVisitor, ThemExpression)
	VISIT(ObservingVisitor, ParenthesesExpression)
	VISIT(ObservingVisitor, IntFunctionExpression)
	VISIT(ObservingVisitor, RegexpExpression)
};

class PyModifyingVisitor : public yaramod::ModifyingVisitor
{
public:
	using yaramod::ModifyingVisitor::ModifyingVisitor;

	VISIT(ModifyingVisitor, StringExpression)
	VISIT(ModifyingVisitor, StringWildcardExpression)
	VISIT(ModifyingVisitor, StringAtExpression)
	VISIT(ModifyingVisitor, StringInRangeExpression)
	VISIT(ModifyingVisitor, StringCountExpression)
	VISIT(ModifyingVisitor, StringOffsetExpression)
	VISIT(ModifyingVisitor, StringLengthExpression)
	VISIT(ModifyingVisitor, NotExpression)
	VISIT(ModifyingVisitor, UnaryMinusExpression)
	VISIT(ModifyingVisitor, BitwiseNotExpression)
	VISIT(ModifyingVisitor, AndExpression)
	VISIT(ModifyingVisitor, OrExpression)
	VISIT(ModifyingVisitor, LtExpression)
	VISIT(ModifyingVisitor, GtExpression)
	VISIT(ModifyingVisitor, LeExpression)
	VISIT(ModifyingVisitor, GeExpression)
	VISIT(ModifyingVisitor, EqExpression)
	VISIT(ModifyingVisitor, NeqExpression)
	VISIT(ModifyingVisitor, ContainsExpression)
	VISIT(ModifyingVisitor, MatchesExpression)
	VISIT(ModifyingVisitor, PlusExpression)
	VISIT(ModifyingVisitor, MinusExpression)
	VISIT(ModifyingVisitor, MultiplyExpression)
	VISIT(ModifyingVisitor, DivideExpression)
	VISIT(ModifyingVisitor, ModuloExpression)
	VISIT(ModifyingVisitor, BitwiseXorExpression)
	VISIT(ModifyingVisitor, BitwiseAndExpression)
	VISIT(ModifyingVisitor, BitwiseOrExpression)
	VISIT(ModifyingVisitor, ShiftLeftExpression)
	VISIT(ModifyingVisitor, ShiftRightExpression)
	VISIT(ModifyingVisitor, ForIntExpression)
	VISIT(ModifyingVisitor, ForStringExpression)
	VISIT(ModifyingVisitor, OfExpression)
	VISIT(ModifyingVisitor, SetExpression)
	VISIT(ModifyingVisitor, RangeExpression)
	VISIT(ModifyingVisitor, IdExpression)
	VISIT(ModifyingVisitor, StructAccessExpression)
	VISIT(ModifyingVisitor, ArrayAccessExpression)
	VISIT(ModifyingVisitor, FunctionCallExpression)
	VISIT(ModifyingVisitor, BoolLiteralExpression)
	VISIT(ModifyingVisitor, StringLiteralExpression)
	VISIT(ModifyingVisitor, IntLiteralExpression)
	VISIT(ModifyingVisitor, DoubleLiteralExpression)
	VISIT(ModifyingVisitor, FilesizeExpression)
	VISIT(ModifyingVisitor, EntrypointExpression)
	VISIT(ModifyingVisitor, AllExpression)
	VISIT(ModifyingVisitor, AnyExpression)
	VISIT(ModifyingVisitor, ThemExpression)
	VISIT(ModifyingVisitor, ParenthesesExpression)
	VISIT(ModifyingVisitor, IntFunctionExpression)
	VISIT(ModifyingVisitor, RegexpExpression)
};

void addVisitorClasses(pybind11::module& module);

#define PURE_REGEXP_VISIT(type) \
	virtual yaramod::RegexpVisitResult visit(yaramod::type* expr) override { \
		PYBIND11_OVERLOAD_PURE_NAME( \
				yaramod::RegexpVisitResult, \
				yaramod::RegexpVisitor, \
				"visit_"#type, \
				visit, \
				expr \
			); \
	}

class PyRegexpVisitor : public yaramod::RegexpVisitor
{
public:
	using yaramod::RegexpVisitor::RegexpVisitor;

	PURE_REGEXP_VISIT(RegexpClass)
	PURE_REGEXP_VISIT(RegexpText)
	PURE_REGEXP_VISIT(RegexpAnyChar)
	PURE_REGEXP_VISIT(RegexpWordChar)
	PURE_REGEXP_VISIT(RegexpNonWordChar)
	PURE_REGEXP_VISIT(RegexpSpace)
	PURE_REGEXP_VISIT(RegexpNonSpace)
	PURE_REGEXP_VISIT(RegexpDigit)
	PURE_REGEXP_VISIT(RegexpNonDigit)
	PURE_REGEXP_VISIT(RegexpWordBoundary)
	PURE_REGEXP_VISIT(RegexpNonWordBoundary)
	PURE_REGEXP_VISIT(RegexpStartOfLine)
	PURE_REGEXP_VISIT(RegexpEndOfLine)
	PURE_REGEXP_VISIT(RegexpIteration)
	PURE_REGEXP_VISIT(RegexpPositiveIteration)
	PURE_REGEXP_VISIT(RegexpOptional)
	PURE_REGEXP_VISIT(RegexpRange)
	PURE_REGEXP_VISIT(RegexpOr)
	PURE_REGEXP_VISIT(RegexpGroup)
	PURE_REGEXP_VISIT(RegexpConcat)
};

#define REGEXP_VISIT(parent, type) \
	virtual yaramod::RegexpVisitResult visit(yaramod::type* expr) override { \
		PYBIND11_OVERLOAD_NAME( \
				yaramod::RegexpVisitResult, \
				yaramod::parent, \
				"visit_"#type, \
				visit, \
				expr \
			); \
	}

class PyObservingRegexpVisitor : public yaramod::ObservingRegexpVisitor
{
public:
	using yaramod::ObservingRegexpVisitor::ObservingRegexpVisitor;

	REGEXP_VISIT(ObservingRegexpVisitor, RegexpClass)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpText)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpAnyChar)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpWordChar)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpNonWordChar)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpSpace)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpNonSpace)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpDigit)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpNonDigit)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpWordBoundary)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpNonWordBoundary)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpStartOfLine)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpEndOfLine)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpIteration)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpPositiveIteration)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpOptional)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpRange)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpOr)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpGroup)
	REGEXP_VISIT(ObservingRegexpVisitor, RegexpConcat)
};

void addRegexpVisitorClasses(pybind11::module& module);
