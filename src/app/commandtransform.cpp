// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include <libldr/metrics.h>
#include <libldr/model.h>

#include "pivotextension.h"

#include "commandtransform.h"

namespace Konstruktor
{

CommandTransform::CommandTransform(const ldraw::matrix &premult,
                                   const ldraw::matrix &postmult,
                                   const QSet<int> &selection,
                                   ldraw::model *model,
                                   Editor::RotationPivot pivotMode,
                                   const ldraw::vector &pivot)
    : CommandBase(selection, model)
{
  setText(QObject::tr("Transform"));
  
  premult_ = premult;
  postmult_ = postmult;
  pivotMode_ = pivotMode;
  pivot_ = pivot;
  
  for (QSet<int>::ConstIterator it = selection.constBegin(); it != selection.constEnd(); ++it) {
    if (model->elements()[*it]->get_type() == ldraw::type_ref)
      oldmatrices_[*it] = CAST_AS_CONST_REF(model->elements()[*it])->get_matrix();
  }
}

CommandTransform::CommandTransform(const QSet<int> &selection,
                                   ldraw::model *model,
                                   Editor::RotationPivot pivotMode,
                                   const ldraw::vector &pivot)
    : CommandBase(selection, model)
{
  setText(QObject::tr("Transform"));
  
  pivotMode_ = pivotMode;
  pivot_ = pivot;
  
  for (QSet<int>::ConstIterator it = selection.constBegin(); it != selection.constEnd(); ++it) {
    if (model->elements()[*it]->get_type() == ldraw::type_ref)
      oldmatrices_[*it] = CAST_AS_CONST_REF(model->elements()[*it])->get_matrix();
  }
}

CommandTransform::~CommandTransform()
{
  
}

bool CommandTransform::needUpdateDimension() const
{
  return true;
}

void CommandTransform::redo()
{
  for (QSet<int>::ConstIterator it = selection_.constBegin(); it != selection_.constEnd(); ++it) {
    if (model_->elements()[*it]->get_type() == ldraw::type_ref) {
      ldraw::element_ref *r = CAST_AS_REF(model_->elements()[*it]);
      ldraw::matrix cmat;
      
      if (!r->get_model())
        continue;
      
      if (pivotMode_ != Editor::PivotEach) {
        ldraw::matrix pretrans, posttrans;
        pretrans.set_translation_vector(pivot_);
        posttrans.set_translation_vector(-pivot_);
	
        cmat = pretrans * premult_ * postmult_ * posttrans * r->get_matrix();
      } else {
        cmat = premult_ * r->get_matrix() * postmult_;
      }
      
      for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
          float e = cmat.value(i, j);
          if (std::fabs(std::fabs(e) - 1.0f) < LDR_EPSILON)
            cmat.value(i, j) = e > 0.0f ? 1.0f:-1.0f;
          else if (std::fabs(e) < LDR_EPSILON)
            cmat.value(i, j) = 0.0f;
          else if (std::fabs(std::fabs(e) - 0.707106f) < LDR_EPSILON)
            cmat.value(i, j) = e > 0.0f ? 0.707106f:-0.707106f;
          else if (std::fabs(std::fabs(e) - 0.5f) < LDR_EPSILON)
            cmat.value(i, j) = e > 0.0f ? 0.5f:-0.5f;
          else if (std::fabs(std::fabs(e) - 0.866025f) < LDR_EPSILON)
            cmat.value(i, j) = e > 0.0f ? 0.866025f:-0.866025f;
        }
      }
      
      for (int i = 0; i < 3; ++i) {
        float v = cmat.value(i, 3);
        float fmod = std::fmod(v, 1.0f);
	
        if (std::fabs(fmod) < LDR_EPSILON) {
          if (fmod < 0.0f)
            cmat.value(i, 3) = std::ceil(v);
          else
            cmat.value(i, 3) = std::floor(v);
        }
      }
      
      r->set_matrix(cmat);
    }
  }
}

void CommandTransform::undo()
{
  for (QSet<int>::ConstIterator it = selection_.constBegin(); it != selection_.constEnd(); ++it) {
    if (model_->elements()[*it]->get_type() == ldraw::type_ref)
      CAST_AS_REF(model_->elements()[*it])->set_matrix(oldmatrices_[*it]);
  }
}

}
