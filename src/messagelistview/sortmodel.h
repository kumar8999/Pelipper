#ifndef SORTMODEL_H
#define SORTMODEL_H

#include <QSortFilterProxyModel>

class SortModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)

public:
    explicit SortModel(QObject *parent = nullptr);

    bool loading() const;
    void setLoading(bool newLoading);

signals:
    void loadingChanged();

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    bool m_loading;
};

#endif // SORTMODEL_H
