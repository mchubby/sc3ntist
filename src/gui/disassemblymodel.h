#pragma once
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "parser/SCXTypes.h"
#include <vector>
#include <utility>

class SCXFile;
class SC3CodeBlock;
struct DisassemblyRow;

class DisassemblyModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  enum class ColumnType { Breakpoint, Address, Code, NumColumns };
  enum class RowType { Label, Instruction, Comment, Blank };

  explicit DisassemblyModel(const SCXFile* script, QObject* parent = 0);
  ~DisassemblyModel() {}

  const SCXFile* script() const { return _script; }

  QVariant data(const QModelIndex& index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  QModelIndex firstIndexForAddress(SCXOffset address) const;
  int firstLabelForAddress(SCXOffset address) const;
  std::pair<int, int> firstInstructionForAddress(SCXOffset address) const;
  int firstInstructionForAddress(int labelId, SCXOffset address) const;
  SCXOffset addressForIndex(const QModelIndex& index) const;

  bool indexIsLabel(const QModelIndex& index) const;
  const SC3CodeBlock* labelForIndex(const QModelIndex& index) const;
  QModelIndex indexForLabel(SCXOffset labelId) const;

 private slots:
  void onCommentChanged(int fileId, SCXOffset address, const QString& text);
  void onLabelNameChanged(int fileId, int labelId, const QString& name);

 private:
  const SCXFile* _script;
  std::vector<DisassemblyRow> _labelRows;

  void reload();
};

struct DisassemblyRow {
  DisassemblyModel::RowType type;
  int id;
  SCXOffset address;
  std::vector<DisassemblyRow> children;
  DisassemblyRow* parent;
};
