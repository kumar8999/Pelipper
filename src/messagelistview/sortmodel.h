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

    ~SortModel();

    bool loading() const;

    void setModel(QAbstractListModel *model);

    void setLoading(bool newLoading);

    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void setSelected(int indexValue);

    void toggleSelected(int indexValue);

    void clearSelections();

    void selectAll();

    bool isSelected(int index);

    QList<QModelIndex> selectedIndexes();

    int selectedItemLength();

    void setSeenFlag(int indexValue);

    void deleteMessages();

    void moveMessages(const QString &destFolderName);

signals:
    void loadingChanged();

    void selectedItemChanged();

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private slots:
    void setLoadingFromModel();

private:
    bool m_loading;
    QItemSelectionModel *m_selectionModel;
};

#endif // SORTMODEL_H
