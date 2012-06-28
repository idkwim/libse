#include <sstream>
#include "gtest/gtest.h"
#include "expr.h"

using namespace se;

// Simple tree postorder traversal to compile-time check Visitor API.
class PostorderVisitor : public Visitor<void> {

public:

  std::stringstream out;

  PostorderVisitor() : out() {}

  void visit(const Expr& expr) { expr.write(out); out << "Wrote Other!"; };
  void visit(const AnyExpr<bool>& expr) { expr.write(out); }
  void visit(const ValueExpr<bool>& expr) { expr.write(out); }
  void visit(const AnyExpr<char>& expr) { expr.write(out); }
  void visit(const ValueExpr<char>& expr) { expr.write(out); }
  void visit(const AnyExpr<short int>& expr) { expr.write(out); }
  void visit(const ValueExpr<short int>& expr) { expr.write(out); }
  void visit(const AnyExpr<int>& expr) { expr.write(out); }
  void visit(const ValueExpr<int>& expr) { expr.write(out); }

  void visit(const CastExpr& expr) {
    expr.get_expr()->walk(this);
    out << types[expr.get_type()];
  }

  void visit(const UnaryExpr& expr) {
    expr.get_expr()->walk(this);
    out << operators[expr.get_op()];
  }

  void visit(const TernaryExpr& expr) {
    expr.get_cond_expr()->walk(this);
    expr.get_then_expr()->walk(this);
    expr.get_else_expr()->walk(this);
  }

  void visit(const NaryExpr& expr) {
    for(SharedExpr operand_expr : expr.get_exprs()) {
      operand_expr->walk(this);
    }
    out << operators[expr.get_op()];
  }

};

TEST(ExprTest, PostorderVisit) {
  const SharedExpr a = SharedExpr(new AnyExpr<int>("A"));
  const SharedExpr b = SharedExpr(new ValueExpr<short>(5));
  const SharedExpr lss = SharedExpr(new NaryExpr(LSS, OperatorTraits<LSS>::attr, a, b));
  const SharedExpr neg = SharedExpr(new UnaryExpr(NOT, lss));
  const SharedExpr c = SharedExpr(new AnyExpr<int>("C"));
  const SharedExpr cast = SharedExpr(new CastExpr(CHAR, c));

  NaryExpr* const nary = new NaryExpr(ADD, OperatorTraits<ADD>::attr);
  nary->append_expr(SharedExpr(new AnyExpr<int>("D")));
  nary->append_expr(SharedExpr(new AnyExpr<int>("E")));
  nary->append_expr(SharedExpr(new AnyExpr<int>("F")));

  const SharedExpr ternary = SharedExpr(new TernaryExpr(neg, cast, SharedExpr(nary)));

  PostorderVisitor postorder_visitor;
  ternary->walk(&postorder_visitor);

  EXPECT_EQ("[A]5<![C]char[D][E][F]+", postorder_visitor.out.str());
}

class OtherExpr : public Expr {
public:

  OtherExpr() : Expr(ext_expr_kind(1u)) {}

  std::ostream& write(std::ostream& out) const { out << "Other!"; };
};

TEST(ExprTest, OtherVisit) {
  OtherExpr other;

  PostorderVisitor postorder_visitor;
  other.walk(&postorder_visitor);

  EXPECT_EQ("Other!Wrote Other!", postorder_visitor.out.str());
}

