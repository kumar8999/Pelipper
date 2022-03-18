#include "sortmodel.h"

#include "messagelistmodel.h"

#include <QDateTime>
#include <QDebug>

SortModel::SortModel(QObject *parent)
    : m_loading(false)
    , m_selectionModel(new QItemSelectionModel(this))
    , QSortFilterProxyModel{parent}
{
}

SortModel::~SortModel()
{
    delete m_selectionModel;
}

bool SortModel::loading() const
{
    return m_loading;
}

bool SortModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left, Roles::DateRole);
    QVariant rightData = sourceModel()->data(source_right, Roles::DateRole);

    if (leftData.userType() == QMetaType::QDateTime)
        return leftData.toDateTime() < rightData.toDateTime();

    return false;
}

void SortModel::setLoadingFromModel()
{
    auto _model = static_cast<MessageListModel *>(sourceModel());

    setLoading(_model->loading());
}

void SortModel::setLoading(bool newLoading)
{
    if (m_loading == newLoading)
        return;
    m_loading = newLoading;
    emit loadingChanged();
}

QVariant SortModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
    case Roles::SelectedRole:
        return m_selectionModel->isSelected(index);
        break;
    }

    return QSortFilterProxyModel::data(index, role);
}

void SortModel::setSelected(int indexValue)
{
    if (indexValue < 0)
        return;

    QModelIndex index = this->index(indexValue, 0);
    m_selectionModel->select(index, QItemSelectionModel::Select);
    emit dataChanged(index, index);
    emit selectedItemChanged();
}

void SortModel::toggleSelected(int indexValue)
{
    if (indexValue < 0)
        return;

    QModelIndex index = this->index(indexValue, 0);
    m_selectionModel->select(index, QItemSelectionModel::Toggle);
    emit dataChanged(index, index);
    emit selectedItemChanged();
}

void SortModel::clearSelections()
{
    if (m_selectionModel->hasSelection()) {
        QModelIndexList selectedIndex = m_selectionModel->selectedIndexes();
        m_selectionModel->clear();
        for (auto indexValue : qAsConst(selectedIndex)) {
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

    if (m_selectionModel->hasSelection()) {
        m_selectionModel->clear();
    }

    for (auto index : indexList) {
        m_selectionModel->select(index, QItemSelectionModel::Select);
    }
    emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(QModelIndex()) - 1, 0, QModelIndex()));
    emit selectedItemChanged();
}

bool SortModel::isSelected(int indexValue)
{
    QModelIndex index = this->index(indexValue, 0);
    return m_selectionModel->isSelected(index);
}

QList<QModelIndex> SortModel::selectedIndexes()
{
    return m_selectionModel->selectedIndexes();
}

int SortModel::selectedItemLength()
{
    return m_selectionModel->selectedIndexes().length();
}

void SortModel::setSeenFlag(int indexValue)
{
    if (indexValue < 0)
        return;

    QModelIndex index = this->index(indexValue, 0);
    auto _model = static_cast<MessageListModel *>(sourceModel());
    QModelIndex sourceIndex = mapToSource(index);
    _model->setSeenFlag(sourceIndex.row());

    emit dataChanged(index, index);
}

void SortModel::deleteMessages()
{
    if (m_selectionModel->hasSelection()) {
        QModelIndexList indexList;
        QModelIndexList selectedIndexes = m_selectionModel->selectedIndexes();

        auto _model = static_cast<MessageListModel *>(sourceModel());

        for (auto selectedIndex : selectedIndexes) {
            indexList.append(mapToSource(selectedIndex));
        }

        _model->deleteMessages(indexList);
    }
}

void SortModel::moveMessages(const QString &destFolderName)
{
    if (m_selectionModel->hasSelection()) {
        QModelIndexList indexList;
        QModelIndexList selectedIndexes = m_selectionModel->selectedIndexes();

        auto _model = static_cast<MessageListModel *>(sourceModel());

        for (auto selectedIndex : selectedIndexes) {
            indexList.append(mapToSource(selectedIndex));
        }

        _model->moveMessages(indexList, destFolderName);
    }
}

void SortModel::setModel(QAbstractListModel *model)
{
    setSourceModel(model);
    auto _model = static_cast<MessageListModel *>(model);

    connect(_model, &MessageListModel::loadingChanged, this, &SortModel::setLoadingFromModel);
}
