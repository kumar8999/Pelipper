#include "sortmodel.h"

#include "messagelistmodel.h"

#include <QDateTime>
#include <QDebug>

SortModel::SortModel(QObject *parent)
    : m_loading(false)
    , QSortFilterProxyModel{parent}
{
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

void SortModel::setModel(QAbstractListModel *model)
{
    setSourceModel(model);
    auto _model = static_cast<MessageListModel *>(model);

    connect(_model, &MessageListModel::loadingChanged, this, &SortModel::setLoadingFromModel);
}
