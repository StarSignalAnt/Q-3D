#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QStringList>

class PropertyStringList : public QWidget
{
    Q_OBJECT

public:
    PropertyStringList(const QString& label, const QStringList& options, QWidget* parent = nullptr);
    ~PropertyStringList();

    void setCurrentText(const QString& text);
    void setCurrentIndex(int index);
    QString getCurrentText() const;
    int getCurrentIndex() const;

    void setOptions(const QStringList& options);
    QStringList getOptions() const;

signals:
    void valueChanged(const QString& value);
    void indexChanged(int index);

private slots:
    void onComboBoxChanged();

private:
    void setupComboBox();

    QLabel* m_label;
    QComboBox* m_comboBox;
    QStringList m_options;
};