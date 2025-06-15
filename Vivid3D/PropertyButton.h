#pragma once

#include <QWidget>
#include <QPushButton>
#include <functional>

class PropertyButton : public QWidget
{
    Q_OBJECT

public:
    PropertyButton(const QString& buttonText, QWidget* parent = nullptr);
    ~PropertyButton();

    void setButtonText(const QString& text);
    QString getButtonText() const;

signals:
    void buttonClicked();

private slots:
    void onButtonClicked();

private:
    void setupButton();

    QPushButton* m_button;
    QString m_buttonText;
};