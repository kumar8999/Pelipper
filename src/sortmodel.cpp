#include "sortmodel.h"

#include "messagelistmodel.h"
#include "roles.h"

SortModel::SortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_SelectionModel = new QItemSelectionModel(this);
}

void SortModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

void SortModel::setSelected(int indexValue)
{
    if (indexValue < 0)
        return;

    QModelIndex index = this->index(indexValue, 0);
    m_SelectionModel->select(index, QItemSelectionModel::Select);
    emit dataChanged(index, index);
    emit selectedItemChanged();
}

void SortModel::toggleSelected(int indexValue)
{
    if (indexValue < 0)
        return;

    QModelIndex index = this->index(indexValue, 0);
    m_SelectionModel->select(index, QItemSelectionModel::Toggle);
    emit dataChanged(index, index);
    emit selectedItemChanged();
}

void SortModel::clearSelections()
{
    if (m_SelectionModel->hasSelection()) {
        QModelIndexList selectedIndex = m_SelectionModel->selectedIndexes();
        m_SelectionModel->clear();
        for (auto indexValue : selectedIndex) {
            emit dataChanged(indexValue, indexValue);
        }
    }
    emit selectedItemChanged();
}

void SortModel::selectAll()
{
    QModelIndexList indexList;
    for (int row = 0; row < rowCount(QModelIndex()); row++) {
        indexList.append(index(row, 0, QModelIndex()));
    }

    if (m_SelectionModel->hasSelection()) {
        m_SelectionModel->clear();
    }

    for (auto index : indexList) {
        m_SelectionModel->select(index, QItemSelectionModel::Select);
    }
    emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(QModelIndex()) - 1, 0, QModelIndex()));
    emit selectedItemChanged();
}

void SortModel::deleteMessages()
{
    MessageListModel *model = static_cast<MessageListModel *>(sourceModel());

    model->deleteMessages(m_SelectionModel->selectedIndexes());
}

void SortModel::onLoadingChanged()
{
    MessageListModel *model = static_cast<MessageListModel *>(sourceModel());

    bool loading = model->loading();
    setLoading(loading);
}

QVariant SortModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
    case Roles::SelectedRole:
        return m_SelectionModel->isSelected(index);
        break;
    }

    return QSortFilterProxyModel::data(index, role);
}

bool SortModel::loading() const
{
    return m_loading;
}

void SortModel::setLoading(bool newLoading)
{
    if (m_loading == newLoading) {
        return;
    }

    m_loading = newLoading;
    emit loadingChanged();
}
