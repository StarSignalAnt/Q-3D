#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <string>

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget* parent = nullptr);

private slots:
    void onSearchTriggered();

private:
    void BeginSearch(std::string term);
    void setupUI();

    QLineEdit* m_textEdit;
    QPushButton* m_searchButton;
    QHBoxLayout* m_layout;
};

#endif // SEARCHWIDGET_H