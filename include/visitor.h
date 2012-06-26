// Copyright 2012, Alex Horn. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef VISITOR_H_
#define VISITOR_H_

namespace se {

// Forward declarations of the kind of vertices the Visitor API must be able
// to traverse in the DAG. These must match the expression kinds in ExprKind.
template<typename T>
class AnyExpr;

template<typename T>
class ValueExpr;

class CastExpr;
class UnaryExpr;
class TernaryExpr;
class NaryExpr;

// Since virtual template functions are not allowed (because there is no bound
// on the number of possibilities for which the vtable would need to account),
// we declare the permissible return types as traits.
template<typename T> struct VisitorTraits;

#define VISITOR_TRAIT_DECL(type)\
template<>\
struct VisitorTraits<type> {\
  typedef type ReturnType;\
};

VISITOR_TRAIT_DECL(void)
VISITOR_TRAIT_DECL(z3::expr)

// Visitor is an interface to traverse an acyclic directed graph (DAG) that
// represents the syntactic structure of an expression. The order in which a
// Visitor's visit member functions is called and the argument of that call is
// determined by the tree traversal algorithm.
//
// Only Expr subclasses that implement the Walker interface can be visited.
template<typename T>
class Visitor {

#define TYPED_VISIT_DECL(type)\
  virtual ReturnType visit(const AnyExpr<type>&) = 0;\
  virtual ReturnType visit(const ValueExpr<type>&) = 0;

public:

  // ReturnType declares the type that each visit member function returns.
  typedef typename VisitorTraits<T>::ReturnType ReturnType;

  TYPED_VISIT_DECL(bool)
  TYPED_VISIT_DECL(char)
  TYPED_VISIT_DECL(short int)
  TYPED_VISIT_DECL(int)

  virtual ReturnType visit(const CastExpr&) = 0;
  virtual ReturnType visit(const UnaryExpr&) = 0;
  virtual ReturnType visit(const TernaryExpr&) = 0;
  virtual ReturnType visit(const NaryExpr&) = 0;

  virtual ~Visitor() {}
};

// Walker is an interface that must be implemented by every Expr subclass.
// The easiest to achieve this is for the Expr class to inherit Walker and
// each Expr subclass to use the WALK_DEF macro inside its class definition.
//
// Example:
//
// class UnaryExpr : public Expr {
//   public:
//   ...
//   WALK_DEF(T1)
//   WALK_DEF(T2)
//   ...
//   WALK_DEF(TN)
// }
//
// where T1 ... TN are types that have registered with VISITOR_TRAIT_DECL.
class Walker {

public:

// Declare a public virtual walk member function that dispatches a
// constant reference to the current polymorphic object by calling the
// visit member function on the supplied Visitor<type> object. The type
// argument must be one of the traits declared with VISITOR_TRAIT_DECL.
#define WALK_DECL(type)\
  virtual typename VisitorTraits<type>::ReturnType\
    walk(Visitor<typename VisitorTraits<type>::ReturnType>* const) const = 0;

// Define member function that has been declared with WALK_DECL.
// Only Expr subclasses are allowed to use this macro.
#define WALK_DEF(type)\
  typename VisitorTraits<type>::ReturnType\
    walk(Visitor<typename VisitorTraits<type>::ReturnType>* const v_ptr) const { return v_ptr->visit(*this); }

  WALK_DECL(void)
  WALK_DECL(z3::expr)

};

}

#endif /* VISITOR_H_ */