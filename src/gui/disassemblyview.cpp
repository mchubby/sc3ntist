#include "disassemblymodel.h"
#include "disassemblyview.h"
#include "disassemblyitemdelegate.h"
#include "project.h"
#include "debuggerapplication.h"

DisassemblyView::DisassemblyView(QWidget* parent) : QTreeView(parent) {
  connect(dApp, &DebuggerApplication::projectOpened, this,
          &DisassemblyView::onProjectOpened);
  connect(dApp, &DebuggerApplication::projectClosed, this,
          &DisassemblyView::onProjectClosed);

  connect(header(), &QHeaderView::sectionCountChanged, this,
          &DisassemblyView::adjustHeader);

  header()->setSectionsMovable(false);
  header()->setSectionsClickable(false);

  setUniformRowHeights(true);
  setItemsExpandable(false);
  setWordWrap(false);
  setIndentation(0);
  setRootIsDecorated(false);

  setItemDelegate(new DisassemblyItemDelegate(this));
}

void DisassemblyView::setModel(QAbstractItemModel* model) {
  QTreeView::setModel(model);

  expandAll();
  scrollToTop();
}

void DisassemblyView::goToAddress(SCXOffset address) {
  const DisassemblyModel* disModel = qobject_cast<DisassemblyModel*>(model());
  if (disModel == nullptr) return;

  QModelIndex index = disModel->firstIndexForAddress(address);
  setCurrentIndex(index);
  // in case the line was already selected, still scroll there
  scrollTo(index);
}

void DisassemblyView::adjustHeader(int oldCount, int newCount) {
  if (newCount != (int)DisassemblyModel::ColumnType::NumColumns) return;
  header()->setSectionResizeMode((int)DisassemblyModel::ColumnType::Breakpoint,
                                 QHeaderView::Fixed);
  // TODO: make breakpoint bounding rectangle square
  header()->resizeSection((int)DisassemblyModel::ColumnType::Breakpoint, 24);
  header()->setSectionResizeMode((int)DisassemblyModel::ColumnType::Address,
                                 QHeaderView::ResizeToContents);
}

void DisassemblyView::onProjectOpened() {
  connect(dApp->project(), &Project::fileSwitched, this,
          &DisassemblyView::onFileSwitched);
}

void DisassemblyView::onProjectClosed() {
  QAbstractItemModel* oldModel = model();
  setModel(nullptr);
  if (oldModel != nullptr) delete oldModel;
}

void DisassemblyView::onFileSwitched(int previousId) {
  QAbstractItemModel* oldModel = model();
  setModel(new DisassemblyModel(dApp->project()->currentFile()));
  if (oldModel != nullptr) delete oldModel;
}