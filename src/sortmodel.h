#ifndef SORTMODEL_H
#define SORTMODEL_H

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

class SortModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit SortModel(QObject *parent = nullptr);

    void setSourceModel(QAbstractItemModel *sourceModel);

    QVariant data(const QModelIndex &index, int role) const;

    bool loading() const;

    void setLoading(bool newLoading);

signals:
    void selectedItemChanged();

    void loadingChanged();

public slots:
    void setSelected(int indexValue);
    void toggleSelected(int indexValue);
    void clearSelections();
    void selectAll();

    void deleteMessages();

    void onLoadingChanged();

private:
    QItemSelectionModel *m_SelectionModel;
    bool m_loading;
};

#endif // SORTMODEL_H
