// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QBrush>
#include <QIcon>

#include <libldr/bfc.h>
#include <libldr/elements.h>
#include <libldr/model.h>

#include "colormanager.h"
#include "document.h"
#include "application.h"
#include "utils.h"
#include "visibilityextension.h"

#include "contentsmodel.h"

namespace Konstruktor
{

ContentsModel::ContentsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
  model_ = 0L;
  checkTable_ = 0L;
  
  connect(this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
}

ContentsModel::~ContentsModel()
{
  
}

void ContentsModel::setDocument(Document *document)
{
  if (document == 0L) {
    model_ = 0L;
    checkTable_ = 0L;
  } else {
    model_ = document->getActiveModel();
    checkTable_ = VisibilityExtension::query(model_);
  }
  
  reset();
}

int ContentsModel::rowCount(const QModelIndex &parent) const
{
  if (model_ && !parent.isValid())
    return model_->elements().size();
  else
    return 0;
}

QVariant ContentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case ColumnIndex:
          return tr("No.");
        case ColumnObjectType:
          return tr("Type");
        case ColumnPosition:
          return tr("Position");
        case ColumnData:
          return tr("Data");
        default:
          return QVariant();
      }
    } else if (role == Qt::DecorationRole && section == ColumnCheck) {
      return Utils::icon("edit-delete");
    }
  }
  
  return QVariant();
}

QVariant ContentsModel::data(const QModelIndex &index, int role) const
{
  ldraw::element_base *b = model_->elements()[index.row()];
  
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case ColumnIndex:
        return QVariant(QString::number(index.row()));
      case ColumnObjectType:
        switch (b->get_type()) {
          case ldraw::type_comment:
            return tr("Comment");
          case ldraw::type_state:
            return tr("Meta");
          case ldraw::type_print:
            return tr("Print");
          case ldraw::type_ref:
            {
              const ldraw::element_ref *cref = CAST_AS_CONST_REF(b);
              if (!cref->get_model())
                return tr("Unresolved");
              else if (cref->get_model()->modeltype() == ldraw::model::submodel)
                return tr("Submodel");
              else if (cref->get_model()->modeltype() == ldraw::model::external_file)
                return tr("File");
              else
                return tr("Part");
            }
          case ldraw::type_line:
            return tr("Line");
          case ldraw::type_triangle:
            return tr("Triangle");
          case ldraw::type_quadrilateral:
            return tr("Quadrilateral");
          case ldraw::type_condline:
            return tr("Conditional Line");
          case ldraw::type_bfc:
            return tr("BFC Control");
          default:
            return QVariant();
        }
      case ColumnPosition:
        if (b->get_type() == ldraw::type_ref) {
          const ldraw::matrix &matrix = CAST_AS_CONST_REF(b)->get_matrix();
          return tr("%1, %2, %3").arg(matrix.value(0, 3)).arg(matrix.value(1, 3)).arg(matrix.value(2, 3));
        } else {
          return tr("-");
        }
      case ColumnData:
        switch (b->get_type()) {
          case ldraw::type_comment:
            {
              const ldraw::element_comment *e = CAST_AS_CONST_COMMENT(b);
              return e->get_comment().c_str();
            }
          case ldraw::type_state:
            {
              const ldraw::element_state *e = CAST_AS_CONST_STATE(b);
              switch (e->get_state()) {
                case ldraw::element_state::state_step:
                  return tr("Step");
                case ldraw::element_state::state_pause:
                  return tr("Pause");
                case ldraw::element_state::state_clear:
                  return tr("Clear");
                case ldraw::element_state::state_save:
                  return tr("Pause");
                default:
                  return QVariant();
              }
            }
          case ldraw::type_print:
            {
              const ldraw::element_print *e = CAST_AS_CONST_PRINT(b);
              return e->get_string().c_str();
            }
          case ldraw::type_ref:
            {
              const ldraw::element_ref *e = CAST_AS_CONST_REF(b);
              return tr("%1 (%2)").arg(e->filename().c_str(), e->get_model() ? e->get_model()->desc().c_str() : tr("Unlinked"));
            }
          case ldraw::type_line:
            {
              const ldraw::element_line *e = CAST_AS_CONST_LINE(b);
              return tr("(%1, %2, %3) - (%4, %5, %6)").arg(e->pos1().x()).arg(e->pos1().y()).arg(e->pos1().z()).arg(e->pos2().x()).arg(e->pos2().y()).arg(e->pos2().z());
					}
          case ldraw::type_triangle:
            {
              const ldraw::element_triangle *e = CAST_AS_CONST_TRIANGLE(b);
              return tr("(%1, %2, %3) - (%4, %5, %6) - (%7, %8, %9)").arg(e->pos1().x()).arg(e->pos1().y()).arg(e->pos1().z()).arg(e->pos2().x()).arg(e->pos2().y()).arg(e->pos2().z()).arg(e->pos3().x()).arg(e->pos3().y()).arg(e->pos3().z());
            }
          case ldraw::type_quadrilateral:
            {
              const ldraw::element_quadrilateral *e = CAST_AS_CONST_QUADRILATERAL(b);
              return tr("(%1, %2, %3) - (%4, %5, %6)").arg(e->pos1().x()).arg(e->pos1().y()).arg(e->pos1().z()).arg(e->pos2().x()).arg(e->pos2().y()).arg(e->pos2().z()) + tr(" - (%1, %2, %3) - (%4, %5, %6)").arg(e->pos3().x()).arg(e->pos3().y()).arg(e->pos3().z()).arg(e->pos4().x()).arg(e->pos4().y()).arg(e->pos4().z());
            }
          case ldraw::type_condline:
            {
              const ldraw::element_condline *e = CAST_AS_CONST_CONDLINE(b);
              return tr("Line: (%1, %2, %3) - (%4, %5, %6)").arg(e->pos1().x()).arg(e->pos1().y()).arg(e->pos1().z()).arg(e->pos2().x()).arg(e->pos2().y()).arg(e->pos2().z()) + tr(", Guide: (%1, %2, %3) - (%4, %5, %6)").arg(e->pos3().x()).arg(e->pos3().y()).arg(e->pos3().z()).arg(e->pos4().x()).arg(e->pos4().y()).arg(e->pos4().z());
            }
          case ldraw::type_bfc:
            {
              ldraw::element_bfc::command c = CAST_AS_BFC(b)->get_command();
              if (c == ldraw::element_bfc::noclip)
                return tr("No Clip");
              else if (c == ldraw::element_bfc::invertnext)
                return tr("Invert Next");
              
              return QString("%1%2").arg(tr(c & ldraw::element_bfc::clip ? "Clip, " : "")).arg(tr(c & ldraw::element_bfc::cw ? "Clockwise" : "Counter-Clockwise"));
            }
          default:
            return QVariant();
        }
      default:
        return QVariant();
    }
  } else if (role == Qt::BackgroundRole) {
    switch (b->get_type()) {
      case ldraw::type_comment:
        if (Application::self()->config()->listCommentColorize())
          return QBrush(Application::self()->config()->listCommentColor());
        else
          return QVariant();
      case ldraw::type_state:
      case ldraw::type_print:
        if (Application::self()->config()->listMetaColorize())
          return QBrush(Application::self()->config()->listMetaColor());
        else
          return QVariant();
      case ldraw::type_ref:
        if (Application::self()->config()->listRefColorize()) {
          const ldraw::element_ref *e = CAST_AS_CONST_REF(b);
          if (!e->get_model())
            return QBrush(Application::self()->config()->listUnresolvedColor());
          else if (e->get_model()->is_submodel_of(model_->parent()))
            return QBrush(Application::self()->config()->listSubmodelColor());
          else
            return QBrush(Application::self()->config()->listPartColor());
        } else {
          return QVariant();
        }
      case ldraw::type_line:
      case ldraw::type_triangle:
      case ldraw::type_quadrilateral:
      case ldraw::type_condline:
        if (Application::self()->config()->listPrimitiveColorize())
          return QBrush(Application::self()->config()->listPrimitiveColor());
        else
          return QVariant();
      default:
        return QVariant();
    }
  } else if (role == Qt::TextAlignmentRole) {
    if (index.column() == ColumnIndex || index.column() == ColumnObjectType)
      return Qt::AlignHCenter;
  } else if (role == Qt::DecorationRole) {
    if (index.column() == ColumnData && model_->elements()[index.row()]->capabilities() & ldraw::capability_color) {
      const ldraw::color &c = dynamic_cast<const ldraw::element_colored_base *>(model_->elements()[index.row()])->get_color();
      return ColorManager::colorPixmap(c);
    }
  } else if (role == Qt::CheckStateRole) {
    if (index.column() == ColumnCheck) {
      if (model_->elements()[index.row()]->line_type() == 0)
        return QVariant();
      else if (checkTable_->find(index.row()))
        return Qt::Checked;
      else
        return Qt::Unchecked;
    }
  }
  
  return QVariant();
}

bool ContentsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.column() == ColumnCheck && role == Qt::CheckStateRole) {
    if (model_->elements()[index.row()]->line_type() == 0)
      return false;
    
    int checkstate = value.toInt();
    
    if (checkstate == Qt::Unchecked) {
      checkTable_->remove(index.row());
      emit unhide(index);
    } else if (checkstate == Qt::Checked) {
      checkTable_->insert(index.row());
      emit hide(index);
    }
    
    emit dataChanged(index, ContentsModel::index(index.row(), ColumnCount - 1));
    
    return true;
  }
  
  return false;
}

QModelIndex ContentsModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return QModelIndex();
  } else {
    // not sure if this is correct
    if (!model_)
      return createIndex(row, column); 
    if (column == ColumnCheck)
      return createIndex(row, column, (void *)model_->elements()[row]);
    else
      return createIndex(row, column);
  }
}

Qt::ItemFlags ContentsModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = Qt::ItemIsEnabled;
  
  if (!checkTable_->find(index.row()))
    flags |= Qt::ItemIsSelectable;
  
  if (index.column() == ColumnCheck && model_->elements()[index.row()]->get_type() == ldraw::type_ref)
    flags |= Qt::ItemIsUserCheckable;
  
  return flags;
}

void ContentsModel::rowsChanged(const CommandBase::AffectedRowInfo &/*rowList*/)
{
  // FIXME: do not force reset
  reset();
  
  /*if (!inverse) {
    for (int i = 0; i < rowList.size(); ++i) {
      int row = rowList[i].first;
      if (row == -1)
        row = rowCount();
      
      if (rowList[i].second == CommandBase::Inserted) {
        insertRow(row);
      } else {
        removeRow(row);
      }
    }
  } else {
    for (int i = rowList.size() - 1; i >= 0; --i) {
      int row = rowList[i].first;
      if (row == -1)
        row = rowCount();
      
      if (rowList[i].second == CommandBase::Inserted) {
        removeRow(row);
      } else {
        insertRow(row);
      }
    }
  }*/
}

void ContentsModel::hideSelected(const QSet<int> &selection)
{
  foreach (int i, selection) {
    if (model_->elements()[i]->line_type() == 0)
      continue;
    
    if (!checkTable_->find(i))
      setData(index(i, 0), QVariant(Qt::Checked), Qt::CheckStateRole);
  }
}

void ContentsModel::unhideAll()
{
  const std::set<int> sset = checkTable_->set();
  
  for (std::set<int>::const_iterator it = sset.begin(); it != sset.end(); ++it)
    setData(index(*it, 0), QVariant(Qt::Unchecked), Qt::CheckStateRole);
  
  checkTable_->clear();
}

void ContentsModel::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
  // If visibility column has changed
  if (topLeft.column() == ColumnIndex || bottomRight.column() == ColumnIndex)
    emit viewChanged();
}

}
